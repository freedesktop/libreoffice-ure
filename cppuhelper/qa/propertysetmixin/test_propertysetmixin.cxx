/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: test_propertysetmixin.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2005-10-27 17:18:26 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#include "sal/config.h"

#include "test/cppuhelper/propertysetmixin/CppSupplier.hpp"
#include "test/cppuhelper/propertysetmixin/JavaSupplier.hpp"
#include "test/cppuhelper/propertysetmixin/XSupplier.hpp"
#include "test/cppuhelper/propertysetmixin/XTest3.hpp"

#include "com/sun/star/beans/Ambiguous.hpp"
#include "com/sun/star/beans/Defaulted.hpp"
#include "com/sun/star/beans/Optional.hpp"
#include "com/sun/star/beans/Property.hpp"
#include "com/sun/star/beans/PropertyAttribute.hpp"
#include "com/sun/star/beans/PropertyChangeEvent.hpp"
#include "com/sun/star/beans/PropertyState.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/beans/PropertyVetoException.hpp"
#include "com/sun/star/beans/UnknownPropertyException.hpp"
#include "com/sun/star/beans/XFastPropertySet.hpp"
#include "com/sun/star/beans/XPropertyAccess.hpp"
#include "com/sun/star/beans/XPropertyChangeListener.hpp"
#include "com/sun/star/beans/XPropertySet.hpp"
#include "com/sun/star/beans/XPropertySetInfo.hpp"
#include "com/sun/star/beans/XVetoableChangeListener.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "cppuhelper/implbase1.hxx"
#include "cppuhelper/servicefactory.hxx"
#include "cppunit/simpleheader.hxx"
#include "osl/mutex.hxx"
#include "osl/thread.h"
#include "rtl/ref.hxx"
#include "rtl/string.h"
#include "rtl/textenc.h"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"

#include <limits>
#include <ostream>

namespace com { namespace sun { namespace star {
    struct EventObject;
} } }

namespace css = com::sun::star;

namespace {

std::ostream & operator <<(std::ostream & out, rtl::OUString const & value) {
    return out << rtl::OUStringToOString(value, RTL_TEXTENCODING_UTF8).getStr();
}

std::ostream & operator <<(std::ostream & out, css::uno::Type const & value) {
    return out << "com::sun::star::uno::Type[" << value.getTypeName() << ']';
}

std::ostream & operator <<(std::ostream & out, css::uno::Any const & value) {
    return
        out << "com::sun::star::uno::Any[" << value.getValueType() << ", ...]";
}

class BoundListener:
    public cppu::WeakImplHelper1< css::beans::XPropertyChangeListener >
{
public:
    BoundListener(): m_count(0) {}

    int count() const {
        osl::MutexGuard g(m_mutex);
        return m_count;
    }

    virtual void SAL_CALL disposing(css::lang::EventObject const &)
        throw (css::uno::RuntimeException)
    {
        osl::MutexGuard g(m_mutex);
        CPPUNIT_ASSERT(m_count < std::numeric_limits< int >::max());
        ++m_count;
    }

    virtual void SAL_CALL propertyChange(
        css::beans::PropertyChangeEvent const &)
        throw (css::uno::RuntimeException)
    { CPPUNIT_FAIL("BoundListener::propertyChange called"); }

private:
    BoundListener(BoundListener &); // not defined
    void operator =(BoundListener &); // not defined

    virtual ~BoundListener() {}

    mutable osl::Mutex m_mutex;
    int m_count;
};

class VetoListener:
    public cppu::WeakImplHelper1< css::beans::XVetoableChangeListener >
{
public:
    VetoListener(): m_count(0) {}

    int count() const {
        osl::MutexGuard g(m_mutex);
        return m_count;
    }

    virtual void SAL_CALL disposing(css::lang::EventObject const &)
        throw (css::uno::RuntimeException)
    {
        osl::MutexGuard g(m_mutex);
        CPPUNIT_ASSERT(m_count < std::numeric_limits< int >::max());
        ++m_count;
    }

    virtual void SAL_CALL vetoableChange(
        css::beans::PropertyChangeEvent const &)
        throw (css::beans::PropertyVetoException, css::uno::RuntimeException)
    { CPPUNIT_FAIL("VetoListener::vetoableChange called"); }

private:
    VetoListener(VetoListener &); // not defined
    void operator =(VetoListener &); // not defined

    virtual ~VetoListener() {}

    mutable osl::Mutex m_mutex;
    int m_count;
};

class Test: public CppUnit::TestFixture {
public:
    virtual void setUp();

    void finish();

    void testCppEmpty1() { testEmpty1(getCppSupplier()); }

    void testCppEmpty2() { testEmpty2(getCppSupplier()); }

    void testCppFull() { testFull(getCppSupplier()); }

    void testJavaEmpty1() { testEmpty1(getJavaSupplier()); }

    void testJavaEmpty2() { testEmpty2(getJavaSupplier()); }

    void testJavaFull() { testFull(getJavaSupplier()); }

    CPPUNIT_TEST_SUITE(Test);
    CPPUNIT_TEST(testCppEmpty1);
    CPPUNIT_TEST(testCppEmpty2);
    CPPUNIT_TEST(testCppFull);
    CPPUNIT_TEST(testJavaEmpty1);
    CPPUNIT_TEST(testJavaEmpty2);
    CPPUNIT_TEST(testJavaFull);
    CPPUNIT_TEST(finish);
    CPPUNIT_TEST_SUITE_END();

private:
    css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
    getCppSupplier() const;

    css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
    getJavaSupplier() const;

    void testEmpty1(
        css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
        const & supplier) const;

    void testEmpty2(
        css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
        const & supplier) const;

    void testFull(
        css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
        const & supplier) const;

    static css::uno::Reference< css::uno::XComponentContext > m_context;
};

void Test::setUp() {
    // For whatever reason, on W32 it does not work to create/destroy a fresh
    // component context for each test in Test::setUp/tearDown; therefore, a
    // single component context is used for all tests and destroyed in the last
    // pseudo-test "finish":
    if (!m_context.is()) {
        char const * fw = getForwardString();
        rtl::OUString forward(fw, rtl_str_getLength(fw),
                         osl_getThreadTextEncoding());
            //TODO: check for string conversion failure
        sal_Int32 index = forward.indexOf('#');
        rtl::OUString registry = forward.copy(0, index);
        rtl::OUString bootstrappath = forward.copy(index+1);

        css::uno::Reference< css::lang::XMultiComponentFactory > factory(
            cppu::createRegistryServiceFactory(
                registry, sal_False, bootstrappath),
            css::uno::UNO_QUERY_THROW);
        css::uno::Reference< css::beans::XPropertySet >(
            factory, css::uno::UNO_QUERY_THROW)->getPropertyValue(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DefaultContext")))
            >>= m_context;
    }
}

void Test::finish() {
    css::uno::Reference< css::lang::XComponent >(
        m_context, css::uno::UNO_QUERY_THROW)->dispose();
}

css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
Test::getCppSupplier() const
{
    return test::cppuhelper::propertysetmixin::CppSupplier::create(m_context);
}

css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
Test::getJavaSupplier() const
{
    return test::cppuhelper::propertysetmixin::JavaSupplier::create(m_context);
}

void Test::testEmpty1(
    css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
    const & supplier) const
{
    css::uno::Reference< css::lang::XComponent > empty1(
        supplier->getEmpty1(), css::uno::UNO_QUERY_THROW);
    CPPUNIT_ASSERT(
        !css::uno::Reference< css::beans::XPropertySet >(
            empty1, css::uno::UNO_QUERY).is());
    CPPUNIT_ASSERT(
        !css::uno::Reference< css::beans::XFastPropertySet >(
            empty1, css::uno::UNO_QUERY).is());
    CPPUNIT_ASSERT(
        !css::uno::Reference< css::beans::XPropertyAccess >(
            empty1, css::uno::UNO_QUERY).is());
    empty1->dispose();
}

void Test::testEmpty2(
    css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
    const & supplier) const
{
    css::uno::Reference< css::lang::XComponent > empty2(
        supplier->getEmpty2(), css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::beans::XPropertySet > empty2p(
        empty2, css::uno::UNO_QUERY);
    CPPUNIT_ASSERT(empty2p.is());
    css::uno::Reference< css::beans::XPropertySetInfo > info(
        empty2p->getPropertySetInfo());
    CPPUNIT_ASSERT(info.is());
    CPPUNIT_ASSERT_EQUAL(
        static_cast< sal_Int32 >(0), info->getProperties().getLength());
    try {
        info->getPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("any")));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    CPPUNIT_ASSERT(
        !info->hasPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("any"))));
    try {
        empty2p->setPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("any")), css::uno::Any());
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        empty2p->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("any")));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    rtl::Reference< BoundListener > boundListener1(new BoundListener);
    empty2p->addPropertyChangeListener(rtl::OUString(), boundListener1.get());
    empty2p->addPropertyChangeListener(rtl::OUString(), boundListener1.get());
    rtl::Reference< BoundListener > boundListener2(new BoundListener);
    empty2p->removePropertyChangeListener(
        rtl::OUString(), boundListener2.get());
    rtl::Reference< VetoListener > vetoListener1(new VetoListener);
    empty2p->addVetoableChangeListener(rtl::OUString(), vetoListener1.get());
    empty2p->addVetoableChangeListener(rtl::OUString(), vetoListener1.get());
    rtl::Reference< VetoListener > vetoListener2(new VetoListener);
    empty2p->addVetoableChangeListener(rtl::OUString(), vetoListener2.get());
    empty2p->removeVetoableChangeListener(rtl::OUString(), vetoListener2.get());
    css::uno::Reference< css::beans::XFastPropertySet > empty2f(
        empty2, css::uno::UNO_QUERY);
    CPPUNIT_ASSERT(empty2f.is());
    try {
        empty2f->setFastPropertyValue(-1, css::uno::Any());
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        empty2f->setFastPropertyValue(0, css::uno::Any());
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        empty2f->getFastPropertyValue(-1);
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        empty2f->getFastPropertyValue(0);
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    css::uno::Reference< css::beans::XPropertyAccess > empty2a(
        empty2, css::uno::UNO_QUERY);
    CPPUNIT_ASSERT(empty2a.is());
    CPPUNIT_ASSERT_EQUAL(
        static_cast< sal_Int32 >(0), empty2a->getPropertyValues().getLength());
    empty2a->setPropertyValues(
        css::uno::Sequence< css::beans::PropertyValue >());
    css::uno::Sequence< css::beans::PropertyValue > vs(2);
    vs[0].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("any1"));
    vs[0].Handle = -1;
    vs[0].State = css::beans::PropertyState_DIRECT_VALUE;
    vs[0].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("any2"));
    vs[0].Handle = -1;
    vs[0].State = css::beans::PropertyState_DIRECT_VALUE;
    try {
        empty2a->setPropertyValues(vs);
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    CPPUNIT_ASSERT_EQUAL(0, boundListener1->count());
    CPPUNIT_ASSERT_EQUAL(0, boundListener2->count());
    CPPUNIT_ASSERT_EQUAL(0, vetoListener1->count());
    CPPUNIT_ASSERT_EQUAL(0, vetoListener2->count());
    empty2->dispose();
    CPPUNIT_ASSERT_EQUAL(2, boundListener1->count());
    CPPUNIT_ASSERT_EQUAL(0, boundListener2->count());
    CPPUNIT_ASSERT_EQUAL(2, vetoListener1->count());
    CPPUNIT_ASSERT_EQUAL(0, vetoListener2->count());
    empty2p->removePropertyChangeListener(
        rtl::OUString(), boundListener1.get());
    empty2p->removePropertyChangeListener(
        rtl::OUString(), boundListener2.get());
    empty2p->removeVetoableChangeListener(rtl::OUString(), vetoListener1.get());
    empty2p->removeVetoableChangeListener(rtl::OUString(), vetoListener2.get());
    empty2p->addPropertyChangeListener(rtl::OUString(), boundListener1.get());
    empty2p->addPropertyChangeListener(rtl::OUString(), boundListener2.get());
    empty2p->addVetoableChangeListener(rtl::OUString(), vetoListener1.get());
    empty2p->addVetoableChangeListener(rtl::OUString(), vetoListener2.get());
    CPPUNIT_ASSERT_EQUAL(3, boundListener1->count());
    CPPUNIT_ASSERT_EQUAL(1, boundListener2->count());
    CPPUNIT_ASSERT_EQUAL(3, vetoListener1->count());
    CPPUNIT_ASSERT_EQUAL(1, vetoListener2->count());
}

void Test::testFull(
    css::uno::Reference< test::cppuhelper::propertysetmixin::XSupplier >
    const & supplier) const
{
    css::uno::Reference< test::cppuhelper::propertysetmixin::XTest3 > full(
        supplier->getFull(), css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::beans::XPropertySet > fullp(
        full, css::uno::UNO_QUERY);
    CPPUNIT_ASSERT(fullp.is());
    css::uno::Reference< css::beans::XPropertySetInfo > info(
        fullp->getPropertySetInfo());
    CPPUNIT_ASSERT(info.is());
    CPPUNIT_ASSERT_EQUAL(
        static_cast< sal_Int32 >(3), info->getProperties().getLength());
    css::beans::Property prop(
        info->getPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("First"))));
    CPPUNIT_ASSERT_EQUAL(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("First")), prop.Name);
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int32 >(0), prop.Handle);
    CPPUNIT_ASSERT_EQUAL(getCppuType(static_cast< sal_Int32 * >(0)), prop.Type);
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int16 >(0), prop.Attributes);
    prop = info->getPropertyByName(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second")));
    CPPUNIT_ASSERT_EQUAL(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second")), prop.Name);
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int32 >(1), prop.Handle);
    CPPUNIT_ASSERT_EQUAL(getCppuType(static_cast< sal_Int32 * >(0)), prop.Type);
    CPPUNIT_ASSERT_EQUAL(
        static_cast< sal_Int16 >(
            css::beans::PropertyAttribute::MAYBEVOID
            | css::beans::PropertyAttribute::BOUND
            | css::beans::PropertyAttribute::CONSTRAINED
            | css::beans::PropertyAttribute::MAYBEAMBIGUOUS
            | css::beans::PropertyAttribute::MAYBEDEFAULT
            | css::beans::PropertyAttribute::OPTIONAL),
        prop.Attributes);
    try {
        info->getPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Third")));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    prop = info->getPropertyByName(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Fourth")));
    CPPUNIT_ASSERT_EQUAL(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Fourth")), prop.Name);
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int32 >(3), prop.Handle);
    CPPUNIT_ASSERT_EQUAL(getCppuType(static_cast< sal_Int32 * >(0)), prop.Type);
    CPPUNIT_ASSERT_EQUAL(
        static_cast< sal_Int16 >(css::beans::PropertyAttribute::OPTIONAL),
        prop.Attributes);
    try {
        info->getPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("first")));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    CPPUNIT_ASSERT(
        info->hasPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("First"))));
    CPPUNIT_ASSERT(
        info->hasPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second"))));
    CPPUNIT_ASSERT(
        !info->hasPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Third"))));
    CPPUNIT_ASSERT(
        info->hasPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Fourth"))));
    CPPUNIT_ASSERT(
        !info->hasPropertyByName(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("first"))));
    CPPUNIT_ASSERT_EQUAL(
        css::uno::makeAny(static_cast< sal_Int32 >(0)),
        fullp->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("First"))));
    fullp->setPropertyValue(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("First")),
        css::uno::makeAny(static_cast< sal_Int32 >(-100)));
    CPPUNIT_ASSERT_EQUAL(
        css::uno::makeAny(static_cast< sal_Int32 >(-100)),
        fullp->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("First"))));
    css::uno::Any voidAny;
    CPPUNIT_ASSERT_EQUAL(
        voidAny,
        fullp->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second"))));
    fullp->setPropertyValue(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second")),
        css::uno::makeAny(static_cast< sal_Int32 >(100)));
    CPPUNIT_ASSERT_EQUAL(
        css::uno::makeAny(static_cast< sal_Int32 >(100)),
        fullp->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second"))));
    CPPUNIT_ASSERT(full->getSecond().Value.Value.IsPresent);
    CPPUNIT_ASSERT_EQUAL(
        static_cast< sal_Int32 >(100), full->getSecond().Value.Value.Value);
    CPPUNIT_ASSERT(!full->getSecond().Value.IsDefaulted);
    CPPUNIT_ASSERT(!full->getSecond().IsAmbiguous);
    fullp->setPropertyValue(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second")),
        css::uno::Any());
    CPPUNIT_ASSERT_EQUAL(
        voidAny,
        fullp->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second"))));
    CPPUNIT_ASSERT(!full->getSecond().Value.Value.IsPresent);
    CPPUNIT_ASSERT(!full->getSecond().Value.IsDefaulted);
    CPPUNIT_ASSERT(!full->getSecond().IsAmbiguous);
    try {
        fullp->setPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Third")),
            css::uno::makeAny(static_cast< sal_Int32 >(100)));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        fullp->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Third")));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        fullp->setPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Fourth")),
            css::uno::makeAny(static_cast< sal_Int32 >(100)));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        fullp->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Fourth")));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        fullp->setPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("first")),
            css::uno::Any());
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        fullp->getPropertyValue(
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("first")));
        CPPUNIT_FAIL("exception expected");
    } catch (css::beans::UnknownPropertyException &) {}
    css::uno::Reference< css::beans::XFastPropertySet > fullf(
        full, css::uno::UNO_QUERY);
    CPPUNIT_ASSERT(fullf.is());
    CPPUNIT_ASSERT_EQUAL(
        css::uno::makeAny(static_cast< sal_Int32 >(-100)),
        fullf->getFastPropertyValue(0));
    fullf->setFastPropertyValue(
        0, css::uno::makeAny(static_cast< sal_Int32 >(0)));
    CPPUNIT_ASSERT_EQUAL(
        css::uno::makeAny(static_cast< sal_Int32 >(0)),
        fullf->getFastPropertyValue(0));
    try {
        fullf->getFastPropertyValue(-1);
    } catch (css::beans::UnknownPropertyException &) {}
    try {
        fullf->setFastPropertyValue(-1, css::uno::Any());
    } catch (css::beans::UnknownPropertyException &) {}
    css::uno::Reference< css::beans::XPropertyAccess > fulla(
        full, css::uno::UNO_QUERY);
    CPPUNIT_ASSERT(fulla.is());
    css::uno::Sequence< css::beans::PropertyValue > vs(
        fulla->getPropertyValues());
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int32 >(2), vs.getLength());
    CPPUNIT_ASSERT_EQUAL(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("First")), vs[0].Name);
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int32 >(0), vs[0].Handle);
    CPPUNIT_ASSERT_EQUAL(
        css::uno::makeAny(static_cast< sal_Int32 >(0)), vs[0].Value);
    CPPUNIT_ASSERT_EQUAL(css::beans::PropertyState_DIRECT_VALUE, vs[0].State);
    CPPUNIT_ASSERT_EQUAL(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second")), vs[1].Name);
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int32 >(1), vs[1].Handle);
    CPPUNIT_ASSERT_EQUAL(voidAny, vs[1].Value);
    CPPUNIT_ASSERT_EQUAL(css::beans::PropertyState_DIRECT_VALUE, vs[1].State);
    vs[0].Value <<= static_cast< sal_Int32 >(-100);
    vs[1].Value <<= static_cast< sal_Int32 >(100);
    vs[1].State = css::beans::PropertyState_AMBIGUOUS_VALUE;
    fulla->setPropertyValues(vs);
    vs = fulla->getPropertyValues();
    CPPUNIT_ASSERT_EQUAL(static_cast< sal_Int32 >(2), vs.getLength());
    CPPUNIT_ASSERT_EQUAL(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("First")), vs[0].Name);
    CPPUNIT_ASSERT_EQUAL(
        css::uno::makeAny(static_cast< sal_Int32 >(-100)), vs[0].Value);
    CPPUNIT_ASSERT_EQUAL(css::beans::PropertyState_DIRECT_VALUE, vs[0].State);
    CPPUNIT_ASSERT_EQUAL(
        rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Second")), vs[1].Name);
    CPPUNIT_ASSERT_EQUAL(
        css::uno::makeAny(static_cast< sal_Int32 >(100)), vs[1].Value);
    CPPUNIT_ASSERT_EQUAL(
        css::beans::PropertyState_AMBIGUOUS_VALUE, vs[1].State);
    CPPUNIT_ASSERT(full->getSecond().Value.Value.IsPresent);
    CPPUNIT_ASSERT_EQUAL(
        static_cast< sal_Int32 >(100), full->getSecond().Value.Value.Value);
    CPPUNIT_ASSERT(!full->getSecond().Value.IsDefaulted);
    CPPUNIT_ASSERT(full->getSecond().IsAmbiguous);
}

css::uno::Reference< css::uno::XComponentContext > Test::m_context;

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Test, "alltests");

}

NOADDITIONAL;
