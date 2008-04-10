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
# $Revision: 1.5 $
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

PRJ=..

PRJNAME=store
TARGET=store
ENABLE_EXCEPTIONS=TRUE

# --- Settings ---

.INCLUDE : settings.mk

# --- Files ---

SLOFILES=	\
    $(SLO)$/object.obj		\
    $(SLO)$/memlckb.obj		\
    $(SLO)$/filelckb.obj	\
    $(SLO)$/storbase.obj	\
    $(SLO)$/storcach.obj	\
    $(SLO)$/stordata.obj	\
    $(SLO)$/storlckb.obj	\
    $(SLO)$/stortree.obj	\
    $(SLO)$/storpage.obj	\
    $(SLO)$/store.obj

.IF "$(debug)" != ""
OBJFILES=	\
    $(OBJ)$/object.obj		\
    $(OBJ)$/memlckb.obj		\
    $(OBJ)$/filelckb.obj	\
    $(OBJ)$/storbase.obj	\
    $(OBJ)$/storcach.obj	\
    $(OBJ)$/stordata.obj	\
    $(OBJ)$/storlckb.obj	\
    $(OBJ)$/stortree.obj	\
    $(OBJ)$/storpage.obj	\
    $(OBJ)$/store.obj
.ENDIF # debug

# --- Targets ---

.INCLUDE : target.mk

