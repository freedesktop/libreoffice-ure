#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.7 $
#
#   last change: $Author: vg $ $Date: 2007-01-16 15:56:03 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

PRJ=..$/..$/..

PRJNAME=sal
TARGET=qa_osl_condition

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

CFLAGS+= $(LFS_CFLAGS)
CXXFLAGS+= $(LFS_CFLAGS)

# BEGIN ----------------------------------------------------------------
# auto generated Target:Condition by codegen.pl 
SHL1OBJS=  \
    $(SLO)$/osl_Condition.obj

SHL1TARGET= osl_Condition
SHL1STDLIBS=\
   $(SALLIB) 
.IF "$(GUI)" == "WNT"
SHL1STDLIBS+=	$(SOLARLIBDIR)$/cppunit.lib
.ENDIF
.IF "$(GUI)" == "UNX"
SHL1STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
.ENDIF

SHL1IMPLIB= i$(SHL1TARGET)
SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)
DEF1EXPORTFILE= export.exp
# auto generated Target:Condition
# END ------------------------------------------------------------------

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : _cppunit.mk
