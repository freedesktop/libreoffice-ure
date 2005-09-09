/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: regview.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:19:21 $
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

#include <stdio.h>

#include "registry/registry.h"

#ifndef _RTL_USTRING_HXX_
#include	<rtl/ustring.hxx>
#endif

#ifndef _RTL_ALLOC_H_
#include	<rtl/alloc.h>
#endif

#ifndef _OSL_PROCESS_H_
#include <osl/process.h>
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _OSL_THREAD_H_
#include <osl/thread.h>
#endif
#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif

#ifdef SAL_UNX
#define SEPARATOR '/'
#else
#define SEPARATOR '\\'
#endif

using namespace ::rtl;
using namespace ::osl;

sal_Bool isFileUrl(const OString& fileName)
{
    if (fileName.indexOf("file://") == 0 )
        return sal_True;
    return sal_False;
}

OUString convertToFileUrl(const OString& fileName)
{
    if ( isFileUrl(fileName) )
    {
        return OStringToOUString(fileName, osl_getThreadTextEncoding());
    }

    OUString uUrlFileName;
    OUString uFileName(fileName.getStr(), fileName.getLength(), osl_getThreadTextEncoding());
    if ( fileName.indexOf('.') == 0 || fileName.indexOf(SEPARATOR) < 0 )
    {
        OUString uWorkingDir;
        OSL_VERIFY( osl_getProcessWorkingDir(&uWorkingDir.pData) == osl_Process_E_None );
        OSL_VERIFY( FileBase::getAbsoluteFileURL(uWorkingDir, uFileName, uUrlFileName) == FileBase::E_None );
    } else
    {
        OSL_VERIFY( FileBase::getFileURLFromSystemPath(uFileName, uUrlFileName) == FileBase::E_None );
    }

    return uUrlFileName;
}


#if (defined UNX) || (defined OS2)
int main( int argc, char * argv[] )
#else
int _cdecl main( int argc, char * argv[] )
#endif
{
    RegHandle 		hReg;
    RegKeyHandle	hRootKey, hKey;

    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "using: regview registryfile [keyName]\n");
        exit(1);
    }

    OUString regName( convertToFileUrl(argv[1]) );
    if (reg_openRegistry(regName.pData, &hReg, REG_READONLY))
    {
        fprintf(stderr, "open registry \"%s\" failed\n", argv[1]); 
        exit(1);
    }

    if (!reg_openRootKey(hReg, &hRootKey))
    {
        if (argc == 3)
        {
            OUString keyName( OUString::createFromAscii(argv[2]) );
            if (!reg_openKey(hRootKey, keyName.pData, &hKey))
            {
                if (reg_dumpRegistry(hKey))
                {
                    fprintf(stderr, "dumping registry \"%s\" failed\n", argv[1]); 
                }
                
                if (reg_closeKey(hKey))
                {
                    fprintf(stderr, "closing key \"%s\" of registry \"%s\" failed\n", 
                            argv[2], argv[1]); 
                }
            } else
            {
                fprintf(stderr, "key \"%s\" not exists in registry \"%s\"\n", 
                        argv[2], argv[1]); 
            }
        } else
        {
            if (reg_dumpRegistry(hRootKey))
            {
                fprintf(stderr, "dumping registry \"%s\" failed\n", argv[1]); 
            }
        }

        if (reg_closeKey(hRootKey))
        {
            fprintf(stderr, "closing root key of registry \"%s\" failed\n", argv[1]); 
        }
    } else
    {
        fprintf(stderr, "open root key of registry \"%s\" failed\n", argv[1]); 
    }

    if (reg_closeRegistry(hReg))
    {
        fprintf(stderr, "closing registry \"%s\" failed\n", argv[1]); 
        exit(1);
    }

    return(0);
}


