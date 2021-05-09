/* This file is part of C++ State Machine Framework.
 *
 * C++ State Machine Framework is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * C++ State Machine Framework is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with C++ State
 * Machine Framework. If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * \file
 *
 * Contains a class for a state machine
 */

// Own header
#include <CppStateMachineFramework/StateMachine.hpp>

// C++ State Machine Framework includes

// Qt includes
#include <QtCore/QLoggingCategory>

// System includes

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

//! Logging category for the state machine
static const QLoggingCategory s_loggingCategory("CppStateMachineFramework.StateMachine");

// -------------------------------------------------------------------------------------------------

namespace CppStateMachineFramework
{

StateMachine::StateMachine()
    : m_validationStatus(ValidationStatus::Unvalidated),
      m_started(false)
{
}

// -------------------------------------------------------------------------------------------------

StateMachine::ValidationStatus StateMachine::validationStatus() const
{
    return m_validationStatus;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::validate()
{
    qCDebug(s_loggingCategory) << "Validating the state machine...";

    // Validation can only be executed if the state machine is stopped
    if (isStarted())
    {
        qCWarning(s_loggingCategory) << "Validation attempted on a started state machine";
        return false;
    }

    // Check if the state machine has at least one state
    if (m_states.isEmpty())
    {
        qCWarning(s_loggingCategory) << "State machine has no states";
        m_validationStatus = ValidationStatus::Invalid;
        return false;
    }

    // Check if initial state is set
    if (m_initialState.isEmpty())
    {
        qCWarning(s_loggingCategory) << "State machine has no initial state";
        m_validationStatus = ValidationStatus::Invalid;
        return false;
    }

    // Validate final states
    for (auto it_state = m_states.begin(); it_state != m_states.end(); it_state++)
    {
        const QString &stateName = it_state.key();
        const StateData &stateData = it_state.value();

        if (stateData.transitions.isEmpty())
        {
            // This is a final state, check if it has an exit method
            if (stateData.exitMethod)
            {
                qCWarning(s_loggingCategory)
                        << "A final state cannot have an exit method:" << stateName;
                m_validationStatus = ValidationStatus::Invalid;
                return false;
            }
        }
    }

    // Check if all of the states can be reached from the initial state
    const QList<QString> keys = m_states.keys();
    const QSet<QString> availableStates(keys.begin(), keys.end());
    QSet<QString> statesReached;

    traverseStates(m_initialState, &statesReached);

    if (statesReached != availableStates)
    {
        qCWarning(s_loggingCategory)
                << "The following states cannot be reached:" << (availableStates - statesReached);
        m_validationStatus = ValidationStatus::Invalid;
        return false;
    }

    // Validation successful
    m_validationStatus = ValidationStatus::Valid;
    qCDebug(s_loggingCategory) << "State machine validated successfully";
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::isStarted()
{
    return m_started;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::start()
{
    qCDebug(s_loggingCategory) << "Starting the state machine...";

    // State machine can be started only if it is stopped and valid
    if (m_started)
    {
        qCWarning(s_loggingCategory) << "State machine is already started";
        return false;
    }

    if (m_validationStatus != ValidationStatus::Valid)
    {
        qCWarning(s_loggingCategory) << "State machine can be started only if it is valid";
        return false;
    }

    // Transition to the initial state
    m_eventQueue.clear();
    m_currentState.clear();
    m_started = true;
    qCDebug(s_loggingCategory) << "State machine started";

    transitionToInitalState();
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::stop()
{
    qCDebug(s_loggingCategory) << "Stopping the state machine...";

    // Check if the state machine can be started
    if (!m_started)
    {
        qCWarning(s_loggingCategory) << "State machine is already stopped";
        return false;
    }

    // Stop the state machine
    qCDebug(s_loggingCategory) << "Current state:" << m_currentState;

    m_started = false;
    qCDebug(s_loggingCategory) << "State machine stopped";
    return true;
}

// -------------------------------------------------------------------------------------------------

QString StateMachine::currentState() const
{
    return m_currentState;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::finalStateReached()
{
    // Check if the current state is set to a valid state
    if (!m_states.contains(m_currentState))
    {
        return false;
    }

    // Check if current state is a final state (no transitions to other states)
    return m_states[m_currentState].transitions.isEmpty();
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::hasPendingEvents() const
{
    return (!m_eventQueue.empty());
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addEvent(std::unique_ptr<Event> event)
{
    // Check if the event is valid
    if (!event)
    {
        qCWarning(s_loggingCategory) << "Attempted to add an empty event";
        return false;
    }

    if (event->name().isEmpty())
    {
        qCWarning(s_loggingCategory) << "Attempted to add an event with an empty name";
        return false;
    }

    // Check if the state machine is started
    if (!m_started)
    {
        qCWarning(s_loggingCategory)
                << "Cannot add an event to a stopped state machine:" << event->name();
        return false;
    }

    qCDebug(s_loggingCategory) << "Added event to event queue:" << event->name();
    m_eventQueue.push_back(std::move(event));
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::processNextEvent()
{
    qCDebug(s_loggingCategory) << "Processing next event...";

    // Check if the state machine is started
    if (!m_started)
    {
        qCWarning(s_loggingCategory) << "State machine is not started";
        return false;
    }

    // Check if there are any pending events to process
    if (m_eventQueue.empty())
    {
        qCWarning(s_loggingCategory) << "No pending events to process!";
        return false;
    }

    // Take the next pending event and check if a state transition needs to be executed
    auto event = std::move(m_eventQueue.front());
    m_eventQueue.pop_front();
    qCDebug(s_loggingCategory) << "Processing event:" << event->name();

    auto transitions = m_states.value(m_currentState).transitions;
    auto it = transitions.find(event->name());

    if (it == transitions.end())
    {
        qCDebug(s_loggingCategory) << "No transitions for this event, ignore it";
        return true;
    }

    // Transition to the next state
    const auto &transitionData = *it;

    executeTransition(transitionData, *event);

    qCDebug(s_loggingCategory) << "Event processed";
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addState(const QString &stateName,
                            StateEntryMethod entryMethod,
                            StateExitMethod exitMethod)
{
    // Check if a state is allowed to be added at this time
    if (m_started)
    {
        qCWarning(s_loggingCategory)
                << "States can be added to the state machine only when it is stopped";
        return false;
    }

    // Check if the state name is valid or duplicate
    if (stateName.isEmpty())
    {
        qCWarning(s_loggingCategory) << "State name cannot be empty!";
        return false;
    }

    if (m_states.contains(stateName))
    {
        qCWarning(s_loggingCategory) << "A state with the same name already exists:" << stateName;
        return false;
    }

    // Add state
    m_states.insert(stateName, StateData { std::move(entryMethod), std::move(exitMethod), {} });
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory) << "Added a new state:" << stateName;
    return true;
}

// -------------------------------------------------------------------------------------------------

QString StateMachine::initialState() const
{
    return m_initialState;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::setInitialState(const QString &stateName)
{
    // Check if the initial state is already set
    if (!m_initialState.isEmpty())
    {
        qCWarning(s_loggingCategory) << "Initial state is already set:" << m_initialState;
        return false;
    }

    // Check if state exists
    if (!m_states.contains(stateName))
    {
        qCWarning(s_loggingCategory)
                << "Only existing states can be set as the initial state:" << stateName;
        return false;
    }

    // Set initial state
    m_initialState = stateName;
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory) << "Set the initial state:" << stateName;
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addTransition(const QString &fromState,
                                 const QString &eventName,
                                 const QString &toState,
                                 TransitionGuardMethod guardMethod,
                                 TransitionActionMethod actionMethod)
{
    // Check if a transition is allowed to be added at this time
    if (m_started)
    {
        qCWarning(s_loggingCategory)
                << "Transitions can be added to the state machine only when it is stopped";
        return false;
    }

    // Check if the state and event names are valid
    if (!m_states.contains(fromState))
    {
        qCWarning(s_loggingCategory) << "State to transition from does not exist:" << fromState;
        return false;
    }

    if (eventName.isEmpty())
    {
        qCWarning(s_loggingCategory)
                << "Name of the event that triggers the transition cannot be empty";
        return false;
    }

    if (!m_states.contains(toState))
    {
        qCWarning(s_loggingCategory) << "State to transition to does not exist:" << toState;
        return false;
    }

    // Check if transition already exists
    auto &stateData = m_states[fromState];

    if (stateData.transitions.contains(eventName))
    {
        qCWarning(s_loggingCategory)
                << QString("Transition from state [%1] with event [%2] already exists")
                   .arg(fromState, eventName);
        return false;
    }

    // Add transition
    stateData.transitions.insert(
                eventName,
                TransitionData { toState, std::move(guardMethod), std::move(actionMethod) });
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory)
            << QString("Added a transition from state [%1] with event [%2] to state [%3]")
               .arg(fromState, eventName, toState);
    return true;
}

// -------------------------------------------------------------------------------------------------

void StateMachine::traverseStates(const QString &stateName, QSet<QString> *statesReached) const
{
    // Record that the specified state was reached
    statesReached->insert(stateName);

    // Traverse all the states that can be transitioned to from the specified state
    for (const auto &transitionData : m_states[stateName].transitions)
    {
        // Check if the transition needs to be processed
        if (statesReached->contains(transitionData.state))
        {
            // State to transition to was already reached, skip the transition
            continue;
        }

        // Traverse state from the transition
        traverseStates(transitionData.state, statesReached);
    }
}

// -------------------------------------------------------------------------------------------------

void StateMachine::transitionToInitalState()
{
    // Transition to the initial state
    m_currentState = m_initialState;
    qCDebug(s_loggingCategory) << "Transitioned to initial state:" << m_currentState;

    // Execute the entry method of the initial state
    auto &stateData = m_states[m_currentState];

    if (stateData.entryMethod)
    {
        qCDebug(s_loggingCategory) << "Executing entry method...";
        stateData.entryMethod(Event(QString()));
        qCDebug(s_loggingCategory) << "Entry method executed";
    }

    // Check if the initial state is also a final state
    if (stateData.transitions.isEmpty())
    {
        qCDebug(s_loggingCategory) << "Transitioned to a final state";
        stop();
    }
}

// -------------------------------------------------------------------------------------------------

void StateMachine::executeTransition(const TransitionData &transitionData, const Event &event)
{
    // Check if the transition is blocked by the guard method
    if (transitionData.guardMethod)
    {
        if (!transitionData.guardMethod(event))
        {
            qCDebug(s_loggingCategory)
                    << QString("Transition from state [%1] with event [%2] to state [%3] was "
                               "blocked by the guard method")
                       .arg(m_currentState, event.name(), transitionData.state);
            return;
        }
    }

    qCDebug(s_loggingCategory)
            << QString("Transitioning from state [%1] with event [%2] to state [%3]...")
               .arg(m_currentState, event.name(), transitionData.state);

    // Execute the exit method of the current state
    auto &currentStateData = m_states[m_currentState];

    if (currentStateData.exitMethod)
    {
        qCDebug(s_loggingCategory) << "Executing exit method...";
        currentStateData.exitMethod(event);
        qCDebug(s_loggingCategory) << "Exit method executed";
    }

    // Execute transition's action
    if (transitionData.actionMethod)
    {
        qCDebug(s_loggingCategory) << "Executing transition's action method...";
        transitionData.actionMethod(event);
        qCDebug(s_loggingCategory) << "Transition's action method executed";
    }

    // Transition to the next state
    m_currentState = transitionData.state;
    qCDebug(s_loggingCategory) << "Transitioned to state:" << m_currentState;

    // Execute the entry method of the next state
    auto &nextStateData = m_states[m_currentState];

    if (nextStateData.entryMethod)
    {
        qCDebug(s_loggingCategory) << "Executing entry method...";
        nextStateData.entryMethod(event);
        qCDebug(s_loggingCategory) << "Entry method executed";
    }

    // Check if the state machine transitioned to a final state
    if (nextStateData.transitions.isEmpty())
    {
        qCDebug(s_loggingCategory) << "Transitioned to a final state";
        stop();
    }
}

} // namespace CppStateMachineFramework
