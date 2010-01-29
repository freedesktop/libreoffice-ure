#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: makefile.mk,v $
#
# $Revision: 1.14 $
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************
PRJ=..$/..

PRJNAME=	stoc
TARGET = reflection.uno
ENABLE_EXCEPTIONS=TRUE
COMP1TYPELIST = corefl

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
DLLPRE =

# ------------------------------------------------------------------

.INCLUDE :  ..$/cppumaker.mk

SLOFILES=	\
        $(SLO)$/crefl.obj	\
        $(SLO)$/crbase.obj	\
        $(SLO)$/crarray.obj	\
        $(SLO)$/crcomp.obj	\
        $(SLO)$/criface.obj	\
        $(SLO)$/crenum.obj

# internal compiler error with Forte 6 update 1 (x86)
# to be reconsidered after compiler upgrade
.IF "$(OS)$(CPU)"=="SOLARISI"
NOOPTFILES += $(SLO)$/criface.obj
.ENDIF

SHL1TARGET=	$(TARGET)
SHL1VERSIONMAP = $(SOLARENV)/src/unloadablecomponent.map
SHL1STDLIBS= \
        $(CPPULIB)		\
        $(CPPUHELPERLIB)	\
        $(SALLIB)

SHL1DEPN=
SHL1IMPLIB=	i$(TARGET)
SHL1LIBS=	$(SLB)$/$(TARGET).lib
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def
SHL1RPATH=  URELIB

DEF1NAME=	$(SHL1TARGET)

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

