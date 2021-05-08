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
#include <QtCore/QSet>

// System includes
#include <deque>

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
     * Type alias for a state machine entry method
     */
    using StateMachineEntryMethod = std::function<void()>;

    /*!
     * Type alias for a state machine exit method
     */
    using StateMachineExitMethod = std::function<void()>;

    /*!
     * Type alias for a state entry method
     *
     * \param   event   Event that triggered the transition to the state
     */
    using StateEntryMethod = std::function<void(const Event &event)>;

    /*!
     * Type alias for a state exit method
     *
     * \param   event   Event that triggered the transition from the state
     */
    using StateExitMethod = std::function<void(const Event &event)>;

    /*!
     * Type alias for a transition guard method
     *
     * \param   event   Event that triggered the transition
     *
     * \retval  true    Transition is allowed
     * \retval  false   Transition is not allowed
     */
    using TransitionGuardMethod = std::function<bool(const Event &event)>;

    /*!
     * Type alias for a transition action method
     *
     * \param   event   Event that triggered the transition
     */
    using TransitionActionMethod = std::function<void(const Event &event)>;

public:
    //! Constructor
    StateMachine();

    //! Copy constructor
    StateMachine(const StateMachine &) = default;

    //! Move constructor
    StateMachine(StateMachine &&) noexcept = default;

    //! Destructor
    ~StateMachine() = default;

    //! Copy assignment operator
    StateMachine &operator=(const StateMachine &) = default;

    //! Move assignment operator
    StateMachine &operator=(StateMachine &&) noexcept = default;

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
     * \retval  false   Failure
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
     * \retval  true    Success
     * \retval  false   Failure
     */
    bool start();

    /*!
     * Start the state machine
     *
     * \retval  true    Success
     * \retval  false   Failure
     */
    bool stop();

    /*!
     * Gets the name of the current state of the state machine
     *
     * \return  State name
     */
    QString currentState() const;

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
     * \retval  false   Failure
     */
    bool addEvent(std::unique_ptr<Event> event);

    /*!
     * Processes the next pending event
     *
     * \retval  true    Success
     * \retval  false   Failure
     */
    bool processNextEvent();

    /*!
     * Gets the initial state of the state machine
     *
     * \return  State name
     */
    QString initialState() const;

    /*!
     * Sets an existing state as the initial state of the state machine and optionally sets the
     * state machine entry method that is executed before transitioning to the initial state
     *
     * \param   stateName               Name of an existing state
     * \param   stateMachineEntryMethod Optional state machine entry method
     *
     * \retval  true    Success
     * \retval  false   Failure
     */
    bool setInitialState(const QString &stateName,
                         StateMachineEntryMethod stateMachineEntryMethod = {});

    /*!
     * Gets the initial state of the state machine
     *
     * \return  State name
     */
    QString finalState() const;

    /*!
     * Adds a new state without state to be used as the final state of the state machine and
     * optionally sets the state machine exit method that is executed after transitioning to the
     * final state
     *
     * \param   stateName               Name of an state
     * \param   stateMachineExitMethod  Optional state machine exit method
     *
     * \retval  true    Success
     * \retval  false   Failure
     *
     * \note    Final state is optional, without it the state machine shall never terminate except
     *          by stopping the state machine explicitly with stop()
     */
    bool setFinalState(const QString &stateName,
                       StateMachineExitMethod stateMachineExitMethod = {});

    /*!
     * Adds a new state to the state machine
     *
     * \param   stateName           State name
     * \param   stateEntryMethod    State entry method
     * \param   stateExitMethod     State exit method
     *
     * \retval  true    Success
     * \retval  false   Failure (invalid operational state, empty or duplicate state name)
     */
    bool addState(const QString &stateName,
                  StateEntryMethod stateEntryMethod = {},
                  StateExitMethod stateExitMethod = {});

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
     * \retval  false   Failure (invalid operational state, invalid state or event names, duplicate
     *                  transition, transition from the final state)
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
     * Traverses from the specified state to all possible states from the configured transitions
     *
     * \param   stateName   Name of the state where the traversal will be started
     *
     * \param[in,out]   statesReached   Container for recording all the reached states
     */
    void traverseStates(const QString &stateName, QSet<QString> *statesReached) const;

    /*!
     * Executes the transition
     *
     * \param   transitionData  Transition data
     * \param   event           Event that triggered the transition
     */
    void executeTransition(const TransitionData &transitionData, const Event &event);

private:
    //! Holds all states in the state machine
    QHash<QString, StateData> m_states;

    /*!
     * Holds the name of the initial state of the state machine. The state machine will
     * automatically transition to this state when the state machine is started.
     */
    QString m_initialState;

    /*!
     * Holds the state machine entry method that is executed before transitioning to the initial
     * state
     */
    StateMachineEntryMethod m_stateMachineEntryMethod;

    /*!
     * Holds the name of the final state of the state machine. This is a pseudo state and it exist
     * primarily to have a target to transition to when the state machine gets stopped through an
     * event.
     *
     * \note    As this is not a proper state it is not allowed to have any transitions from it to
     *          another state!
     */
    QString m_finalState;

    //! Holds the state machine exit method that is executed after transitioning to the final state
    StateMachineExitMethod m_stateMachineExitMethod;

    //! Holds the validation status
    ValidationStatus m_validationStatus;

    //! Holds the started flag
    bool m_started;

    //! Holds the name of the current state of the state machine
    QString m_currentState;

    //! Holds the queued events
    std::deque<std::unique_ptr<Event>> m_eventQueue;
};

} // namespace CppStateMachineFramework
