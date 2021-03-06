# Design

The state machine shall be divided into the following segments:

* Configuration
* Validation
* Event processing
* Polling
* Startup procedure
* Shutdown procedure

Class diagram for the state machine framework:

![State machine](Diagrams/ClassDiagrams/StateMachine.svg "State machine")


## Configuration

The state machine shall be configured by:

* adding states
* setting entry, state, and/or exit actions to states (optional)
* setting the initial transition
* adding state and/or internal transitions to states (optional)
* setting default state or internal transition to states (optional)


### State machine states

The state machine shall have a container for all of its states. It shall be possible to add states
but not to remove or rename them.

A state shall have:

* a name
* an entry action (optional)
* a state action (optional)
* an exit action (optional)
* state transitions (optional)
* internal transitions (optional)
* default state or internal transition (optional)


### Initial transition

A valid state machine shall need to have its initial transition set. The initial transition shall
need to know to which state to transition to during startup and optionally it shall be possible to
set an action for the transition.

Workflow for executing the initial transitions:

![Initial transition workflow](Diagrams/FlowCharts/InitialTransitionWorkflow.svg "Initial transition workflow")


#### Initial transition's action

The initial transition's action shall have the following context when executed:

* event that triggered the transition
* name of the initial state


### State transitions

It shall be possible to add state transitions to a state. A state transition shall have:

* a trigger (name of the event that triggers the transition)
* a state to transition to
* a guard condition (optional)
* an action (optional)

Workflow for executing state transitions:

![State transition workflow](Diagrams/FlowCharts/StateTransitionWorkflow.svg "State transition workflow")


#### State transition's guard condition

A state transition's guard condition shall have the following context when executed:

* event that triggered the transition
* name of the current state
* name of the next state

The result of executing a guard condition shall be whether the condition was satisfied or not.


#### State transition's action

A state transition's action shall have the following context when executed:

* event that triggered the transition
* name of the current state
* name of the next state


### Internal transitions

It shall be possible to add internal transitions to a state. An internal transition shall have:

* a trigger (name of the event that triggers the transition)
* a guard condition (optional)
* an action

Workflow for executing internal transitions:

![Internal transition workflow](Diagrams/FlowCharts/InternalTransitionWorkflow.svg "Internal transition workflow")


#### Internal transition's guard condition

An internal transition's guard condition shall have the following context when executed:

* event that triggered the transition
* name of the current state

The result of executing a guard condition shall be whether the condition was satisfied or not.


#### Internal transition's action

An internal transition's action shall have the following context when executed:

* event that triggered the transition
* name of the current state


### Default transition

It shall be possible to set a single state or internal transition (but not both!) that shall be
executed only in case the event that is being processes does not trigger any of the state and
internal transitions.

Workflow for executing default transitions shall be the same as with its non-default counterpart (an
ordinary state or internal transition).


### Final states

If a state has no state, internal, nor default transitions it shall be treated as a final state. A
final state can have an entry action, but not an exit action. The state machine shall be stopped on
entry to a final state and the event that triggered the transition shall be stored and exposed to
the user of the state machine.

It shall be possible to have zero or more final states in a state machine.


## Validation

A state machine shall be valid if:

* it has at least one state
* the initial transition is set
* all transitions reference existing states
* all internal transitions have actions
* all state and internal transitions have a valid trigger (non-empty event name)
* at most one of the default transitions (state or internal) is set in each state
* final states do not have their state or exit action set
* all states of the state machine can be reached

The validation shall be done as soon as it makes sense:

* on adding a state the state's name shall be checked if it's empty or if a state with the same name
  already exists
* on setting the initial transition the initial state shall reference an existing state
* on adding a state transition the trigger name shall be checked if it's empty and if the referenced
  states exist
* on adding an internal transition the trigger name shall be checked if it's empty and if the action
  was provided
* everything else shall be checked with the validation procedure


## Event processing

It shall be possible to add events to the state machine's event queue, either to the front (next
pending event) or the back (last pending event), but only if the state machine is started. It shall
be possible to query if there is at least one pending event in the event queue.

The events shall not be processed immediately, event processing shall need to be triggered
explicitly by the user of the state machine.

Events shall be processed one at a time by taking the next pending event from the event queue and
using it to trigger a transition if possible. The event is available and passed to all guard
conditions and transition and state actions that are executed during the complete transition
workflow.

The startup procedure (initial transition) and event processing (state transitions) shall be the
only ways to change the state of the state machine.

Events shall be processed using the following workflows:

![Transition workflow](Diagrams/FlowCharts/TransitionWorkflow.svg "Transition workflow")


## Polling

It shall be possible to poll the state machine. Each time the state machine is polled it shall
execute all pending events and then execute the current state's state action (if it is set).

![Polling workflow](Diagrams/FlowCharts/PollingWorkflow.svg "Polling workflow")


## Startup procedure

For a user to start the state machine they shall have to pass it an event that shall be used for the
initial transition.

On start the state machine shall clear/reset its operational status (for example the event queue,
current state, final event) and execute the initial transition as shown in this workflow:

![Initial transition workflow](Diagrams/FlowCharts/InitialTransitionWorkflow.svg "Initial transition workflow")


## Shutdown procedure

A user shall be able to stop a stated state machine at any time except when it's processing an
event.

On shutdown the state machine shall keep the operation status as it is, but it shall no longer allow
adding new events to the event queue and processing existing events.

The state machine shall also be shutdown automatically on entering one of the final states:

![Final transition workflow](Diagrams/FlowCharts/FinalTransitionWorkflow.svg "Final transition workflow")
