#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.6 $
#
#   last change: $Author: rt $ $Date: 2004-07-23 15:17:37 $
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

PRJ=..$/..$/..$/..$/..$/..$/..
PRJNAME = juhelper
PACKAGE = com$/sun$/star$/lib$/uno$/helper
TARGET  = com_sun_star_lib_uno_helper_test

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Files --------------------------------------------------------

JARFILES = jurt.jar ridl.jar juh.jar

JAVACLASSFILES=	\
    $(CLASSDIR)$/$(PACKAGE)$/WeakBase_Test.class \
    $(CLASSDIR)$/$(PACKAGE)$/ComponentBase_Test.class \
    $(CLASSDIR)$/$(PACKAGE)$/InterfaceContainer_Test.class \
    $(CLASSDIR)$/$(PACKAGE)$/MultiTypeInterfaceContainer_Test.class \
    $(CLASSDIR)$/$(PACKAGE)$/ProxyProvider.class \
    $(CLASSDIR)$/$(PACKAGE)$/AWeakBase.class    \
        $(CLASSDIR)$/$(PACKAGE)$/PropertySet_Test.class \
    $(CLASSDIR)$/$(PACKAGE)$/UnoUrlTest.class	\
    $(CLASSDIR)$/$(PACKAGE)$/Factory_Test.class

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

CPATH_JARS = java_uno.jar $(JARFILES)
CPATH_TMP1 = $(foreach,j,$(CPATH_JARS) $(SOLARBINDIR)$/$j)
CPATH_TMP2 = $(strip $(subst,!,$(PATH_SEPERATOR) $(CPATH_TMP1:s/ /!/)))
CPATH = $(CPATH_TMP2)$(PATH_SEPERATOR)$(OUT)$/bin$/factory_test.jar$(PATH_SEPERATOR)$(XCLASSPATH)

$(OUT)$/bin$/factory_test.jar : $(CLASSDIR)$/$(PACKAGE)$/Factory_Test.class
    -rm -f $@
    @echo RegistrationClassName: com.sun.star.lib.uno.helper.Factory_Test > $(OUT)$/bin$/manifest.mf
    -jar cvfm $@ $(OUT)$/bin$/manifest.mf -C $(CLASSDIR) $(PACKAGE)$/Factory_Test.class

run_factory_test : $(OUT)$/bin$/factory_test.jar
    -$(GNUCOPY) $(SOLARBINDIR)$/udkapi.rdb $(OUT)$/bin$/factory_test.rdb
    -java -classpath $(CPATH) com.sun.star.lib.uno.helper.Factory_Test $(OUT)$/bin$/factory_test.jar $(OUT)$/bin$/factory_test.rdb
