/*************************************************************************
 *
 *  $RCSfile: converteuctw.c,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: sb $ $Date: 2001-10-12 10:44:53 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef INCLUDED_RTL_TEXTENC_CONVERTEUCTW_H
#include "converteuctw.h"
#endif

#ifndef INCLUDED_RTL_TEXTENC_CONTEXT_H
#include "context.h"
#endif
#ifndef INCLUDED_RTL_TEXTENC_CONVERTER_H
#include "converter.h"
#endif
#ifndef INCLUDED_RTL_TEXTENC_TENCHELP_H
#include "tenchelp.h"
#endif

#ifndef _RTL_ALLOC_H_
#include "rtl/alloc.h"
#endif
#ifndef _RTL_TEXTCVT_H
#include "rtl/textcvt.h"
#endif
#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif

typedef enum
{
    IMPL_EUC_TW_TO_UNICODE_STATE_0,
    IMPL_EUC_TW_TO_UNICODE_STATE_1,
    IMPL_EUC_TW_TO_UNICODE_STATE_2_1,
    IMPL_EUC_TW_TO_UNICODE_STATE_2_2,
    IMPL_EUC_TW_TO_UNICODE_STATE_2_3
} ImplEucTwToUnicodeState;

typedef struct
{
    ImplEucTwToUnicodeState m_eState;
    sal_Int32 m_nPlane; /* 0--15 */
    sal_Int32 m_nRow; /* 0--93 */
} ImplEucTwToUnicodeContext;

void * ImplCreateEucTwToUnicodeContext(void)
{
    void * pContext = rtl_allocateMemory(sizeof (ImplEucTwToUnicodeContext));
    ((ImplEucTwToUnicodeContext *) pContext)->m_eState
        = IMPL_EUC_TW_TO_UNICODE_STATE_0;
    return pContext;
}

void ImplResetEucTwToUnicodeContext(void * pContext)
{
    if (pContext)
        ((ImplEucTwToUnicodeContext *) pContext)->m_eState
            = IMPL_EUC_TW_TO_UNICODE_STATE_0;
}

sal_Size ImplConvertEucTwToUnicode(ImplTextConverterData const * pData,
                                   void * pContext,
                                   sal_Char const * pSrcBuf,
                                   sal_Size nSrcBytes,
                                   sal_Unicode * pDestBuf,
                                   sal_Size nDestChars,
                                   sal_uInt32 nFlags,
                                   sal_uInt32 * pInfo,
                                   sal_Size * pSrcCvtBytes)
{
    sal_Unicode const * pCns116431992Data
        = ((ImplEucTwConverterData const *) pData)->
              m_pCns116431992ToUnicodeData;
    sal_Int32 const * pCns116431992RowOffsets
        = ((ImplEucTwConverterData const *) pData)->
              m_pCns116431992ToUnicodeRowOffsets;
    sal_Int32 const * pCns116431992PlaneOffsets
        = ((ImplEucTwConverterData const *) pData)->
              m_pCns116431992ToUnicodePlaneOffsets;
    ImplEucTwToUnicodeState eState = IMPL_EUC_TW_TO_UNICODE_STATE_0;
    sal_Int32 nPlane;
    sal_Int32 nRow;
    sal_uInt32 nInfo = 0;
    sal_Size nConverted = 0;
    sal_Unicode * pDestBufPtr = pDestBuf;
    sal_Unicode * pDestBufEnd = pDestBuf + nDestChars;

    if (pContext)
    {
        eState = ((ImplEucTwToUnicodeContext *) pContext)->m_eState;
        nPlane = ((ImplEucTwToUnicodeContext *) pContext)->m_nPlane;
        nRow = ((ImplEucTwToUnicodeContext *) pContext)->m_nRow;
    }

    for (; nConverted < nSrcBytes; ++nConverted)
    {
        sal_Bool bUndefined = sal_True;
        sal_uInt32 nChar = *((sal_uChar const *) pSrcBuf)++;
        switch (eState)
        {
        case IMPL_EUC_TW_TO_UNICODE_STATE_0:
            if (nChar < 0x80)
                if (pDestBufPtr != pDestBufEnd)
                    *pDestBufPtr++ = (sal_Unicode) nChar;
                else
                    goto no_output;
            else if (nChar >= 0xA1 && nChar <= 0xFE)
            {
                nRow = nChar - 0xA1;
                eState = IMPL_EUC_TW_TO_UNICODE_STATE_1;
            }
            else if (nChar == 0x8E)
                eState = IMPL_EUC_TW_TO_UNICODE_STATE_2_1;
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_EUC_TW_TO_UNICODE_STATE_1:
            if (nChar >= 0xA1 && nChar <= 0xFE)
            {
                sal_Int32 nOffset = pCns116431992RowOffsets[nRow];
                if (nOffset == -1)
                    goto bad_input;
                else
                {
                    sal_Unicode nUnicode;
                    nOffset += nChar - 0xA1;
                    nUnicode = pCns116431992Data[nOffset];
                    if (nUnicode == 0xFFFF)
                        goto bad_input;
                    else if (nUnicode < RTL_UNICODE_START_HIGH_SURROGATES
                             || nUnicode > RTL_UNICODE_END_HIGH_SURROGATES)
                        if (pDestBufPtr != pDestBufEnd)
                            *pDestBufPtr++ = nUnicode;
                        else
                            goto no_output;
                    else
                        if (pDestBufEnd - pDestBufPtr >= 2)
                        {
                            *pDestBufPtr++ = nUnicode;
                            *pDestBufPtr++ = pCns116431992Data[nOffset + 94];
                        }
                        else
                            goto no_output;
                    eState = IMPL_EUC_TW_TO_UNICODE_STATE_0;
                }
            }
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_EUC_TW_TO_UNICODE_STATE_2_1:
            if (nChar >= 0xA1 && nChar <= 0xB0)
            {
                nPlane = nChar - 0xA1;
                ++eState;
            }
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_EUC_TW_TO_UNICODE_STATE_2_2:
            if (nChar >= 0xA1 && nChar <= 0xFE)
            {
                nRow = nChar - 0xA1;
                ++eState;
            }
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_EUC_TW_TO_UNICODE_STATE_2_3:
            if (nChar >= 0xA1 && nChar <= 0xFE)
            {
                sal_Int32 nPlaneOffset = pCns116431992PlaneOffsets[nPlane];
                if (nPlaneOffset == -1)
                    goto bad_input;
                else
                {
                    sal_Int32 nOffset
                        = pCns116431992RowOffsets[nPlaneOffset + nRow];
                    if (nOffset == -1)
                        goto bad_input;
                    else
                    {
                        sal_Unicode nUnicode;
                        nOffset += nChar - 0xA1;
                        nUnicode = pCns116431992Data[nOffset];
                        if (nUnicode == 0xFFFF)
                            goto bad_input;
                        else if (nUnicode < RTL_UNICODE_START_HIGH_SURROGATES
                                 || nUnicode
                                        > RTL_UNICODE_END_HIGH_SURROGATES)
                            if (pDestBufPtr != pDestBufEnd)
                                *pDestBufPtr++ = nUnicode;
                            else
                                goto no_output;
                        else
                            if (pDestBufEnd - pDestBufPtr >= 2)
                            {
                                *pDestBufPtr++ = nUnicode;
                                *pDestBufPtr++
                                    = pCns116431992Data[nOffset + 94];
                            }
                            else
                                goto no_output;
                        eState = IMPL_EUC_TW_TO_UNICODE_STATE_0;
                    }
                }
            }
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;
        }
        continue;

    bad_input:
        switch (ImplHandleBadInputMbTextToUnicodeConversion(bUndefined,
                                                            nFlags,
                                                            &pDestBufPtr,
                                                            pDestBufEnd,
                                                            &nInfo))
        {
        case IMPL_BAD_INPUT_STOP:
            eState = IMPL_EUC_TW_TO_UNICODE_STATE_0;
            break;

        case IMPL_BAD_INPUT_CONTINUE:
            eState = IMPL_EUC_TW_TO_UNICODE_STATE_0;
            continue;

        case IMPL_BAD_INPUT_NO_OUTPUT:
            goto no_output;
        }
        break;

    no_output:
        --pSrcBuf;
        nInfo |= RTL_TEXTTOUNICODE_INFO_DESTBUFFERTOSMALL;
        break;
    }

    if (eState != IMPL_EUC_TW_TO_UNICODE_STATE_0
        && (nInfo & (RTL_TEXTTOUNICODE_INFO_ERROR
                         | RTL_TEXTTOUNICODE_INFO_DESTBUFFERTOSMALL))
               == 0)
        if ((nFlags & RTL_TEXTTOUNICODE_FLAGS_FLUSH) == 0)
            nInfo |= RTL_TEXTTOUNICODE_INFO_SRCBUFFERTOSMALL;
        else
            switch (ImplHandleBadInputMbTextToUnicodeConversion(sal_False,
                                                                nFlags,
                                                                &pDestBufPtr,
                                                                pDestBufEnd,
                                                                &nInfo))
            {
            case IMPL_BAD_INPUT_STOP:
            case IMPL_BAD_INPUT_CONTINUE:
                eState = IMPL_EUC_TW_TO_UNICODE_STATE_0;
                break;

            case IMPL_BAD_INPUT_NO_OUTPUT:
                nInfo |= RTL_TEXTTOUNICODE_INFO_DESTBUFFERTOSMALL;
                break;
            }

    if (pContext)
    {
        ((ImplEucTwToUnicodeContext *) pContext)->m_eState = eState;
        ((ImplEucTwToUnicodeContext *) pContext)->m_nPlane = nPlane;
        ((ImplEucTwToUnicodeContext *) pContext)->m_nRow = nRow;
    }
    if (pInfo)
        *pInfo = nInfo;
    if (pSrcCvtBytes)
        *pSrcCvtBytes = nConverted;

    return pDestBufPtr - pDestBuf;
}

sal_Size ImplConvertUnicodeToEucTw(ImplTextConverterData const * pData,
                                   void * pContext,
                                   sal_Unicode const * pSrcBuf,
                                   sal_Size nSrcChars,
                                   sal_Char * pDestBuf,
                                   sal_Size nDestBytes,
                                   sal_uInt32 nFlags,
                                   sal_uInt32 * pInfo,
                                   sal_Size * pSrcCvtChars)
{
    /* TODO! RTL_UNICODETOTEXT_FLAGS_UNDEFINED_REPLACE
             RTL_UNICODETOTEXT_FLAGS_UNDEFINED_REPLACESTR
             RTL_UNICODETOTEXT_FLAGS_PRIVATE_MAPTO0
             RTL_UNICODETOTEXT_FLAGS_NONSPACING_IGNORE
             RTL_UNICODETOTEXT_FLAGS_CONTROL_IGNORE
             RTL_UNICODETOTEXT_FLAGS_PRIVATE_IGNORE
             RTL_UNICODETOTEXT_FLAGS_NOCOMPOSITE */

    sal_uInt32 const * pCns116431992Data
        = ((ImplEucTwConverterData const *) pData)->
              m_pUnicodeToCns116431992Data;
    sal_Int32 const * pCns116431992PageOffsets
        = ((ImplEucTwConverterData const *) pData)->
              m_pUnicodeToCns116431992PageOffsets;
    sal_Int32 const * pCns116431992PlaneOffsets
        = ((ImplEucTwConverterData const *) pData)->
              m_pUnicodeToCns116431992PlaneOffsets;
    sal_Unicode nHighSurrogate = 0;
    sal_uInt32 nInfo = 0;
    sal_Size nConverted = 0;
    sal_Char * pDestBufPtr = pDestBuf;
    sal_Char * pDestBufEnd = pDestBuf + nDestBytes;

    if (pContext)
        nHighSurrogate
            = ((ImplUnicodeToTextContext *) pContext)->m_nHighSurrogate;

    for (; nConverted < nSrcChars; ++nConverted)
    {
        sal_Bool bUndefined = sal_True;
        sal_uInt32 nChar = *pSrcBuf++;
        if (nHighSurrogate == 0)
            if (nChar < 0x80)
                if (pDestBufPtr != pDestBufEnd)
                    *pDestBufPtr++ = (sal_Char) nChar;
                else
                    goto no_output;
            else if (nChar >= RTL_UNICODE_START_HIGH_SURROGATES
                     && nChar <= RTL_UNICODE_END_HIGH_SURROGATES)
                nHighSurrogate = (sal_Unicode) nChar;
            else if ((nChar < RTL_UNICODE_START_LOW_SURROGATES
                          || nChar > RTL_UNICODE_END_LOW_SURROGATES)
                     && (nChar < 0xFDD0 || nChar > 0xFDEF)
                     && nChar < 0xFFFE)
                goto translate;
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
        else
            if (nChar >= RTL_UNICODE_START_LOW_SURROGATES
                && nChar <= RTL_UNICODE_END_LOW_SURROGATES)
            {
                nChar = ((nHighSurrogate & 0x3FF) << 10 | nChar & 0x3FF)
                            + 0x10000;
                if ((nChar & 0xFFFF) < 0xFFFE)
                    goto translate;
                else
                {
                    bUndefined = sal_False;
                    goto bad_input;
                }
            }
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
        continue;

    translate:
        {
            sal_Int32 nOffset = pCns116431992PlaneOffsets[nChar >> 16];
            sal_uInt32 nData;
            sal_uInt32 nPlane;
            if (nOffset == -1)
                goto bad_input;
            nOffset
                = pCns116431992PageOffsets[nOffset + ((nChar & 0xFF00) >> 8)];
            if (nOffset == -1)
                goto bad_input;
            nData = pCns116431992Data[nOffset + (nChar & 0xFF)];
            if (nData == 0)
                goto bad_input;
            nPlane = nData >> 16;
            if (pDestBufEnd - pDestBufPtr < (nPlane == 1 ? 2 : 4))
                goto no_output;
            if (nPlane != 1)
            {
                *pDestBufPtr++ = (sal_Char) 0x8E;
                *pDestBufPtr++ = (sal_Char) (0xA0 + nPlane);
            }
            *pDestBufPtr++ = (sal_Char) (nData >> 8 & 0xFF);
            *pDestBufPtr++ = (sal_Char) (nData & 0xFF);
            nHighSurrogate = 0;
        }
        continue;

    bad_input:
        switch (ImplHandleBadInputUnicodeToTextConversion(bUndefined,
                                                          nFlags,
                                                          &pDestBufPtr,
                                                          pDestBufEnd,
                                                          &nInfo))
        {
        case IMPL_BAD_INPUT_STOP:
            nHighSurrogate = 0;
            break;

        case IMPL_BAD_INPUT_CONTINUE:
            nHighSurrogate = 0;
            continue;

        case IMPL_BAD_INPUT_NO_OUTPUT:
            goto no_output;
        }
        break;

    no_output:
        --pSrcBuf;
        nInfo |= RTL_UNICODETOTEXT_INFO_DESTBUFFERTOSMALL;
        break;
    }

    if (nHighSurrogate != 0
        && (nInfo & (RTL_UNICODETOTEXT_INFO_ERROR
                         | RTL_UNICODETOTEXT_INFO_DESTBUFFERTOSMALL))
               == 0)
        if ((nFlags & RTL_UNICODETOTEXT_FLAGS_FLUSH) != 0)
            nInfo |= RTL_UNICODETOTEXT_INFO_SRCBUFFERTOSMALL;
        else
            switch (ImplHandleBadInputUnicodeToTextConversion(sal_False,
                                                              nFlags,
                                                              &pDestBufPtr,
                                                              pDestBufEnd,
                                                              &nInfo))
            {
            case IMPL_BAD_INPUT_STOP:
            case IMPL_BAD_INPUT_CONTINUE:
                nHighSurrogate = 0;
                break;

            case IMPL_BAD_INPUT_NO_OUTPUT:
                nInfo |= RTL_UNICODETOTEXT_INFO_DESTBUFFERTOSMALL;
                break;
            }

    if (pContext)
        ((ImplUnicodeToTextContext *) pContext)->m_nHighSurrogate
            = nHighSurrogate;
    if (pInfo)
        *pInfo = nInfo;
    if (pSrcCvtChars)
        *pSrcCvtChars = nConverted;

    return pDestBufPtr - pDestBuf;
}
