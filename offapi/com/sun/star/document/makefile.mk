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
# $Revision: 1.46 $
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

PRJ=..$/..$/..$/..

PRJNAME=offapi

TARGET=cssdocument
PACKAGE=com$/sun$/star$/document

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    AmbigousFilterRequest.idl\
    BrokenPackageRequest.idl\
        CorruptedFilterConfigurationException.idl\
    DocumentInfo.idl\
        DocumentProperties.idl\
    EventDescriptor.idl\
    EventObject.idl\
    Events.idl\
    ExportFilter.idl\
    ExtendedTypeDetection.idl\
        ExtendedTypeDetectionFactory.idl\
    FilterAdapter.idl\
    FilterFactory.idl\
    FilterOptionsRequest.idl\
    HeaderFooterSettings.idl\
    ImportFilter.idl\
    LinkTarget.idl\
    LinkTargets.idl\
    LockedDocumentRequest.idl\
    MacroExecMode.idl\
    MediaDescriptor.idl\
    NoSuchFilterRequest.idl\
    OfficeDocument.idl\
    OleEmbeddedServerRegistration.idl\
    PrinterIndependentLayout.idl\
    RedlineDisplayType.idl \
    Settings.idl\
    StandaloneDocumentInfo.idl\
    TypeDetection.idl\
    UpdateDocMode.idl\
    XActionLockable.idl\
    XBinaryStreamResolver.idl\
    XDocumentInfo.idl\
    XDocumentInfoSupplier.idl\
    XDocumentInsertable.idl\
    XDocumentProperties.idl\
    XDocumentPropertiesSupplier.idl\
    XDocumentSubStorageSupplier.idl\
    XEmbeddedObjectResolver.idl\
    XEmbeddedObjectSupplier.idl\
    XEmbeddedObjectSupplier2.idl\
    XEmbeddedScripts.idl\
    XEventBroadcaster.idl\
    XEventListener.idl\
    XEventsSupplier.idl\
    XExporter.idl\
    XExtendedFilterDetection.idl\
    XFilter.idl\
    XFilterAdapter.idl\
    XGraphicObjectResolver.idl\
    XImporter.idl\
    XInteractionFilterOptions.idl\
    XInteractionFilterSelect.idl\
    XLinkTargetSupplier.idl\
    XMimeTypeInfo.idl\
    XRedlinesSupplier.idl \
    XScriptInvocationContext.idl\
    XStandaloneDocumentInfo.idl\
    XStorageBasedDocument.idl\
    XStorageChangeListener.idl\
    XTypeDetection.idl\
    XViewDataSupplier.idl\
    LinkUpdateModes.idl\
    XDocumentRevisionListPersistence.idl\
    DocumentRevisionListPersistence.idl \
    XDocumentLanguages.idl

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
