#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: hr $ $Date: 2003-08-07 15:12:30 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************
PRJ=..$/..

PRJNAME=sal
TARGET=rtl_strings
# TESTDIR=TRUE

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# BEGIN ----------------------------------------------------------------
# auto generated Target:FileBase by codegen.pl 
SHL1OBJS=  \
    $(SLO)$/rtl_String_Utils.obj \
    $(SLO)$/rtl_OString.obj

SHL1TARGET= rtl_OString
SHL1STDLIBS=\
   $(SALLIB) 
.IF "$(GUI)" == "WNT"
SHL1STDLIBS+=	$(SOLARLIBDIR)$/cppunit.lib
.ENDIF
.IF "$(GUI)" == "UNX"
SHL1STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
.ENDIF

SHL1IMPLIB= i$(SHL1TARGET)
# SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)
# DEF1EXPORTFILE= export.exp
SHL1VERSIONMAP = export.map

# auto generated Target:FileBase
# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
# auto generated Target:FileBase by codegen.pl 
SHL2OBJS=  \
    $(SLO)$/rtl_String_Utils.obj \
    $(SLO)$/rtl_OUString.obj

SHL2TARGET= rtl_OUString
SHL2STDLIBS=\
   $(SALLIB) 
.IF "$(GUI)" == "WNT"
SHL2STDLIBS+=	$(SOLARLIBDIR)$/cppunit.lib
.ENDIF
.IF "$(GUI)" == "UNX"
SHL2STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
.ENDIF

SHL2IMPLIB= i$(SHL2TARGET)
# SHL2DEF=    $(MISC)$/$(SHL2TARGET).def

DEF2NAME    =$(SHL2TARGET)
# DEF2EXPORTFILE= export.exp
SHL2VERSIONMAP = export.map

# auto generated Target:FileBase
# END ------------------------------------------------------------------

# BEGIN ----------------------------------------------------------------
# auto generated Target:FileBase by codegen.pl 
SHL3OBJS=  \
    $(SLO)$/rtl_String_Utils.obj \
    $(SLO)$/rtl_OUStringBuffer.obj

SHL3TARGET= rtl_OUStringBuffer
SHL3STDLIBS=\
   $(SALLIB) 
.IF "$(GUI)" == "WNT"
SHL3STDLIBS+=	$(SOLARLIBDIR)$/cppunit.lib
.ENDIF
.IF "$(GUI)" == "UNX"
SHL3STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
.ENDIF

SHL3IMPLIB= i$(SHL3TARGET)
# SHL3DEF=    $(MISC)$/$(SHL3TARGET).def

DEF3NAME    =$(SHL3TARGET)
# DEF3EXPORTFILE= export.exp
SHL3VERSIONMAP = export.map

# auto generated Target:FileBase
# END ------------------------------------------------------------------

#------------------------------- All object files -------------------------------
# do this here, so we get right dependencies
SLOFILES=\
    $(SHL1OBJS) \
    $(SHL2OBJS) \
    $(SHL3OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk


