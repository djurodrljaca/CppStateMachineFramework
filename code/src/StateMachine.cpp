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
#include <utility>

// C++ State Machine Framework includes

// Qt includes
#include <QtCore/QLoggingCategory>

// System includes

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

//! Logging category for the state machine
static const QLoggingCategory s_stateMachineLoggingCategory(
        "CppStateMachineFramework.StateMachine");

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
    qCDebug(s_stateMachineLoggingCategory) << "Validating the state machine...";

    // Validation can only be executed if the state machine is stopped
    if (isStarted())
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Validation attempted on a started state machine";
        return false;
    }

    // Check if the state machine has at least one state
    if (m_states.isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory) << "State machine has no states";
        m_validationStatus = ValidationStatus::Invalid;
        return false;
    }

    // Check if initial state is set
    if (m_initialState.isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory) << "State machine has no initial state";
        m_validationStatus = ValidationStatus::Invalid;
        return false;
    }

    // Validate all transitions
    for (auto it_state = m_states.begin(); it_state != m_states.end(); it_state++)
    {
        const QString &stateName = it_state.key();
        const StateData &stateData = it_state.value();

        for (auto it_transition = stateData.transitions.begin();
             it_transition != stateData.transitions.end();
             it_transition++)
        {
            const QString &eventName = it_transition.key();
            const TransitionData &transitionData = it_transition.value();

            if (!m_states.contains(transitionData.state))
            {
                qCWarning(s_stateMachineLoggingCategory)
                        << QString("Invalid transition from state [%1] with event [%2] to unknown "
                                   "state [%3]")
                           .arg(stateName, eventName, transitionData.state);
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
        qCWarning(s_stateMachineLoggingCategory)
                << "The following states cannot be reached:" << (availableStates - statesReached);
        m_validationStatus = ValidationStatus::Invalid;
        return false;
    }

    // Validation successful
    m_validationStatus = ValidationStatus::Valid;
    qCDebug(s_stateMachineLoggingCategory) << "State machine validated successfully";
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
    qCDebug(s_stateMachineLoggingCategory) << "Starting the state machine...";

    // State machine can be started only if it is stopped and valid
    if (m_started)
    {
        qCWarning(s_stateMachineLoggingCategory) << "State machine is already started";
        return false;
    }

    if (m_validationStatus != ValidationStatus::Valid)
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "State machine can be started only if it is valid";
        return false;
    }

    // Clear all pending events and the current state
    m_eventQueue.clear();
    m_currentState.clear();

    // Call the state machine entry method
    if (m_stateMachineEntryMethod)
    {
        qCDebug(s_stateMachineLoggingCategory) << "Executing entry action...";
        m_stateMachineEntryMethod();
        qCDebug(s_stateMachineLoggingCategory) << "Entry action executed";
    }

    // Transition to the initial state
    m_currentState = m_initialState;
    qCDebug(s_stateMachineLoggingCategory) << "Initial state set to:" << m_currentState;

    m_started = true;
    qCDebug(s_stateMachineLoggingCategory) << "State machine started successfully";
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::stop()
{
    qCDebug(s_stateMachineLoggingCategory) << "Stopping the state machine...";

    // Check if the state machine can be started
    if (!m_started)
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "State machine can only be stopped if it is currently started";
        return false;
    }

    // Stop the state machine
    qCDebug(s_stateMachineLoggingCategory) << "Current state:" << m_currentState;

    m_started = false;
    qCDebug(s_stateMachineLoggingCategory) << "State machine stopped successfully";
    return true;
}

// -------------------------------------------------------------------------------------------------

QString StateMachine::currentState() const
{
    return m_currentState;
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
        qCWarning(s_stateMachineLoggingCategory) << "Attempted to add an empty event";
        return false;
    }

    if (event->name().isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory) << "Attempted to add an event with an empty name";
        return false;
    }

    // Check if the state machine is started
    if (!m_started)
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Cannot add an event to a stopped state machine:" << event->name();
        return false;
    }

    qCDebug(s_stateMachineLoggingCategory) << "Added event to event queue:" << event->name();
    m_eventQueue.push_back(std::move(event));
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::processNextEvent()
{
    qCDebug(s_stateMachineLoggingCategory) << "Processing next event...";

    // Check if the state machine is started
    if (!m_started)
    {
        qCWarning(s_stateMachineLoggingCategory) << "State machine is not started";
        return false;
    }

    // Check if there are any pending events to process
    if (m_eventQueue.empty())
    {
        qCWarning(s_stateMachineLoggingCategory) << "No pending events to process!";
        return false;
    }

    // Take the next pending event and check if a state transition needs to be executed
    auto event = std::move(m_eventQueue.front());
    m_eventQueue.pop_front();
    qCDebug(s_stateMachineLoggingCategory) << "Processing event:" << event->name();

    auto transitions = m_states.value(m_currentState).transitions;
    auto it = transitions.find(event->name());

    if (it == transitions.end())
    {
        qCDebug(s_stateMachineLoggingCategory) << "No transitions for this event, ignore it";
        return false;
    }

    // Transition to the next state
    const auto &transitionData = *it;

    executeTransition(transitionData, *event);

    qCDebug(s_stateMachineLoggingCategory) << "Event processed";
    return true;
}

// -------------------------------------------------------------------------------------------------

QString StateMachine::initialState() const
{
    return m_initialState;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::setInitialState(const QString &stateName,
                                   StateMachineEntryMethod stateMachineEntryMethod)
{
    // Check if the initial state is allowed to be set at this time
    if (m_started)
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Initial state can be set only when the state machine is stopped";
        return false;
    }

    // Check if the initial state is already set
    if (!m_initialState.isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory) << "Initial state is already set:" << m_finalState;
        return false;
    }

    // Check if state exists
    if (!m_states.contains(stateName))
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Only existing states can be set as the initial state:" << stateName;
        return false;
    }

    // Check if initial state is the same as final state
    if (stateName == m_finalState)
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Initial state cannot be set to the same state as the final state:" << stateName;
        return false;
    }

    // Set initial state
    m_initialState = stateName;
    m_stateMachineEntryMethod = std::move(stateMachineEntryMethod);
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_stateMachineLoggingCategory) << "Set the initial state:" << stateName;
    return true;
}

// -------------------------------------------------------------------------------------------------

QString StateMachine::finalState() const
{
    return m_finalState;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::setFinalState(const QString &stateName,
                                 StateMachineExitMethod stateMachineExitMethod)
{
    // Check if the final state is allowed to be set at this time
    if (m_started)
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Final state can be set only when the state machine is stopped";
        return false;
    }

    // Check if the final state is already set
    if (!m_finalState.isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory) << "Final state is already set:" << m_finalState;
        return false;
    }

    // Check if final state needs to be disabled
    if (stateName.isEmpty())
    {
        // Disable final state
        m_finalState.clear();
        m_validationStatus = ValidationStatus::Unvalidated;

        qCDebug(s_stateMachineLoggingCategory) << "Disabled final state";
        return true;
    }

    // Add new state and set it as the final state
    if (m_states.contains(stateName))
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "A state with the same name already exists:" << stateName;
        return false;
    }

    m_states.insert(stateName, StateData {});
    m_finalState = stateName;
    m_stateMachineExitMethod = std::move(stateMachineExitMethod);
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_stateMachineLoggingCategory)
            << "Added a new state and set it as the final state:" << stateName;
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addState(const QString &stateName,
                            StateEntryMethod stateEntryMethod,
                            StateExitMethod stateExitMethod)
{
    // Check if a state is allowed to be added at this time
    if (m_started)
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "States can be added to the state machine only when it is stopped";
        return false;
    }

    // Check if the state name is valid or duplicate
    if (stateName.isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory) << "State name cannot be empty!";
        return false;
    }

    if (m_states.contains(stateName))
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "A state with the same name already exists:" << stateName;
        return false;
    }

    // Add state
    m_states.insert(stateName,
                    StateData { std::move(stateEntryMethod), std::move(stateExitMethod), {} });
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_stateMachineLoggingCategory) << "Added a new state:" << stateName;
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
        qCWarning(s_stateMachineLoggingCategory)
                << "Transitions can be added to the state machine only when it is stopped";
        return false;
    }

    // Check if the state and event names are valid
    if (fromState.isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Name of the state to transition from cannot be empty";
        return false;
    }

    if (!m_states.contains(fromState))
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "State to transition from does not exist:" << fromState;
        return false;
    }

    if (fromState == m_finalState)
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Cannot transition from final state:" << fromState;
        return false;
    }

    if (eventName.isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Name of the event that triggers the transition cannot be empty";
        return false;
    }

    if (toState.isEmpty())
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "Name of the state to transition to cannot be empty";
        return false;
    }

    if (!m_states.contains(toState))
    {
        qCWarning(s_stateMachineLoggingCategory)
                << "State to transition to does not exist:" << toState;
        return false;
    }

    // Check if transition already exists
    auto &stateData = m_states[fromState];

    if (stateData.transitions.contains(eventName))
    {
        qCWarning(s_stateMachineLoggingCategory)
                << QString("Transition from state [%1] with event [%2] already exists")
                   .arg(fromState, eventName);
        return false;
    }

    // Add transition
    stateData.transitions.insert(
                eventName,
                TransitionData { toState, std::move(guardMethod), std::move(actionMethod) });
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_stateMachineLoggingCategory)
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

void StateMachine::executeTransition(const TransitionData &transitionData,
                                     const Event &event)
{
    // Check if the transition is blocked by the guard method
    if (transitionData.guardMethod)
    {
        if (!transitionData.guardMethod(event))
        {
            qCDebug(s_stateMachineLoggingCategory)
                    << QString("Transition from state [%1] with event [%2] to state [%3] was "
                               "blocked by the guard method")
                       .arg(m_currentState, event.name(), transitionData.state);
            return;
        }
    }

    qCDebug(s_stateMachineLoggingCategory)
            << QString("Transitioning from state [%1] with event [%2] to state [%3]...")
               .arg(m_currentState, event.name(), transitionData.state);

    // Execute transition's action
    if (transitionData.actionMethod)
    {
        qCDebug(s_stateMachineLoggingCategory) << "Executing transition's action method...";
        transitionData.actionMethod(event);
        qCDebug(s_stateMachineLoggingCategory) << "Transition's action method executed";
    }

    // Transition to the next state
    m_currentState = transitionData.state;

    if (m_currentState != m_finalState)
    {
        // Transitioned to non-final state
        qCDebug(s_stateMachineLoggingCategory) << "Transitioned to state:" << m_currentState;
        return;
    }

    // Transitioned to the final state
    qCDebug(s_stateMachineLoggingCategory) << "Transitioned to the final state:" << m_currentState;

    // Call the exit action
    if (m_stateMachineExitMethod)
    {
        qCDebug(s_stateMachineLoggingCategory) << "Executing exit action...";
        m_stateMachineExitMethod();
        qCDebug(s_stateMachineLoggingCategory) << "Exit action executed";
    }

    // Stop state machine
    stop();
}

} // namespace CppStateMachineFramework
