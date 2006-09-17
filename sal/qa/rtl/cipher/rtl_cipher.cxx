/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rtl_cipher.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 08:55:20 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"

#include <cppunit/simpleheader.hxx>
#include <rtl/strbuf.hxx>
#include <rtl/cipher.h>

// -----------------------------------------------------------------------------
namespace rtl_cipher
{

rtl::OString createHex(sal_uInt8 *_pKeyBuffer, sal_uInt32 _nKeyLen)
{
    // Create hex-value string from the  value to keep the string size minimal
    rtl::OStringBuffer aBuffer( _nKeyLen * 2 + 1 );
    for ( sal_uInt32 i = 0; i < _nKeyLen; i++ )
    {
        sal_Int32 nValue = (sal_Int32)_pKeyBuffer[i];
        if (nValue < 16)                         // maximul hex value for 1 byte
        {
            aBuffer.append( sal_Int32(0), 16 /* radix */ );
        }
        aBuffer.append( nValue, 16 /* radix */ );
    }
    
    return aBuffer.makeStringAndClear();
}

// -----------------------------------------------------------------------------

class create : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void create_001()    
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);
            rtl_cipher_destroy(aCipher);
        }
    void create_002()    
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmInvalid, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create provide wrong object.", aCipher == NULL);
        }
    void create_003()    
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeCBC);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);
            rtl_cipher_destroy(aCipher);
        }
    void create_004()    
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmInvalid, rtl_Cipher_ModeCBC);
            CPPUNIT_ASSERT_MESSAGE("create provide wrong object.", aCipher == NULL);
        }
    void create_005()    
        {    
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeStream);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);
            rtl_cipher_destroy(aCipher);
        }
    void create_006()    
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmInvalid, rtl_Cipher_ModeStream);
            CPPUNIT_ASSERT_MESSAGE("create provide wrong object.", aCipher == NULL);
        }
    void create_007()    
        {   
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeInvalid);
            CPPUNIT_ASSERT_MESSAGE("create provide wrong object.", aCipher == NULL);
        }
    void create_008()    
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmInvalid, rtl_Cipher_ModeInvalid);
            CPPUNIT_ASSERT_MESSAGE("create provide wrong object.", aCipher == NULL);
        }

    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(create);
    CPPUNIT_TEST(create_001);
    CPPUNIT_TEST(create_002);
    CPPUNIT_TEST(create_003);
    CPPUNIT_TEST(create_004);
    CPPUNIT_TEST(create_005);
    CPPUNIT_TEST(create_006);
    CPPUNIT_TEST(create_007);
    CPPUNIT_TEST(create_008);
    CPPUNIT_TEST_SUITE_END();
}; // class create

// -----------------------------------------------------------------------------
class createBF : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void createBF_001()    
        {
            rtlCipher aCipher = rtl_cipher_createBF(rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);
            rtl_cipher_destroy(aCipher);
        }
    void createBF_002()    
        {
            rtlCipher aCipher = rtl_cipher_createBF(rtl_Cipher_ModeCBC);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);
            rtl_cipher_destroy(aCipher);
        }
    void createBF_003()    
        {
            rtlCipher aCipher = rtl_cipher_createBF(rtl_Cipher_ModeStream);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);
            rtl_cipher_destroy(aCipher);
        }
    void createBF_004()    
        {
            rtlCipher aCipher = rtl_cipher_createBF(rtl_Cipher_ModeInvalid);
            CPPUNIT_ASSERT_MESSAGE("create provide wrong object.", aCipher == NULL);
            // rtl_cipher_destroy(aCipher);
        }
    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(createBF);
    CPPUNIT_TEST(createBF_001);
    CPPUNIT_TEST(createBF_002);
    CPPUNIT_TEST(createBF_003);
    CPPUNIT_TEST(createBF_004);
    CPPUNIT_TEST_SUITE_END();
}; // class createBF
// -----------------------------------------------------------------------------
class decode : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void test_encode(sal_uInt8 _nKeyValue, sal_uInt8 _nArgValue, rtl::OString const& _sPlainTextStr)
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);

            sal_uInt32     nKeyLen = 16;
            sal_uInt8     *pKeyBuffer = new sal_uInt8[ nKeyLen ];
            memset(pKeyBuffer, 0, nKeyLen);
            pKeyBuffer[0] = _nKeyValue;

            sal_uInt32     nArgLen = 16;
            sal_uInt8     *pArgBuffer = new sal_uInt8[ nArgLen ];
            memset(pArgBuffer, 0, nArgLen);
            pArgBuffer[0] = _nArgValue;

            t_print(T_VERBOSE, "  init Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "  init Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            rtlCipherError aError = rtl_cipher_init(aCipher, rtl_Cipher_DirectionEncode, pKeyBuffer, nKeyLen, pArgBuffer, nArgLen);
            CPPUNIT_ASSERT_MESSAGE("wrong init", aError == rtl_Cipher_E_None);

            sal_uInt32     nPlainTextLen = 16;
            sal_uInt8     *pPlainTextBuffer = new sal_uInt8[ nPlainTextLen ];
            memset(pPlainTextBuffer, 0, nPlainTextLen);
            strncpy((char*)pPlainTextBuffer, _sPlainTextStr.getStr(), 16);

            sal_uInt32     nCipherLen = 16;
            sal_uInt8     *pCipherBuffer = new sal_uInt8[ nCipherLen ];
            memset(pCipherBuffer, 0, nCipherLen);

            /* rtlCipherError */ aError = rtl_cipher_encode(aCipher, pPlainTextBuffer, nPlainTextLen, pCipherBuffer, nCipherLen);
            CPPUNIT_ASSERT_MESSAGE("wrong encode", aError == rtl_Cipher_E_None);

            t_print(T_VERBOSE, "       Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "       Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());
            t_print(T_VERBOSE, "     Plain: %s\n", createHex(pPlainTextBuffer, nPlainTextLen).getStr());
            t_print(           "Cipher Buf: %s\n", createHex(pCipherBuffer, nCipherLen).getStr());

            sal_uInt32     nPlainText2Len = 16;
            sal_uInt8     *pPlainText2Buffer = new sal_uInt8[ nPlainText2Len ];
            memset(pPlainText2Buffer, 0, nPlainText2Len);

            /* rtlCipherError */ aError = rtl_cipher_decode(aCipher, pCipherBuffer, nCipherLen, pPlainText2Buffer, nPlainText2Len);
            CPPUNIT_ASSERT_MESSAGE("decode should not work", aError != rtl_Cipher_E_None);
            
            // rtl::OString sPlainText2Str((char*)pPlainText2Buffer, nPlainText2Len);
            // t_print(T_VERBOSE, " Plain: %s\n", createHex(pPlainText2Buffer, nPlainText2Len).getStr());
            // t_print(T_VERBOSE, " ascii: %s\n", sPlainText2Str.getStr());
            // 
            // // t_print("   Buf: %s\n", createHex(pCipherBuffer, nCipherLen).getStr());
            // 
            // sal_Int32 nCompare = memcmp(pPlainTextBuffer, pPlainText2Buffer, 16);
            // 
            // CPPUNIT_ASSERT_MESSAGE("compare between plain and decoded plain failed", nCompare == 0);
            // 
            // delete [] pPlainText2Buffer;
            // 
            // delete [] pCipherBuffer;
            // delete [] pPlainTextBuffer;
            // 
            // delete [] pArgBuffer;
            // delete [] pKeyBuffer;
            // 
            // rtl_cipher_destroy(aCipher);
        }

    void test_encode_and_decode(sal_uInt8 _nKeyValue, sal_uInt8 _nArgValue, rtl::OString const& _sPlainTextStr)
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);

            sal_uInt32     nKeyLen = 16;
            sal_uInt8     *pKeyBuffer = new sal_uInt8[ nKeyLen ];
            memset(pKeyBuffer, 0, nKeyLen);
            pKeyBuffer[0] = _nKeyValue;

            sal_uInt32     nArgLen = 16;
            sal_uInt8     *pArgBuffer = new sal_uInt8[ nArgLen ];
            memset(pArgBuffer, 0, nArgLen);
            pArgBuffer[0] = _nArgValue;

            t_print(T_VERBOSE, "  init Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "  init Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            rtlCipherError aError = rtl_cipher_init(aCipher, rtl_Cipher_DirectionBoth, pKeyBuffer, nKeyLen, pArgBuffer, nArgLen);
            CPPUNIT_ASSERT_MESSAGE("wrong init", aError == rtl_Cipher_E_None);

            sal_uInt32     nPlainTextLen = 16;
            sal_uInt8     *pPlainTextBuffer = new sal_uInt8[ nPlainTextLen ];
            memset(pPlainTextBuffer, 0, nPlainTextLen);
            strncpy((char*)pPlainTextBuffer, _sPlainTextStr.getStr(), 16);

            sal_uInt32     nCipherLen = 16;
            sal_uInt8     *pCipherBuffer = new sal_uInt8[ nCipherLen ];
            memset(pCipherBuffer, 0, nCipherLen);

            /* rtlCipherError */ aError = rtl_cipher_encode(aCipher, pPlainTextBuffer, nPlainTextLen, pCipherBuffer, nCipherLen);
            CPPUNIT_ASSERT_MESSAGE("wrong encode", aError == rtl_Cipher_E_None);

            t_print(T_VERBOSE, "       Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "       Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());
            t_print(T_VERBOSE, "     Plain: %s\n", createHex(pPlainTextBuffer, nPlainTextLen).getStr());
            t_print(           "Cipher Buf: %s\n", createHex(pCipherBuffer, nCipherLen).getStr());

            sal_uInt32     nPlainText2Len = 16;
            sal_uInt8     *pPlainText2Buffer = new sal_uInt8[ nPlainText2Len ];
            memset(pPlainText2Buffer, 0, nPlainText2Len);

            /* rtlCipherError */ aError = rtl_cipher_decode(aCipher, pCipherBuffer, nCipherLen, pPlainText2Buffer, nPlainText2Len);
            CPPUNIT_ASSERT_MESSAGE("wrong decode", aError == rtl_Cipher_E_None);
            
            rtl::OString sPlainText2Str((char*)pPlainText2Buffer, nPlainText2Len);
            t_print(T_VERBOSE, "     Plain: %s\n", createHex(pPlainText2Buffer, nPlainText2Len).getStr());
            t_print(T_VERBOSE, "  as ascii: %s\n", sPlainText2Str.getStr());

            // t_print("   Buf: %s\n", createHex(pCipherBuffer, nCipherLen).getStr());

            sal_Int32 nCompare = memcmp(pPlainTextBuffer, pPlainText2Buffer, 16);
            
            CPPUNIT_ASSERT_MESSAGE("compare between plain and decoded plain failed", nCompare == 0);

            delete [] pPlainText2Buffer;

            delete [] pCipherBuffer;
            delete [] pPlainTextBuffer;

            delete [] pArgBuffer;
            delete [] pKeyBuffer;

            rtl_cipher_destroy(aCipher);
        }

    void decode_001()
        {
            test_encode_and_decode(0,0,"");
            test_encode_and_decode(0,0,"hallo");
            test_encode_and_decode(1,0,"B2Aahg5B");
            test_encode_and_decode(1,2,"Longer text string");
        }

    void decode_002()
        {
            test_encode(0,0,"");
            test_encode(0,0,"hallo");
            test_encode(1,0,"B2Aahg5B");
            test_encode(1,2,"Longer text string");
        }
    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(decode);
    CPPUNIT_TEST(decode_001);
    CPPUNIT_TEST(decode_002);
    CPPUNIT_TEST_SUITE_END();
}; // class decode
// -----------------------------------------------------------------------------
class decodeBF : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void decodeBF_001()    
        {
        }
    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(decodeBF);
    CPPUNIT_TEST(decodeBF_001);
    CPPUNIT_TEST_SUITE_END();
}; // class decodeBF
// -----------------------------------------------------------------------------
class destroy : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void destroy_001()    
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeCBC);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);
            rtl_cipher_destroy(aCipher);
        }
    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(destroy);
    CPPUNIT_TEST(destroy_001);
    CPPUNIT_TEST_SUITE_END();
}; // class destroy
// -----------------------------------------------------------------------------
class destroyBF : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void destroyBF_001()    
        {
            rtlCipher aCipher = rtl_cipher_createBF(rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);
            rtl_cipher_destroyBF(aCipher);
            // more proforma
            // should not GPF
        }
    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(destroyBF);
    CPPUNIT_TEST(destroyBF_001);
    CPPUNIT_TEST_SUITE_END();
}; // class destroyBF
// -----------------------------------------------------------------------------
class encode : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void test_encode(sal_uInt8 _nKeyValue, sal_uInt8 _nArgValue, sal_uInt8 _nDataValue)
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);

            sal_uInt32     nKeyLen = 16;
            sal_uInt8     *pKeyBuffer = new sal_uInt8[ nKeyLen ];
            memset(pKeyBuffer, 0, nKeyLen);
            pKeyBuffer[0] = _nKeyValue;

            sal_uInt32     nArgLen = 16;
            sal_uInt8     *pArgBuffer = new sal_uInt8[ nArgLen ];
            memset(pArgBuffer, 0, nArgLen);
            pArgBuffer[0] = _nArgValue;

            t_print(T_VERBOSE, "init Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "init Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            rtlCipherError aError = rtl_cipher_init(aCipher, rtl_Cipher_DirectionEncode, pKeyBuffer, nKeyLen, pArgBuffer, nArgLen);
            CPPUNIT_ASSERT_MESSAGE("wrong init", aError == rtl_Cipher_E_None);

            sal_uInt32     nDataLen = 16;
            sal_uInt8     *pDataBuffer = new sal_uInt8[ nDataLen ];
            memset(pDataBuffer, 0, nDataLen);
            pDataBuffer[0] = _nDataValue;

            sal_uInt32     nLen = 16;
            sal_uInt8     *pBuffer = new sal_uInt8[ nLen ];
            memset(pBuffer, 0, nLen);

            /* rtlCipherError */ aError = rtl_cipher_encode(aCipher, pDataBuffer, nDataLen, pBuffer, nLen);
            CPPUNIT_ASSERT_MESSAGE("wrong encode", aError == rtl_Cipher_E_None);

            t_print(T_VERBOSE, " Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, " Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());
            t_print(T_VERBOSE, "Data: %s\n", createHex(pDataBuffer, nDataLen).getStr());
            t_print(T_VERBOSE, " Buf: %s\n", createHex(pBuffer, nLen).getStr());

            delete [] pBuffer;
            delete [] pDataBuffer;

            delete [] pArgBuffer;
            delete [] pKeyBuffer;

            rtl_cipher_destroy(aCipher);
        }

    void encode_001()
        {
            test_encode(0,0,0);
            test_encode(1,0,0);
            test_encode(0,1,0);
            test_encode(1,1,0);

            test_encode(0,0,1);
            test_encode(1,0,1);
            test_encode(0,1,1);
            test_encode(1,1,1);
        }

    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(encode);
    CPPUNIT_TEST(encode_001);
    CPPUNIT_TEST_SUITE_END();
}; // class encode
// -----------------------------------------------------------------------------
class encodeBF : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void encodeBF_001()    
        {
        }
    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(encodeBF);
    CPPUNIT_TEST(encodeBF_001);
    CPPUNIT_TEST_SUITE_END();
}; // class encodeBF
// -----------------------------------------------------------------------------
class init : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void init_001()
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);

            sal_uInt32     nKeyLen = 16;
            sal_uInt8     *pKeyBuffer = new sal_uInt8[ nKeyLen ];
            memset(pKeyBuffer, 0, nKeyLen);

            sal_uInt32     nArgLen = 16;
            sal_uInt8     *pArgBuffer = new sal_uInt8[ nArgLen ];
            memset(pArgBuffer, 0, nArgLen);

            t_print(T_VERBOSE, "Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            rtlCipherError aError = rtl_cipher_init(aCipher, rtl_Cipher_DirectionEncode, pKeyBuffer, nKeyLen, pArgBuffer, nArgLen);
            CPPUNIT_ASSERT_MESSAGE("wrong init", aError == rtl_Cipher_E_None);

            t_print(T_VERBOSE, "Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            delete [] pArgBuffer;
            delete [] pKeyBuffer;

            rtl_cipher_destroy(aCipher);
        }

    void init_002()
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);

            sal_uInt32     nKeyLen = 16;
            sal_uInt8     *pKeyBuffer = new sal_uInt8[ nKeyLen ];
            memset(pKeyBuffer, 0, nKeyLen);
            pKeyBuffer[0] = 1;

            sal_uInt32     nArgLen = 16;
            sal_uInt8     *pArgBuffer = new sal_uInt8[ nArgLen ];
            memset(pArgBuffer, 0, nArgLen);

            t_print(T_VERBOSE, "Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            rtlCipherError aError = rtl_cipher_init(aCipher, rtl_Cipher_DirectionEncode, pKeyBuffer, nKeyLen, pArgBuffer, nArgLen);
            CPPUNIT_ASSERT_MESSAGE("wrong init", aError == rtl_Cipher_E_None);

            t_print(T_VERBOSE, "Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            delete [] pArgBuffer;
            delete [] pKeyBuffer;

            rtl_cipher_destroy(aCipher);
        }
    void init_003()
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);

            sal_uInt32     nKeyLen = 16;
            sal_uInt8     *pKeyBuffer = new sal_uInt8[ nKeyLen ];
            memset(pKeyBuffer, 0, nKeyLen);

            sal_uInt32     nArgLen = 16;
            sal_uInt8     *pArgBuffer = new sal_uInt8[ nArgLen ];
            memset(pArgBuffer, 0, nArgLen);
            pArgBuffer[0] = 1;

            t_print(T_VERBOSE, "Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            rtlCipherError aError = rtl_cipher_init(aCipher, rtl_Cipher_DirectionEncode, pKeyBuffer, nKeyLen, pArgBuffer, nArgLen);
            CPPUNIT_ASSERT_MESSAGE("wrong init", aError == rtl_Cipher_E_None);

            t_print(T_VERBOSE, "Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            delete [] pArgBuffer;
            delete [] pKeyBuffer;

            rtl_cipher_destroy(aCipher);
        }
    void init_004()
        {
            rtlCipher aCipher = rtl_cipher_create(rtl_Cipher_AlgorithmBF, rtl_Cipher_ModeECB);
            CPPUNIT_ASSERT_MESSAGE("create failed.", aCipher != NULL);

            sal_uInt32     nKeyLen = 16;
            sal_uInt8     *pKeyBuffer = new sal_uInt8[ nKeyLen ];
            memset(pKeyBuffer, 0, nKeyLen);
            pKeyBuffer[0] = 1;

            sal_uInt32     nArgLen = 16;
            sal_uInt8     *pArgBuffer = new sal_uInt8[ nArgLen ];
            memset(pArgBuffer, 0, nArgLen);
            pArgBuffer[0] = 1;

            t_print(T_VERBOSE, "Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            rtlCipherError aError = rtl_cipher_init(aCipher, rtl_Cipher_DirectionEncode, pKeyBuffer, nKeyLen, pArgBuffer, nArgLen);
            CPPUNIT_ASSERT_MESSAGE("wrong init", aError == rtl_Cipher_E_None);

            t_print(T_VERBOSE, "Key: %s\n", createHex(pKeyBuffer, nKeyLen).getStr());
            t_print(T_VERBOSE, "Arg: %s\n", createHex(pArgBuffer, nArgLen).getStr());

            delete [] pArgBuffer;
            delete [] pKeyBuffer;

            rtl_cipher_destroy(aCipher);
        }
    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(init);
    CPPUNIT_TEST(init_001);
    CPPUNIT_TEST(init_002);
    CPPUNIT_TEST(init_003);
    CPPUNIT_TEST(init_004);
    CPPUNIT_TEST_SUITE_END();
}; // class init
// -----------------------------------------------------------------------------
class initBF : public CppUnit::TestFixture
{
public:
    // initialise your test code values here.
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void initBF_001()    
        {
            // seems to be the same as init, so empty
        }

    // Change the following lines only, if you add, remove or rename 
    // member functions of the current class, 
    // because these macros are need by auto register mechanism.

    CPPUNIT_TEST_SUITE(initBF);
    CPPUNIT_TEST(initBF_001);
    CPPUNIT_TEST_SUITE_END();
}; // class initBF

// -----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::create, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::createBF, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::decode, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::decodeBF, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::destroy, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::destroyBF, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::encode, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::encodeBF, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::init, "rtl_cipher");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(rtl_cipher::initBF, "rtl_cipher");

} // namespace rtl_cipher


// -----------------------------------------------------------------------------

// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.
NOADDITIONAL;
