/*************************************************************************
*
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* Copyright 2000, 2010 Oracle and/or its affiliates.
*
* OpenOffice.org - a multi-platform office productivity suite
*
* This file is part of OpenOffice.org.
*
* OpenOffice.org is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License version 3
* only, as published by the Free Software Foundation.
*
* OpenOffice.org is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License version 3 for more details
* (a copy is included in the LICENSE file that accompanied this code).
*
* You should have received a copy of the GNU Lesser General Public License
* version 3 along with OpenOffice.org.  If not, see
* <http://www.openoffice.org/license.html>
* for a copy of the LGPLv3 License.
*
************************************************************************/

#include "precompiled_sal.hxx"
#include "sal/config.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

#include "cppunittester/protectorfactory.hxx"
#include "osl/module.h"
#include "osl/module.hxx"
#include "osl/thread.h"
#include "rtl/process.h"
#include "rtl/string.h"
#include "rtl/string.hxx"
#include "rtl/textcvt.h"
#include "rtl/ustring.hxx"
#include "sal/main.h"
#include "sal/types.h"

#include "preextstl.h"
#include "cppunit/CompilerOutputter.h"
#include "cppunit/TestResult.h"
#include "cppunit/TestResultCollector.h"
#include "cppunit/TestRunner.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/plugin/PlugInManager.h"
#include "cppunit/portability/Stream.h"
#include "postextstl.h"

namespace {

void usageFailure() {
    std::cerr
        << ("Usage: cppunittester (--protector <shared-library-path>"
            " <function-symbol>)* <shared-library-path>")
        << std::endl;
    std::exit(EXIT_FAILURE);
}

rtl::OUString getArgument(sal_Int32 index) {
    rtl::OUString arg;
    rtl_getAppCommandArg(index, &arg.pData);
    return arg;
}

std::string convertLazy(rtl::OUString const & s16) {
    rtl::OString s8(rtl::OUStringToOString(s16, osl_getThreadTextEncoding()));
    return std::string(
        s8.getStr(),
        ((static_cast< sal_uInt32 >(s8.getLength())
          > std::numeric_limits< std::string::size_type >::max())
         ? std::numeric_limits< std::string::size_type >::max()
         : static_cast< std::string::size_type >(s8.getLength())));
}

std::string convertStrict(rtl::OUString const & s16) {
    rtl::OString s8;
    if (!s16.convertToString(
            &s8, osl_getThreadTextEncoding(),
            (RTL_UNICODETOTEXT_FLAGS_UNDEFINED_ERROR
             | RTL_UNICODETOTEXT_FLAGS_INVALID_ERROR))
        || (static_cast< sal_uInt32 >(s8.getLength())
            > std::numeric_limits< std::string::size_type >::max()))
    {
        std::cerr
            << "Failure converting argument from UTF-16 back to system encoding"
            << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return std::string(
        s8.getStr(), static_cast< std::string::size_type >(s8.getLength()));
}

}

SAL_IMPLEMENT_MAIN() {
    CppUnit::TestResult result;
    sal_uInt32 index = 0;
    for (; index < rtl_getAppCommandArgCount(); index += 3) {
        if (!getArgument(index).equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM("--protector")))
        {
            break;
        }
        if (rtl_getAppCommandArgCount() - index < 3) {
            usageFailure();
        }
        rtl::OUString lib(getArgument(index + 1));
        rtl::OUString sym(getArgument(index + 2));
        oslGenericFunction fn = (new osl::Module(lib))->getFunctionSymbol(sym);
        CppUnit::Protector * p = fn == 0
            ? 0
            : (*reinterpret_cast< cppunittester::ProtectorFactory * >(fn))();
        if (p == 0) {
            std::cerr
                << "Failure instantiating protector \"" << convertLazy(lib)
                << "\", \"" << convertLazy(sym) << '"' << std::endl;
            std::exit(EXIT_FAILURE);
        }
        result.pushProtector(p);
    }
    if (rtl_getAppCommandArgCount() - index != 1) {
        usageFailure();
    }
    CppUnit::PlugInManager manager;
    manager.load(convertStrict(getArgument(index)));
    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    CppUnit::TestResultCollector collector;
    result.addListener(&collector);
    runner.run(result);
    CppUnit::CompilerOutputter(&collector, CppUnit::stdCErr()).write();
    fprintf( stderr, "-----\n" ); fflush( stderr );
    return collector.wasSuccessful() ? EXIT_SUCCESS : EXIT_FAILURE;
}
