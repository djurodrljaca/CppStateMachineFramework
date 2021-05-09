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
 * Contains unit tests for the EventWithData class
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

static const QString s_name1("test1");
static const QString s_name2("test2");

using IntEvent = EventWithData<int>;
static const int s_intValue1 = 1;
static const int s_intValue2 = 2;

using StringEvent = EventWithData<QString>;
static const QString s_stringValue1("value1");
static const QString s_stringValue2("value2");

using PtrEvent = EventWithData<std::unique_ptr<int>>;

class TestEventWithData : public QObject
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

void TestEventWithData::initTestCase()
{
}

void TestEventWithData::cleanupTestCase()
{
}

// Test init/cleanup methods -----------------------------------------------------------------------

void TestEventWithData::init()
{
}

void TestEventWithData::cleanup()
{
}

// Test: Constructor -------------------------------------------------------------------------------

void TestEventWithData::testConstructor()
{
    // Integer
    IntEvent intEvent(s_name1, s_intValue1);
    QCOMPARE(intEvent.name(), s_name1);
    QCOMPARE(intEvent.data(), s_intValue1);

    // String
    StringEvent stringEvent(s_name1, s_stringValue1);
    QCOMPARE(stringEvent.name(), s_name1);
    QCOMPARE(stringEvent.data(), s_stringValue1);

    // Unique pointer
    PtrEvent ptrEvent(s_name1, std::make_unique<int>(s_intValue1));
    QCOMPARE(ptrEvent.name(), s_name1);
    QVERIFY(ptrEvent.data());
    QCOMPARE(*ptrEvent.data(), s_intValue1);
}

// Test: Copy constructor and copy assignment operator ---------------------------------------------

void TestEventWithData::testCopy()
{
    // Integer: copy constructor
    const IntEvent intEventSource1(s_name1, s_intValue1);
    IntEvent intEvent(intEventSource1);
    QCOMPARE(intEvent.name(), s_name1);
    QCOMPARE(intEvent.data(), s_intValue1);

    // Integer: copy assignment operator
    const IntEvent intEventSource2(s_name2, s_intValue2);
    intEvent = intEventSource2;
    QCOMPARE(intEvent.name(), s_name2);
    QCOMPARE(intEvent.data(), s_intValue2);

    // String: copy constructor
    const StringEvent stringEventSource1(s_name1, s_stringValue1);
    StringEvent stringEvent(stringEventSource1);
    QCOMPARE(stringEvent.name(), s_name1);
    QCOMPARE(stringEvent.data(), s_stringValue1);

    // String: copy assignment operator
    const StringEvent stringEventSource2(s_name2, s_stringValue2);
    stringEvent = stringEventSource2;
    QCOMPARE(stringEvent.name(), s_name2);
    QCOMPARE(stringEvent.data(), s_stringValue2);

    // Unique pointer: copy constructor not available
    // Unique pointer: copy assignment operator not available
}

// Test: Move constructor and move assignment operator ---------------------------------------------

void TestEventWithData::testMove()
{
    // Integer: move constructor
    IntEvent intEvent((IntEvent(s_name1, s_intValue1)));
    QCOMPARE(intEvent.name(), s_name1);
    QCOMPARE(intEvent.data(), s_intValue1);

    // Integer: move assignment operator
    intEvent = IntEvent(s_name2, s_intValue2);
    QCOMPARE(intEvent.name(), s_name2);
    QCOMPARE(intEvent.data(), s_intValue2);

    // String: move constructor
    StringEvent stringEvent(StringEvent(s_name1, s_stringValue1));
    QCOMPARE(stringEvent.name(), s_name1);
    QCOMPARE(stringEvent.data(), s_stringValue1);

    // String: move assignment operator
    stringEvent = StringEvent(s_name2, s_stringValue2);
    QCOMPARE(stringEvent.name(), s_name2);
    QCOMPARE(stringEvent.data(), s_stringValue2);

    // Unique pointer: move constructor
    PtrEvent ptrEvent(PtrEvent(s_name1, std::make_unique<int>(s_intValue1)));
    QCOMPARE(ptrEvent.name(), s_name1);
    QVERIFY(ptrEvent.data());
    QCOMPARE(*ptrEvent.data(), s_intValue1);

    // Unique pointer: move assignment operator
    ptrEvent = PtrEvent(s_name2, std::make_unique<int>(s_intValue2));
    QCOMPARE(ptrEvent.name(), s_name2);
    QVERIFY(ptrEvent.data());
    QCOMPARE(*ptrEvent.data(), s_intValue2);
}

// Test: create() method ---------------------------------------------------------------------------

void TestEventWithData::testCreate()
{
    // Integer
    const auto intEvent = IntEvent::create(s_name1, s_intValue1);
    QVERIFY(intEvent);
    QCOMPARE(intEvent->name(), s_name1);
    QCOMPARE(intEvent->data(), s_intValue1);

    // String
    const auto stringEvent = StringEvent::create(s_name1, s_stringValue1);
    QVERIFY(stringEvent);
    QCOMPARE(stringEvent->name(), s_name1);
    QCOMPARE(stringEvent->data(), s_stringValue1);

    // Unique pointer
    const auto ptrEvent = PtrEvent::create(s_name1, std::make_unique<int>(s_intValue1));
    QVERIFY(ptrEvent);
    QCOMPARE(ptrEvent->name(), s_name1);
    QVERIFY(ptrEvent->data());
    QCOMPARE(*ptrEvent->data(), s_intValue1);
}

// Main function -----------------------------------------------------------------------------------

QTEST_MAIN(TestEventWithData)
#include "testEventWithData.moc"
