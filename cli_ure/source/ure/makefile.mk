#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.15 $
#
#   last change: $Author: kz $ $Date: 2006-02-03 17:15:52 $
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

PRJ = ..$/..
PRJNAME = cli_ure

# for dummy
TARGET = ure

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/util$/makefile.pmk

.INCLUDE : $(PRJ)$/util$/target.pmk
.INCLUDE : target.mk

.IF "$(USE_SHELL)"!="4nt"
ECHOQUOTE='
.ELSE
ECHOQUOTE=
.ENDIF

.IF "$(BUILD_FOR_CLI)" != ""

POLICYASSEMBLY = policy.1.0.cli_ure.dll

#!!! Always change version if code has changed. Provide a publisher
#policy assembly!!!
ASSEMBLY_VERSION="1.0.2.0"

ASSEMBLY_ATTRIBUTES = $(MISC)$/assembly_ure_$(TARGET).cs

ALLTAR : \
    $(BIN)$/cli_ure.dll \
    $(OUT)$/bin$/$(POLICYASSEMBLY)	


CSFILES = \
    uno$/util$/DisposeGuard.cs					\
    uno$/util$/WeakAdapter.cs					\
    uno$/util$/WeakBase.cs						\
    uno$/util$/WeakComponentBase.cs	\
    $(ASSEMBLY_ATTRIBUTES)

$(ASSEMBLY_ATTRIBUTES) : assembly.cs makefile.mk $(BIN)$/cliuno.snk
    $(GNUCOPY) -p assembly.cs $@
    +echo $(ECHOQUOTE) \
    [assembly:System.Reflection.AssemblyVersion( $(ASSEMBLY_VERSION))] \
    [assembly:System.Reflection.AssemblyKeyFile(@"$(BIN)$/cliuno.snk")]$(ECHOQUOTE) \
    >> $@

$(BIN)$/cli_ure.dll : $(CSFILES) $(BIN)$/cli_types.dll
    +$(CSC) $(CSCFLAGS) \
        -target:library \
        -out:$@ \
        -reference:$(OUT)$/bin$/cli_types.dll \
        -reference:System.dll \
        $(CSFILES)
    @echo "If code has changed then provide a policy assembly and change the version!"


#do not forget to deliver cli_ure.config. It is NOT embedded in the policy file.
$(OUT)$/bin$/$(POLICYASSEMBLY) : cli_ure.config
    +$(COPY) cli_ure.config $(OUT)$/bin  
    +$(WRAPCMD) AL.exe -out:$@ \
            -version:2.0.0.0 \
            -keyfile:$(BIN)$/cliuno.snk \
            -link:cli_ure.config


.ENDIF
    

