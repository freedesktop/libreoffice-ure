/*
 * t_leak.cxx
 */

#include "sal/main.h"
#include "osl/process.h"
#include "osl/thread.h"

 int main (int /*argc*/, char ** /*argv*/)
//SAL_IMPLEMENT_MAIN()
{
    rtl_Locale * pLocale = 0;
    osl_getProcessLocale (&pLocale);
#if 0
    rtl_TextEncoding te = osl_getThreadTextEncoding();
    (void) te;
#endif
    return 0;
}
