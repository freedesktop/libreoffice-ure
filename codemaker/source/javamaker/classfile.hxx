/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: classfile.hxx,v $
 * $Revision: 1.5 $
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

#ifndef INCLUDED_CODEMAKER_SOURCE_JAVAMAKER_CLASSFILE_HXX
#define INCLUDED_CODEMAKER_SOURCE_JAVAMAKER_CLASSFILE_HXX

#include "codemaker/unotype.hxx"
#include "sal/types.h"

#include <list>
#include <map>
#include <utility>
#include <vector>

class FileStream;
namespace rtl { class OString; }

namespace codemaker { namespace javamaker {

class ClassFile {
public:
    enum AccessFlags {
        ACC_PUBLIC = 0x0001,
        ACC_PRIVATE = 0x0002,
        ACC_STATIC = 0x0008,
        ACC_FINAL = 0x0010,
        ACC_SUPER = 0x0020,
        ACC_VARARGS = 0x0080,
        ACC_INTERFACE = 0x0200,
        ACC_ABSTRACT = 0x0400,
        ACC_SYNTHETIC = 0x1000
    };

    class Code {
    public:
        typedef std::vector< unsigned char >::size_type Branch;
        typedef std::vector< unsigned char >::size_type Position;

        ~Code();

        void instrAastore();

        void instrAconstNull();

        void instrAnewarray(rtl::OString const & type);

        void instrAreturn();

        void instrAthrow();

        void instrCheckcast(rtl::OString const & type);

        void instrDup();

        void instrGetstatic(
            rtl::OString const & type, rtl::OString const & name,
            rtl::OString const & descriptor);

        Branch instrIfAcmpne();

        Branch instrIfeq();

        Branch instrIfnull();

        void instrInstanceof(rtl::OString const & type);

        void instrInvokeinterface(
            rtl::OString const & type, rtl::OString const & name,
            rtl::OString const & descriptor, sal_uInt8 args);

        void instrInvokespecial(
            rtl::OString const & type, rtl::OString const & name,
            rtl::OString const & descriptor);

        void instrInvokestatic(
            rtl::OString const & type, rtl::OString const & name,
            rtl::OString const & descriptor);

        void instrInvokevirtual(
            rtl::OString const & type, rtl::OString const & name,
            rtl::OString const & descriptor);

        void instrLookupswitch(
            Code const * defaultBlock,
            std::list< std::pair< sal_Int32, Code * > > const & blocks);

        void instrNew(rtl::OString const & type);

        void instrNewarray(codemaker::UnoType::Sort sort);

        void instrPop();

        void instrPutfield(
            rtl::OString const & type, rtl::OString const & name,
            rtl::OString const & descriptor);

        void instrPutstatic(
            rtl::OString const & type, rtl::OString const & name,
            rtl::OString const & descriptor);

        void instrReturn();

        void instrSwap();

        void instrTableswitch(
            Code const * defaultBlock, sal_Int32 low,
            std::list< Code * > const & blocks);

        void loadIntegerConstant(sal_Int32 value);

        void loadStringConstant(rtl::OString const & value);

        void loadLocalInteger(sal_uInt16 index);

        void loadLocalLong(sal_uInt16 index);

        void loadLocalFloat(sal_uInt16 index);

        void loadLocalDouble(sal_uInt16 index);

        void loadLocalReference(sal_uInt16 index);

        void storeLocalReference(sal_uInt16 index);

        void branchHere(Branch branch);

        void addException(
            Position start, Position end, Position handler,
            rtl::OString const & type);

        void setMaxStackAndLocals(sal_uInt16 maxStack, sal_uInt16 maxLocals)
        { m_maxStack = maxStack; m_maxLocals = maxLocals; }

        Position getPosition() const;

    private:
        Code(Code &); // not implemented
        void operator =(Code); // not implemented

        Code(ClassFile & classFile);

        void ldc(sal_uInt16 index);

        void accessLocal(
            sal_uInt16 index, sal_uInt8 fastOp, sal_uInt8 normalOp);

        ClassFile & m_classFile;
        sal_uInt16 m_maxStack;
        sal_uInt16 m_maxLocals;
        std::vector< unsigned char > m_code;
        sal_uInt16 m_exceptionTableLength;
        std::vector< unsigned char > m_exceptionTable;

        friend class ClassFile;
    };

    ClassFile(
        AccessFlags accessFlags, rtl::OString const & thisClass,
        rtl::OString const & superClass, rtl::OString const & signature);

    ~ClassFile();

    Code * newCode();

    sal_uInt16 addIntegerInfo(sal_Int32 value);

    sal_uInt16 addFloatInfo(float value);

    sal_uInt16 addLongInfo(sal_Int64 value);

    sal_uInt16 addDoubleInfo(double value);

    void addInterface(rtl::OString const & interface);

    void addField(
        AccessFlags accessFlags, rtl::OString const & name,
        rtl::OString const & descriptor, sal_uInt16 constantValueIndex,
        rtl::OString const & signature);

    void addMethod(
        AccessFlags accessFlags, rtl::OString const & name,
        rtl::OString const & descriptor, Code const * code,
        std::vector< rtl::OString > const & exceptions,
        rtl::OString const & signature);

    void write(FileStream & file) const; //TODO

private:
    typedef std::map< rtl::OString, sal_uInt16 > Map;

    ClassFile(ClassFile &); // not implemented
    void operator =(ClassFile); // not implemented

    sal_uInt16 nextConstantPoolIndex(sal_uInt16 width);

    sal_uInt16 addUtf8Info(rtl::OString const & value);

    sal_uInt16 addClassInfo(rtl::OString const & type);

    sal_uInt16 addStringInfo(rtl::OString const & value);

    sal_uInt16 addFieldrefInfo(
        rtl::OString const & type, rtl::OString const & name,
        rtl::OString const & descriptor);

    sal_uInt16 addMethodrefInfo(
        rtl::OString const & type, rtl::OString const & name,
        rtl::OString const & descriptor);

    sal_uInt16 addInterfaceMethodrefInfo(
        rtl::OString const & type, rtl::OString const & name,
        rtl::OString const & descriptor);

    sal_uInt16 addNameAndTypeInfo(
        rtl::OString const & name, rtl::OString const & descriptor);

    void appendSignatureAttribute(
        std::vector< unsigned char > & stream, rtl::OString const & signature);

    sal_uInt16 m_constantPoolCount;
    std::vector< unsigned char > m_constantPool;
    std::map< rtl::OString, sal_uInt16 > m_utf8Infos;
    std::map< sal_Int32, sal_uInt16 > m_integerInfos;
    std::map< sal_Int64, sal_uInt16 > m_longInfos;
    std::map< float, sal_uInt16 > m_floatInfos;
    std::map< double, sal_uInt16 > m_doubleInfos;
    std::map< sal_uInt16, sal_uInt16 > m_classInfos;
    std::map< sal_uInt16, sal_uInt16 > m_stringInfos;
    std::map< sal_uInt32, sal_uInt16 > m_fieldrefInfos;
    std::map< sal_uInt32, sal_uInt16 > m_methodrefInfos;
    std::map< sal_uInt32, sal_uInt16 > m_interfaceMethodrefInfos;
    std::map< sal_uInt32, sal_uInt16 > m_nameAndTypeInfos;
    AccessFlags m_accessFlags;
    sal_uInt16 m_thisClass;
    sal_uInt16 m_superClass;
    sal_uInt16 m_interfacesCount;
    std::vector< unsigned char > m_interfaces;
    sal_uInt16 m_fieldsCount;
    std::vector< unsigned char > m_fields;
    sal_uInt16 m_methodsCount;
    std::vector< unsigned char > m_methods;
    sal_uInt16 m_attributesCount;
    std::vector< unsigned char > m_attributes;

    friend class Code;
};

} }

#endif // INCLUDED_CODEMAKER_SOURCE_JAVAMAKER_CLASSFILE_HXX
