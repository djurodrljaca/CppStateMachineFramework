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
#include <QtTest/QTest>

// System includes

// Forward declarations

// Macros

// Test class declaration --------------------------------------------------------------------------

using namespace CppStateMachineFramework;

class TestStateMachine : public QObject
{
    Q_OBJECT

private slots:
    // Functions executed by QtTest before and after test suite
    void initTestCase();
    void cleanupTestCase();

    // Functions executed by QtTest before and after each test
    void init();
    void cleanup();

    // Test functions
    void testValidation();
    void testStartStop();
    // TODO: testAddState
    // TODO: testSetInitialState
    // TODO: testAddTransition
    // TODO: test entry, exit, transition guard, and transition action methods
    // TODO: test transitions to the same state
    // TODO: test state loops
    // TODO: test state machines with and without final states
};

// Test Case init/cleanup methods ------------------------------------------------------------------

void TestStateMachine::initTestCase()
{
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

// Test: Constructor -------------------------------------------------------------------------------

void TestStateMachine::testValidation()
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
    QVERIFY(stateMachine.setInitialState("init"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);
    QCOMPARE(stateMachine.initialState(), "init");

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Detect unreachable states
    QVERIFY(stateMachine.addState("a"));
    QCOMPARE(stateMachine.initialState(), "init");

    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);

    // Add transition to the unreachable state to make the state machine valid
    QVERIFY(stateMachine.addTransition("init", "to_a", "a"));
    QCOMPARE(stateMachine.initialState(), "init");
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Add a new unreachable state with a transition
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addTransition("b", "to_a", "a"));

    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Invalid);

    // It must be possible to start an invalid state machine
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(!stateMachine.start());
    QVERIFY(!stateMachine.isStarted());

    // Now make the new state reachable
    QVERIFY(stateMachine.addTransition("init", "to_b", "b"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // It must be possible to start a valid state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // It must not be possible to execute the validation procedure or add a state or transition when
    // the state machine is started but this must not invalidate the state machine
    QVERIFY(!stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.addState("c"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.addTransition("a", "to_b", "b"));
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Stop the state machine and check if validation still works
    QVERIFY(stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Add an invalid final state (no transitions from it to other states and an exit method)
    QVERIFY(stateMachine.addState("c", {}, [](const Event &) {}));

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
    QVERIFY(stateMachine.setInitialState("init"));
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.isStarted());
    QVERIFY(stateMachine.start());

    // Since the state machine has a single state that is both the initial and final state the state
    // machine must stop immediately
    QVERIFY(!stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), "init");

    // Stopping the state machine must fail if it is already stopped but it must not have any effect
    // on the state machine
    QVERIFY(!stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());

    // Extend the state machine with another state and start it again
    QVERIFY(stateMachine.addState("final"));
    QVERIFY(stateMachine.addTransition("init", "event", "final"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(!stateMachine.isStarted());
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), "init");

    // Starting the state machine must fail if it is already started but it must not have any effect
    // on the state machine
    QVERIFY(stateMachine.isStarted());
    QVERIFY(!stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Stop the state machine while it is still active
    QVERIFY(stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), "init");

    // Stopping the state machine must fail if it is already stopped but it must not have any effect
    // on the state machine
    QVERIFY(!stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());
}

// Main function -----------------------------------------------------------------------------------

QTEST_MAIN(TestStateMachine)
#include "testStateMachine.moc"
