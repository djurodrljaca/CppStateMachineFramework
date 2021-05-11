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
 * Contains an interface for a state object
 */

#pragma once

// C++ State Machine Framework includes
#include <CppStateMachineFramework/Event.hpp>

// Qt includes
#include <QtCore/QString>

// System includes
#include <memory>

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

namespace CppStateMachineFramework
{

//! This is a convenience interface for making state objects
class CPPSTATEMACHINEFRAMEWORK_EXPORT IState
{
public:
    //! Destructor
    virtual ~IState() = default;

    /*!
     * Gets the state's name
     *
     * \return  State name
     */
    virtual QString stateName() const = 0;

    /*!
     * Executes the state's entry method
     *
     * \param   event       Event that triggered the transition to the state
     * \param   fromState   Name of the state transitioned from
     * \param   toState     Name of the state transitioning to
     */
    virtual void stateEntryMethod(const Event &event,
                                  const QString &fromState,
                                  const QString &toState) = 0;

    /*!
     * Executes the state's exit method
     *
     * \param   event       Event that triggered the transition to the state
     * \param   fromState   Name of the state to transitioning from
     * \param   toState     Name of the state to transition to
     */
    virtual void stateExitMethod(const Event &event,
                                 const QString &fromState,
                                 const QString &toState) = 0;
};

} // namespace CppStateMachineFramework
