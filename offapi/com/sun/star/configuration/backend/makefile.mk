#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: rt $ $Date: 2004-03-30 14:50:41 $
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

PRJ = ..$/..$/..$/..$/..
PRJNAME = offapi

TARGET = csscbackend
PACKAGE = com$/sun$/star$/configuration$/backend

.INCLUDE: $(PRJ)$/util$/makefile.pmk

IDLFILES = \
    Backend.idl				\
    SingleBackend.idl	    \
    LocalSingleBackend.idl	\
    LdapSingleBackend.idl	\
    BackendAdapter.idl		\
    SingleBackendAdapter.idl \
    OfflineBackend.idl		\
    OnlineBackend.idl		\
    HierarchyBrowser.idl	\
    LocalHierarchyBrowser.idl	\
    DataImporter.idl	\
    LocalDataImporter.idl	\
    LayerUpdateMerger.idl	\
    Importer.idl			\
    MergeImporter.idl		\
    CopyImporter.idl		\
    Schema.idl				\
    Layer.idl				\
    UpdatableLayer.idl      \
    SingleLayerStratum.idl  \
    LocalSingleStratum.idl  \
    LdapSingleStratum.idl	\
    MultiStratumBackend.idl \
    MultiLayerStratum.idl   \
    LdapMultiLayerStratum.idl \
    SchemaSupplier.idl	    \
    LocalSchemaSupplier.idl \
    PolicyLayerBackend.idl  \
    PlatformBackend.idl  \
    SystemIntegration.idl \
    LayerDescriber.idl    \
    XBackend.idl			\
    XBackendEntities.idl	\
    XSchemaSupplier.idl		\
    XSingleLayerStratum.idl	\
    XMultiLayerStratum.idl	\
    XSchema.idl				\
    XLayer.idl				\
    XCompositeLayer.idl     \
    XUpdatableLayer.idl     \
    XSchemaHandler.idl		\
    XLayerHandler.idl		\
    XUpdateHandler.idl      \
    XLayerImporter.idl      \
    TemplateIdentifier.idl  \
    SchemaAttribute.idl     \
    NodeAttribute.idl       \
    MalformedDataException.idl	\
    BackendSetupException.idl	\
    AuthenticationFailedException.idl	\
    CannotConnectException.idl	\
    InvalidAuthenticationMechanismException.idl \
    BackendAccessException.idl	\
    ConnectionLostException.idl	\
    InsufficientAccessRightsException.idl	\
    ComponentChangeEvent.idl    \
    XBackendChangesNotifier.idl	\
    XBackendChangesListener.idl	\
    PropertyInfo.idl			\
    XLayerContentDescriber.idl


.INCLUDE: target.mk
.INCLUDE: $(PRJ)$/util$/target.pmk
