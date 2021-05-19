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

#pragma once

// C++ State Machine Framework includes
#include <CppStateMachineFramework/CppStateMachineFrameworkExport.hpp>

// Qt includes
#include <QtCore/QString>

// System includes
#include <memory>

// Forward declarations

// Macros

// -------------------------------------------------------------------------------------------------

namespace CppStateMachineFramework
{

//! This is an interface for an event parameter
class CPPSTATEMACHINEFRAMEWORK_EXPORT IEventParameter
{
public:
    //! Destructor
    virtual ~IEventParameter() = default;
};

// -------------------------------------------------------------------------------------------------

//! This class holds the event
class CPPSTATEMACHINEFRAMEWORK_EXPORT Event
{
public:
    /*!
     * Constructor
     *
     * \param   name    Event name
     */
    Event(const QString &name);

    /*!
     * Constructor
     *
     * \param   name        Event name
     * \param   parameter   Event parameter
     */
    Event(const QString &name, std::unique_ptr<IEventParameter> parameter);

    //! Copy constructor is disabled
    Event(const Event &) = delete;

    //! Move constructor
    Event(Event &&) noexcept = default;

    //! Destructor
    virtual ~Event() = default;

    //! Copy assignment operator is disabled
    Event &operator=(const Event &) = delete;

    //! Move assignment operator
    Event &operator=(Event &&) noexcept = default;

    //! Gets the event's name
    QString name() const;

    /*!
     * Checks if the event has a parameter
     *
     * \retval  true    Event has a parameter
     * \retval  false   Event does not have a parameter
     */
    bool hasParameter() const;

    //! Gets the event's parameter
    const IEventParameter *parameter() const;

    //! Gets the event's parameter
    template<typename T>
    const T *parameter() const
    {
        static_assert(std::is_base_of<IEventParameter, T>::value,
                      "T must be derived from IEventParameter");

        return dynamic_cast<const T*>(m_parameter.get());
    }

    //! Gets the event's parameter
    IEventParameter *parameter();

    //! Gets the event's parameter
    template<typename T>
    T *parameter()
    {
        static_assert(std::is_base_of<IEventParameter, T>::value,
                      "T must be derived from IEventParameter");

        return dynamic_cast<T*>(m_parameter.get());
    }

private:
    //! Event's name
    QString m_name;

    //! Event's name
    std::unique_ptr<IEventParameter> m_parameter;
};

// -------------------------------------------------------------------------------------------------

/*! This class holds the event parameter value
 *
 * \tparam  T   Data type of the event parameter's value
 */
template<typename T>
class EventParameter : public IEventParameter
{
public:
    /*!
     * Constructor
     *
     * \param   value   Event parameter's value
     */
    EventParameter(const T &value)
        : IEventParameter(),
          m_value(value)
    {
        static_assert(std::is_copy_constructible<T>::value, "Value cannot be copied");
    }

    /*!
     * Constructor
     *
     * \param   value   Event parameter's value
     */
    EventParameter(T &&value)
        : IEventParameter(),
          m_value(std::move(value))
    {
        static_assert(std::is_move_constructible<T>::value, "Value cannot be moved");
    }

    //! Copy constructor
    EventParameter(const EventParameter &other) = default;

    //! Move constructor
    EventParameter(EventParameter &&other) noexcept = default;

    //! Destructor
    ~EventParameter() override = default;

    //! Copy assignment operator
    EventParameter &operator=(const EventParameter &other) = default;

    //! Move assignment operator
    EventParameter &operator=(EventParameter &&other) noexcept = default;

    //! Gets the event's data
    const T &value() const
    {
        return m_value;
    }

    //! Gets the event parameter's value
    T &value()
    {
        return m_value;
    }

    /*!
     * Creates an instance of an event parameter
     *
     * \param   value   Event parameter's value
     *
     * \return  Event parameter instance
     */
    static std::unique_ptr<EventParameter<T>> create(const T &value)
    {
        static_assert(std::is_copy_constructible<T>::value, "Value cannot be copied");

        return std::make_unique<EventParameter<T>>(value);
    }

    /*!
     * Creates an instance of an event parameter
     *
     * \param   value   Event parameter's value
     *
     * \return  Event parameter instance
     */
    static std::unique_ptr<EventParameter<T>> create(T &&value)
    {
        static_assert(std::is_move_constructible<T>::value, "Value cannot be moved");

        return std::make_unique<EventParameter<T>>(std::move(value));
    }

private:
    //! Holds the value
    T m_value;
};

} // namespace CppStateMachineFramework
