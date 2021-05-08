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
 * Contains a class for an event
 */

// Own header
#include <CppStateMachineFramework/Event.hpp>

// C++ State Machine Framework includes

// Qt includes

// System includes

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

namespace CppStateMachineFramework
{

Event::Event(const QString &name)
    : m_name(name)
{
}

// -------------------------------------------------------------------------------------------------

QString Event::name() const
{
    return m_name;
}

// -------------------------------------------------------------------------------------------------

std::unique_ptr<Event> Event::create(const QString &name)
{
    return std::make_unique<Event>(name);
}

} // namespace CppStateMachineFramework
