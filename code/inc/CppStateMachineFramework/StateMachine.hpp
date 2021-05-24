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

#pragma once

// C++ State Machine Framework includes
#include <CppStateMachineFramework/Event.hpp>
#include <CppStateMachineFramework/HashFunctions.hpp>

// Qt includes
#include <QtCore/QMutex>

// System includes
#include <deque>
#include <functional>
#include <unordered_map>

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

namespace CppStateMachineFramework
{

//! This class holds the state machine
class CPPSTATEMACHINEFRAMEWORK_EXPORT StateMachine
{
public:
    //! Enumerates the validation states
    enum class ValidationStatus
    {
        //! Validation was not executed or the state machine was changed after last validation
        Unvalidated,

        //! State machine is valid (validation was successful)
        Valid,

        //! State machine is not valid (validation failed)
        Invalid
    };

    /*!
     * Type alias for a state entry action method
     *
     * \param   trigger         Event that triggered the transition to the state
     * \param   currentState    Name of the current state
     * \param   previousState   Name of the previous state
     */
    using StateEntryAction = std::function<void(const Event &trigger,
                                                const QString &currentState,
                                                const QString &previousState)>;

    /*!
     * Type alias for a state exit action method
     *
     * \param   trigger         Event that triggered the transition from the state
     * \param   currentState    Name of the current state
     * \param   nextState       Name of the next state
     */
    using StateExitAction = std::function<void(const Event &trigger,
                                               const QString &currentState,
                                               const QString &nextState)>;

    /*!
     * Type alias for a state transition guard condition method
     *
     * \param   trigger         Event that triggered the transition
     * \param   currentState    Name of the current state
     * \param   nextState       Name of the next state
     *
     * \retval  true    Transition is allowed
     * \retval  false   Transition is not allowed
     */
    using StateTransitionGuardCondition = std::function<bool(const Event &trigger,
                                                             const QString &currentState,
                                                             const QString &nextState)>;

    /*!
     * Type alias for a state transition action method
     *
     * \param   trigger     Event that triggered the transition
     * \param   currentState    Name of the current state
     * \param   nextState       Name of the next state
     */
    using StateTransitionAction = std::function<void(const Event &trigger,
                                                     const QString &currentState,
                                                     const QString &nextState)>;

    /*!
     * Type alias for an internal transition guard condition method
     *
     * \param   trigger         Event that triggered the transition
     * \param   currentState    Name of the current state
     *
     * \retval  true    Transition is allowed
     * \retval  false   Transition is not allowed
     */
    using InternalTransitionGuardCondition = std::function<bool(const Event &trigger,
                                                                const QString &currentState)>;

    /*!
     * Type alias for an internal transition action method
     *
     * \param   trigger         Event that triggered the transition
     * \param   currentState    Name of the current state
     */
    using InternalTransitionAction = std::function<void(const Event &trigger,
                                                        const QString &currentState)>;

    /*!
     * Type alias for an initial transition action method
     *
     * \param   trigger         Event that triggered the transition
     * \param   initialState    Name of the initial state
     */
    using InitialTransitionAction = std::function<void(const Event &trigger,
                                                       const QString &initialState)>;

public:
    //! Constructor
    StateMachine();

    //! Copy constructor is disabled
    StateMachine(const StateMachine &) = delete;

    //! Move constructor
    StateMachine(StateMachine &&other) noexcept;

    //! Destructor
    ~StateMachine() = default;

    //! Copy assignment operator is disabled
    StateMachine &operator=(const StateMachine &) = delete;

    //! Move assignment operator
    StateMachine &operator=(StateMachine &&other) noexcept;

    /*!
     * Gets the validation status of the state machine
     *
     * \return  Validation status
     */
    ValidationStatus validationStatus() const;

    /*!
     * Validates the state and state transitions
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine not stopped, no states, no initial transition,
     *                  invalid final states, unreachable states)
     */
    bool validate();

    /*!
     * Checks if the state machine is started
     *
     * \retval  true    Started
     * \retval  false   Not started
     */
    bool isStarted();

    /*!
     * Start the state machine
     *
     * \param   event   Startup event to use in the initial transition
     *
     * \retval  true    Success
     * \retval  false   Failure (invalid startup event, state machine already started or invalid)
     */
    bool start(Event &&event);

    /*!
     * Start the state machine
     *
     * \param   eventName       Name of the startup event to use in the initial transition
     * \param   eventParameter  Event parameter for the startup event
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started or invalid)
     */
    inline bool start(const QString &eventName = QStringLiteral("Started"),
                      std::unique_ptr<IEventParameter> &&eventParameter = {})
    {
        return start(Event(eventName, std::move(eventParameter)));
    }

    /*!
     * Start the state machine
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already stopped)
     */
    bool stop();

    /*!
     * Gets the name of the current state of the state machine
     *
     * \return  State name
     */
    QString currentState() const;

    /*!
     * Checks if the state machine has reached a final state (current state is set to a final state)
     *
     * \retval  true    Final state was reached
     * \retval  false   Final state not reached
     */
    bool finalStateReached() const;

    /*!
     * Checks if the state machine has a final event (event used in the transition to a final state)
     *
     * \retval  true    State machine has a final event
     * \retval  false   State machine does not have a final event
     */
    bool hasFinalEvent() const;

    /*!
     * Takes the event which triggered the transition to the final state
     *
     * \return  Event or nullptr if final state was not reached
     */
    std::unique_ptr<Event> takeFinalEvent();

    /*!
     * Checks if the state machine has any pending events
     *
     * \retval  true    State machine has at least one pending event
     * \retval  false   State machine has no pending events
     */
    bool hasPendingEvents() const;

    /*!
     * Adds an event to the front of the event queue
     *
     * \param   event   Event
     *
     * \retval  true    Success
     * \retval  false   Failure (empty event name, state machine not started)
     */
    bool addEventToFront(Event &&event);

    /*!
     * Adds an event to the front of the event queue
     *
     * \param   eventName       Event name
     * \param   eventParameter  Event parameter
     *
     * \retval  true    Success
     * \retval  false   Failure (empty event name, state machine not started)
     */
    inline bool addEventToFront(const QString &eventName,
                                std::unique_ptr<IEventParameter> &&eventParameter = {})
    {
        return addEventToFront(Event(eventName, std::move(eventParameter)));
    }

    /*!
     * Adds an event to the back of the event queue
     *
     * \param   event   Event
     *
     * \retval  true    Success
     * \retval  false   Failure (empty event name, state machine not started)
     */
    bool addEventToBack(Event &&event);

    /*!
     * Adds an event to the back of the event queue
     *
     * \param   eventName       Event name
     * \param   eventParameter  Event parameter
     *
     * \retval  true    Success
     * \retval  false   Failure (empty event name, state machine not started)
     */
    inline bool addEventToBack(const QString &eventName,
                               std::unique_ptr<IEventParameter> &&eventParameter = {})
    {
        return addEventToBack(Event(eventName, std::move(eventParameter)));
    }

    /*!
     * Processes the next pending event
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine not started, empty event queue)
     */
    bool processNextEvent();

    /*!
     * Adds a new state to the state machine
     *
     * \param   stateName   State name
     * \param   entryAction State entry action method
     * \param   exitAction  State exit action method
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started, empty state name or duplicate state)
     */
    bool addState(const QString &stateName,
                  StateEntryAction entryAction = {},
                  StateExitAction exitAction = {});

    /*!
     * Gets the initial state of the state machine
     *
     * \return  State name
     */
    QString initialState() const;

    /*!
     * Sets an existing state as the initial state of the state machine
     *
     * \param   initialState    Name of an existing state to use as the initial state
     * \param   action          Optional transition action method
     *
     * \retval  true    Success
     * \retval  false   Failure (already set, state does not exit)
     *
     * The initial transition is executed at startup. First the initial transition's action method
     * (if available) is executed followed by the initial state's entry action (if available).
     * After the transition is finished the state machine is in the initial state and processing of
     * queued events can start.
     */
    bool setInitialTransition(const QString &initialState, InitialTransitionAction action = {});

    /*!
     * Adds a new state transition
     *
     * \param   fromState   Name of the state to transition from
     * \param   trigger     Name of the event that triggers the transition
     * \param   toState     Name of the state to transition to
     * \param   action      Optional state transition action method
     * \param   guard       Optional state transition guard condition method
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started, invalid state or event names,
     *                  duplicate transition)
     *
     * A state transition is the only way for the state machine to change the state machine state
     * from one state to another and it is triggered by the specified event.
     */
    bool addStateTransition(const QString &fromState,
                            const QString &trigger,
                            const QString &toState,
                            StateTransitionAction action = {},
                            StateTransitionGuardCondition guard = {});

    /*!
     * Adds a new internal transition
     *
     * \param   state   Name of the state to which the transition belongs to
     * \param   trigger Name of the event that triggers the transition
     * \param   action  Internal transition action method
     * \param   guard   Optional internal transition guard condition method
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started, invalid state or event names,
     *                  missing action, duplicate transition)
     *
     * An internal transition is the only way to execute an action, triggered by the specified
     * event, that does not result in the change of the state machine state.
     *
     * A similar effect can be achieved by creating a self-transition (transition from one state to
     * itself) but in this case the state's exit and entry actions would be executed.
     */
    bool addInternalTransition(const QString &state,
                               const QString &trigger,
                               InternalTransitionAction action = {},
                               InternalTransitionGuardCondition guard = {});

    /*!
     * Sets the default state transition
     *
     * \param   fromState   Name of the state to transition from
     * \param   toState     Name of the state to transition to
     * \param   action      Optional state transition action method
     * \param   guard       Optional state transition guard condition method
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started, invalid state or event names,
     *                  already set)
     *
     * A default state transition will be executed if an event does not trigger any of the
     * configured state nor internal transitions.
     *
     * \note    There can be at most one default transition, either state or internal transition.
     */
    bool setDefaultTransition(const QString &fromState,
                              const QString &toState,
                              StateTransitionAction action = {},
                              StateTransitionGuardCondition guard = {});

    /*!
     * Sets the default internal transition
     *
     * \param   state   Name of the state to which the transition belongs to
     * \param   action  Internal transition action method
     * \param   guard   Optional internal transition guard condition method
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started, invalid state name, missing action,
     *                  already set)
     *
     * A default state transition will be executed if an event does not trigger any of the
     * configured state nor internal transitions.
     *
     * \note    There can be at most one default transition, either state or internal transition.
     */
    bool setDefaultTransition(const QString &state,
                              InternalTransitionAction action,
                              InternalTransitionGuardCondition guard = {});

private:
    //! Holds the initial transition data
    struct InitialTransitionData
    {
        //! Holds the name of the initial state
        QString state;

        //! Holds an initial transition action method
        InitialTransitionAction action;
    };

    //! Holds the state transition data
    struct StateTransitionData
    {
        //! Holds the name of the state to transition to
        QString state;

        //! Holds an optional state transition guard method
        StateTransitionGuardCondition guard;

        //! Holds an optional state transition action method
        StateTransitionAction action;
    };

    //! Holds the internal transition data
    struct InternalTransitionData
    {
        //! Holds an optional internal transition guard method
        InternalTransitionGuardCondition guard;

        //! Holds an internal transition action method
        InternalTransitionAction action;
    };

    //! Holds the state data
    struct StateData
    {
        //! Holds an optional state entry action method
        StateEntryAction entryAction;

        //! Holds an optional state exit action method
        StateExitAction exitAction;

        /*!
         * Holds the state's state transitions. The key contains the name of the event that triggers
         * the transition and the value contains the state transition data.
         */
        std::unordered_map<QString, StateTransitionData> stateTransitions;

        /*!
         * Holds the state's internal transitions. The key contains the name of the event that
         * triggers the transition and the value contains the state transition data.
         */
        std::unordered_map<QString, InternalTransitionData> internalTransitions;

        //! Holds the state's optional default state transitions
        std::unique_ptr<StateTransitionData> defaultStateTransition;

        //! Holds the state's optional default internal transitions
        std::unique_ptr<InternalTransitionData> defaultInternalTransition;
    };

private:
    /*!
     * Stops the state machine
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already stopped)
     *
     * \note    The only difference between stopInternal() and stop() is that stopInternal() assumes
     *          that the mutex is already locked and stop() locks the mutex. This method is needed
     *          just to be able to stop the state machine after the mutex was locked.
     */
    bool stopInternal();

    /*!
     * Traverses from the specified state to all possible states from the configured transitions
     *
     * \param   stateName   Name of the state where the traversal will be started
     *
     * \param[in,out]   statesReached   Container for recording all the reached states
     */
    void traverseStates(const QString &stateName, QSet<QString> *statesReached) const;

    /*!
     * Checks if the specified state is a final state
     *
     * \param   stateData   State data
     *
     * \retval  true    Specified state is a final state
     * \retval  false   Specified state is not a final state
     */
    bool isFinalState(const StateData &stateData) const;

    /*!
     * Executes the initial transition
     *
     * \param   event   Event that triggered the transition
     */
    void executeInitialTransition(Event &&event);

    /*!
     * Executes the state transition
     *
     * \param   transitionData  Transition data
     * \param   event           Event that triggered the transition
     */
    void executeStateTransition(const StateTransitionData &transitionData, Event &&event);

    /*!
     * Executes the internal transition
     *
     * \param   transitionData  Transition data
     * \param   event           Event that triggered the transition
     */
    void executeInternalTransition(const InternalTransitionData &transitionData,
                                   const Event &event);

private:
    //! Holds all states in the state machine
    std::unordered_map<QString, StateData> m_states;

    //! Holds the initial transition of the state machine
    InitialTransitionData m_initialTransition;

    //! Holds the validation status
    ValidationStatus m_validationStatus;

    //! Holds the started flag
    bool m_started;

    //! Holds the name of the current state of the state machine
    QString m_currentState;

    //! Holds the queued events
    std::deque<Event> m_eventQueue;

    //! Holds the event which triggered the transition to the final state
    std::unique_ptr<Event> m_finalEvent;

    //! Holds the mutex used to make access to the started flag thread safe
    mutable QMutex m_startedMutex;

    //! Holds the mutex used to make access to the event queue thread safe
    mutable QMutex m_eventQueueMutex;

    //! Holds the mutex used to make the API thread safe
    mutable QMutex m_apiMutex;
};

} // namespace CppStateMachineFramework
