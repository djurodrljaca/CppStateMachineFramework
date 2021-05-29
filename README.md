# CppStateMachineFramework

The purpose of this framework is to simplify creation of state machines for C++ applications.

The features and terminology is based on the
[UML state machine](https://en.wikipedia.org/wiki/UML_state_machine). This framework does not
support all of those features and it does some things a bit differently. For example while
hierarchical state machines are not directly supported a similar functionality could be achieved by
adding a state that is configured to operate another state machine.


## Requirements

Requirements document can be found [here](docs/Requirements.md).


## Design

Design document can be found [here](docs/Design.md).


## Build and deployment

For building the following requirements are needed:

* CMake v3.5 or later
* C++14 compliant compiler
* Qt v5.9 or later (only Core and Test modules)

If the library needs to be deployed to a custom location you need to set the
```CMAKE_INSTALL_PREFIX``` variable or if you need to use a custom Qt installation you need to set
the ```CMAKE_PREFIX_PATH``` variable.

Example:

```
$ cmake -DCMAKE_INSTALL_PREFIX=path/to/install/dir -DCMAKE_PREFIX_PATH=path/to/qt/dir path/to/source/dir
```

Then build and deploy the library:

```
$ cmake --build . --target install
```


## Usage

### CMake Integration

To use this module in a CMake project you need to build and deploy it. Then just add it as a package
to your project's CMakeLists.txt:

```CMake
find_package(CedarFramework REQUIRED)
```

And link it to your target:

```CMake
target_link_libraries(target_name PUBLIC CedarFramework::CedarFramework)
```


### How to use the framework

A state machine can be created with the state machine framework by following these steps:

* Configuration
* Validation
* Execution


#### Configuration of a state machine


##### Adding states

To configure a state machine you must start by adding states to it. If a state needs to have entry
and exit actions then they need to be set while the state is being added to the state machine:

```C++
// State with both entry and exit actions
stateMachine.addState(
    "state1",
    [](const Event &trigger, const QString &currentState, const QString &previousState)
    {
        /* state's entry action */
    },
    [](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        /* state's exit action */
    });

// State without actions
stateMachine.addState("state2");

// State with an entry action but without an exit action
stateMachine.addState(
    "state3",
    [](const Event &trigger, const QString &currentState, const QString &previousState)
    {
        /* state's entry action */
    });

// State with an exit action but without an entry action
stateMachine.addState(
    "state4",
    {},
    [](const Event &trigger, const QString &currentState, const QString &nextState)
    {
        /* state's exit action */
    });
```

*Note: entry or exit action arguments can be anything that can be stored in a std::function with
appropriate arguments.*


##### Adding the initial transition

Once the states are added to the state machine you should set the initial transition. An initial
transition must define which of the state machine states will be the initial state:

```C++
stateMachine.setInitialTransition("state1");
```

But it can also set a transition action for when state machine is started and it transitions to the
initial state:

```C++
stateMachine.setInitialTransition(
    "state1",
    [](const Event &trigger, const QString &initialState)
    {
        /* initial transition's action */
    });
```


##### Adding transitions to states

After the initial transition you should add all the other transitions to states. A state can have:

* State transitions
* Internal transitions
* Default state transition
* Default internal transition

*Note: State transitions and the default state transition can optionally have an action and/or a
guard condition. Internal transition and the default internal transition are a bit different as they
must have an action but the guard condition is optional.*

*Note: a state can be without any transitions (a final state) or any combination of transitions
except both a default state and internal transition.*

```C++
// State transition with both an action and a guard condition
stateMachine.addStateTransition(
    "state1", "event1", "state2",
    [](const Event &trigger, const QString &fromState, const QString &toState)
    {
        /* state transition's action */
    },
    [](const Event &trigger, const QString &fromState, const QString &toState)
    {
        /* state transition's guard condition */
        return guardConditionValue;
    });

// State transition without an action and guard condition
stateMachine.addStateTransition("state1", "event2", "state2");

// State transition with an action but without a guard condition
stateMachine.addStateTransition(
    "state1", "event3", "state2",
    [](const Event &trigger, const QString &fromState, const QString &toState)
    {
        /* state transition's action */
    });

// State transition without an action but with a guard condition
stateMachine.addStateTransition(
    "state1", "event4", "state2",
    {},
    [](const Event &trigger, const QString &fromState, const QString &toState)
    {
        /* state transition's guard condition */
        return guardConditionValue;
    });
```

```C++
// Internal transition with both an action and a guard condition
stateMachine.addInternalTransition(
    "state1", "event1",
    [](const Event &trigger, const QString &currentState)
    {
        /* internal transition's action */
    },
    [](const Event &trigger, const QString &currentState)
    {
        /* internal transition's guard condition */
        return guardConditionValue;
    });

// Internal transition with an action but without a guard condition
stateMachine.addInternalTransition(
    "state1", "event3",
    [](const Event &trigger, const QString &currentState)
    {
        /* internal transition's action */
    });
```

```C++
// Default state transition with both an action and a guard condition
stateMachine.setDefaultTransition(
    "state1", "state3",
    [](const Event &trigger, const QString &fromState, const QString &toState)
    {
        /* default state transition's action */
    },
    [](const Event &trigger, const QString &fromState, const QString &toState)
    {
        /* default state transition's guard condition */
        return guardConditionValue;
    });

// Default state transition without an action and guard condition
stateMachine.setDefaultTransition("state2", "state3");

// Default state transition with an action but without a guard condition
stateMachine.setDefaultTransition(
    "state3", "state4",
    [](const Event &trigger, const QString &fromState, const QString &toState)
    {
        /* default state transition's action */
    });

// Default state transition without an action but with a guard condition
stateMachine.addStateTransition(
    "state2", "state1",
    {},
    [](const Event &trigger, const QString &fromState, const QString &toState)
    {
        /* default state transition's guard condition */
        return guardConditionValue;
    });
```

```C++
// Default internal transition with both an action and a guard condition
stateMachine.addInternalTransition(
    "state1",
    [](const Event &trigger, const QString &currentState)
    {
        /* default internal transition's action */
    },
    [](const Event &trigger, const QString &currentState)
    {
        /* internal transition's guard condition */
        return guardConditionValue;
    });

// Default internal transition with an action but without a guard condition
stateMachine.addInternalTransition(
    "state2",
    [](const Event &trigger, const QString &currentState)
    {
        /* default internal transition's action */
    });
```

*Note: action and guard condition arguments can be anything that can be stored in a std::function
with appropriate arguments.*


#### Validation of the state machine

A state machine is initially unvalidated. When all the states and transitions are configured the
state machine can be validated:

```C++
bool result = stateMachine.validate();
```

The validation status can also be queried at any time:

```C++
StateMachine::ValidationStatus status = stateMachine.validationStatus();
```

If the state machine is already valid (validation was successful) and the state machine
configuration is changed (for example a state or transition is added) then the state machine's
validation status will go back to unvalidated.


#### Execution

To start a state machine it must first be configured and validated:

```C++
bool result = stateMachine.start();
```

The "started" status can also be queried at any time:

```C++
bool status = stateMachine.isStarted();
```

A state machine is driven by events. An event must have a name and optionally also a parameter.

Events should be added to the end of the event queue in most cases:

```C++
stateMachine.addEventToBack("event1");
stateMachine.addEventToBack("event2", EventParameter<int>::create(42));
```

But in cases where an event should be processed immediately it can also be added to the front of the
event queue:

```C++
stateMachine.addEventToFront("event1");
stateMachine.addEventToFront("event2", EventParameter<int>::create(42));
```

Adding events to the front of the event queue can be used to ensure that a state transition occurs
as a consequence of an event being processed in a state's entry or exit action or a transition
without losing the events that are already in the event queue. By adding an event to the front of
the queue a specific state transition can be triggered so that the currently queued events are then
processed in the appropriate state. Otherwise the queued events might get ignored as they would have
been processed while in a state that doesn't react to them.

When a state machine has at least one event queued the events can be processed. The events are
processed one at a time so it might be necessary to keep processing events until the event queue is
empty and while the state machine is still running:

```C++
while (stateMachine.hasPendingEvents() && stateMachine.isStarted())
{
    stateMachine.processNextEvent();
}
```

*Note: If a state machine is stopped (either manually or automatically after entering a final state)
and there are still some pending events in the event queue then a loop that only checks if there are
any pending events without also checking if the state machine is still running it would become an
infinite loop!*

A state machine can be stopped manually:

```C++
bool result = stateMachine.stop();
```

Or automatically after it reaches a final state. The reason for why a state machine was stopped can
be determined by checking if the final state was reached:

```C++
bool result = stateMachine.finalStateReached();
```

When a state machine reaches the final state it also stores the event that triggered the transition
to the final state. The presence of the event can be queried and it can also be taken from the state
machine:

```C++
if (stateMachine.hasFinalEvent())
{
    std::unique_ptr<Event> event = stateMachine.takeFinalEvent();
    ...
}
```
