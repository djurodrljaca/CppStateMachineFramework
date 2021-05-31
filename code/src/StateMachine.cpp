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

StateMachine::StateMachine(StateMachine &&other) noexcept
    : m_states(std::move(other.m_states)),
      m_initialTransition(std::move(other.m_initialTransition)),
      m_validationStatus(other.m_validationStatus),
      m_started(other.m_started),
      m_currentState(std::move(other.m_currentState)),
      m_eventQueue(std::move(other.m_eventQueue)),
      m_finalEvent(std::move(other.m_finalEvent))
{
}

// -------------------------------------------------------------------------------------------------

StateMachine &StateMachine::operator=(StateMachine &&other) noexcept
{
    if (this != (&other))
    {
        m_states = std::move(other.m_states);
        m_initialTransition = std::move(other.m_initialTransition);
        m_validationStatus = other.m_validationStatus;
        m_started = other.m_started;
        m_currentState = std::move(other.m_currentState);
        m_eventQueue = std::move(other.m_eventQueue);
        m_finalEvent = std::move(other.m_finalEvent);
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

StateMachine::ValidationStatus StateMachine::validationStatus() const
{
    QMutexLocker locker(&m_apiMutex);

    return m_validationStatus;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::validate()
{
    QMutexLocker locker(&m_apiMutex);

    qCDebug(s_loggingCategory) << "Validating the state machine...";

    // Validation can only be executed if the state machine is stopped
    if (isStarted())
    {
        qCWarning(s_loggingCategory) << "Validation attempted on a started state machine";
        return false;
    }

    // Check if the state machine has at least one state
    if (m_states.empty())
    {
        qCWarning(s_loggingCategory) << "State machine has no states";
        m_validationStatus = ValidationStatus::Invalid;
        return false;
    }

    // Check if initial transition is set
    if (m_initialTransition.state.isEmpty())
    {
        qCWarning(s_loggingCategory) << "State machine has no initial transition";
        m_validationStatus = ValidationStatus::Invalid;
        return false;
    }

    // Validate final states
    QSet<QString> availableStates;

    for (auto it_state = m_states.begin(); it_state != m_states.end(); it_state++)
    {
        const QString &stateName = it_state->first;
        const StateData &stateData = it_state->second;
        availableStates.insert(stateName);

        if (isFinalState(stateData))
        {
            // This is a final state, check if it has an exit action
            if (stateData.exitAction)
            {
                qCWarning(s_loggingCategory)
                        << "A final state cannot have an exit action:" << stateName;
                m_validationStatus = ValidationStatus::Invalid;
                return false;
            }
        }
    }

    // Check if all of the states can be reached from the initial state
    QSet<QString> statesReached;
    traverseStates(m_initialTransition.state, &statesReached);

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
    QMutexLocker locker(&m_startedMutex);

    return m_started;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::start(Event &&event)
{
    QMutexLocker apiLocker(&m_apiMutex);

    qCDebug(s_loggingCategory) << "Starting the state machine...";

    // Check if the event is valid
    if (event.name().isEmpty())
    {
        qCWarning(s_loggingCategory) << "Attempted to add an event with an empty name";
        return false;
    }

    // State machine can be started only if it is stopped and valid
    QMutexLocker startedLocker(&m_startedMutex);

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

    // Execute initial transition
    QMutexLocker eventQueueLocker(&m_eventQueueMutex);

    m_eventQueue.clear();
    m_currentState.clear();
    m_finalEvent.reset();
    m_started = true;

    qCDebug(s_loggingCategory) << "State machine started";

    eventQueueLocker.unlock();
    startedLocker.unlock();

    executeInitialTransition(std::move(event));
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::stop()
{
    QMutexLocker locker(&m_apiMutex);

    return stopInternal();
}

// -------------------------------------------------------------------------------------------------

QString StateMachine::currentState() const
{
    QMutexLocker locker(&m_apiMutex);

    return m_currentState;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::finalStateReached() const
{
    QMutexLocker locker(&m_apiMutex);

    // Check if the the state is set to a valid state
    auto it = m_states.find(m_currentState);

    if (it == m_states.end())
    {
        // This should not be possible as the current state cannot be changed externally and it
        // should always store the name of an existing state
        qCWarning(s_loggingCategory) << "Current state is invalid!";
        return false;
    }

    // Check if the state is a final state (no transitions)
    return isFinalState(it->second);
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::hasFinalEvent() const
{
    return (m_finalEvent != nullptr);
}

// -------------------------------------------------------------------------------------------------

std::unique_ptr<Event> StateMachine::takeFinalEvent()
{
    QMutexLocker locker(&m_apiMutex);

    return std::move(m_finalEvent);
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::hasPendingEvents() const
{
    QMutexLocker locker(&m_eventQueueMutex);

    return (!m_eventQueue.empty());
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addEventToFront(Event &&event)
{
    QMutexLocker eventQueueLocker(&m_eventQueueMutex);
    QMutexLocker startedLocker(&m_startedMutex);

    // Check if the event is valid
    if (event.name().isEmpty())
    {
        qCWarning(s_loggingCategory) << "Attempted to add an event with an empty name";
        return false;
    }

    // Check if the state machine is started
    if (!m_started)
    {
        qCWarning(s_loggingCategory)
                << "Cannot add an event to a stopped state machine:" << event.name();
        return false;
    }

    qCDebug(s_loggingCategory) << "Added event to the front of the event queue:" << event.name();
    m_eventQueue.push_front(std::move(event));
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addEventToBack(Event &&event)
{
    QMutexLocker eventQueueLocker(&m_eventQueueMutex);
    QMutexLocker startedLocker(&m_startedMutex);

    // Check if the event is valid
    if (event.name().isEmpty())
    {
        qCWarning(s_loggingCategory) << "Attempted to add an event with an empty name";
        return false;
    }

    // Check if the state machine is started
    if (!m_started)
    {
        qCWarning(s_loggingCategory)
                << "Cannot add an event to a stopped state machine:" << event.name();
        return false;
    }

    qCDebug(s_loggingCategory) << "Added event to the back of the event queue:" << event.name();
    m_eventQueue.push_back(std::move(event));
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::processNextEvent()
{
    QMutexLocker apiLocker(&m_apiMutex);

    qCDebug(s_loggingCategory) << "Processing next event...";

    // Check if the state machine is started
    if (!isStarted())
    {
        qCWarning(s_loggingCategory) << "State machine is not started";
        return false;
    }

    // Check if there are any pending events to process
    QMutexLocker eventQueuelocker(&m_eventQueueMutex);

    if (m_eventQueue.empty())
    {
        qCWarning(s_loggingCategory) << "No pending events to process!";
        return false;
    }

    // Take the next pending event
    auto event = std::move(m_eventQueue.front());
    m_eventQueue.pop_front();
    qCDebug(s_loggingCategory) << "Processing event:" << event.name();

    eventQueuelocker.unlock();

    // Check if a transition needs to be executed
    auto itState = m_states.find(m_currentState);

    if (itState == m_states.end())
    {
        // This should not be possible as the current state cannot be changed externally and it
        // should always store the name of an existing state
        qCWarning(s_loggingCategory) << "Current state is invalid!";
        return false;
    }

    const auto &stateData = itState->second;

    if (!stateData.internalTransitions.empty())
    {
        // Check if an internal transition needs to be triggered
        auto itTransition = stateData.internalTransitions.find(event.name());

        if (itTransition != stateData.internalTransitions.end())
        {
            // Execute internal transition
            executeInternalTransition(itTransition->second, event);

            qCDebug(s_loggingCategory) << "Event processed";
            return true;
        }
    }

    if (!stateData.stateTransitions.empty())
    {
        // Check if a state transition needs to be triggered
        auto itTransition = stateData.stateTransitions.find(event.name());

        if (itTransition != stateData.stateTransitions.end())
        {
            // Execute state transition
            executeStateTransition(itTransition->second, std::move(event));

            qCDebug(s_loggingCategory) << "Event processed";
            return true;
        }
    }

    if (stateData.defaultInternalTransition)
    {
        // Execute default internal transition
        executeInternalTransition(*stateData.defaultInternalTransition, event);

        qCDebug(s_loggingCategory) << "Event processed";
        return true;
    }

    if (stateData.defaultStateTransition)
    {
        // Execute default state transition
        executeStateTransition(*stateData.defaultStateTransition, std::move(event));

        qCDebug(s_loggingCategory) << "Event processed";
        return true;
    }

    qCDebug(s_loggingCategory) << "No transitions for this event, ignore it:" << event.name();
    qCDebug(s_loggingCategory) << "Event processed";
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addState(const QString &stateName)
{
    QMutexLocker locker(&m_apiMutex);

    // Check if a state is allowed to be added at this time
    if (isStarted())
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

    if (m_states.find(stateName) != m_states.end())
    {
        qCWarning(s_loggingCategory) << "A state with the same name already exists:" << stateName;
        return false;
    }

    // Add state
    m_states[stateName] = {};
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory) << "Added a new state:" << stateName;
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::setStateEntryAction(const QString &stateName, StateEntryAction entryAction)
{
    QMutexLocker locker(&m_apiMutex);

    // Check if a state's entry action is allowed to be set at this time
    if (isStarted())
    {
        qCWarning(s_loggingCategory)
                << "State's entry action can be set only when the state machine is stopped";
        return false;
    }

    // Check if the action is empty
    if (!entryAction)
    {
        qCWarning(s_loggingCategory) << "Invalid state's entry action:" << stateName;
        return false;
    }

    // Check if the state name is valid
    auto itState = m_states.find(stateName);

    if (itState == m_states.end())
    {
        qCWarning(s_loggingCategory) << "State does not exist:" << stateName;
        return false;
    }

    // Check if state's entry action already exists
    auto &stateData = itState->second;

    if (stateData.entryAction)
    {
        qCWarning(s_loggingCategory) << "The state's entry action is already set:" << stateName;
        return false;
    }

    // Set state's entry action
    stateData.entryAction = std::move(entryAction);
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory) << "Set the state's entry action:" << stateName;
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::setStateExitAction(const QString &stateName, StateExitAction exitAction)
{
    QMutexLocker locker(&m_apiMutex);

    // Check if a state's exit action is allowed to be set at this time
    if (isStarted())
    {
        qCWarning(s_loggingCategory)
                << "State's exit action can be set only when the state machine is stopped";
        return false;
    }

    // Check if the action is empty
    if (!exitAction)
    {
        qCWarning(s_loggingCategory) << "Invalid state's exit action:" << stateName;
        return false;
    }

    // Check if the state name is valid
    auto itState = m_states.find(stateName);

    if (itState == m_states.end())
    {
        qCWarning(s_loggingCategory) << "State does not exist:" << stateName;
        return false;
    }

    // Check if state's exit action already exists
    auto &stateData = itState->second;

    if (stateData.exitAction)
    {
        qCWarning(s_loggingCategory) << "The state's exit action is already set:" << stateName;
        return false;
    }

    // Set state's exit action
    stateData.exitAction = std::move(exitAction);
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory) << "Set the state's exit action:" << stateName;
    return true;
}

// -------------------------------------------------------------------------------------------------

QString StateMachine::initialState() const
{
    QMutexLocker locker(&m_apiMutex);

    return m_initialTransition.state;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::setInitialTransition(const QString &initialState, InitialTransitionAction action)
{
    QMutexLocker locker(&m_apiMutex);

    // Check if the initial state is already set
    if (!m_initialTransition.state.isEmpty())
    {
        qCWarning(s_loggingCategory) << "Initial state is already set:"
                                     << m_initialTransition.state;
        return false;
    }

    // Check if state exists
    if (m_states.find(initialState) == m_states.end())
    {
        qCWarning(s_loggingCategory)
                << "Only existing states can be set as the initial state:" << initialState;
        return false;
    }

    // Set initial state
    m_initialTransition.state = initialState;
    m_initialTransition.action = std::move(action);
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory) << "Set the initial state:" << initialState;
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addStateTransition(const QString &fromState,
                                      const QString &trigger,
                                      const QString &toState,
                                      StateTransitionAction action,
                                      StateTransitionGuardCondition guard)
{
    QMutexLocker locker(&m_apiMutex);

    // Check if a transition is allowed to be added at this time
    if (isStarted())
    {
        qCWarning(s_loggingCategory)
                << "Transitions can be added to the state machine only when it is stopped";
        return false;
    }

    // Check if the state and event names are valid
    auto itFromState = m_states.find(fromState);

    if (itFromState == m_states.end())
    {
        qCWarning(s_loggingCategory) << "State to transition from does not exist:" << fromState;
        return false;
    }

    if (trigger.isEmpty())
    {
        qCWarning(s_loggingCategory)
                << "Name of the event that triggers the transition cannot be empty";
        return false;
    }

    if (m_states.find(toState) == m_states.end())
    {
        qCWarning(s_loggingCategory) << "State to transition to does not exist:" << toState;
        return false;
    }

    // Check if transition already exists
    auto &stateData = itFromState->second;

    if ((stateData.stateTransitions.find(trigger) != stateData.stateTransitions.end()) ||
        (stateData.internalTransitions.find(trigger) != stateData.internalTransitions.end()))
    {
        qCWarning(s_loggingCategory)
                << QString("Transition from state [%1] with event [%2] already exists")
                   .arg(fromState, trigger);
        return false;
    }

    // Add transition
    stateData.stateTransitions[trigger] = { toState, std::move(guard), std::move(action) };
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory)
            << QString("Added a state transition from state [%1] with event [%2] to state [%3]")
               .arg(fromState, trigger, toState);
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::addInternalTransition(const QString &state,
                                         const QString &trigger,
                                         InternalTransitionAction action,
                                         InternalTransitionGuardCondition guard)
{
    QMutexLocker locker(&m_apiMutex);

    // Check if a transition is allowed to be added at this time
    if (isStarted())
    {
        qCWarning(s_loggingCategory)
                << "Transitions can be added to the state machine only when it is stopped";
        return false;
    }

    // Check if the state and event names are valid
    auto itState = m_states.find(state);

    if (itState == m_states.end())
    {
        qCWarning(s_loggingCategory) << "State of the internal transition does not exist:" << state;
        return false;
    }

    if (trigger.isEmpty())
    {
        qCWarning(s_loggingCategory)
                << "Name of the event that triggers the transition cannot be empty";
        return false;
    }

    // Check if transition has an action
    if (!action)
    {
        qCWarning(s_loggingCategory) << "Internal transition does not have an action:" << state;
        return false;
    }

    // Check if transition already exists
    auto &stateData = itState->second;

    if ((stateData.stateTransitions.find(trigger) != stateData.stateTransitions.end()) ||
        (stateData.internalTransitions.find(trigger) != stateData.internalTransitions.end()))
    {
        qCWarning(s_loggingCategory)
                << QString("Transition from state [%1] with event [%2] already exists")
                   .arg(state, trigger);
        return false;
    }

    // Add transition
    stateData.internalTransitions[trigger] = { std::move(guard), std::move(action) };
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory)
            << QString("Added an internal transition to state [%1] with event [%2]")
               .arg(state, trigger);
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::setDefaultTransition(const QString &fromState,
                                        const QString &toState,
                                        StateTransitionAction action,
                                        StateTransitionGuardCondition guard)
{
    QMutexLocker locker(&m_apiMutex);

    // Check if a transition is allowed to be added at this time
    if (isStarted())
    {
        qCWarning(s_loggingCategory)
                << "Transitions can be added to the state machine only when it is stopped";
        return false;
    }

    // Check if the state names are valid
    auto itFromState = m_states.find(fromState);

    if (itFromState == m_states.end())
    {
        qCWarning(s_loggingCategory) << "State to transition from does not exist:" << fromState;
        return false;
    }

    if (m_states.find(toState) == m_states.end())
    {
        qCWarning(s_loggingCategory) << "State to transition to does not exist:" << toState;
        return false;
    }

    // Check if a default transition already exists
    auto &stateData = itFromState->second;

    if (stateData.defaultStateTransition || stateData.defaultInternalTransition)
    {
        qCWarning(s_loggingCategory)
                << QString("A default transition for state [%1] already exists").arg(fromState);
        return false;
    }

    // Set default transition
    stateData.defaultStateTransition =
            std::make_unique<StateTransitionData>(
                StateTransitionData { toState, std::move(guard), std::move(action) });
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory)
            << QString("Set a default state transition from state [%1] to state [%2]")
               .arg(fromState, toState);
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::setDefaultTransition(const QString &state,
                                        InternalTransitionAction action,
                                        InternalTransitionGuardCondition guard)
{
    QMutexLocker locker(&m_apiMutex);

    // Check if a transition is allowed to be added at this time
    if (isStarted())
    {
        qCWarning(s_loggingCategory)
                << "Transitions can be added to the state machine only when it is stopped";
        return false;
    }

    // Check if the state name is valid
    auto itState = m_states.find(state);

    if (itState == m_states.end())
    {
        qCWarning(s_loggingCategory) << "State of the internal transition does not exist:" << state;
        return false;
    }

    // Check if transition has an action
    if (!action)
    {
        qCWarning(s_loggingCategory) << "Internal transition does not have an action:" << state;
        return false;
    }

    // Check if a default transition already exists
    auto &stateData = itState->second;

    if (stateData.defaultStateTransition || stateData.defaultInternalTransition)
    {
        qCWarning(s_loggingCategory)
                << QString("A default transition for state [%1] already exists").arg(state);
        return false;
    }

    // Set default transition
    stateData.defaultInternalTransition =
            std::make_unique<InternalTransitionData>(
                InternalTransitionData { std::move(guard), std::move(action) });
    m_validationStatus = ValidationStatus::Unvalidated;

    qCDebug(s_loggingCategory)
            << QString("Set a default internal transition for state [%1]").arg(state);
    return true;
}

// -------------------------------------------------------------------------------------------------

bool StateMachine::stopInternal()
{
    QMutexLocker locker(&m_startedMutex);

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

void StateMachine::traverseStates(const QString &stateName, QSet<QString> *statesReached) const
{
    // Record that the specified state was reached
    statesReached->insert(stateName);

    // Traverse all the states that can be transitioned to from the specified state
    auto it = m_states.find(stateName);

    if (it == m_states.end())
    {
        // This should not be possible as the method will always be passed an existing state's name
        qCWarning(s_loggingCategory) << "State name is invalid!";
        return;
    }

    const auto &stateData = it->second;

    for (const auto &item : stateData.stateTransitions)
    {
        // Check if the transition needs to be processed
        const auto &transitionData = item.second;

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

bool StateMachine::isFinalState(const StateData &stateData) const
{
    return (stateData.stateTransitions.empty() &&
            stateData.internalTransitions.empty() &&
            (!stateData.defaultStateTransition) &&
            (!stateData.defaultInternalTransition));
}

// -------------------------------------------------------------------------------------------------

void StateMachine::executeInitialTransition(Event &&event)
{
    qCDebug(s_loggingCategory)
            << QString("Transitioning to initial state [%1] with event [%2]...")
               .arg(m_initialTransition.state, event.name());

    // Execute transition's action
    if (m_initialTransition.action)
    {
        qCDebug(s_loggingCategory) << "Executing initial transition's action...";
        m_initialTransition.action(event, m_initialTransition.state);
        qCDebug(s_loggingCategory) << "Initial transition's action executed";
    }

    // Execute the entry action of the initial state
    auto it = m_states.find(m_initialTransition.state);

    if (it == m_states.end())
    {
        // This should not be possible as the initial state cannot be changed after it is set and it
        // should always store the name of an existing state
        qCWarning(s_loggingCategory) << "Initial state is invalid!";
        return;
    }

    auto &stateData = it->second;

    if (stateData.entryAction)
    {
        qCDebug(s_loggingCategory) << "Executing entry action...";
        stateData.entryAction(event, m_initialTransition.state, QString());
        qCDebug(s_loggingCategory) << "entry action executed";
    }

    // Transition to the initial state
    m_currentState = m_initialTransition.state;
    qCDebug(s_loggingCategory) << "Transitioned to initial state:" << m_currentState;

    // Check if the initial state is also a final state
    if (isFinalState(stateData))
    {
        // Store final event
        m_finalEvent = std::make_unique<Event>(std::move(event));

        qCDebug(s_loggingCategory) << "Transitioned to a final state";
        stopInternal();
    }
}

// -------------------------------------------------------------------------------------------------

void StateMachine::executeStateTransition(const StateTransitionData &transitionData, Event &&event)
{
    // Check if the transition is blocked by the guard condition
    if (transitionData.guard)
    {
        if (!transitionData.guard(event, m_currentState, transitionData.state))
        {
            qCDebug(s_loggingCategory)
                    << QString("Transition from state [%1] with event [%2] to state [%3] was "
                               "blocked by the guard condition")
                       .arg(m_currentState, event.name(), transitionData.state);
            return;
        }
    }

    qCDebug(s_loggingCategory)
            << QString("Transitioning from state [%1] with event [%2] to state [%3]...")
               .arg(m_currentState, event.name(), transitionData.state);

    // Execute the exit action of the current state
    auto it = m_states.find(m_currentState);

    if (it == m_states.end())
    {
        // This should not be possible as the current state cannot be changed externally and it
        // should always store the name of an existing state
        qCWarning(s_loggingCategory) << "Current state is invalid!";
        return;
    }

    auto &currentStateData = it->second;

    if (currentStateData.exitAction)
    {
        qCDebug(s_loggingCategory) << "Executing state's exit action...";
        currentStateData.exitAction(event, m_currentState, transitionData.state);
        qCDebug(s_loggingCategory) << "State's exit action executed";
    }

    // Execute transition's action
    if (transitionData.action)
    {
        qCDebug(s_loggingCategory) << "Executing state transition's action...";
        transitionData.action(event, m_currentState, transitionData.state);
        qCDebug(s_loggingCategory) << "State transition's action executed";
    }

    // Execute the entry action of the next state
    auto &nextStateData = m_states[transitionData.state];

    if (nextStateData.entryAction)
    {
        qCDebug(s_loggingCategory) << "Executing entry action...";
        nextStateData.entryAction(event, transitionData.state, m_currentState);
        qCDebug(s_loggingCategory) << "entry action executed";
    }

    // Transition to the next state
    m_currentState = transitionData.state;
    qCDebug(s_loggingCategory) << "Transitioned to state:" << m_currentState;

    // Check if the state machine transitioned to a final state
    if (isFinalState(nextStateData))
    {
        // Store final event
        m_finalEvent = std::make_unique<Event>(std::move(event));

        qCDebug(s_loggingCategory) << "Transitioned to a final state";
        stopInternal();
    }
}

// -------------------------------------------------------------------------------------------------

void StateMachine::executeInternalTransition(const InternalTransitionData &transitionData,
                                             const Event &event)
{
    // Check if the transition is blocked by the guard condition
    if (transitionData.guard)
    {
        if (!transitionData.guard(event, m_currentState))
        {
            qCDebug(s_loggingCategory)
                    << QString("Internal transition of state [%1] with event [%2] was blocked by "
                               "the guard condition")
                       .arg(m_currentState, event.name());
            return;
        }
    }

    qCDebug(s_loggingCategory)
            << QString("Executing internal transition of state [%1] with event [%2]...")
               .arg(m_currentState, event.name());

    // Execute transition's action
    qCDebug(s_loggingCategory) << "Executing state transition's action...";
    transitionData.action(event, m_currentState);
    qCDebug(s_loggingCategory) << "State transition's action executed";

    qCDebug(s_loggingCategory) << "Transition finished";
}

} // namespace CppStateMachineFramework
