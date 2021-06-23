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
 * Contains unit tests for the StateMachine class
 */

// C++ State Machine Framework includes
#include <CppStateMachineFramework/StateMachine.hpp>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtTest/QTest>

// System includes

// Forward declarations

// Macros

// Test class declaration --------------------------------------------------------------------------

using namespace CppStateMachineFramework;

class TestStateMachine : public QObject
{
    Q_OBJECT

public:
    TestStateMachine();

private slots:
    // Functions executed by QtTest before and after test suite
    void initTestCase();
    void cleanupTestCase();

    // Functions executed by QtTest before and after each test
    void init();
    void cleanup();

    // Test functions
    void testConstructors();
    void testMoveAssignmentOperator();
    void testValidate();
    void testStartStop();
    void testAddState();
    void testSetStateEntryAction();
    void testSetStateAction();
    void testSetStateExitAction();
    void testSetInitialTransition();
    void testAddStateTransition();
    void testAddInternalTransition();
    void testAddDefaultStateTransition();
    void testAddDefaultInternalTransition();
    void testAddEventToFront();
    void testAddEventToBack();
    void testProcessNextEvent();
    void testPoll();
    void testStateAndTransitionMethods();
    void testStateMachineWithLoop();
    void testAddEventFromAction();
    void testCreateClassMethodsFull();
    void testCreateClassMethodsVoid();

private:
    const StateEntryAction m_dummyStateEntryAction;
    const StateAction m_dummyStateAction;
    const StateExitAction m_dummyStateExitAction;
    const StateTransitionGuardCondition m_dummyStateTransitionGuard;
    const StateTransitionAction m_dummyStateTransitionAction;
    const InternalTransitionGuardCondition m_dummyInternalTransitionGuard;
    const InternalTransitionAction m_dummyInternalTransitionAction;
};

class LogHelper
{
public:
    void initialTransitionActionFull(const Event &trigger, const QString &initialState)
    {
        m_log.append(QString("initialTransitionActionFull: [%1], [%2]").arg(trigger.name(),
                                                                            initialState));
    }

    void initialTransitionActionVoid()
    {
        m_log.append("initialTransitionActionVoid");
    }

    void stateEntryActionFull(const Event &trigger,
                              const QString &currentState,
                              const QString &previousState)
    {
        m_log.append(QString("stateEntryActionFull: [%1], [%2], [%3]").arg(trigger.name(),
                                                                           currentState,
                                                                           previousState));
    }

    void stateEntryActionVoid()
    {
        m_log.append("stateEntryActionVoid");
    }

    void stateActionFull(const QString &currentState)
    {
        m_log.append(QString("stateActionFull: [%1]").arg(currentState));
    }

    void stateActionVoid()
    {
        m_log.append("stateActionVoid");
    }

    void stateExitActionFull(const Event &trigger,
                              const QString &currentState,
                              const QString &nextState)
    {
        m_log.append(QString("stateExitActionFull: [%1], [%2], [%3]").arg(trigger.name(),
                                                                          currentState,
                                                                          nextState));
    }

    void stateExitActionVoid()
    {
        m_log.append("stateExitActionVoid");
    }

    bool stateTransitionGuardConditionFull(const Event &trigger,
                                           const QString &currentState,
                                           const QString &nextState)
    {
        m_log.append(QString("stateTransitionGuardConditionFull: [%1], [%2], [%3]")
                     .arg(trigger.name(), currentState, nextState));
        return true;
    }

    bool stateTransitionGuardConditionVoid()
    {
        m_log.append("stateTransitionGuardConditionVoid");
        return true;
    }

    void stateTransitionActionFull(const Event &trigger,
                                   const QString &currentState,
                                   const QString &nextState)
    {
        m_log.append(QString("stateTransitionActionFull: [%1], [%2], [%3]").arg(trigger.name(),
                                                                                currentState,
                                                                                nextState));
    }

    void stateTransitionActionVoid()
    {
        m_log.append("stateTransitionActionVoid");
    }

    bool internalTransitionGuardConditionFull(const Event &trigger, const QString &currentState)
    {
        m_log.append(QString("internalTransitionGuardConditionFull: [%1], [%2]").arg(trigger.name(),
                                                                                     currentState));
        return true;
    }

    bool internalTransitionGuardConditionVoid()
    {
        m_log.append("internalTransitionGuardConditionVoid");
        return true;
    }

    void internalTransitionActionFull(const Event &trigger, const QString &currentState)
    {
        m_log.append(QString("internalTransitionActionFull: [%1], [%2]").arg(trigger.name(),
                                                                             currentState));
    }

    void internalTransitionActionVoid()
    {
        m_log.append("internalTransitionActionVoid");
    }

    const QStringList &log() const
    {
        return m_log;
    }

private:
    QStringList m_log;
};

// Constructor -------------------------------------------------------------------------------------

TestStateMachine::TestStateMachine()
    : m_dummyStateEntryAction([](auto &, auto &, auto &) {}),
      m_dummyStateAction([](auto &) {}),
      m_dummyStateExitAction([](auto &, auto &, auto &) {}),
      m_dummyStateTransitionGuard([](auto &, auto &, auto &) { return true; }),
      m_dummyStateTransitionAction([](auto &, auto &, auto &) {}),
      m_dummyInternalTransitionGuard([](auto &, auto &) { return true; }),
      m_dummyInternalTransitionAction([](auto &, auto &) {})
{
}

// Test Case init/cleanup methods ------------------------------------------------------------------

void TestStateMachine::initTestCase()
{
    QLoggingCategory::setFilterRules("*.debug=true");
}

void TestStateMachine::cleanupTestCase()
{
}

// Test init/cleanup methods -----------------------------------------------------------------------

void TestStateMachine::init()
{
}

void TestStateMachine::cleanup()
{
}

// Test: Constructors ------------------------------------------------------------------------------

void TestStateMachine::testConstructors()
{
    // Default constructor
    StateMachine stateMachine;
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(stateMachine.currentState().isEmpty());
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.initialState().isEmpty());

    // Initialize, validate, and start the state machine
    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));

    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    // Transition to state "b" and add the event to transition to the final state
    QVERIFY(stateMachine.addEventToBack("a_to_b"));
    QVERIFY(stateMachine.processNextEvent());
    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.addEventToBack("b_to_c"));
    QVERIFY(stateMachine.hasPendingEvents());

    // Now create a new instance from the original state machine and check if it has the same state
    StateMachine movedStateMachine(std::move(stateMachine));

    QCOMPARE(movedStateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);
    QVERIFY(movedStateMachine.isStarted());
    QCOMPARE(movedStateMachine.currentState(), QString("b"));
    QVERIFY(!movedStateMachine.finalStateReached());
    QVERIFY(!movedStateMachine.takeFinalEvent());
    QVERIFY(movedStateMachine.hasPendingEvents());
    QCOMPARE(movedStateMachine.initialState(), QString("a"));

    // Processing the pending event must transition the state machine to its final state
    QVERIFY(movedStateMachine.processNextEvent());
    QCOMPARE(movedStateMachine.currentState(), QString("c"));
    QVERIFY(movedStateMachine.finalStateReached());
    QVERIFY(!movedStateMachine.isStarted());

    QVERIFY(movedStateMachine.hasFinalEvent());
    auto event = movedStateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), QString("b_to_c"));
}

// Test: Move assignment operator ------------------------------------------------------------------

void TestStateMachine::testMoveAssignmentOperator()
{
    // Initialize, validate, and start the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));

    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    // Transition to state "b" and add the event to transition to the final state
    QVERIFY(stateMachine.addEventToBack("a_to_b"));
    QVERIFY(stateMachine.processNextEvent());
    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.addEventToBack("b_to_c"));
    QVERIFY(stateMachine.hasPendingEvents());

    // Now create a new instance from the original state machine and check if it has the same state
    StateMachine movedStateMachine;
    movedStateMachine = std::move(stateMachine);

    QCOMPARE(movedStateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);
    QVERIFY(movedStateMachine.isStarted());
    QCOMPARE(movedStateMachine.currentState(), QString("b"));
    QVERIFY(!movedStateMachine.finalStateReached());
    QVERIFY(!movedStateMachine.takeFinalEvent());
    QVERIFY(movedStateMachine.hasPendingEvents());
    QCOMPARE(movedStateMachine.initialState(), QString("a"));

    // Processing the pending event must transition the state machine to its final state
    QVERIFY(movedStateMachine.processNextEvent());
    QCOMPARE(movedStateMachine.currentState(), QString("c"));
    QVERIFY(movedStateMachine.finalStateReached());
    QVERIFY(!movedStateMachine.isStarted());

    QVERIFY(movedStateMachine.hasFinalEvent());
    auto event = movedStateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), QString("b_to_c"));
}

// Test: validate() --------------------------------------------------------------------------------

void TestStateMachine::testValidate()
{
    StateMachine stateMachine;

    // Initially the state machine must be "unvalidated"
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    // Validation of an "empty" state machine must fail
    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);

    // Adding a state must put the state machine to "unvalidated" state
    QVERIFY(stateMachine.addState("init"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);

    // With initial state set the state machine must now finally be valid
    QVERIFY(stateMachine.setInitialTransition("init"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);
    QCOMPARE(stateMachine.initialState(), QString("init"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Detect unreachable states
    QVERIFY(stateMachine.addState("a"));
    QCOMPARE(stateMachine.initialState(), QString("init"));

    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);

    // Add transition to the unreachable state to make the state machine valid
    QVERIFY(stateMachine.addStateTransition("init", "to_a", "a"));
    QCOMPARE(stateMachine.initialState(), QString("init"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Add a new unreachable state with a transition
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addStateTransition("b", "to_a", "a"));

    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);

    // It must be possible to start an invalid state machine
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(!stateMachine.start());
    QVERIFY(!stateMachine.isStarted());

    // Now make the new state reachable
    QVERIFY(stateMachine.addStateTransition("init", "to_b", "b"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // It must be possible to start a valid state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // It must not be possible to execute the validation procedure or modify the state machine
    // configuration when the state machine is started but this must not invalidate the state
    // machine
    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.addState("c"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.setStateEntryAction("a", m_dummyStateEntryAction));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.setStateAction("a", m_dummyStateAction));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.setStateExitAction("a", m_dummyStateExitAction));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.addStateTransition("a", "to_b", "b"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Stop the state machine and check if validation still works
    QVERIFY(stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Add an invalid final state (no transitions from it to other states and an exit method)
    QVERIFY(stateMachine.addState("c"));
    QVERIFY(stateMachine.addStateTransition("b", "to_c", "c"));
    QVERIFY(stateMachine.setStateExitAction("c", m_dummyStateExitAction));

    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);

    // Change "c" to a non-final state
    QVERIFY(stateMachine.addStateTransition("c", "to_a", "a"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Add an invalid final state (no transitions from it to other states and a state method)
    QVERIFY(stateMachine.addState("d"));
    QVERIFY(stateMachine.addStateTransition("b", "to_d", "d"));
    QVERIFY(stateMachine.setStateAction("d", m_dummyStateAction));

    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);
}

// Test: start() and stop() ------------------------------------------------------------------------

void TestStateMachine::testStartStop()
{
    // Create a state machine with a single state
    StateMachine stateMachine;
    QVERIFY(stateMachine.addState("init"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    // It must not be possible to start an unvalidated state machine
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(!stateMachine.start());
    QVERIFY(!stateMachine.isStarted());

    // It must not be possible to start an unvalidated state machine
    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);

    // It must not be possible to start an invalid state machine
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(!stateMachine.start());
    QVERIFY(!stateMachine.isStarted());

    // With initial state set the state machine must now finally be valid and can be stated
    QVERIFY(stateMachine.setInitialTransition("init"));
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Try to start the state machine with empty event and with an event that has an empty name
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(!stateMachine.start({}));
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(!stateMachine.start(QString()));
    QVERIFY(!stateMachine.isStarted());

    // Start the state machine (with the default initialization event)
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(stateMachine.start());

    // Since the state machine has a single state that is both the initial and final state the state
    // machine must stop immediately
    QVERIFY(!stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("init"));
    QVERIFY(stateMachine.finalStateReached());

    auto event = stateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), QString("Started"));

    // Stopping the state machine must fail if it is already stopped but it must not have any effect
    // on the state machine
    QVERIFY(!stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());

    // Extend the state machine with another state and start it again
    QVERIFY(stateMachine.addState("final"));
    QVERIFY(stateMachine.addStateTransition("init", "event", "final"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.isStarted());
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("init"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    // Starting the state machine must fail if it is already started but it must not have any effect
    // on the state machine
    QVERIFY(stateMachine.isStarted());
    QVERIFY(!stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Stop the state machine while it is still active
    QVERIFY(stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("init"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    // Stopping the state machine must fail if it is already stopped but it must not have any effect
    // on the state machine
    QVERIFY(!stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("init"));
    QVERIFY(!stateMachine.finalStateReached());
}

// Test: addState() --------------------------------------------------------------------------------

void TestStateMachine::testAddState()
{
    StateMachine stateMachine;

    // Add a state
    QVERIFY(stateMachine.addState("a"));

    // Add a state with duplicate name
    QVERIFY(!stateMachine.addState("a"));

    // Add a state with an empty name
    QVERIFY(!stateMachine.addState(""));

    // Add a few more valid states
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));
    QVERIFY(stateMachine.addState("d"));

    // Set the initial state and add transitions to make the state machine valid then start it
    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("a", "to_c", "c"));
    QVERIFY(stateMachine.addStateTransition("a", "to_d", "d"));

    QVERIFY(stateMachine.addStateTransition("c", "to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("d", "to_b", "b"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Add a state while state machine is started
    QVERIFY(!stateMachine.addState("e"));
}

// Test: setStateEntryAction() ---------------------------------------------------------------------

void TestStateMachine::testSetStateEntryAction()
{
    StateMachine stateMachine;

    // Add a state
    QVERIFY(stateMachine.addState("a"));

    // Set an empty entry action
    QVERIFY(!stateMachine.setStateEntryAction("a", {}));

    // Set a valid entry action
    QVERIFY(stateMachine.setStateEntryAction("a", m_dummyStateEntryAction));

    // Set another valid entry action
    QVERIFY(!stateMachine.setStateEntryAction("a", m_dummyStateEntryAction));

    // Set a valid entry action to a state that doesn't exist
    QVERIFY(!stateMachine.setStateEntryAction("b", m_dummyStateEntryAction));
}

// Test: setStateAction() --------------------------------------------------------------------------

void TestStateMachine::testSetStateAction()
{
    StateMachine stateMachine;

    // Add a state
    QVERIFY(stateMachine.addState("a"));

    // Set an empty state action
    QVERIFY(!stateMachine.setStateAction("a", {}));

    // Set a valid state action
    QVERIFY(stateMachine.setStateAction("a", m_dummyStateAction));

    // Set another valid state action
    QVERIFY(!stateMachine.setStateAction("a", m_dummyStateAction));

    // Set a valid state action to a state that doesn't exist
    QVERIFY(!stateMachine.setStateAction("b", m_dummyStateAction));
}

// Test: setStateExitAction() ----------------------------------------------------------------------

void TestStateMachine::testSetStateExitAction()
{
    StateMachine stateMachine;

    // Add a state
    QVERIFY(stateMachine.addState("a"));

    // Set an empty exit action
    QVERIFY(!stateMachine.setStateExitAction("a", {}));

    // Set a valid exit action
    QVERIFY(stateMachine.setStateExitAction("a", m_dummyStateExitAction));

    // Set another valid exit action
    QVERIFY(!stateMachine.setStateExitAction("a", m_dummyStateExitAction));

    // Set a valid exit action to a state that doesn't exist
    QVERIFY(!stateMachine.setStateExitAction("b", m_dummyStateExitAction));
}

// Test: setInitialState() -------------------------------------------------------------------------

void TestStateMachine::testSetInitialTransition()
{
    StateMachine stateMachine;

    // Add states
    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.initialState().isEmpty());

    // Set an initial state that with an empty name
    QVERIFY(!stateMachine.setInitialTransition(""));
    QVERIFY(stateMachine.initialState().isEmpty());

    // Set an initial state that doesn't exists
    QVERIFY(!stateMachine.setInitialTransition("x"));
    QVERIFY(stateMachine.initialState().isEmpty());

    // Set a valid initial state
    QVERIFY(stateMachine.setInitialTransition("a"));
    QCOMPARE(stateMachine.initialState(), QString("a"));

    // Set the same initial state again
    QVERIFY(!stateMachine.setInitialTransition("a"));
    QCOMPARE(stateMachine.initialState(), QString("a"));

    // Set the another initial state
    QVERIFY(!stateMachine.setInitialTransition("b"));
    QCOMPARE(stateMachine.initialState(), QString("a"));
}

// Test: addStateTransition() ----------------------------------------------------------------------

void TestStateMachine::testAddStateTransition()
{
    StateMachine stateMachine;

    // Add states
    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));

    // Add transitions with invalid state and event names
    QVERIFY(!stateMachine.addStateTransition("",  "event", "c"));
    QVERIFY(!stateMachine.addStateTransition("d", "event", "c"));
    QVERIFY(!stateMachine.addStateTransition("a", "",      "c"));
    QVERIFY(!stateMachine.addStateTransition("a", "event", ""));
    QVERIFY(!stateMachine.addStateTransition("a", "event", "d"));

    // Add transitions with and without guard and action methods
    QVERIFY(stateMachine.addStateTransition("a", "event1", "b"));
    QVERIFY(stateMachine.addStateTransition("a", "event2", "b", {}, m_dummyStateTransitionGuard));
    QVERIFY(stateMachine.addStateTransition("a", "event3", "b", m_dummyStateTransitionAction));
    QVERIFY(stateMachine.addStateTransition("a", "event4", "b",
                                            m_dummyStateTransitionAction,
                                            m_dummyStateTransitionGuard));

    // Add duplicate transition
    QVERIFY(!stateMachine.addStateTransition("a", "event1", "b"));
    QVERIFY(!stateMachine.addStateTransition("a", "event1", "c"));

    // Set the initial transition and add transitions to make the state machine valid then start it
    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("b", "to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Add a transition while state machine is started
    QVERIFY(!stateMachine.addStateTransition("a", "event5", "b"));
}

// Test: addInternalTransition() -------------------------------------------------------------------

void TestStateMachine::testAddInternalTransition()
{
    StateMachine stateMachine;

    // Add states
    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));

    // Add transitions with invalid state and event names
    QVERIFY(!stateMachine.addInternalTransition("",  "event"));
    QVERIFY(!stateMachine.addInternalTransition("d", "event"));
    QVERIFY(!stateMachine.addInternalTransition("a", ""));

    // Add transitions with and without guard and action methods
    QVERIFY(!stateMachine.addInternalTransition("a", "event1"));
    QVERIFY(!stateMachine.addInternalTransition("a", "event2", {}, m_dummyInternalTransitionGuard));
    QVERIFY(stateMachine.addInternalTransition("a", "event3", m_dummyInternalTransitionAction));
    QVERIFY(stateMachine.addInternalTransition("a", "event4",
                                               m_dummyInternalTransitionAction,
                                               m_dummyInternalTransitionGuard));

    // Add duplicate transition
    QVERIFY(!stateMachine.addInternalTransition("a", "event3", m_dummyInternalTransitionAction));

    // Set the initial transition and add transitions to make the state machine valid then start it
    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "to_b", "b"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Add a transition while state machine is started
    QVERIFY(!stateMachine.addInternalTransition("a", "event5"));
}

// Test: setDefaultTransition() (state transition) -------------------------------------------------

void TestStateMachine::testAddDefaultStateTransition()
{
    StateMachine stateMachine;

    // Add states
    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));

    // Set transitions with invalid state names
    QVERIFY(!stateMachine.setDefaultTransition("", "b"));
    QVERIFY(!stateMachine.setDefaultTransition("d", "b"));
    QVERIFY(!stateMachine.setDefaultTransition("a", "d"));
    QVERIFY(!stateMachine.setDefaultTransition("a", ""));

    // Set the initial transition to make the state machine valid then start it
    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "to_b", "b"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Add a transition while state machine is started
    QVERIFY(!stateMachine.setDefaultTransition("a", "b"));

    // Stop the state machine
    QVERIFY(stateMachine.stop());

    // Set valid default transition
    QVERIFY(stateMachine.setDefaultTransition("a", "b",
                                              m_dummyStateTransitionAction,
                                              m_dummyStateTransitionGuard));

    // Set duplicate default state transition
    QVERIFY(!stateMachine.setDefaultTransition("a", "b"));

    // Set default internal transition (not allowed to have both types of default transitions)
    QVERIFY(!stateMachine.setDefaultTransition("a", m_dummyInternalTransitionAction));

    // Validate and start the state machine again
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
}

// Test: setDefaultTransition() (internal transition) ----------------------------------------------

void TestStateMachine::testAddDefaultInternalTransition()
{
    StateMachine stateMachine;

    // Add states
    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));

    // Set transitions with invalid state names
    QVERIFY(!stateMachine.setDefaultTransition("", m_dummyInternalTransitionAction));
    QVERIFY(!stateMachine.setDefaultTransition("d", m_dummyInternalTransitionAction));

    // Set transition with invalid action
    QVERIFY(!stateMachine.setDefaultTransition("a", InternalTransitionAction()));

    // Set the initial transition to make the state machine valid then start it
    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "to_b", "b"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Add a transition while state machine is started
    QVERIFY(!stateMachine.setDefaultTransition("a", m_dummyInternalTransitionAction));

    // Stop the state machine
    QVERIFY(stateMachine.stop());

    // Set valid default transition
    QVERIFY(stateMachine.setDefaultTransition("a", m_dummyInternalTransitionAction));

    // Set duplicate default state transition
    QVERIFY(!stateMachine.setDefaultTransition("a", m_dummyInternalTransitionAction));

    // Set default state transition (not allowed to have both types of default transitions)
    QVERIFY(!stateMachine.setDefaultTransition("a", "b"));

    // Validate and start the state machine again
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
}

// Test: addEventToFront() -------------------------------------------------------------------------

void TestStateMachine::testAddEventToFront()
{
    // Initialize the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));

    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c"));

    // Try to add an event to an unvalidated state machine
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEventToFront("test"));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Validate the state machine
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Try to add an event to a stopped state machine
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEventToFront("test"));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Try to add an event with an empty name to a stopped state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEventToFront(""));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Start the state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Try to add an event with an empty name to a started state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEventToFront(""));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Add an event to a started state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.addEventToFront("test"));
    QVERIFY(stateMachine.hasPendingEvents());
}

// Test: addEventToBack() --------------------------------------------------------------------------

void TestStateMachine::testAddEventToBack()
{
    // Initialize the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));

    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c"));

    // Try to add an event to an unvalidated state machine
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEventToBack("test"));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Validate the state machine
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Try to add an event to a stopped state machine
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEventToBack("test"));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Try to add an event with an empty name to a stopped state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEventToBack(""));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Start the state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Try to add an event with an empty name to a started state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEventToBack(""));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Add an event to a started state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.addEventToBack("test"));
    QVERIFY(stateMachine.hasPendingEvents());
}

// Test: processNextEvent() ------------------------------------------------------------------------

void TestStateMachine::testProcessNextEvent()
{
    // Initialize the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));
    QVERIFY(stateMachine.addState("d"));

    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addInternalTransition("a", "unexpected1", m_dummyInternalTransitionAction));
    QVERIFY(stateMachine.setDefaultTransition("a", "a", m_dummyStateTransitionAction));

    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c"));
    QVERIFY(stateMachine.setDefaultTransition("b", m_dummyInternalTransitionAction));

    QVERIFY(stateMachine.addStateTransition("c", "c_to_d", "d"));

    // Try to process an event in an unvalidated state machine
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());

    // Validate the state machine
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Try to process an event in a stopped state machine
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());

    // Start the state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    // Try to process an event in a started state machine which has no pending events
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());

    // Add an unexpected event and process it
    QVERIFY(stateMachine.addEventToBack("unexpected"));
    QVERIFY(stateMachine.hasPendingEvents());

    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    // Add an expected event and process it
    QVERIFY(stateMachine.addEventToBack("a_to_b"));
    QVERIFY(stateMachine.hasPendingEvents());

    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("b"));

    // Restart the state machine and add multiple events
    QVERIFY(stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    QVERIFY(stateMachine.addEventToBack("unexpected1"));
    QVERIFY(stateMachine.addEventToBack("a_to_b"));
    QVERIFY(stateMachine.addEventToBack("unexpected2"));
    QVERIFY(stateMachine.addEventToBack("b_to_c"));
    QVERIFY(stateMachine.addEventToBack("unexpected3"));
    QVERIFY(stateMachine.addEventToBack("c_to_d"));

    // Next processed event ("unexpected1") will trigger an internal transition
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    // Next processed event must make a transition to state "b"
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("b"));

    // Next processed event ("unexpected2") will trigger a default internal transition
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("b"));

    // Next processed event must make a transition to state "c"
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QCOMPARE(stateMachine.currentState(), QString("c"));

    // Next processed event ("unexpected3") must be ignored
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("c"));

    // Next (last) processed event must make a transition to state "d" (final state)
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("d"));
    QVERIFY(stateMachine.finalStateReached());

    auto event = stateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), QString("c_to_d"));
}

// Test: poll() ------------------------------------------------------------------------------------

void TestStateMachine::testPoll()
{
    // Initialize the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));
    QVERIFY(stateMachine.addState("d"));

    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.setStateAction("a",
                                        [&](auto &) { stateMachine.addEventToBack("a_to_b"); }));

    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c"));
    QVERIFY(stateMachine.setStateAction("b",
                                        [&](auto &) { stateMachine.addEventToBack("b_to_c"); }));

    QVERIFY(stateMachine.addStateTransition("c", "c_to_d", "d"));
    QVERIFY(stateMachine.setStateAction("c",
                                        [&](auto &) { stateMachine.addEventToBack("c_to_d"); }));

    // Try to poll an unvalidated state machine
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    QVERIFY(!stateMachine.poll());

    // Validate the state machine
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Try to poll a stopped state machine
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(!stateMachine.poll());

    // Start the state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    // First polling cycle must create an event but stay in state "a"
    QVERIFY(stateMachine.poll());

    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    // Next polling cycle must create another event and transition to state "b"
    QVERIFY(stateMachine.poll());

    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("b"));

    // Next polling cycle must create another event and transition to state "c"
    QVERIFY(stateMachine.poll());

    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("c"));

    // Next polling cycle must not create an event and transition to the final state "d"
    QVERIFY(stateMachine.poll());

    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("d"));
    QVERIFY(stateMachine.finalStateReached());

    auto event = stateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), QString("c_to_d"));
}

// Test: Execution of state entry/exit and transition guard/action methods -------------------------

void TestStateMachine::testStateAndTransitionMethods()
{
    QStringList log;
    bool guardValueAB = false;
    bool guardValueBB = false;
    bool guardValueBC = false;

    bool guardValueA = false;
    bool guardValueB = false;

    // Initialize and validate the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.setStateEntryAction(
                "a",
                [&](const Event &trigger, const QString &currentState, const QString &previousState)
    {
        log.append(QString("entryA: '%1' --> '%2' --> '%3'")
                   .arg(previousState,
                        trigger.name(),
                        currentState));
    }));
    QVERIFY(stateMachine.setStateExitAction(
                "a",
                [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        log.append(QString("exitA: '%1' --> '%2' --> '%3'").arg(currentState,
                                                                trigger.name(),
                                                                nextState));
    }));

    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.setStateEntryAction(
                "b",
                [&](const Event &trigger, const QString &currentState, const QString &previousState)
    {
        log.append(QString("entryB: '%1' --> '%2' --> '%3'").arg(previousState,
                                                                 trigger.name(),
                                                                 currentState));
    }));
    QVERIFY(stateMachine.setStateExitAction(
                "b", [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        log.append(QString("exitB: '%1' --> '%2' --> '%3'").arg(currentState,
                                                                trigger.name(),
                                                                nextState));
    }));

    QVERIFY(stateMachine.addState("c"));
    QVERIFY(stateMachine.setStateEntryAction(
                "c",
                [&](const Event &trigger, const QString &currentState, const QString &previousState)
    {
        log.append(QString("entryC: '%1' --> '%2' --> '%3'").arg(previousState,
                                                                 trigger.name(),
                                                                 currentState));
    }));

    QVERIFY(stateMachine.setInitialTransition(
                "a",
                [&](const Event &trigger, const QString &initialState)
    {
        log.append(QString("init: '%1' --> '%2'").arg(trigger.name(), initialState));
    }));

    auto actionAB = [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        log.append(QString("actionAB: '%1' --> '%2' --> '%3'")
                   .arg(currentState, trigger.name(), nextState));
    };
    auto guardAB = [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        if (guardValueAB)
        {
            log.append(QString("guardAB: '%1' --> '%2' --> '%3': Allowed")
                       .arg(currentState, trigger.name(), nextState));
        }
        else
        {
            log.append(QString("guardAB: '%1' --> '%2' --> '%3': Blocked")
                       .arg(currentState, trigger.name(), nextState));
        }

        return guardValueAB;
    };
    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b", actionAB, guardAB));

    auto actionA = [&](const Event &trigger, const QString &currentState)
    {
        log.append(QString("actionA: '%1' --> '%2'")
                   .arg(currentState, trigger.name()));
    };
    auto guardA = [&](const Event &trigger, const QString &currentState)
    {
        if (guardValueA)
        {
            log.append(QString("guardA: '%1' --> '%2': Allowed")
                       .arg(currentState, trigger.name()));
        }
        else
        {
            log.append(QString("guardA: '%1' --> '%2': Blocked")
                       .arg(currentState, trigger.name()));
        }

        return guardValueA;
    };
    QVERIFY(stateMachine.addInternalTransition("a", "int_a", actionA, guardA));
    QVERIFY(stateMachine.setDefaultTransition(
                "a",
                "a",
                [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        log.append(QString("defaultActionAA: '%1' --> '%2' --> '%3'")
                   .arg(currentState, trigger.name(), nextState));
    }));

    auto actionBB = [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        log.append(QString("actionBB: '%1' --> '%2' --> '%3'")
                   .arg(currentState, trigger.name(), nextState));
    };
    auto guardBB = [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        if (guardValueBB)
        {
            log.append(QString("guardBB: '%1' --> '%2' --> '%3': Allowed")
                       .arg(currentState, trigger.name(), nextState));
        }
        else
        {
            log.append(QString("guardBB: '%1' --> '%2' --> '%3': Blocked")
                       .arg(currentState, trigger.name(), nextState));
        }

        return guardValueBB;
    };
    QVERIFY(stateMachine.addStateTransition("b", "b_to_b", "b", actionBB, guardBB));

    auto actionBC = [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        log.append(QString("actionBC: '%1' --> '%2' --> '%3'")
                   .arg(currentState, trigger.name(), nextState));
    };
    auto guardBC = [&](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        if (guardValueBC)
        {
            log.append(QString("guardBC: '%1' --> '%2' --> '%3': Allowed")
                       .arg(currentState, trigger.name(), nextState));
        }
        else
        {
            log.append(QString("guardBC: '%1' --> '%2' --> '%3': Blocked")
                       .arg(currentState, trigger.name(), nextState));
        }

        return guardValueBC;
    };
    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c", actionBC, guardBC));

    auto actionB = [&](const Event &trigger, const QString &currentState)
    {
        log.append(QString("actionB: '%1' --> '%2'")
                   .arg(currentState, trigger.name()));
    };
    auto guardB = [&](const Event &trigger, const QString &currentState)
    {
        if (guardValueB)
        {
            log.append(QString("guardB: '%1' --> '%2': Allowed")
                       .arg(currentState, trigger.name()));
        }
        else
        {
            log.append(QString("guardB: '%1' --> '%2': Blocked")
                       .arg(currentState, trigger.name()));
        }

        return guardValueB;
    };
    QVERIFY(stateMachine.addInternalTransition("b", "int_b", actionB, guardB));

    QVERIFY(stateMachine.setDefaultTransition(
                "b",
                [&](const Event &trigger, const QString &currentState)
    {
        log.append(QString("defaultActionB: '%1' --> '%2'")
                   .arg(currentState, trigger.name()));
    }));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Start the state machine
    QVERIFY(log.isEmpty());

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    QStringList expectedLog
    {
        "init: 'Started' --> 'a'",
        "entryA: '' --> 'Started' --> 'a'"
    };
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("a"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Block state transition from "a" to "b" with the guard method
    guardValueAB = false;
    QVERIFY(stateMachine.addEventToBack("a_to_b"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardAB: 'a' --> 'a_to_b' --> 'b': Blocked");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("a"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Block internal transition in "a" with the guard method
    guardValueA = false;
    QVERIFY(stateMachine.addEventToBack("int_a"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardA: 'a' --> 'int_a': Blocked");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("a"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger internal transition in "a"
    guardValueA = true;
    QVERIFY(stateMachine.addEventToBack("int_a"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardA: 'a' --> 'int_a': Allowed");
    expectedLog.append("actionA: 'a' --> 'int_a'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("a"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger default state transition from state "a" to state "a"
    QVERIFY(stateMachine.addEventToBack("xyz"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("exitA: 'a' --> 'xyz' --> 'a'");
    expectedLog.append("defaultActionAA: 'a' --> 'xyz' --> 'a'");
    expectedLog.append("entryA: 'a' --> 'xyz' --> 'a'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("a"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger state transition from state "a" to state "b"
    guardValueAB = true;
    QVERIFY(stateMachine.addEventToBack("a_to_b"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardAB: 'a' --> 'a_to_b' --> 'b': Allowed");
    expectedLog.append("exitA: 'a' --> 'a_to_b' --> 'b'");
    expectedLog.append("actionAB: 'a' --> 'a_to_b' --> 'b'");
    expectedLog.append("entryB: 'a' --> 'a_to_b' --> 'b'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Block state transition from "b" to "b" with the guard method
    guardValueBB = false;
    QVERIFY(stateMachine.addEventToBack("b_to_b"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardBB: 'b' --> 'b_to_b' --> 'b': Blocked");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Block internal transition in "b" with the guard method
    guardValueB = false;
    QVERIFY(stateMachine.addEventToBack("int_b"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardB: 'b' --> 'int_b': Blocked");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger internal transition in "b"
    guardValueB = true;
    QVERIFY(stateMachine.addEventToBack("int_b"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardB: 'b' --> 'int_b': Allowed");
    expectedLog.append("actionB: 'b' --> 'int_b'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger state transition from state "b" to state "b"
    guardValueBB = true;
    QVERIFY(stateMachine.addEventToBack("b_to_b"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardBB: 'b' --> 'b_to_b' --> 'b': Allowed");
    expectedLog.append("exitB: 'b' --> 'b_to_b' --> 'b'");
    expectedLog.append("actionBB: 'b' --> 'b_to_b' --> 'b'");
    expectedLog.append("entryB: 'b' --> 'b_to_b' --> 'b'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger default internal transition in state "b"
    QVERIFY(stateMachine.addEventToBack("xyz"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("defaultActionB: 'b' --> 'xyz'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Block state transition from "b" to "c" with the guard method
    guardValueBC = false;
    QVERIFY(stateMachine.addEventToBack("b_to_c"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardBC: 'b' --> 'b_to_c' --> 'c': Blocked");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("b"));
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.hasFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger state transition from state "b" to state "c"
    guardValueBC = true;
    QVERIFY(stateMachine.addEventToBack("b_to_c"));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardBC: 'b' --> 'b_to_c' --> 'c': Allowed");
    expectedLog.append("exitB: 'b' --> 'b_to_c' --> 'c'");
    expectedLog.append("actionBC: 'b' --> 'b_to_c' --> 'c'");
    expectedLog.append("entryC: 'b' --> 'b_to_c' --> 'c'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), QString("c"));
    QVERIFY(stateMachine.finalStateReached());

    QVERIFY(stateMachine.hasFinalEvent());
    auto event = stateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), QString("b_to_c"));
}

// Test: Execution of state machine with a state loop ----------------------------------------------

void TestStateMachine::testStateMachineWithLoop()
{
    QStringList log;

    // Initialize and validate the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.setStateEntryAction("a",
                                             [&](auto &, auto &, auto &) { log.append("a"); }));

    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.setStateEntryAction("b",
                                             [&](auto &, auto &, auto &) { log.append("b"); }));

    QVERIFY(stateMachine.addState("c"));
    QVERIFY(stateMachine.setStateEntryAction("c",
                                             [&](auto &, auto &, auto &) { log.append("c"); }));

    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_a", "a"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Start the state machine
    QVERIFY(log.isEmpty());

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Create a transition chain: "a", "b", "a", "b", "c"
    const QStringList expectedLog { "a", "b", "a", "b", "c" };

    QVERIFY(stateMachine.addEventToBack("a_to_b"));
    QVERIFY(stateMachine.addEventToBack("b_to_a"));
    QVERIFY(stateMachine.addEventToBack("a_to_b"));
    QVERIFY(stateMachine.addEventToBack("b_to_c"));

    for (int i = 0; i < 4; i++)
    {
        QVERIFY(stateMachine.hasPendingEvents());
        QVERIFY(stateMachine.processNextEvent());
    }

    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), QString("c"));
    QVERIFY(stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());

    QCOMPARE(log, expectedLog);
}

// Test: Adding of tests during execution of an action ---------------------------------------------

void TestStateMachine::testAddEventFromAction()
{
    // Initialize and validate the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.setStateEntryAction("a",
                                             [&](auto &, auto &, auto &)
    {
        stateMachine.addEventToFront("a_to_b");
    }));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.setStateEntryAction("b",
                                             [&](auto &, auto &, auto &)
    {
        stateMachine.addEventToFront("b_to_c");
    }));
    QVERIFY(stateMachine.addState("c"));

    QVERIFY(stateMachine.setInitialTransition("a"));

    QVERIFY(stateMachine.addStateTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addStateTransition("b", "b_to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Start the state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), QString("a"));

    // Entry to state "a" should have added an event to transition to "b"
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QCOMPARE(stateMachine.currentState(), QString("b"));

    // Entry to state "b" should have added an event to transition to "c"
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QCOMPARE(stateMachine.currentState(), QString("c"));
}

// Test: Helper methods for creating actions and guard conditions (all arguments) ------------------

void TestStateMachine::testCreateClassMethodsFull()
{
    LogHelper logHelper;

    // Initialize and validate the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.setStateEntryAction(
                "a", createStateEntryAction(&logHelper, &LogHelper::stateEntryActionFull)));
    QVERIFY(stateMachine.setStateExitAction(
                "a", createStateExitAction(&logHelper, &LogHelper::stateExitActionFull)));

    QVERIFY(stateMachine.addState("b"));

    QVERIFY(stateMachine.setInitialTransition(
                "a", createInitialTransitionAction(&logHelper,
                                                   &LogHelper::initialTransitionActionFull)));

    QVERIFY(stateMachine.addInternalTransition(
                "a",
                "event1",
                createInternalTransitionAction(
                    &logHelper, &LogHelper::internalTransitionActionFull),
                createInternalTransitionGuardCondition(
                    &logHelper, &LogHelper::internalTransitionGuardConditionFull)));

    QVERIFY(stateMachine.addStateTransition(
                "a",
                "event2",
                "b",
                createStateTransitionAction(&logHelper, &LogHelper::stateTransitionActionFull),
                createStateTransitionGuardCondition(
                    &logHelper, &LogHelper::stateTransitionGuardConditionFull)));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Start the state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Transition to final state
    QVERIFY(stateMachine.addEventToBack("event1"));
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(stateMachine.isStarted());

    QVERIFY(stateMachine.addEventToBack("event2"));
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());

    // Check log
    QStringList expectedLog
    {
        "initialTransitionActionFull: [Started], [a]",
        "stateEntryActionFull: [Started], [a], []",
        "internalTransitionGuardConditionFull: [event1], [a]",
        "internalTransitionActionFull: [event1], [a]",
        "stateTransitionGuardConditionFull: [event2], [a], [b]",
        "stateExitActionFull: [event2], [a], [b]",
        "stateTransitionActionFull: [event2], [a], [b]",
    };

    QCOMPARE(logHelper.log(), expectedLog);
}

// Test: Helper methods for creating actions and guard conditions (no arguments) -------------------

void TestStateMachine::testCreateClassMethodsVoid()
{
    LogHelper logHelper;

    // Initialize and validate the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.setStateEntryAction(
                "a", createStateEntryAction(&logHelper, &LogHelper::stateEntryActionVoid)));
    QVERIFY(stateMachine.setStateExitAction(
                "a", createStateExitAction(&logHelper, &LogHelper::stateExitActionVoid)));

    QVERIFY(stateMachine.addState("b"));

    QVERIFY(stateMachine.setInitialTransition(
                "a", createInitialTransitionAction(&logHelper,
                                                   &LogHelper::initialTransitionActionVoid)));

    QVERIFY(stateMachine.addInternalTransition(
                "a",
                "event1",
                createInternalTransitionAction(
                    &logHelper, &LogHelper::internalTransitionActionVoid),
                createInternalTransitionGuardCondition(
                    &logHelper, &LogHelper::internalTransitionGuardConditionVoid)));

    QVERIFY(stateMachine.addStateTransition(
                "a",
                "event2",
                "b",
                createStateTransitionAction(&logHelper, &LogHelper::stateTransitionActionVoid),
                createStateTransitionGuardCondition(
                    &logHelper, &LogHelper::stateTransitionGuardConditionVoid)));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Start the state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Transition to final state
    QVERIFY(stateMachine.addEventToBack("event1"));
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(stateMachine.isStarted());

    QVERIFY(stateMachine.addEventToBack("event2"));
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());

    // Check log
    QStringList expectedLog
    {
        "initialTransitionActionVoid",
        "stateEntryActionVoid",
        "internalTransitionGuardConditionVoid",
        "internalTransitionActionVoid",
        "stateTransitionGuardConditionVoid",
        "stateExitActionVoid",
        "stateTransitionActionVoid",
    };

    QCOMPARE(logHelper.log(), expectedLog);
}

// Main function -----------------------------------------------------------------------------------

QTEST_MAIN(TestStateMachine)
#include "testStateMachine.moc"
