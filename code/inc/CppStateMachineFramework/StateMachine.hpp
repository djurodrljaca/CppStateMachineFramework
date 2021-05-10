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

// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>

// System includes
#include <deque>

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

namespace CppStateMachineFramework
{

// TODO: add IState interface and addStateObject() as convenience method for addState()

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
     * Type alias for a state entry method
     *
     * \param   event       Event that triggered the transition to the state
     * \param   fromState   Name of the state to transition from
     * \param   toState     Name of the state to transition to
     */
    using StateEntryMethod = std::function<void(const Event &event,
                                                const QString &fromState,
                                                const QString &toState)>;

    /*!
     * Type alias for a state exit method
     *
     * \param   event   Event that triggered the transition from the state
     * \param   fromState   Name of the state to transition from
     * \param   toState     Name of the state to transition to
     */
    using StateExitMethod = std::function<void(const Event &event,
                                               const QString &fromState,
                                               const QString &toState)>;

    /*!
     * Type alias for a transition guard method
     *
     * \param   event       Event that triggered the transition
     * \param   fromState   Name of the state to transition from
     * \param   toState     Name of the state to transition to
     *
     * \retval  true    Transition is allowed
     * \retval  false   Transition is not allowed
     */
    using TransitionGuardMethod = std::function<bool(const Event &event,
                                                     const QString &fromState,
                                                     const QString &toState)>;

    /*!
     * Type alias for a transition action method
     *
     * \param   event       Event that triggered the transition
     * \param   fromState   Name of the state to transition from
     * \param   toState     Name of the state to transition to
     */
    using TransitionActionMethod = std::function<void(const Event &event,
                                                      const QString &fromState,
                                                      const QString &toState)>;

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
     * \retval  false   Failure (state machine not stopped, no states, no initial state, invalid
     *                  final states, unreachable states)
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
     * \param   event   Event that will be sent to the initial state
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started or invalid)
     */
    bool start(std::unique_ptr<Event> event = std::make_unique<Event>(QStringLiteral("Started")));

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
    bool finalStateReached();

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
     * Adds an event to the event queue
     *
     * \param   event   Event
     *
     * \retval  true    Success
     * \retval  false   Failure (null, empty event name, state machine not started)
     */
    bool addEvent(std::unique_ptr<Event> event);

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
     * \param   entryMethod State entry method
     * \param   exitMethod  State exit method
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started, empty or duplicate state name)
     */
    bool addState(const QString &stateName,
                  StateEntryMethod entryMethod = {},
                  StateExitMethod exitMethod = {});

    /*!
     * Gets the initial state of the state machine
     *
     * \return  State name
     */
    QString initialState() const;

    /*!
     * Sets an existing state as the initial state of the state machine
     *
     * \param   stateName   Name of an existing state
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started, already set, state does not exit)
     *
     * \note    If the state has an entry method it shall be executed before transitioning to the
     *          initial state with an "empty" event (event with no name)
     */
    bool setInitialState(const QString &stateName);

    /*!
     * Adds a new transition between states
     *
     * \param   fromState       Name of the state to transition from
     * \param   eventName       Name of the event that triggers the transition
     * \param   toState         Name of the state to transition to
     * \param   guardMethod     Optional transition guard method
     * \param   actionMethod    Optional transition action method
     *
     * \retval  true    Success
     * \retval  false   Failure (state machine already started, invalid state or event names,
     *                  duplicate transition)
     */
    bool addTransition(const QString &fromState,
                       const QString &eventName,
                       const QString &toState,
                       TransitionGuardMethod guardMethod = {},
                       TransitionActionMethod actionMethod = {});

private:
    //! Holds the transition data
    struct TransitionData
    {
        //! Holds the name of the state to transition to
        QString state;

        //! Holds an optional transition guard method
        TransitionGuardMethod guardMethod;

        //! Holds an optional transition action method
        TransitionActionMethod actionMethod;
    };

    //! Holds the state data
    struct StateData
    {
        //! Holds an optional state entry method
        StateEntryMethod entryMethod;

        //! Holds an optional state exit method
        StateExitMethod exitMethod;

        /*!
         * Holds the state's transitions. The key contains the name of the event that triggers the
         * transition and the value contains the transition data.
         */
        QHash<QString, TransitionData> transitions;
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
     * Transition the state machine to the initial state
     *
     * \param   event   Event
     *
     * \note    This method is only executed in start()
     */
    void transitionToInitalState(std::unique_ptr<Event> event);

    /*!
     * Executes the transition
     *
     * \param   transitionData  Transition data
     * \param   event           Event that triggered the transition
     */
    void executeTransition(const TransitionData &transitionData, std::unique_ptr<Event> event);

private:
    //! Holds all states in the state machine
    QHash<QString, StateData> m_states;

    /*!
     * Holds the name of the initial state of the state machine. The state machine will
     * automatically transition to this state when the state machine is started.
     */
    QString m_initialState;

    //! Holds the validation status
    ValidationStatus m_validationStatus;

    //! Holds the started flag
    bool m_started;

    //! Holds the name of the current state of the state machine
    QString m_currentState;

    //! Holds the queued events
    std::deque<std::unique_ptr<Event>> m_eventQueue;

    //! Holds the event which triggered the transition to the final state
    std::unique_ptr<Event> m_finalEvent;

    //! Holds the mutex used to make the API thread safe
    mutable QMutex m_mutex;
};

} // namespace CppStateMachineFramework
