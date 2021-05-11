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

class StateObject : public IState
{
public:
    StateObject(const QString &name, QStringList &log)
        : m_name(name),
          m_log(log)
    {
    }

    StateObject(const StateObject &) = delete;
    StateObject(StateObject &&) = delete;

    ~StateObject() override = default;

    StateObject &operator=(const StateObject &) = delete;
    StateObject &operator=(StateObject &&) = delete;

    QString stateName() const override
    {
        return m_name;
    }

    void stateEntryMethod(const Event &event,
                          const QString &fromState,
                          const QString &toState) override
    {
        m_log.append(QString("stateEntryMethod: '%1' --> '%2' --> '%3'")
                     .arg(fromState, event.name(), toState));
    }

    void stateExitMethod(const Event &event,
                         const QString &fromState,
                         const QString &toState) override
   {
       m_log.append(QString("stateExitMethod: '%1' --> '%2' --> '%3'")
                    .arg(fromState, event.name(), toState));
   }

private:
    QString m_name;
    QStringList &m_log;
};

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
    void testAddStateFromObject();
    void testSetInitialState();
    void testAddTransition();
    void testAddEvent();
    void testProcessNextEvent();
    void testStateAndTransitionMethods();
    void testStateMachineWithLoop();

private:
    const StateMachine::StateEntryMethod m_dummyEntryMethod;
    const StateMachine::StateExitMethod m_dummyExitMethod;
    const StateMachine::TransitionGuardMethod m_dummyGuardMethod;
    const StateMachine::TransitionActionMethod m_dummyActionMethod;
};

// Constructor -------------------------------------------------------------------------------------

TestStateMachine::TestStateMachine()
    : m_dummyEntryMethod([](const Event &, const QString &, const QString &) {}),
      m_dummyExitMethod([](const Event &, const QString &, const QString &) {}),
      m_dummyGuardMethod([](const Event &, const QString &, const QString &) { return true; }),
      m_dummyActionMethod([](const Event &, const QString &, const QString &) {})
{
}

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

    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addTransition("b", "b_to_b", "b"));
    QVERIFY(stateMachine.addTransition("b", "b_to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), "a");

    // Transition to state "b" and add the event to transition to the final state
    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.processNextEvent());
    QCOMPARE(stateMachine.currentState(), "b");
    QVERIFY(!stateMachine.finalStateReached());

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.addEvent(Event::create("b_to_c")));
    QVERIFY(stateMachine.hasPendingEvents());

    // Now create a new instance from the original state machine and check if it has the same state
    StateMachine movedStateMachine(std::move(stateMachine));

    QCOMPARE(movedStateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);
    QVERIFY(movedStateMachine.isStarted());
    QCOMPARE(movedStateMachine.currentState(), "b");
    QVERIFY(!movedStateMachine.finalStateReached());
    QVERIFY(!movedStateMachine.takeFinalEvent());
    QVERIFY(movedStateMachine.hasPendingEvents());
    QCOMPARE(movedStateMachine.initialState(), "a");

    // Processing the pending event must transition the state machine to its final state
    QVERIFY(movedStateMachine.processNextEvent());
    QCOMPARE(movedStateMachine.currentState(), "c");
    QVERIFY(movedStateMachine.finalStateReached());
    QVERIFY(!movedStateMachine.isStarted());

    auto event = movedStateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), "b_to_c");
}

// Test: Move assignment operator ------------------------------------------------------------------

void TestStateMachine::testMoveAssignmentOperator()
{
    // Initialize, validate, and start the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));

    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addTransition("b", "b_to_b", "b"));
    QVERIFY(stateMachine.addTransition("b", "b_to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), "a");

    // Transition to state "b" and add the event to transition to the final state
    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.processNextEvent());
    QCOMPARE(stateMachine.currentState(), "b");
    QVERIFY(!stateMachine.finalStateReached());

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.addEvent(Event::create("b_to_c")));
    QVERIFY(stateMachine.hasPendingEvents());

    // Now create a new instance from the original state machine and check if it has the same state
    StateMachine movedStateMachine;
    movedStateMachine = std::move(stateMachine);

    QCOMPARE(movedStateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);
    QVERIFY(movedStateMachine.isStarted());
    QCOMPARE(movedStateMachine.currentState(), "b");
    QVERIFY(!movedStateMachine.finalStateReached());
    QVERIFY(!movedStateMachine.takeFinalEvent());
    QVERIFY(movedStateMachine.hasPendingEvents());
    QCOMPARE(movedStateMachine.initialState(), "a");

    // Processing the pending event must transition the state machine to its final state
    QVERIFY(movedStateMachine.processNextEvent());
    QCOMPARE(movedStateMachine.currentState(), "c");
    QVERIFY(movedStateMachine.finalStateReached());
    QVERIFY(!movedStateMachine.isStarted());

    auto event = movedStateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), "b_to_c");
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
    QVERIFY(stateMachine.addState("c", {}, m_dummyExitMethod));

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

    // Try to start the state machine with empty event and with an event that has an empty name
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(!stateMachine.start({}));
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(!stateMachine.start(Event::create(QString())));
    QVERIFY(!stateMachine.isStarted());

    // Start the state machine (with the default initialization event)
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());
    QVERIFY(stateMachine.start());

    // Since the state machine has a single state that is both the initial and final state the state
    // machine must stop immediately
    QVERIFY(!stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), "init");
    QVERIFY(stateMachine.finalStateReached());

    auto event = stateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), "Started");

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
    QCOMPARE(stateMachine.currentState(), "init");
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    // Stopping the state machine must fail if it is already stopped but it must not have any effect
    // on the state machine
    QVERIFY(!stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), "init");
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

    // Add a state with entry method
    QVERIFY(stateMachine.addState("b", [](const Event &, const QString &, const QString &) {}));

    // Add a state with exit method
    QVERIFY(stateMachine.addState("c", {}, [](const Event &, const QString &, const QString &) {}));

    // Add a state with entry and exit method
    QVERIFY(stateMachine.addState("d",
                                  [](const Event &, const QString &, const QString &) {},
                                  [](const Event &, const QString &, const QString &) {}));

    // Set the initial state and add transitions to make the state machine valid then start it
    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("a", "to_b", "b"));
    QVERIFY(stateMachine.addTransition("a", "to_c", "c"));
    QVERIFY(stateMachine.addTransition("a", "to_d", "d"));

    QVERIFY(stateMachine.addTransition("c", "to_b", "b"));
    QVERIFY(stateMachine.addTransition("d", "to_b", "b"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Add a state while state machine is started
    QVERIFY(!stateMachine.addState("e"));
}

// Test: addState(IState) --------------------------------------------------------------------------

void TestStateMachine::testAddStateFromObject()
{
    QStringList log;
    StateMachine stateMachine;

    // Add state objects
    StateObject stateA("a", log);
    StateObject stateB("b", log);
    StateObject stateC("c", log);

    QVERIFY(stateMachine.addState(stateA));
    QVERIFY(stateMachine.addState(stateB));
    QVERIFY(stateMachine.addState("c"));

    // Set the initial state and add transitions to make the state machine valid then start it
    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addTransition("b", "b_to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Add events to transition the state machine to the final state
    const QStringList expectedLog
    {
        "stateEntryMethod: '' --> 'Started' --> 'a'",

        "stateExitMethod: 'a' --> 'a_to_b' --> 'b'",
        "stateEntryMethod: 'a' --> 'a_to_b' --> 'b'",

        "stateExitMethod: 'b' --> 'b_to_c' --> 'c'",
    };

    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.addEvent(Event::create("b_to_c")));

    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());

    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());

    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), "c");
    QVERIFY(stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());

    QCOMPARE(log, expectedLog);
}

// Test: setInitialState() -------------------------------------------------------------------------

void TestStateMachine::testSetInitialState()
{
    StateMachine stateMachine;

    // Add states
    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.initialState().isEmpty());

    // Set an initial state that with an empty name
    QVERIFY(!stateMachine.setInitialState(""));
    QVERIFY(stateMachine.initialState().isEmpty());

    // Set an initial state that doesn't exists
    QVERIFY(!stateMachine.setInitialState("x"));
    QVERIFY(stateMachine.initialState().isEmpty());

    // Set a valid initial state
    QVERIFY(stateMachine.setInitialState("a"));
    QCOMPARE(stateMachine.initialState(), "a");

    // Set the same initial state again
    QVERIFY(!stateMachine.setInitialState("a"));
    QCOMPARE(stateMachine.initialState(), "a");

    // Set the another initial state
    QVERIFY(!stateMachine.setInitialState("b"));
    QCOMPARE(stateMachine.initialState(), "a");
}

// Test: addTransition() ---------------------------------------------------------------------------

void TestStateMachine::testAddTransition()
{
    StateMachine stateMachine;

    // Add states
    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));

    // Add transitions with invalid state and event names
    QVERIFY(!stateMachine.addTransition("",  "event", "c"));
    QVERIFY(!stateMachine.addTransition("d", "event", "c"));
    QVERIFY(!stateMachine.addTransition("a", "",      "c"));
    QVERIFY(!stateMachine.addTransition("a", "event", ""));
    QVERIFY(!stateMachine.addTransition("a", "event", "d"));

    // Add transitions with and without guard and action methods
    QVERIFY(stateMachine.addTransition("a", "event1", "b"));
    QVERIFY(stateMachine.addTransition("a", "event2", "b", m_dummyGuardMethod));
    QVERIFY(stateMachine.addTransition("a", "event3", "b", {}, m_dummyActionMethod));
    QVERIFY(stateMachine.addTransition("a", "event4", "b",
                                       m_dummyGuardMethod, m_dummyActionMethod));

    // Add duplicate transition
    QVERIFY(stateMachine.addTransition("a", "event", "c"));
    QVERIFY(!stateMachine.addTransition("a", "event", "c"));

    // Set the initial state and add transitions to make the state machine valid then start it
    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("b", "to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Add a transition while state machine is started
    QVERIFY(!stateMachine.addTransition("a", "event5", "b"));
}

// Test: addEvent() ---------------------------------------------------------------------------

void TestStateMachine::testAddEvent()
{
    // Initialize the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a"));
    QVERIFY(stateMachine.addState("b"));
    QVERIFY(stateMachine.addState("c"));

    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addTransition("b", "b_to_c", "c"));

    // Try to add an event to an unvalidated state machine
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Unvalidated);

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEvent(Event::create("test")));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Validate the state machine
    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Try to add an event to a stopped state machine
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEvent(Event::create("test")));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Try to add an empty event to a stopped state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEvent({}));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Try to add an event with an empty name to a stopped state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEvent(Event::create("")));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Start the state machine
    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Try to add an empty event to a started state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEvent({}));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Try to add an event with an empty name to a started state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.addEvent(Event::create("")));
    QVERIFY(!stateMachine.hasPendingEvents());

    // Add an event to a started state machine
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.addEvent(Event::create("test")));
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

    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addTransition("b", "b_to_c", "c"));

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
    QCOMPARE(stateMachine.currentState(), "a");

    // Try to process an event in a started state machine which has no pending events
    QVERIFY(!stateMachine.hasPendingEvents());
    QVERIFY(!stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());

    // Add an unexpected event and process is
    QVERIFY(stateMachine.addEvent(Event::create("unexpected")));
    QVERIFY(stateMachine.hasPendingEvents());

    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), "a");

    // Add an expected event and process is
    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.hasPendingEvents());

    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), "b");

    // Restart the state machine and add multiple events
    QVERIFY(stateMachine.stop());
    QVERIFY(!stateMachine.isStarted());

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());
    QCOMPARE(stateMachine.currentState(), "a");

    QVERIFY(stateMachine.addEvent(Event::create("unexpected1")));
    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.addEvent(Event::create("unexpected2")));
    QVERIFY(stateMachine.addEvent(Event::create("b_to_c")));

    // Next processed event ("unexpected1") must be ignored
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), "a");

    // Next processed event must make a transition to state "b"
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), "b");

    // Next processed event ("unexpected2") must be ignored
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), "b");

    // Next (last) processed event must make a transition to state "c" (final state)
    QVERIFY(stateMachine.hasPendingEvents());
    QVERIFY(stateMachine.processNextEvent());
    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), "c");
    QVERIFY(stateMachine.finalStateReached());

    auto event = stateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), "b_to_c");
}

// Test: Execution of state entry/exit and transition guard/action methods -------------------------

void TestStateMachine::testStateAndTransitionMethods()
{
    QStringList log;
    bool guardValueAB = false;
    bool guardValueBB = false;
    bool guardValueBC = false;

    auto entryA = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        log.append(QString("entryA: '%1' --> '%2' --> '%3'").arg(fromState, event.name(), toState));
    };
    auto exitA = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        log.append(QString("exitA: '%1' --> '%2' --> '%3'").arg(fromState, event.name(), toState));
    };

    auto guardAB = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        if (guardValueAB)
        {
            log.append(QString("guardAB: '%1' --> '%2' --> '%3': Allowed")
                       .arg(fromState, event.name(), toState));
        }
        else
        {
            log.append(QString("guardAB: '%1' --> '%2' --> '%3': Blocked")
                       .arg(fromState, event.name(), toState));
        }

        return guardValueAB;
    };
    auto actionAB = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        log.append(QString("actionAB: '%1' --> '%2' --> '%3'")
                   .arg(fromState, event.name(), toState));
    };

    auto entryB = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        log.append(QString("entryB: '%1' --> '%2' --> '%3'").arg(fromState, event.name(), toState));
    };
    auto exitB = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        log.append(QString("exitB: '%1' --> '%2' --> '%3'").arg(fromState, event.name(), toState));
    };

    auto guardBB = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        if (guardValueBB)
        {
            log.append(QString("guardBB: '%1' --> '%2' --> '%3': Allowed")
                       .arg(fromState, event.name(), toState));
        }
        else
        {
            log.append(QString("guardBB: '%1' --> '%2' --> '%3': Blocked")
                       .arg(fromState, event.name(), toState));
        }

        return guardValueBB;
    };
    auto actionBB = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        log.append(QString("actionBB: '%1' --> '%2' --> '%3'")
                   .arg(fromState, event.name(), toState));
    };

    auto guardBC = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        if (guardValueBC)
        {
            log.append(QString("guardBC: '%1' --> '%2' --> '%3': Allowed")
                       .arg(fromState, event.name(), toState));
        }
        else
        {
            log.append(QString("guardBC: '%1' --> '%2' --> '%3': Blocked")
                       .arg(fromState, event.name(), toState));
        }

        return guardValueBC;
    };
    auto actionBC = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        log.append(QString("actionBC: '%1' --> '%2' --> '%3'")
                   .arg(fromState, event.name(), toState));
    };

    auto entryC = [&](const Event &event, const QString &fromState, const QString &toState)
    {
        log.append(QString("entryC: '%1' --> '%2' --> '%3'").arg(fromState, event.name(), toState));
    };

    // Initialize and validate the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a", entryA, exitA));
    QVERIFY(stateMachine.addState("b", entryB, exitB));
    QVERIFY(stateMachine.addState("c", entryC));

    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("a", "a_to_b", "b", guardAB, actionAB));
    QVERIFY(stateMachine.addTransition("b", "b_to_b", "b", guardBB, actionBB));
    QVERIFY(stateMachine.addTransition("b", "b_to_c", "c", guardBC, actionBC));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Start the state machine
    QVERIFY(log.isEmpty());

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    QStringList expectedLog { "entryA: '' --> 'Started' --> 'a'" };
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), "a");
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    log.clear();
    expectedLog.clear();

    // Block transition from "a" to "b" with the guard method
    guardValueAB = false;
    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardAB: 'a' --> 'a_to_b' --> 'b': Blocked");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), "a");
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger transition from state "a" to state "b"
    guardValueAB = true;
    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardAB: 'a' --> 'a_to_b' --> 'b': Allowed");
    expectedLog.append("exitA: 'a' --> 'a_to_b' --> 'b'");
    expectedLog.append("actionAB: 'a' --> 'a_to_b' --> 'b'");
    expectedLog.append("entryB: 'a' --> 'a_to_b' --> 'b'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), "b");
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    log.clear();
    expectedLog.clear();

    // Block transition from "b" to "b" with the guard method
    guardValueBB = false;
    QVERIFY(stateMachine.addEvent(Event::create("b_to_b")));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardBB: 'b' --> 'b_to_b' --> 'b': Blocked");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), "b");
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger transition from state "b" to state "b"
    guardValueBB = true;
    QVERIFY(stateMachine.addEvent(Event::create("b_to_b")));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardBB: 'b' --> 'b_to_b' --> 'b': Allowed");
    expectedLog.append("exitB: 'b' --> 'b_to_b' --> 'b'");
    expectedLog.append("actionBB: 'b' --> 'b_to_b' --> 'b'");
    expectedLog.append("entryB: 'b' --> 'b_to_b' --> 'b'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), "b");
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    log.clear();
    expectedLog.clear();

    // Block transition from "b" to "c" with the guard method
    guardValueBC = false;
    QVERIFY(stateMachine.addEvent(Event::create("b_to_c")));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardBC: 'b' --> 'b_to_c' --> 'c': Blocked");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), "b");
    QVERIFY(!stateMachine.finalStateReached());
    QVERIFY(!stateMachine.takeFinalEvent());

    log.clear();
    expectedLog.clear();

    // Trigger transition from state "b" to state "c"
    guardValueBC = true;
    QVERIFY(stateMachine.addEvent(Event::create("b_to_c")));
    QVERIFY(stateMachine.processNextEvent());

    expectedLog.append("guardBC: 'b' --> 'b_to_c' --> 'c': Allowed");
    expectedLog.append("exitB: 'b' --> 'b_to_c' --> 'c'");
    expectedLog.append("actionBC: 'b' --> 'b_to_c' --> 'c'");
    expectedLog.append("entryC: 'b' --> 'b_to_c' --> 'c'");
    QCOMPARE(log, expectedLog);

    QCOMPARE(stateMachine.currentState(), "c");
    QVERIFY(stateMachine.finalStateReached());

    auto event = stateMachine.takeFinalEvent();
    QVERIFY(event);
    QCOMPARE(event->name(), "b_to_c");
}

// Test: Execution of state machine with a state loop ----------------------------------------------

void TestStateMachine::testStateMachineWithLoop()
{
    QStringList log;

    // Initialize and validate the state machine
    StateMachine stateMachine;

    QVERIFY(stateMachine.addState("a", [&](auto &, auto &, auto &) { log.append("a"); }));
    QVERIFY(stateMachine.addState("b", [&](auto &, auto &, auto &) { log.append("b"); }));
    QVERIFY(stateMachine.addState("c", [&](auto &, auto &, auto &) { log.append("c"); }));

    QVERIFY(stateMachine.setInitialState("a"));

    QVERIFY(stateMachine.addTransition("a", "a_to_b", "b"));
    QVERIFY(stateMachine.addTransition("b", "b_to_a", "a"));
    QVERIFY(stateMachine.addTransition("b", "b_to_c", "c"));

    QVERIFY(stateMachine.validate());
    QCOMPARE(stateMachine.validationStatus(), StateMachine::ValidationStatus::Valid);

    // Start the state machine
    QVERIFY(log.isEmpty());

    QVERIFY(stateMachine.start());
    QVERIFY(stateMachine.isStarted());

    // Create a transition chain: "a", "b", "a", "b", "c"
    const QStringList expectedLog { "a", "b", "a", "b", "c" };

    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.addEvent(Event::create("b_to_a")));
    QVERIFY(stateMachine.addEvent(Event::create("a_to_b")));
    QVERIFY(stateMachine.addEvent(Event::create("b_to_c")));

    for (int i = 0; i < 4; i++)
    {
        QVERIFY(stateMachine.hasPendingEvents());
        QVERIFY(stateMachine.processNextEvent());
    }

    QVERIFY(!stateMachine.hasPendingEvents());
    QCOMPARE(stateMachine.currentState(), "c");
    QVERIFY(stateMachine.finalStateReached());
    QVERIFY(!stateMachine.isStarted());

    QCOMPARE(log, expectedLog);
}

// Main function -----------------------------------------------------------------------------------

QTEST_MAIN(TestStateMachine)
#include "testStateMachine.moc"
