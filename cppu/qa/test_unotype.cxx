/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_cppu.hxx"

#include "sal/config.h"

#include <cppunit/TestSuite.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestCase.h>
#include <cppunit/plugin/TestPlugIn.h>
#include <cppunit/extensions/HelperMacros.h>

#include <typeinfo>

#include "com/sun/star/beans/Optional.hpp"
#include "com/sun/star/beans/PropertyChangeEvent.hpp"
#include "com/sun/star/lang/EventObject.hpp"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/Type.hxx"
#include "com/sun/star/uno/TypeClass.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "cppu/unotype.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace com { namespace sun { namespace star { namespace uno {
    class Any;
} } } }

namespace {

namespace css = com::sun::star;

struct DerivedStruct1: css::lang::EventObject {};

struct DerivedStruct2: css::beans::PropertyChangeEvent {};

struct DerivedException1: css::uno::Exception {};

struct DerivedException2: css::uno::RuntimeException {};

struct DerivedInterface1: css::uno::XInterface {};

struct DerivedInterface2: css::uno::XComponentContext {};

class Test: public ::CppUnit::TestFixture {
public:
    void testUnoType();

    void testGetTypeFavourUnsigned();

    void testGetTypeFavourChar();

    CPPUNIT_TEST_SUITE(Test);
    CPPUNIT_TEST(testUnoType);
    CPPUNIT_TEST(testGetTypeFavourUnsigned);
    CPPUNIT_TEST(testGetTypeFavourChar);
    CPPUNIT_TEST_SUITE_END();
};

void Test::testUnoType() {
    css::uno::Type t;
    t = ::cppu::UnoType< ::cppu::UnoVoidType >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_VOID, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("void")), t.getTypeName());
    t = ::cppu::UnoType< bool >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_BOOLEAN, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("boolean")),
        t.getTypeName());
    CPPUNIT_ASSERT(::cppu::UnoType< ::sal_Bool >::get() == t);
    t = ::cppu::UnoType< ::sal_Int8 >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_BYTE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("byte")), t.getTypeName());
    t = ::cppu::UnoType< ::sal_Int16 >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_SHORT, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("short")), t.getTypeName());
    t = ::cppu::UnoType< ::cppu::UnoUnsignedShortType >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_UNSIGNED_SHORT, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("unsigned short")),
        t.getTypeName());
    t = ::cppu::UnoType< ::sal_Int32 >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_LONG, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("long")), t.getTypeName());
    t = ::cppu::UnoType< ::sal_uInt32 >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_UNSIGNED_LONG, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("unsigned long")),
        t.getTypeName());
    t = ::cppu::UnoType< ::sal_Int64 >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_HYPER, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("hyper")), t.getTypeName());
    t = ::cppu::UnoType< ::sal_uInt64 >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_UNSIGNED_HYPER, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("unsigned hyper")),
        t.getTypeName());
    t = ::cppu::UnoType< float >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_FLOAT, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("float")), t.getTypeName());
    t = ::cppu::UnoType< double >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_DOUBLE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("double")),
        t.getTypeName());
    t = ::cppu::UnoType< ::cppu::UnoCharType >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_CHAR, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("char")), t.getTypeName());
    t = ::cppu::UnoType< ::rtl::OUString >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_STRING, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("string")),
        t.getTypeName());
    t = ::cppu::UnoType< css::uno::Type >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_TYPE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("type")), t.getTypeName());
    t = ::cppu::UnoType< css::uno::Any >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_ANY, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("any")), t.getTypeName());
    t = ::cppu::UnoType< ::cppu::UnoSequenceType< ::sal_Int8 > >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_SEQUENCE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("[]byte")),
        t.getTypeName());
    CPPUNIT_ASSERT(
        ::cppu::UnoType< css::uno::Sequence< ::sal_Int8 > >::get() == t);
    t = ::cppu::UnoType<
        ::cppu::UnoSequenceType< ::cppu::UnoUnsignedShortType > >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_SEQUENCE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("[]unsigned short")),
        t.getTypeName());
    t = ::cppu::UnoType<
        ::cppu::UnoSequenceType< ::cppu::UnoCharType > >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_SEQUENCE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("[]char")),
        t.getTypeName());
    t = ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::sal_Int8 > > >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_SEQUENCE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("[][]byte")),
        t.getTypeName());
    CPPUNIT_ASSERT(
        ::cppu::UnoType<
        css::uno::Sequence< css::uno::Sequence< ::sal_Int8 > > >::get() == t);
    t = ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::cppu::UnoUnsignedShortType > > >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_SEQUENCE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("[][]unsigned short")),
        t.getTypeName());
    t = ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::cppu::UnoCharType > > >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_SEQUENCE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("[][]char")),
        t.getTypeName());
    t = ::cppu::UnoType< css::uno::TypeClass >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_ENUM, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.uno.TypeClass")),
        t.getTypeName());
    t = ::cppu::UnoType< css::lang::EventObject >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_STRUCT, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.lang.EventObject")),
        t.getTypeName());
    CPPUNIT_ASSERT(::cppu::UnoType< DerivedStruct1 >::get() == t);
    t = ::cppu::UnoType< css::beans::PropertyChangeEvent >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_STRUCT, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM(
                "com.sun.star.beans.PropertyChangeEvent")),
        t.getTypeName());
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(::cppu::UnoType< DerivedStruct2 >::get() == t);
#endif
    t = ::cppu::UnoType< css::beans::Optional< ::sal_Int8 > >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_STRUCT, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.beans.Optional<byte>")),
        t.getTypeName());
    t = ::cppu::UnoType< css::uno::Exception >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_EXCEPTION, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.uno.Exception")),
        t.getTypeName());
    CPPUNIT_ASSERT(::cppu::UnoType< DerivedException1 >::get() == t);
    t = ::cppu::UnoType< css::uno::RuntimeException >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_EXCEPTION, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.uno.RuntimeException")),
        t.getTypeName());
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(::cppu::UnoType< DerivedException2 >::get() == t);
#endif
    t = ::cppu::UnoType< css::uno::XInterface >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_INTERFACE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.uno.XInterface")),
        t.getTypeName());
    CPPUNIT_ASSERT(
        ::cppu::UnoType< css::uno::Reference< css::uno::XInterface > >::get() ==
        t);
    CPPUNIT_ASSERT(::cppu::UnoType< DerivedInterface1 >::get() == t);
    CPPUNIT_ASSERT(
        ::cppu::UnoType< css::uno::Reference< DerivedInterface1 > >::get() ==
        t);
    t = ::cppu::UnoType< css::uno::XComponentContext >::get();
    CPPUNIT_ASSERT_EQUAL(css::uno::TypeClass_INTERFACE, t.getTypeClass());
    CPPUNIT_ASSERT_EQUAL(
        ::rtl::OUString(
            RTL_CONSTASCII_USTRINGPARAM("com.sun.star.uno.XComponentContext")),
        t.getTypeName());
    CPPUNIT_ASSERT(
        ::cppu::UnoType<
        css::uno::Reference< css::uno::XComponentContext > >::get() == t);
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(::cppu::UnoType< DerivedInterface2 >::get() == t);
    CPPUNIT_ASSERT(
        ::cppu::UnoType< css::uno::Reference< DerivedInterface2 > >::get() ==
        t);
#endif
}

void Test::testGetTypeFavourUnsigned() {
    CPPUNIT_ASSERT(typeid(::sal_Unicode) == typeid(::sal_uInt16));
    CPPUNIT_ASSERT(
        ::getCppuType(static_cast< ::sal_Unicode * >(0)) ==
        ::getCppuType(static_cast< ::sal_uInt16 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< ::cppu::UnoVoidType * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoVoidType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< bool * >(0)) ==
        ::cppu::UnoType< bool >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< bool * >(0)) ==
        ::getCppuType(static_cast< bool * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Bool * >(0)) ==
        ::cppu::UnoType< bool >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Bool * >(0)) ==
        ::getCppuType(static_cast< ::sal_Bool * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Int8 * >(0)) ==
        ::cppu::UnoType< ::sal_Int8 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Int8 * >(0)) ==
        ::getCppuType(static_cast< ::sal_Int8 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Int16 * >(0)) ==
        ::cppu::UnoType< ::sal_Int16 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Int16 * >(0)) ==
        ::getCppuType(static_cast< ::sal_Int16 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< ::cppu::UnoUnsignedShortType * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoUnsignedShortType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_uInt16 * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoUnsignedShortType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_uInt16 * >(0)) ==
        ::getCppuType(static_cast< ::sal_uInt16 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Int32 * >(0)) ==
        ::cppu::UnoType< ::sal_Int32 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Int32 * >(0)) ==
        ::getCppuType(static_cast< ::sal_Int32 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_uInt32 * >(0)) ==
        ::cppu::UnoType< ::sal_uInt32 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_uInt32 * >(0)) ==
        ::getCppuType(static_cast< ::sal_uInt32 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Int64 * >(0)) ==
        ::cppu::UnoType< ::sal_Int64 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Int64 * >(0)) ==
        ::getCppuType(static_cast< ::sal_Int64 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_uInt64 * >(0)) ==
        ::cppu::UnoType< ::sal_uInt64 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_uInt64 * >(0)) ==
        ::getCppuType(static_cast< ::sal_uInt64 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< float * >(0)) ==
        ::cppu::UnoType< float >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< float * >(0)) ==
        ::getCppuType(static_cast< float * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< double * >(0)) ==
        ::cppu::UnoType< double >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< double * >(0)) ==
        ::getCppuType(static_cast< double * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< ::cppu::UnoCharType * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoCharType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Unicode * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoUnsignedShortType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::sal_Unicode * >(0)) ==
        ::getCppuType(static_cast< ::sal_Unicode * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::rtl::OUString * >(0)) ==
        ::cppu::UnoType< ::rtl::OUString >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< ::rtl::OUString * >(0)) ==
        ::getCppuType(static_cast< ::rtl::OUString * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< css::uno::Type * >(0)) ==
        ::cppu::UnoType< css::uno::Type >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< css::uno::Type * >(0)) ==
        ::getCppuType(static_cast< css::uno::Type * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< css::uno::Any * >(0)) ==
        ::cppu::UnoType< css::uno::Any >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< css::uno::Any * >(0)) ==
        ::getCppuType(static_cast< css::uno::Any * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast<
            ::cppu::UnoSequenceType< ::cppu::UnoUnsignedShortType > * >(0)) ==
        ::cppu::UnoType<
        ::cppu::UnoSequenceType< ::cppu::UnoUnsignedShortType > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Sequence< ::sal_uInt16 > * >(0)) ==
        ::cppu::UnoType<
        ::cppu::UnoSequenceType< ::cppu::UnoUnsignedShortType > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Sequence< ::sal_uInt16 > * >(0)) ==
        ::getCppuType(static_cast< css::uno::Sequence< ::sal_uInt16 > * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
            ::cppu::UnoUnsignedShortType > > * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::cppu::UnoUnsignedShortType > > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Sequence< css::uno::Sequence<
            ::sal_uInt16 > > * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::cppu::UnoUnsignedShortType > > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Sequence< css::uno::Sequence<
            ::sal_uInt16 > > * >(0)) ==
        ::getCppuType(
            static_cast< css::uno::Sequence< css::uno::Sequence<
            ::sal_uInt16 > > * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Sequence< ::sal_Unicode > * >(0)) ==
        ::cppu::UnoType<
        ::cppu::UnoSequenceType< ::cppu::UnoUnsignedShortType > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Sequence< ::sal_Unicode > * >(0)) ==
        ::getCppuType(static_cast< css::uno::Sequence< ::sal_Unicode > * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Sequence< css::uno::Sequence<
            ::sal_Unicode > > * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::cppu::UnoUnsignedShortType > > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Sequence< css::uno::Sequence<
            ::sal_Unicode > > * >(0)) ==
        ::getCppuType(
            static_cast< css::uno::Sequence< css::uno::Sequence<
            ::sal_Unicode > > * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::TypeClass * >(0)) ==
        ::cppu::UnoType< css::uno::TypeClass >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::TypeClass * >(0)) ==
        ::getCppuType(static_cast< css::uno::TypeClass * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::lang::EventObject * >(0)) ==
        ::cppu::UnoType< css::lang::EventObject >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::lang::EventObject * >(0)) ==
        ::getCppuType(static_cast< css::lang::EventObject * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedStruct1 * >(0)) ==
        ::cppu::UnoType< css::lang::EventObject >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedStruct1 * >(0)) ==
        ::getCppuType(static_cast< DerivedStruct1 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::beans::PropertyChangeEvent * >(0)) ==
        ::cppu::UnoType< css::beans::PropertyChangeEvent >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::beans::PropertyChangeEvent * >(0)) ==
        ::getCppuType(static_cast< css::beans::PropertyChangeEvent * >(0)));
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedStruct2 * >(0)) ==
        ::cppu::UnoType< css::beans::PropertyChangeEvent >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedStruct2 * >(0)) ==
        ::getCppuType(static_cast< DerivedStruct2 * >(0)));
#endif
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::beans::Optional< ::sal_Int8 > * >(0)) ==
        ::cppu::UnoType< css::beans::Optional< ::sal_Int8 > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::beans::Optional< ::sal_Int8 > * >(0)) ==
        ::getCppuType(static_cast< css::beans::Optional< ::sal_Int8 > * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Exception * >(0)) ==
        ::cppu::UnoType< css::uno::Exception >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Exception * >(0)) ==
        ::getCppuType(static_cast< css::uno::Exception * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedException1 * >(0)) ==
        ::cppu::UnoType< css::uno::Exception >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedException1 * >(0)) ==
        ::getCppuType(static_cast< DerivedException1 * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::RuntimeException * >(0)) ==
        ::cppu::UnoType< css::uno::RuntimeException >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::RuntimeException * >(0)) ==
        ::getCppuType(static_cast< css::uno::RuntimeException * >(0)));
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedException2 * >(0)) ==
        ::cppu::UnoType< css::uno::RuntimeException >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedException2 * >(0)) ==
        ::getCppuType(static_cast< DerivedException2 * >(0)));
#endif
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::XInterface * >(0)) ==
        ::cppu::UnoType< css::uno::XInterface >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Reference< css::uno::XInterface > * >(0)) ==
        ::cppu::UnoType< css::uno::XInterface >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Reference< css::uno::XInterface > * >(0)) ==
        ::getCppuType(
            static_cast< css::uno::Reference< css::uno::XInterface > * >(0)));
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedInterface1 * >(0)) ==
        ::cppu::UnoType< css::uno::XInterface >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Reference< DerivedInterface1 > * >(0)) ==
        ::cppu::UnoType< css::uno::XInterface >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::XComponentContext * >(0)) ==
        ::cppu::UnoType< css::uno::XComponentContext >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast<
            css::uno::Reference< css::uno::XComponentContext > * >(0)) ==
        ::cppu::UnoType< css::uno::XComponentContext >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast<
            css::uno::Reference< css::uno::XComponentContext > * >(0)) ==
        ::getCppuType(
            static_cast<
            css::uno::Reference< css::uno::XComponentContext > * >(0)));
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(static_cast< DerivedInterface2 * >(0)) ==
        ::cppu::UnoType< css::uno::XComponentContext >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourUnsigned(
            static_cast< css::uno::Reference< DerivedInterface2 > * >(0)) ==
        ::cppu::UnoType< css::uno::XComponentContext >::get());
#endif
}

void Test::testGetTypeFavourChar() {
    CPPUNIT_ASSERT(typeid(::sal_Unicode) == typeid(::sal_uInt16));
    CPPUNIT_ASSERT(
        ::getCppuType< ::sal_Unicode >() == ::getCppuType< ::sal_uInt16 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::cppu::UnoVoidType * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoVoidType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< bool * >(0)) ==
        ::cppu::UnoType< bool >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< bool * >(0)) ==
        ::getCppuType< bool >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Bool * >(0)) ==
        ::cppu::UnoType< bool >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Bool * >(0)) ==
        ::getCppuType< ::sal_Bool >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Int8 * >(0)) ==
        ::cppu::UnoType< ::sal_Int8 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Int8 * >(0)) ==
        ::getCppuType< ::sal_Int8 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Int16 * >(0)) ==
        ::cppu::UnoType< ::sal_Int16 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Int16 * >(0)) ==
        ::getCppuType< ::sal_Int16 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< ::cppu::UnoUnsignedShortType * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoUnsignedShortType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_uInt16 * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoCharType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Int32 * >(0)) ==
        ::cppu::UnoType< ::sal_Int32 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Int32 * >(0)) ==
        ::getCppuType< ::sal_Int32 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_uInt32 * >(0)) ==
        ::cppu::UnoType< ::sal_uInt32 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_uInt32 * >(0)) ==
        ::getCppuType< ::sal_uInt32 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Int64 * >(0)) ==
        ::cppu::UnoType< ::sal_Int64 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Int64 * >(0)) ==
        ::getCppuType< ::sal_Int64 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_uInt64 * >(0)) ==
        ::cppu::UnoType< ::sal_uInt64 >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_uInt64 * >(0)) ==
        ::getCppuType< ::sal_uInt64 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< float * >(0)) ==
        ::cppu::UnoType< float >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< float * >(0)) ==
        ::getCppuType< float >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< double * >(0)) ==
        ::cppu::UnoType< double >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< double * >(0)) ==
        ::getCppuType< double >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::cppu::UnoCharType * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoCharType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Unicode * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoCharType >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::sal_Unicode * >(0)) ==
        ::getCppuType< ::sal_Unicode >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::rtl::OUString * >(0)) ==
        ::cppu::UnoType< ::rtl::OUString >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< ::rtl::OUString * >(0)) ==
        ::getCppuType< ::rtl::OUString >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< css::uno::Type * >(0)) ==
        ::cppu::UnoType< css::uno::Type >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< css::uno::Type * >(0)) ==
        ::getCppuType< css::uno::Type >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< css::uno::Any * >(0)) ==
        ::cppu::UnoType< css::uno::Any >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< css::uno::Any * >(0)) ==
        ::getCppuType< css::uno::Any >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast<
            ::cppu::UnoSequenceType< ::cppu::UnoUnsignedShortType > * >(0)) ==
        ::cppu::UnoType<
        ::cppu::UnoSequenceType< ::cppu::UnoUnsignedShortType > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::Sequence< ::sal_uInt16 > * >(0)) ==
        ::cppu::UnoType<
        ::cppu::UnoSequenceType< ::cppu::UnoCharType > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
            ::cppu::UnoUnsignedShortType > > * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::cppu::UnoUnsignedShortType > > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::Sequence< css::uno::Sequence<
            ::sal_uInt16 > > * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::cppu::UnoCharType > > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::Sequence< ::sal_Unicode > * >(0)) ==
        ::cppu::UnoType<
        ::cppu::UnoSequenceType< ::cppu::UnoCharType > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::Sequence< css::uno::Sequence<
            ::sal_Unicode > > * >(0)) ==
        ::cppu::UnoType< ::cppu::UnoSequenceType< ::cppu::UnoSequenceType<
        ::cppu::UnoCharType > > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< css::uno::TypeClass * >(0)) ==
        ::cppu::UnoType< css::uno::TypeClass >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< css::uno::TypeClass * >(0)) ==
        ::getCppuType< css::uno::TypeClass >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::lang::EventObject * >(0)) ==
        ::cppu::UnoType< css::lang::EventObject >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::lang::EventObject * >(0)) ==
        ::getCppuType< css::lang::EventObject >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedStruct1 * >(0)) ==
        ::cppu::UnoType< css::lang::EventObject >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedStruct1 * >(0)) ==
        ::getCppuType< DerivedStruct1 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::beans::PropertyChangeEvent * >(0)) ==
        ::cppu::UnoType< css::beans::PropertyChangeEvent >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::beans::PropertyChangeEvent * >(0)) ==
        ::getCppuType< css::beans::PropertyChangeEvent >());
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedStruct2 * >(0)) ==
        ::cppu::UnoType< css::beans::PropertyChangeEvent >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedStruct2 * >(0)) ==
        ::getCppuType< DerivedStruct2 >());
#endif
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::beans::Optional< ::sal_Int8 > * >(0)) ==
        ::cppu::UnoType< css::beans::Optional< ::sal_Int8 > >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::beans::Optional< ::sal_Int8 > * >(0)) ==
        ::getCppuType< css::beans::Optional< ::sal_Int8 > >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< css::uno::Exception * >(0)) ==
        ::cppu::UnoType< css::uno::Exception >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< css::uno::Exception * >(0)) ==
        ::getCppuType< css::uno::Exception >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedException1 * >(0)) ==
        ::cppu::UnoType< css::uno::Exception >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedException1 * >(0)) ==
        ::getCppuType< DerivedException1 >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::RuntimeException * >(0)) ==
        ::cppu::UnoType< css::uno::RuntimeException >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::RuntimeException * >(0)) ==
        ::getCppuType< css::uno::RuntimeException >());
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedException2 * >(0)) ==
        ::cppu::UnoType< css::uno::RuntimeException >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedException2 * >(0)) ==
        ::getCppuType< DerivedException2 >());
#endif
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::XInterface * >(0)) ==
        ::cppu::UnoType< css::uno::XInterface >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::Reference< css::uno::XInterface > * >(0)) ==
        ::cppu::UnoType< css::uno::XInterface >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::Reference< css::uno::XInterface > * >(0)) ==
        ::getCppuType< css::uno::Reference< css::uno::XInterface > >());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedInterface1 * >(0)) ==
        ::cppu::UnoType< css::uno::XInterface >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::Reference< DerivedInterface1 > * >(0)) ==
        ::cppu::UnoType< css::uno::XInterface >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::XComponentContext * >(0)) ==
        ::cppu::UnoType< css::uno::XComponentContext >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast<
            css::uno::Reference< css::uno::XComponentContext > * >(0)) ==
        ::cppu::UnoType< css::uno::XComponentContext >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast<
            css::uno::Reference< css::uno::XComponentContext > * >(0)) ==
        ::getCppuType< css::uno::Reference< css::uno::XComponentContext > >());
#if !(defined __SUNPRO_CC && __SUNPRO_CC <= 0x550) // erroneous ambiguity stated
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(static_cast< DerivedInterface2 * >(0)) ==
        ::cppu::UnoType< css::uno::XComponentContext >::get());
    CPPUNIT_ASSERT(
        ::cppu::getTypeFavourChar(
            static_cast< css::uno::Reference< DerivedInterface2 > * >(0)) ==
        ::cppu::UnoType< css::uno::XComponentContext >::get());
#endif
}

CPPUNIT_TEST_SUITE_REGISTRATION(Test);

}

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
