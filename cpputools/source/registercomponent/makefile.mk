#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.16 $
#
#   last change: $Author: vg $ $Date: 2006-05-24 14:30:10 $
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
PRJ=..$/..

PRJNAME=cpputools
TARGET=regcomp
LIBTARGET=NO

ENABLE_EXCEPTIONS=TRUE

.IF "$(OS)" == "LINUX"
LINKFLAGSRUNPATH = -Wl,-rpath,\''$$ORIGIN/../lib:$$ORIGIN'\'
.ELIF "$(OS)" == "SOLARIS"
LINKFLAGSRUNPATH = -R\''$$ORIGIN/../lib:$$ORIGIN'\'
.ENDIF

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk

UNOUCRDEP=$(SOLARBINDIR)$/udkapi.rdb 
UNOUCRRDB=$(SOLARBINDIR)$/udkapi.rdb

NO_OFFUH=TRUE
CPPUMAKERFLAGS += -C

UNIXTEXT= $(MISC)$/regcomp.sh
UNOTYPES=\
             com.sun.star.uno.TypeClass \
             com.sun.star.lang.XMultiServiceFactory \
             com.sun.star.lang.XSingleServiceFactory \
             com.sun.star.lang.XMultiComponentFactory \
             com.sun.star.lang.XSingleComponentFactory \
             com.sun.star.lang.XComponent \
             com.sun.star.container.XContentEnumerationAccess \
             com.sun.star.container.XSet \
             com.sun.star.loader.CannotActivateFactoryException \
            com.sun.star.registry.XImplementationRegistration

# --- Files --------------------------------------------------------
CDEFS += -DDLL_VERSION=\"$(UPD)$(DLLPOSTFIX)\"

DEPOBJFILES=   $(OBJ)$/registercomponent.obj 

APP1TARGET= $(TARGET)
APP1OBJS=$(DEPOBJFILES)  

APP1STDLIBS=\
            $(SALLIB) \
            $(CPPULIB)	\
            $(CPPUHELPERLIB)

.IF "$(GUI)"=="WNT"
APP1STDLIBS+= \
            $(LIBCMT)
.ENDIF


.INCLUDE :  target.mk


.IF "$(GUI)"=="UNX"
ALLTAR: REGCOMPSH

REGCOMPSH : $(UNIXTEXT)
    +-chmod +x $(UNIXTEXT)
.ENDIF	
