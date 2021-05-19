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

static const QString s_name1("test1");
static const QString s_name2("test2");

using IntEventParameter = EventParameter<int>;
static const int s_intValue1 = 1;
static const int s_intValue2 = 2;

using StringEventParameter = EventParameter<QString>;
static const QString s_stringValue("value");

using PtrEventParameter = EventParameter<std::unique_ptr<int>>;

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
    void testMove();
    void testEventParameter();
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
    Event event1(s_name1);
    Event event2(s_name2, IntEventParameter::create(123));

    QCOMPARE(event1.name(), s_name1);
    QCOMPARE(event2.name(), s_name2);

    QVERIFY(!event1.hasParameter());
    QVERIFY(event2.hasParameter());

    QVERIFY(qAsConst(event1).parameter() == nullptr);
    QVERIFY(event1.parameter() == nullptr);
    QVERIFY(event1.parameter<StringEventParameter>() == nullptr);

    const auto *param2 = event2.parameter<IntEventParameter>();
    QVERIFY(param2 != nullptr);
    QCOMPARE(param2->value(), 123);

    QVERIFY(qAsConst(event2).parameter() != nullptr);
    QVERIFY(event2.parameter() != nullptr);
    QVERIFY(event2.parameter<StringEventParameter>() == nullptr);
}

// Test: Move constructor and move assignment operator ---------------------------------------------

void TestEvent::testMove()
{
    // Move constructor
    Event moved1((Event(s_name1)));
    QCOMPARE(moved1.name(), s_name1);
    QVERIFY(!moved1.hasParameter());

    Event moved2((Event(s_name2, IntEventParameter::create(s_intValue2))));
    QCOMPARE(moved2.name(), s_name2);
    QVERIFY(moved2.hasParameter());

    const auto *param2 = moved2.parameter<IntEventParameter>();
    QVERIFY(param2 != nullptr);
    QCOMPARE(param2->value(), s_intValue2);

    // Move assignment operator
    moved1 = Event(s_name2, IntEventParameter::create(s_intValue1));
    QCOMPARE(moved1.name(), s_name2);
    QVERIFY(moved1.hasParameter());

    const auto *param1 = moved1.parameter<IntEventParameter>();
    QVERIFY(param1 != nullptr);
    QCOMPARE(param1->value(), s_intValue1);

    moved2 = Event(s_name1);
    QCOMPARE(moved2.name(), s_name1);
    QVERIFY(!moved2.hasParameter());
}

// Test: Events with parameters --------------------------------------------------------------------

void TestEvent::testEventParameter()
{
    // Integer
    Event intEvent(s_name1, IntEventParameter::create(s_intValue1));
    QCOMPARE(intEvent.name(), s_name1);
    QVERIFY(intEvent.hasParameter());
    QVERIFY(intEvent.parameter<IntEventParameter>() != nullptr);
    QCOMPARE(intEvent.parameter<IntEventParameter>()->value(), s_intValue1);

    // String
    Event stringEvent(s_name1, StringEventParameter::create(s_stringValue));
    QCOMPARE(stringEvent.name(), s_name1);
    QVERIFY(stringEvent.hasParameter());
    QVERIFY(stringEvent.parameter<StringEventParameter>() != nullptr);
    QCOMPARE(stringEvent.parameter<StringEventParameter>()->value(), s_stringValue);

    // Unique pointer
    Event ptrEvent(s_name1, PtrEventParameter::create(std::make_unique<int>(s_intValue1)));
    QCOMPARE(ptrEvent.name(), s_name1);
    QVERIFY(ptrEvent.hasParameter());
    QVERIFY(ptrEvent.parameter<PtrEventParameter>() != nullptr);
    QVERIFY(ptrEvent.parameter<PtrEventParameter>()->value().get() != nullptr);
    QCOMPARE(*(ptrEvent.parameter<PtrEventParameter>()->value()), s_intValue1);
}

// Main function -----------------------------------------------------------------------------------

QTEST_MAIN(TestEvent)
#include "testEvent.moc"
