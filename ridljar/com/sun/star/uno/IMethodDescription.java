/*************************************************************************
 *
 *  $RCSfile: IMethodDescription.java,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: kr $ $Date: 2001-05-08 09:34:18 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

package com.sun.star.uno;


import java.lang.reflect.Method;


/**
 * The <code>IMethodDescription</code> allows to examine a method
 * in detail. It gives a view to java methods from a UNO point.
 * <p>
 * @version 	$Revision: 1.1 $ $ $Date: 2001-05-08 09:34:18 $
 * @author 	    Kay Ramme
 * @since       UDK3.0
 */
public interface IMethodDescription extends IMemberDescription {
    /**
     * Indicates if this method is <code>oneWay</code>,
     * respectivly if this method may become executed asynchronously.
     * <p>
     * @return  true means may execute asynchronously .
     */
    boolean isOneway();

    /**
     * Gives the relative index of this method in the declaring 
     * interface.
     * <p>
     * @return  the realtive index of this method
     */
    int getIndex();

    /**
     * Indicates if this method is const.
     * <p>
     * @return true means it is const.
     */
    boolean isConst();

    /**
     * Gives any array of <code>ITypeDescription> of
     * the [in] parameters.
     * <p>
     * @return  the in parameters
     */
    ITypeDescription[] getInSignature();

    /**
     * Gives any array of <code>ITypeDescription> of
     * the [out] parameters.
     * <p>
     * @return  the out parameters
     */
    ITypeDescription[] getOutSignature();

    /**
     * Gives the <code>ITypeDescription</code> of
     * the return type.
     * <p>
     * @return  the return type <code>ITypeDescription</code>
     */
    ITypeDescription getReturnSignature();

    /**
     * Gives native java method of this method.
     * <p>
     * @return  the java methodd
     */
    Method getMethod();
}
