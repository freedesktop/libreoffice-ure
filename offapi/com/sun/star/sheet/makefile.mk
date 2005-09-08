#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.22 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 05:25:33 $
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

PRJ=..$/..$/..$/..

PRJNAME=api

TARGET=csssheet
PACKAGE=com$/sun$/star$/sheet

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    AccessibleCell.idl\
    AccessibleSpreadsheet.idl\
    AccessibleSpreadsheetDocumentView.idl\
    AccessibleSpreadsheetPageView.idl\
    AccessiblePageHeaderFooterAreasView.idl\
    AccessibleCsvRuler.idl\
    AccessibleCsvTable.idl\
    AccessibleCsvCell.idl\
    ActivationEvent.idl\
    CellAnnotationsEnumeration.idl\
    CellAreaLinksEnumeration.idl\
    DatabaseRangesEnumeration.idl\
    DataPilotFieldsEnumeration.idl\
    DataPilotItem.idl\
    DataPilotItems.idl\
    DataPilotItemsEnumeration.idl\
    DataPilotTablesEnumeration.idl\
    DDELinksEnumeration.idl\
    FunctionCategory.idl\
    FunctionDescriptionEnumeration.idl\
    LabelRangesEnumeration.idl\
    MoveDirection.idl\
    NamedRangesEnumeration.idl\
    ScenariosEnumeration.idl\
    SheetCellRangesEnumeration.idl\
    SheetLinksEnumeration.idl\
    SpreadsheetsEnumeration.idl\
    SpreadsheetViewPanesEnumeration.idl\
    StatusBarFunction.idl\
    SubTotalFieldsEnumeration.idl\
    TableAutoFormatEnumeration.idl\
    TableAutoFormatsEnumeration.idl\
    TableCellStyle.idl\
    TableConditionalEntryEnumeration.idl\
    UniqueCellFormatRanges.idl\
    UniqueCellFormatRangesEnumeration.idl\
    RangeSelectionArguments.idl\
    RangeSelectionEvent.idl\
    XRangeSelection.idl\
    XRangeSelectionChangeListener.idl\
    XRangeSelectionListener.idl\
    AddIn.idl\
    Border.idl\
    CellAnnotation.idl\
    CellAnnotations.idl\
    CellAnnotationsEnumeration.idl\
    CellAnnotationShape.idl\
    CellAreaLink.idl\
    CellAreaLinks.idl\
    CellAreaLinksEnumeration.idl\
    CellDeleteMode.idl\
    CellFlags.idl\
    CellFormatRanges.idl\
    CellFormatRangesEnumeration.idl\
    CellInsertMode.idl\
    Cells.idl\
    CellsEnumeration.idl\
    ConditionOperator.idl\
    ConsolidationDescriptor.idl\
    DatabaseImportDescriptor.idl\
    DatabaseRange.idl\
    DatabaseRanges.idl\
    DatabaseRangesEnumeration.idl\
    DataImportMode.idl\
    DataPilotDescriptor.idl\
    DataPilotField.idl\
    DataPilotFieldAutoShowInfo.idl\
    DataPilotFieldGroup.idl\
    DataPilotFieldGroupBy.idl\
    DataPilotFieldGroupInfo.idl\
    DataPilotFieldGroupItem.idl\
    DataPilotFieldGroups.idl\
    DataPilotFieldLayoutInfo.idl\
    DataPilotFieldLayoutMode.idl\
    DataPilotFieldOrientation.idl\
    DataPilotFieldReference.idl\
    DataPilotFieldReferenceType.idl\
    DataPilotFieldReferenceItemType.idl\
    DataPilotFieldShowItemsMode.idl\
    DataPilotFieldSortInfo.idl\
    DataPilotFieldSortMode.idl\
    DataPilotFields.idl\
    DataPilotFieldsEnumeration.idl\
    DataPilotSource.idl\
    DataPilotSourceDimension.idl\
    DataPilotSourceDimensions.idl\
    DataPilotSourceHierarchies.idl\
    DataPilotSourceHierarchy.idl\
    DataPilotSourceLevel.idl\
    DataPilotSourceLevels.idl\
    DataPilotSourceMember.idl\
    DataPilotSourceMembers.idl\
    DataPilotTable.idl\
    DataPilotTables.idl\
    DataPilotTablesEnumeration.idl\
    DataResult.idl\
    DataResultFlags.idl\
    DDELink.idl\
    DDELinks.idl\
    DDELinksEnumeration.idl\
    DocumentSettings.idl\
    FillDateMode.idl\
    FillDirection.idl\
    FillMode.idl\
    FilterConnection.idl\
    FilterOperator.idl\
    FormulaResult.idl\
    FunctionAccess.idl\
    FunctionArgument.idl\
    FunctionCategory.idl\
    FunctionDescription.idl\
    FunctionDescriptions.idl\
    FunctionDescriptionEnumeration.idl\
    GeneralFunction.idl\
    GlobalSheetSettings.idl\
    GoalResult.idl\
    HeaderFooterContent.idl\
    LabelRange.idl\
    LabelRanges.idl\
    LabelRangesEnumeration.idl\
    LocalizedName.idl\
    MemberResult.idl\
    MemberResultFlags.idl\
    MoveDirection.idl\
    NamedRange.idl\
    NamedRangeFlag.idl\
    NamedRanges.idl\
    NamedRangesEnumeration.idl\
    PasteOperation.idl\
    RangeSelectionArguments.idl\
    RangeSelectionEvent.idl\
    RecentFunctions.idl\
    ResultEvent.idl\
    Scenario.idl\
    Scenarios.idl\
    ScenariosEnumeration.idl\
    Shape.idl\
    SheetCell.idl\
    SheetCellCursor.idl\
    SheetCellRange.idl\
    SheetCellRanges.idl\
    SheetCellRangesEnumeration.idl\
    SheetFilterDescriptor.idl\
    SheetLink.idl\
    SheetLinkMode.idl\
    SheetLinks.idl\
    SheetLinksEnumeration.idl\
    SheetRangesQuery.idl\
    SheetSortDescriptor.idl\
    SheetSortDescriptor2.idl\
    Spreadsheet.idl\
    SpreadsheetDocument.idl\
    SpreadsheetDocumentSettings.idl\
    SpreadsheetDrawPage.idl\
    Spreadsheets.idl\
    SpreadsheetsEnumeration.idl\
    SpreadsheetView.idl\
    SpreadsheetViewPane.idl\
    SpreadsheetViewPanesEnumeration.idl\
    SpreadsheetViewSettings.idl\
    StatusBarFunction.idl\
    SubTotalColumn.idl\
    SubTotalDescriptor.idl\
    SubTotalField.idl\
    SubTotalFieldsEnumeration.idl\
    TableAutoFormat.idl\
    TableAutoFormatEnumeration.idl\
    TableAutoFormatField.idl\
    TableAutoFormats.idl\
    TableAutoFormatsEnumeration.idl\
    TableCellStyle.idl\
    TableConditionalEntry.idl\
    TableConditionalEntryEnumeration.idl\
    TableConditionalFormat.idl\
    TableFilterField.idl\
    TableOperationMode.idl\
    TablePageBreakData.idl\
    TablePageStyle.idl\
    TableValidation.idl\
    UniqueCellFormatRanges.idl\
    UniqueCellFormatRangesEnumeration.idl\
    TableValidationVisibility.idl\
    ValidationAlertStyle.idl\
    ValidationType.idl\
    VolatileResult.idl\
    XActivationEventListener.idl\
    XActivationBroadcaster.idl\
    XAddIn.idl\
    XAreaLink.idl\
    XAreaLinks.idl\
    XArrayFormulaRange.idl\
    XCalculatable.idl\
    XCellAddressable.idl\
    XCellFormatRangesSupplier.idl\
    XCellRangeAddressable.idl\
    XCellRangeData.idl\
    XCellRangeFormula.idl\
    XCellRangeMovement.idl\
    XCellRangeReferrer.idl\
    XCellRangesQuery.idl\
    XCellSeries.idl\
    XCompatibilityNames.idl\
    XConsolidatable.idl\
    XConsolidationDescriptor.idl\
    XDatabaseRange.idl\
    XDatabaseRanges.idl\
    XDataPilotDescriptor.idl\
    XDataPilotField.idl\
    XDataPilotFieldGrouping.idl\
    XDataPilotMemberResults.idl\
    XDataPilotResults.idl\
    XDataPilotTable.idl\
    XDataPilotTables.idl\
    XDataPilotTablesSupplier.idl\
    XDDELink.idl\
    XDimensionsSupplier.idl\
    XDocumentAuditing.idl\
    XEnhancedMouseClickBroadcaster.idl\
    XFillAcrossSheet.idl\
    XFormulaQuery.idl\
    XFunctionAccess.idl\
    XFunctionDescriptions.idl\
    XGoalSeek.idl\
    XHeaderFooterContent.idl\
    XHierarchiesSupplier.idl\
    XLabelRange.idl\
    XLabelRanges.idl\
    XLevelsSupplier.idl\
    XMembersSupplier.idl\
    XMultipleOperation.idl\
    XNamedRange.idl\
    XNamedRanges.idl\
    XPrintAreas.idl\
    XRangeSelection.idl\
    XRangeSelectionChangeListener.idl\
    XRangeSelectionListener.idl\
    XRecentFunctions.idl\
    XResultListener.idl\
    XScenario.idl\
    XScenarioEnhanced.idl\
    XScenarios.idl\
    XScenariosSupplier.idl\
    XSheetAnnotation.idl\
    XSheetAnnotationAnchor.idl\
    XSheetAnnotations.idl\
    XSheetAnnotationShapeSupplier.idl\
    XSheetAnnotationsSupplier.idl\
    XSheetAuditing.idl\
    XSheetCellCursor.idl\
    XSheetCellRange.idl\
    XSheetCellRangeContainer.idl\
    XSheetCellRanges.idl\
    XSheetCondition.idl\
    XSheetConditionalEntries.idl\
    XSheetConditionalEntry.idl\
    XSheetFilterable.idl\
    XSheetFilterableEx.idl\
    XSheetFilterDescriptor.idl\
    XSheetLinkable.idl\
    XSheetOperation.idl\
    XSheetOutline.idl\
    XSheetPageBreak.idl\
    XSheetPastable.idl\
    XSpreadsheet.idl\
    XSpreadsheetDocument.idl\
    XSpreadsheets.idl\
    XSpreadsheetView.idl\
    XSubTotalCalculatable.idl\
    XSubTotalDescriptor.idl\
    XSubTotalField.idl\
    XUniqueCellFormatRangesSupplier.idl\
    XUsedAreaCursor.idl\
    XViewFreezable.idl\
    XViewPane.idl\
    XViewPanesSupplier.idl\
    XViewSplitable.idl\
    XVolatileResult.idl\
    _NamedRange.idl\

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
