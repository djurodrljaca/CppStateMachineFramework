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
 * Contains unit tests for the Event class
 */

// C++ State Machine Framework includes
#include <CppStateMachineFramework/Event.hpp>

// Qt includes
#include <QtCore/QDebug>
#include <QtTest/QTest>

// System includes

// Forward declarations

// Macros

// Test class declaration --------------------------------------------------------------------------

using namespace CppStateMachineFramework;

class TestEvent : public QObject
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
    void testConstructor();
    void testCopy();
    void testMove();
    void testCreate();
};

// Test Case init/cleanup methods ------------------------------------------------------------------

void TestEvent::initTestCase()
{
}

void TestEvent::cleanupTestCase()
{
}

// Test init/cleanup methods -----------------------------------------------------------------------

void TestEvent::init()
{
}

void TestEvent::cleanup()
{
}

// Test: Constructor -------------------------------------------------------------------------------

void TestEvent::testConstructor()
{
    const QString name("test");

    QCOMPARE(Event(name).name(), name);
}

// Test: Copy constructor and copy assignment operator ---------------------------------------------

void TestEvent::testCopy()
{
    const QString name1("test1");
    const QString name2("test2");

    // Copy constructor
    const Event source1(name1);
    Event event(source1);
    QCOMPARE(event.name(), name1);

    // Copy assignment operator
    const Event source2(name2);
    event = source2;
    QCOMPARE(event.name(), name2);
}

// Test: Move constructor and move assignment operator ---------------------------------------------

void TestEvent::testMove()
{
    const QString name1("test1");
    const QString name2("test2");

    // Move constructor
    Event event((Event(name1)));
    QCOMPARE(event.name(), name1);

    // Move assignment operator
    event = Event(name2);
    QCOMPARE(event.name(), name2);
}

// Test: create() method ---------------------------------------------------------------------------

void TestEvent::testCreate()
{
    const QString name("test");

    const std::unique_ptr<Event> event = Event::create(name);

    QVERIFY(event);
    QCOMPARE(event->name(), name);
}

// Main function -----------------------------------------------------------------------------------

QTEST_MAIN(TestEvent)
#include "testEvent.moc"
