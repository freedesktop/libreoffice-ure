/*************************************************************************
 *
 *  $RCSfile: user9x.h,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: tra $ $Date: 2001-08-06 08:37:28 $
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

#pragma once 

#ifndef _WINDOWS_
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif

//------------------------------------------------------------------------
// undefine the macros defined in the winuser.h file in order to avoid
// warnings because of multiple defines 
//------------------------------------------------------------------------

#ifdef SendMessageW
#undef SendMessageW
#endif

#ifdef CreateWindowExW
#undef CreateWindowExW
#endif

#ifdef RegisterClassExW
#undef RegisterClassExW
#endif

#ifdef UnregisterClassW
#undef UnregisterClassW
#endif

#ifdef RegisterClipboardFormatW
#undef RegisterClipboardFormatW
#endif

#ifdef GetClipboardFormatNameW
#undef GetClipboardFormatNameW
#endif

#ifdef SetWindowTextW
#undef SetWindowTextW
#endif

#ifdef GetWindowTextW
#undef GetWindowTextW
#endif

#ifdef InsertMenuItemW
#undef InsertMenuItemW
#endif

#ifndef DrawTextW
#undef DrawTextW
#endif

//------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------

#define USER9X_API extern

//------------------------------------------------------------------------
// declare function pointers to the appropriate user32 functions
//------------------------------------------------------------------------

USER9X_API LRESULT ( WINAPI * lpfnSendMessageW) (
    HWND hWnd,		// handle to the destination window
    UINT Msg,		// message
    WPARAM wParam,	// first message parameter
    LPARAM lParam	// second message parameter
);

USER9X_API HWND ( WINAPI * lpfnCreateWindowExW ) (
    DWORD dwExStyle,      // extended window style
    LPCWSTR lpClassName,  // registered class name
    LPCWSTR lpWindowName, // window name
    DWORD dwStyle,        // window style
    int x,                // horizontal position of window
    int y,                // vertical position of window
    int nWidth,           // window width
    int nHeight,          // window height
    HWND hWndParent,      // handle to parent or owner window
    HMENU hMenu,          // menu handle or child identifier
    HINSTANCE hInstance,  // handle to application instance
    LPVOID lpParam        // window-creation data
);

USER9X_API ATOM ( WINAPI * lpfnRegisterClassExW ) ( 
    CONST WNDCLASSEXW* lpwcx // class data
);

USER9X_API BOOL ( WINAPI * lpfnUnregisterClassW ) (
    LPCWSTR lpClassName, // class name
    HINSTANCE hInstance	 // handle to application instance
);

USER9X_API UINT (WINAPI * lpfnRegisterClipboardFormatW) (
    LPCWSTR lpszFormat // name of new format
);

USER9X_API int ( WINAPI * lpfnGetClipboardFormatNameW ) (
    UINT   format,			// clipboard format to retrieve
    LPWSTR lpszFormatName,	// format name
    int    cchMaxCount		// length of format name buffer
);

USER9X_API BOOL ( WINAPI * lpfnSetWindowTextW ) (
    HWND hWnd,
    LPCWSTR lpString
);

USER9X_API int ( WINAPI * lpfnGetWindowTextW ) (
    HWND hWnd,			// handle to the window or control
    LPWSTR lpString,	// text buffer
    int    nMaxCount		// length of text buffer
);

USER9X_API BOOL ( WINAPI * lpfnInsertMenuItemW ) (
    HMENU hMenu,            // handle to menu
    UINT uItem,             // identifier or position
    BOOL fByPosition,       // meaning of uItem
    LPCMENUITEMINFOW lpmii  // menu item information
);

USER9X_API int ( WINAPI * lpfnDrawTextW ) (
  HDC hDC,          // handle to DC
  LPCWSTR lpString, // text to draw
  int nCount,       // text length
  LPRECT lpRect,    // formatting dimensions
  UINT uFormat      // text-drawing options
);

//------------------------------------------------------------------------
// redefine the above undefined macros so that the preprocessor replaces
// all occurrences of this macros with our function pointer
//------------------------------------------------------------------------

#define SendMessageW				lpfnSendMessageW
#define CreateWindowExW				lpfnCreateWindowExW
#define RegisterClassExW			lpfnRegisterClassExW
#define UnregisterClassW			lpfnUnregisterClassW
#define RegisterClipboardFormatW	lpfnRegisterClipboardFormatW
#define GetClipboardFormatNameW		lpfnGetClipboardFormatNameW
#define SetWindowTextW				lpfnSetWindowTextW
#define GetWindowTextW              lpfnGetWindowTextW
#define InsertMenuItemW				lpfnInsertMenuItemW
#define DrawTextW                   lpfnDrawTextW

#ifdef __cplusplus
}
#endif
