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

    //! Copy constructor
    Event(const Event &) = default;

    //! Move constructor
    Event(Event &&) noexcept = default;

    //! Destructor
    virtual ~Event() = default;

    //! Copy assignment operator
    Event &operator=(const Event &) = default;

    //! Move assignment operator
    Event &operator=(Event &&) noexcept = default;

    //! Gets the event's name
    QString name() const;

    /*! Creates an instance of an event
     *
     * \param   name    Event name
     *
     * \return  Event instance
     */
    static std::unique_ptr<Event> create(const QString &name);

private:
    //! Event's name
    QString m_name;
};

// -------------------------------------------------------------------------------------------------

/*! This class holds the event with data
 *
 * \tparam  T   Data type of the event's data
 */
template<typename T>
class EventWithData : public Event
{
public:
    /*!
     * Constructor
     *
     * \param   name    Event name
     * \param   data    Event data
     */
    EventWithData(const QString &name, const T &data)
        : Event(name),
          m_data(data)
    {
        static_assert(std::is_copy_constructible<T>::value, "Data cannot be copied");
    }

    /*!
     * Constructor
     *
     * \param   name    Event name
     * \param   data    Event data
     */
    EventWithData(const QString &name, T &&data)
        : Event(name),
          m_data(std::move(data))
    {
        static_assert(std::is_move_constructible<T>::value, "Data cannot be moved");
    }

    //! Copy constructor
    EventWithData(const EventWithData &other) = default;

    //! Move constructor
    EventWithData(EventWithData &&other) noexcept = default;

    //! Destructor
    ~EventWithData() override = default;

    //! Copy assignment operator
    EventWithData &operator=(const EventWithData &other) = default;

    //! Move assignment operator
    EventWithData &operator=(EventWithData &&other) noexcept = default;

    //! Gets the event's data
    const T &data() const
    {
        return m_data;
    }

    //! Gets the event's data
    T &data()
    {
        return m_data;
    }

    /*! Creates an instance of an event
     *
     * \param   name    Event name
     * \param   data    Event data
     *
     * \return  Event instance
     */
    static std::unique_ptr<EventWithData<T>> create(const QString &name, const T &data)
    {
        static_assert(std::is_copy_constructible<T>::value, "Data cannot be copied");

        return std::make_unique<EventWithData<T>>(name, data);
    }

    /*! Creates an instance of an event
     *
     * \param   name    Event name
     * \param   data    Event data
     *
     * \return  Event instance
     */
    static std::unique_ptr<EventWithData<T>> create(const QString &name, T &&data)
    {
        static_assert(std::is_move_constructible<T>::value, "Data cannot be moved");

        return std::make_unique<EventWithData<T>>(name, std::move(data));
    }

private:
    //! Event's data
    T m_data;
};

} // namespace CppStateMachineFramework
