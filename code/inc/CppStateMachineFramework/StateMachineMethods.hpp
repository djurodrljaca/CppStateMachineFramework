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
 * Contains type definitions and helper methods for state machine actions and guard conditions
 */

#pragma once

// C++ State Machine Framework includes
#include <CppStateMachineFramework/Event.hpp>

// Qt includes
#include <functional>

// System includes

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

namespace CppStateMachineFramework
{

/*!
 * Type alias for an initial transition action method
 *
 * \param   trigger         Event that triggered the transition
 * \param   initialState    Name of the initial state
 */
using InitialTransitionAction = std::function<void(const Event &trigger,
                                                   const QString &initialState)>;

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

/*!
 * Type alias for a state action method
 *
 * \param   currentState    Name of the current state
 */
using StateAction = std::function<void(const QString &currentState)>;

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

/*!
 * Type alias for an internal transition action method
 *
 * \param   trigger         Event that triggered the transition
 * \param   currentState    Name of the current state
 */
using InternalTransitionAction = std::function<void(const Event &trigger,
                                                    const QString &currentState)>;

// -------------------------------------------------------------------------------------------------

//! Helper method for creating initial transition actions
template<typename T>
InitialTransitionAction createInitialTransitionAction(
        T *instance, void (T::*method)(const Event &, const QString &))
{
    return std::bind(method, instance, std::placeholders::_1, std::placeholders::_2);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating initial transition actions
template<typename T>
InitialTransitionAction createInitialTransitionAction(T *instance, void (T::*method)())
{
    return std::bind(method, instance);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state entry actions
template<typename T>
StateEntryAction createStateEntryAction(
        T *instance, void (T::*method)(const Event &, const QString &, const QString &))
{
    return std::bind(method,
                     instance,
                     std::placeholders::_1,
                     std::placeholders::_2,
                     std::placeholders::_3);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state entry actions
template<typename T>
StateEntryAction createStateEntryAction(T *instance, void (T::*method)())
{
    return std::bind(method, instance);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state actions
template<typename T>
StateAction createStateAction(T *instance, void (T::*method)(const QString &))
{
    return std::bind(method, instance, std::placeholders::_1);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state actions
template<typename T>
StateAction createStateAction(T *instance, void (T::*method)())
{
    return std::bind(method, instance);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state exit actions
template<typename T>
StateExitAction createStateExitAction(
        T *instance, void (T::*method)(const Event &, const QString &, const QString &))
{
    return std::bind(method,
                     instance,
                     std::placeholders::_1,
                     std::placeholders::_2,
                     std::placeholders::_3);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state exit actions
template<typename T>
StateExitAction createStateExitAction(T *instance, void (T::*method)())
{
    return std::bind(method, instance);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state transition guard conditions
template<typename T>
StateTransitionGuardCondition createStateTransitionGuardCondition(
        T *instance, bool (T::*method)(const Event &, const QString &, const QString &))
{
    return std::bind(method,
                     instance,
                     std::placeholders::_1,
                     std::placeholders::_2,
                     std::placeholders::_3);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state transition guard conditions
template<typename T>
StateTransitionGuardCondition createStateTransitionGuardCondition(T *instance, bool (T::*method)())
{
    return std::bind(method, instance);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state transition actions
template<typename T>
StateTransitionAction createStateTransitionAction(
        T *instance, void (T::*method)(const Event &, const QString &, const QString &))
{
    return std::bind(method,
                     instance,
                     std::placeholders::_1,
                     std::placeholders::_2,
                     std::placeholders::_3);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating state transition actions
template<typename T>
StateTransitionAction createStateTransitionAction(T *instance, void (T::*method)())
{
    return std::bind(method, instance);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating internal transition guard conditions
template<typename T>
InternalTransitionGuardCondition createInternalTransitionGuardCondition(
        T *instance, bool (T::*method)(const Event &, const QString &))
{
    return std::bind(method, instance, std::placeholders::_1, std::placeholders::_2);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating internal transition guard conditions
template<typename T>
InternalTransitionGuardCondition createInternalTransitionGuardCondition(
        T *instance, bool (T::*method)())
{
    return std::bind(method, instance);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating internal transition actions
template<typename T>
InternalTransitionAction createInternalTransitionAction(
        T *instance, void (T::*method)(const Event &, const QString &))
{
    return std::bind(method, instance, std::placeholders::_1, std::placeholders::_2);
}

// -------------------------------------------------------------------------------------------------

//! Helper method for creating internal transition actions
template<typename T>
InternalTransitionAction createInternalTransitionAction(T *instance, void (T::*method)())
{
    return std::bind(method, instance);
}

} // namespace CppStateMachineFramework
