/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: GetDiskFreeSpaceExA.cpp,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 16:16:03 $
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

#include "macros.h"

// GetDiskSpaceExA wrapper for Win 95A

IMPLEMENT_THUNK( kernel32, TRYLOAD, BOOL, WINAPI, GetDiskFreeSpaceExA,(
  LPCSTR lpRootPathName,                  // directory name
  PULARGE_INTEGER lpFreeBytesAvailable,    // bytes available to caller
  PULARGE_INTEGER lpTotalNumberOfBytes,    // bytes on disk
  PULARGE_INTEGER lpTotalNumberOfFreeBytes // free bytes on disk
))
{
    DWORD	dwSectorsPerCluster, dwBytesPerSector, dwNumberOfFreeClusters, dwTotalNumberOfClusters;

    BOOL	fSuccess = GetDiskFreeSpaceA( lpRootPathName, &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters );

    if ( fSuccess )
    {
        ULONGLONG	ulBytesPerCluster = (ULONGLONG)dwSectorsPerCluster * (ULONGLONG)dwBytesPerSector;

        if ( lpFreeBytesAvailable )
            lpFreeBytesAvailable->QuadPart = ulBytesPerCluster * (ULONGLONG)dwNumberOfFreeClusters;

        if ( lpTotalNumberOfBytes )
            lpTotalNumberOfBytes->QuadPart = ulBytesPerCluster * (ULONGLONG)dwTotalNumberOfClusters;

        if ( lpTotalNumberOfFreeBytes )
            lpTotalNumberOfFreeBytes->QuadPart = ulBytesPerCluster * (ULONGLONG)dwNumberOfFreeClusters;
    }

    return fSuccess;
}

