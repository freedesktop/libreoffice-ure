/*************************************************************************
 *
 *  $RCSfile: permissions.h,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: dbo $ $Date: 2002-03-04 17:43:21 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
#ifndef _STOC_SEC_PERMISSIONS_H_
#define _STOC_SEC_PERMISSIONS_H_

#include <rtl/unload.h>
#include <rtl/ref.hxx>
#include <rtl/ustring.hxx>
#include <salhelper/simplereferenceobject.hxx>

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/RuntimeException.hpp>


namespace stoc_sec
{

extern ::rtl_StandardModuleCount s_moduleCount;

//==================================================================================================
class Permission : public ::salhelper::SimpleReferenceObject
{
public:    
    ::rtl::Reference< Permission > m_next;
    // mode
    enum t_type { ALL, RUNTIME, SOCKET, FILE } m_type;
    
    inline Permission(
        t_type type,
        ::rtl::Reference< Permission > const & next = ::rtl::Reference< Permission >() )
        SAL_THROW( () )
        : m_next( next )
        , m_type( type )
        {}
    
    virtual bool implies( Permission const & perm ) const SAL_THROW( () ) = 0;
    virtual ::rtl::OUString toString() const SAL_THROW( () ) = 0;
};
//==================================================================================================
class AllPermission : public Permission
{
public:
    inline AllPermission(
        ::rtl::Reference< Permission > const & next = ::rtl::Reference< Permission >() )
        SAL_THROW( () )
        : Permission( ALL, next )
        {}
    
    virtual bool implies( Permission const & ) const SAL_THROW( () );
    virtual ::rtl::OUString toString() const SAL_THROW( () );
};

//==================================================================================================
class PermissionCollection
{
    ::rtl::Reference< Permission > m_head;
public:
    inline PermissionCollection() SAL_THROW( () )
        {}
    inline PermissionCollection( PermissionCollection const & collection ) SAL_THROW( () )
        : m_head( collection.m_head )
        {}
    inline PermissionCollection( ::rtl::Reference< Permission > const & single ) SAL_THROW( () )
        : m_head( single )
        {}
    PermissionCollection(
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > const & permissions,
        PermissionCollection const & addition = PermissionCollection() )
        SAL_THROW( (::com::sun::star::uno::RuntimeException) );
    
    ::com::sun::star::uno::Sequence< ::rtl::OUString > toStrings() const SAL_THROW( () );
    
    bool implies( Permission const & ) const SAL_THROW( () );
    void checkPermission( ::com::sun::star::uno::Any const & perm )
        SAL_THROW( (::com::sun::star::uno::RuntimeException) );
};

}

#endif
