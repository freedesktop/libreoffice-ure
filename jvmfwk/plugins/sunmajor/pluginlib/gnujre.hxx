/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: gnujre.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: vg $ $Date: 2006-09-27 10:54:16 $
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

#if !defined INCLUDED_JFW_PLUGIN_GNUJRE_HXX
#define  INCLUDED_JFW_PLUGIN_GNUJRE_HXX

#include "vendorbase.hxx"
#include "vendorlist.hxx"

namespace jfw_plugin
{

class GnuInfo: public VendorBase
{
private:
    rtl::OUString m_sJavaHome;
public:
    static char const* const* getJavaExePaths(int * size);

    static rtl::Reference<VendorBase> createInstance();

    virtual char const* const* getRuntimePaths(int * size);

    virtual bool initialize(
        std::vector<std::pair<rtl::OUString, rtl::OUString> > props);
    virtual int compareVersions(const rtl::OUString& sSecond) const;

};

}
#endif
