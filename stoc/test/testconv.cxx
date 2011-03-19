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
#include "precompiled_stoc.hxx"

#include <sal/main.h>
#include <osl/diagnose.h>
#include <rtl/ustrbuf.hxx>
#include <cppuhelper/servicefactory.hxx>

#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/script/XTypeConverter.hpp>
#include <com/sun/star/reflection/FieldAccessMode.hpp>
#include <com/sun/star/registry/XImplementationRegistration.hpp>

#include <float.h>
#include <stdio.h>


using namespace cppu;
using namespace osl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::script;
using namespace com::sun::star::reflection;
using namespace com::sun::star::registry;

using ::rtl::OUString;
using ::rtl::OUStringToOString;

const double MIN_DOUBLE		= -DBL_MAX;
const double MAX_DOUBLE		= DBL_MAX;
const double MIN_FLOAT		= -FLT_MAX;
const double MAX_FLOAT		= FLT_MAX;

//==================================================================================================
static void printValue( const Any & rVal )
{
    // print value
    OString aStr( OUStringToOString( rVal.getValueType().getTypeName(), RTL_TEXTENCODING_ISO_8859_1 ) );
    printf( "(%s)", aStr.getStr() );

    switch (rVal.getValueTypeClass())
    {
    case TypeClass_VOID:
        printf( "void" );
        break;
    case TypeClass_ANY:
        if (rVal.hasValue())
            printValue( *(Any *)rVal.getValue() );
        break;
    case TypeClass_BOOLEAN:
        printf( "%s", (*(sal_Bool *)rVal.getValue() ? "true" : "false") );
        break;
    case TypeClass_CHAR:
    {
        char ar[2];
        ar[0] = (char)*(sal_Unicode *)rVal.getValue();
        ar[1] = 0;
        printf( "%s", ar );
        break;
    }
    case TypeClass_BYTE:
        printf( "%x", (int)*(sal_Int8 *)rVal.getValue() );
        break;
    case TypeClass_SHORT:
        printf( "%x", *(sal_Int16 *)rVal.getValue() );
        break;
    case TypeClass_UNSIGNED_SHORT:
        printf( "%x", *(sal_uInt16 *)rVal.getValue() );
        break;
    case TypeClass_LONG:
        printf( "%lx", static_cast<long>(*(sal_Int32 *)rVal.getValue()) );
        break;
    case TypeClass_UNSIGNED_LONG:
        printf( "%lx", static_cast<unsigned long>(*(sal_uInt32 *)rVal.getValue()) );
        break;
    case TypeClass_HYPER:
        printf( "%lx", (long)*(sal_Int64 *)rVal.getValue() );
        break;
    case TypeClass_UNSIGNED_HYPER:
        printf( "%lx", (unsigned long)*(sal_uInt64 *)rVal.getValue() );
        break;
    case TypeClass_FLOAT:
        printf( "%f", *(float *)rVal.getValue() );
        break;
    case TypeClass_DOUBLE:
        printf( "%g", *(double *)rVal.getValue() );
        break;
    case TypeClass_STRING:
    {
        OString aStr2( OUStringToOString( *(OUString *)rVal.getValue(), RTL_TEXTENCODING_ISO_8859_1 ) );
        printf( aStr2.getStr() );
        break;
    }
    case TypeClass_ENUM:
    {
        typelib_EnumTypeDescription * pEnumTD = 0;
        TYPELIB_DANGER_GET( (typelib_TypeDescription **)&pEnumTD, rVal.getValueTypeRef() );

        for ( sal_Int32 nPos = pEnumTD->nEnumValues; nPos--; )
        {
            if (pEnumTD->pEnumValues[nPos] == *(int *)rVal.getValue())
            {
                printf( OUStringToOString(pEnumTD->ppEnumNames[nPos]->buffer, RTL_TEXTENCODING_ASCII_US).getStr() );
                TYPELIB_DANGER_RELEASE( (typelib_TypeDescription *)pEnumTD );
                return;
            }
        }
        TYPELIB_DANGER_RELEASE( (typelib_TypeDescription *)pEnumTD );
        printf( ">ENUM not found!<" );
        break;
    }
    case TypeClass_SEQUENCE:
    {
        uno_Sequence * pSeq = *(uno_Sequence **)rVal.getValue();
        typelib_TypeDescription * pSeqTD = 0;
        TYPELIB_DANGER_GET( &pSeqTD, rVal.getValueTypeRef() );
        typelib_TypeDescription * pElemTD = 0;
        TYPELIB_DANGER_GET( &pElemTD, ((typelib_IndirectTypeDescription *)pSeqTD)->pType );

        sal_Int32 nLen = pSeq->nElements;
        if (nLen)
        {
            printf( "{ " );
            for ( sal_Int32 nPos = 0; nPos < nLen; ++nPos )
            {
                printValue( Any( ((char *)pSeq->elements) + (nPos * pElemTD->nSize), pElemTD ) );
                if (nPos < (nLen-1))
                    printf( ", " );
            }
            printf( " }" );
        }

        TYPELIB_DANGER_RELEASE( pElemTD );
        TYPELIB_DANGER_RELEASE( pSeqTD );
        break;
    }

    default:
        printf( ">not printable<" );
        break;
    }
}

static Reference< XTypeConverter > s_xConverter;

//==================================================================================================
static sal_Bool convertTo( const Type & rDestType, const Any & rVal, sal_Bool bExpectSuccess )
{
    sal_Bool bCanConvert = sal_False;
    Any aRet;

    OString aExcMsg;

    try
    {
        aRet = s_xConverter->convertTo( rVal, rDestType );
        bCanConvert = sal_True;
    }
    catch (Exception & rExc)
    {
        aExcMsg = OUStringToOString( rExc.Message, RTL_TEXTENCODING_ASCII_US );
    }

    if (bExpectSuccess && !bCanConvert)
    {
        printf( "# conversion of " );
        printValue( rVal );
        printf( " to " );
        printf( OUStringToOString(rDestType.getTypeName(), RTL_TEXTENCODING_ASCII_US).getStr() );
        printf( " failed, but success was expected! [" );
        printf( aExcMsg.getStr() );
        printf( "]\n" );
        aRet = s_xConverter->convertTo( rVal, rDestType );
#if OSL_DEBUG_LEVEL > 1
        // for debugging, to trace again
        try
        {
            aRet = s_xConverter->convertTo( rVal, rDestType );
        }
        catch (Exception &)
        {
        }
#endif
        return sal_False;
    }
    if (!bExpectSuccess && bCanConvert)
    {
        printf( "# conversion of " );
        printValue( rVal );
        printf( " to " );
        printValue( aRet );
        printf( " was successful, but was not expected to be!\n" );
#if OSL_DEBUG_LEVEL > 1
        // for debugging, to trace again
        aRet = s_xConverter->convertTo( rVal, rDestType );
#endif
        return sal_False;
    }

#ifdef __RECONVERSION_OUTPUT__
//= re-conversion output =
    if (bCanConvert)
    {
        // re convert to original type
        sal_Bool bReConvert = sal_False;
        Any aRet2;

        try
        {
            aRet2 = s_xConverter->convertTo( aRet, rVal.getValueType() );
            bReConvert = sal_True;
        }
        catch (Exception & rExc)
        {
            aExcMsg = OUStringToOString( rExc.Message, RTL_TEXTENCODING_ISO_8859_1 );
        }

        if (bReConvert)
        {
            if (rVal != aRet2)
            {
                printf( "# re-conversion of " );
                printValue( rVal );
                printf( " to " );
                printValue( aRet );
                printf( " to " );
                printValue( aRet2 );
                printf( ": first and last do not match!\n" );
            }
        }
        else
        {
            printf( "# re-conversion of " );
            printValue( aRet );
            printf( " to " );
            printf( rVal.getValueType().getTypeName().getStr() );
            printf( " failed! [" );
            printf( aExcMsg.getStr() );
            printf( "]\n" );
        }
    }
#endif

    return sal_True;
}


//==================================================================================================
typedef struct _ConvBlock
{
    Any			_value;
    sal_Bool	_toString, _toDouble, _toFloat;
    sal_Bool	_toUINT32, _toINT32, _toUINT16, _toINT16, _toBYTE, _toBOOL, _toChar;
    sal_Bool	_toTypeClass, _toSeqINT16, _toSeqAny;

    _ConvBlock()
    {
    }
    _ConvBlock( const Any & rValue_,
                sal_Bool toString_, sal_Bool toDouble_, sal_Bool toFloat_,
                sal_Bool toUINT32_, sal_Bool toINT32_, sal_Bool toUINT16_, sal_Bool toINT16_,
                sal_Bool toBYTE_, sal_Bool toBOOL_, sal_Bool toChar_,
                sal_Bool toTypeClass_, sal_Bool toSeqINT16_, sal_Bool toSeqAny_ )
        : _value( rValue_ )
        , _toString( toString_ ), _toDouble( toDouble_ ), _toFloat( toFloat_ )
        , _toUINT32( toUINT32_ ), _toINT32( toINT32_ ), _toUINT16( toUINT16_ ), _toINT16( toINT16_ )
        , _toBYTE( toBYTE_ ), _toBOOL( toBOOL_ ), _toChar( toChar_ )
        , _toTypeClass( toTypeClass_ ), _toSeqINT16( toSeqINT16_ ), _toSeqAny( toSeqAny_ )
    {
    }
} ConvBlock;


//==================================================================================================
static sal_Int32 initBlocks( ConvBlock * pTestBlocks )
{
    Any aVal;

    sal_uInt32 nElems = 0;

    // ==BYTE==
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0xff"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("255"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_Int8)0xffu;
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0x80"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("128"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_Int8)( 0x80u );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0x7f"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("127"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
    aVal <<= (sal_Int8)( 0x7f );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("5"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("+5"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
    aVal <<= (sal_Int8)( 5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("-5"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int8)( -5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("256"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==UINT16==
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("65535"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0xffff"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_uInt16)( 0xffff );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("32768"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_uInt16)( 0x8000 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("32767"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0x7fff"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_uInt16)( 0x7fff );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("256"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0x100"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_uInt16)( 0x100 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_uInt16)( 5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_uInt16)( -5 ); // is 0xfffb
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==INT16==
    aVal <<= (sal_Int16)( -1 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int16)( -0x8000 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int16)( 0x7fff );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int16)( 0x100 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int16)( 5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int16)( -5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==UINT32==
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("+4294967295"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("4294967295"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0xffffffff"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_uInt32)( 0xffffffff );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("-2147483648"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("-0x80000000"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_uInt32)( 0x80000000 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("2147483647"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0x7fffffff"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_uInt32)( 0x7fffffff );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("65536"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0x10000"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_uInt32)( 0x10000 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_uInt32)( 0x8000 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_uInt32)( 5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0xfffffffb"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (sal_uInt32)( -5 ); // is 0xfffffffb
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==INT32==
    aVal <<= (sal_Int32)( 0xffffffff ); // is -1
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int32)( 0x80000000 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int32)( 0x7fffffff );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int32)( 0x10000 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int32)( -0x8001 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int32)( 5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (sal_Int32)( -5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==FLOAT==
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("-3.4e+38"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (float)( MIN_FLOAT );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("+3.4e+38"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (float)( MAX_FLOAT );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("9e-20"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
    aVal <<= (float)( 9e-20 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("+.7071067811865"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
    aVal <<= (float)( .7071067811865 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("3.14159265359"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
    aVal <<= (float)( 3.14159265359 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (float)( 5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==DOUBLE==
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("-1.7976931348623155e+308"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (double)( MIN_DOUBLE );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("1.7976931348623155e+308"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    aVal <<= (double)( MAX_DOUBLE );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (double)( MIN_FLOAT );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (double)( MAX_FLOAT );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (double)( -((double)0x80000000) );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (double)( -((double)0x80000001) );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (double)( 0x7fffffff );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (double)( 0x80000000 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (double)( 0xffffffff );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0x100000000"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
#ifndef OS2
    aVal <<= (double)( SAL_CONST_INT64(0x100000000) );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
#endif
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= (double)( 5 );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==CHAR==
    sal_Unicode c = 'A';
    aVal.setValue( &c, ::getCharCppuType() );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("A"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==BOOL==
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("1"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("False"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("true"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa

    sal_Bool bTmp = sal_True;
    aVal.setValue( &bTmp, getBooleanCppuType() );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==ZERO STRINGS==
    aVal <<= OUString();
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("-"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("-0"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==TYPECLASS ENUM==
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("eNuM"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal <<= OUString(RTL_CONSTASCII_USTRINGPARAM("DOUBLE"));
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    int e = 1;
    aVal.setValue( &e, ::getCppuType( (const TypeClass *)0 ) );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    aVal.setValue( &e, ::getCppuType( (const FieldAccessMode *)0 ) );
    pTestBlocks[nElems++] = ConvBlock( aVal, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==SEQ of INT==
    Sequence< sal_Int32 > aINT32Seq( 3 ), aINT32Seq2( 3 );
    sal_Int32 * pINT32Seq = aINT32Seq.getArray();
    pINT32Seq[0]	 = -32768;
    pINT32Seq[1]	 = 0;
    pINT32Seq[2]	 = 32767;
    aVal <<= aINT32Seq;
    pTestBlocks[nElems++] = ConvBlock( aVal, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    pINT32Seq = aINT32Seq2.getArray();
    pINT32Seq[0]	 = -32768;
    pINT32Seq[1]	 = -32769;
    pINT32Seq[2]	 = 32767;
    aVal <<= aINT32Seq2;
    pTestBlocks[nElems++] = ConvBlock( aVal, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    // ==SEQ of ANY==
    Sequence< Any > aAnySeq( 2 ), aAnySeq2( 2 ), aAnySeq3( 2 );
    Any * pAnySeq	 = aAnySeq.getArray();
    pAnySeq[0]		 = makeAny( aINT32Seq );
    pAnySeq[1]		 = makeAny( OUString(RTL_CONSTASCII_USTRINGPARAM("lala")) );
    aVal <<= aAnySeq;
    pTestBlocks[nElems++] = ConvBlock( aVal, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    pAnySeq = aAnySeq2.getArray();
    pAnySeq[0]		 <<= (sal_Int32)4711;
    pAnySeq[1]		 <<= OUString(RTL_CONSTASCII_USTRINGPARAM("0815"));
    aVal <<= aAnySeq2;
    pTestBlocks[nElems++] = ConvBlock( aVal, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    pAnySeq = aAnySeq3.getArray();
    pAnySeq[0]		 <<= OUString(RTL_CONSTASCII_USTRINGPARAM("TypeClass_UNION"));
    pAnySeq[1]		 <<= OUString(RTL_CONSTASCII_USTRINGPARAM("TypeClass_ENUM"));
    aVal <<= aAnySeq3;
    pTestBlocks[nElems++] = ConvBlock( aVal, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 );
                                         // st,do,fl,u3,i3,u1,i1,by,bo,ch,tc,si,sa
    return nElems;
}

//==================================================================================================
static void test_Conversion( const Reference< XMultiServiceFactory > & xMgr )
{
    printf( "test_Conversion(): start...\n" );

    Reference< XTypeConverter > xConverter( xMgr->createInstance(
        OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.script.Converter")) ), UNO_QUERY );

    ConvBlock * pTestBlocks = new ConvBlock[256];
    sal_Int32 nPos = initBlocks( pTestBlocks );

    s_xConverter = xConverter;
    while (nPos--)
    {
        const ConvBlock& rBlock = pTestBlocks[nPos];
        const Any & rVal		= rBlock._value;

        convertTo( ::getCppuType( (const OUString *)0 ), rVal, rBlock._toString );
        convertTo( ::getCppuType( (const float *)0 ), rVal, rBlock._toFloat );
        convertTo( ::getCppuType( (const double *)0 ), rVal, rBlock._toDouble );
        convertTo( ::getCppuType( (const sal_uInt32 *)0 ), rVal, rBlock._toUINT32 );
        convertTo( ::getCppuType( (const sal_Int32 *)0 ), rVal, rBlock._toINT32 );
        convertTo( ::getCppuType( (const sal_uInt16 *)0 ), rVal, rBlock._toUINT16 );
        convertTo( ::getCppuType( (const sal_Int16 *)0 ), rVal, rBlock._toINT16 );
        convertTo( ::getCppuType( (const sal_Int8 *)0 ), rVal, rBlock._toBYTE );
        convertTo( ::getBooleanCppuType(), rVal, rBlock._toBOOL );
        convertTo( ::getCharCppuType(), rVal, rBlock._toChar );
        convertTo( ::getCppuType( (const TypeClass *)0 ), rVal, rBlock._toTypeClass );
        convertTo( ::getCppuType( (const Sequence< sal_Int16 > *)0 ), rVal, rBlock._toSeqINT16 );
        convertTo( ::getCppuType( (const Sequence< Any > *)0 ), rVal, rBlock._toSeqAny );

        convertTo( ::getVoidCppuType(), rVal, sal_True ); // anything converts to void
    }
    s_xConverter.clear();

    delete [] pTestBlocks;

    Any aRet;
    aRet = xConverter->convertTo( Any( &xMgr, ::getCppuType( (const Reference< XMultiServiceFactory > *)0 ) ),
                                  ::getCppuType( (const Reference< XServiceInfo > *)0 ) );
    aRet = xConverter->convertTo( aRet, ::getCppuType( (const Reference< XMultiServiceFactory > *)0 ) );
    aRet = xConverter->convertTo( aRet, ::getCppuType( (const Reference< XServiceInfo > *)0 ) );
    aRet <<= SAL_CONST_INT64(0x7fffffffffffffff);
    aRet = xConverter->convertTo( aRet, ::getCppuType( (const sal_uInt64 *)0 ) );
    OSL_ASSERT( *(const sal_uInt64 *)aRet.getValue() == SAL_CONST_UINT64(0x7fffffffffffffff) );
    aRet <<= SAL_CONST_UINT64(0xffffffffffffffff);
    aRet = xConverter->convertTo( aRet, ::getCppuType( (const sal_uInt64 *)0 ) );
    OSL_ASSERT( *(const sal_uInt64 *)aRet.getValue() == SAL_CONST_UINT64(0xffffffffffffffff) );
    aRet <<= SAL_CONST_INT64(-1);
    aRet = xConverter->convertTo( aRet, ::getCppuType( (const sal_Int8 *)0 ) );
    OSL_ASSERT( *(const sal_Int8 *)aRet.getValue() == (-1) );
    printf( "test_Conversion(): end.\n" );
}

SAL_IMPLEMENT_MAIN()
{
    Reference< XMultiServiceFactory > xMgr( createRegistryServiceFactory( OUString(RTL_CONSTASCII_USTRINGPARAM("stoctest.rdb")) ) );

    try
    {
        Reference< XImplementationRegistration > xImplReg(
            xMgr->createInstance( OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.registry.ImplementationRegistration")) ), UNO_QUERY );
        OSL_ENSURE( xImplReg.is(), "### no impl reg!" );
        
        OUString aLibName(
            RTL_CONSTASCII_USTRINGPARAM("stocservices.uno" SAL_DLLEXTENSION) );
        xImplReg->registerImplementation(
            OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.loader.SharedLibrary")),
            aLibName, Reference< XSimpleRegistry >() );

        test_Conversion( xMgr );
    }
    catch (Exception & rExc)
    {
        OSL_FAIL( "### exception occurred!" );
        OString aMsg( OUStringToOString( rExc.Message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_TRACE( "### exception occurred: " );
        OSL_TRACE( "%s", aMsg.getStr() );
        OSL_TRACE( "\n" );
    }

    Reference< XComponent >( xMgr, UNO_QUERY )->dispose();
    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
