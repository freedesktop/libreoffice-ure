/*************************************************************************
 *
 *  $RCSfile: TestEquals.java,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2003-04-23 16:34:30 $
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
 *  Copyright: 2002 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

package test.java_uno.equals;

import com.sun.star.bridge.XBridge;
import com.sun.star.bridge.XBridgeFactory;
import com.sun.star.bridge.XInstanceProvider;
import com.sun.star.comp.helper.Bootstrap;
import com.sun.star.connection.XAcceptor;
import com.sun.star.connection.XConnection;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XSingleComponentFactory;
import com.sun.star.lib.uno.typeinfo.MethodTypeInfo;
import com.sun.star.lib.uno.typeinfo.TypeInfo;
import com.sun.star.loader.XImplementationLoader;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.uno.XInterface;
import java.io.File;
import java.net.MalformedURLException;
import java.util.HashMap;
import java.util.Hashtable;
import test.java_uno.TestBed;

// In this test scenario, the Java server (see implementation of method
// notifyAccepting) has a remote bridge to the Java client and a local JNI
// bridge to a C++ com.sun.star.test.bridges.testequals.impl service.  The C++
// service and the Java client are also connected via a remote bridge.
//
// The Java server gets two objects (INSTANCE1, INSTANCE2), once directly from
// the Java client via the remote bridge (proxies test1A, test2A), and once
// through the C++ service via the JNI bridge (proxies test1B, test2B).
// Exhaustive tests on the proxies' equals and hashCode methods are done.

public final class TestEquals {
    // args[0] must be a file system path to a types.rdb,
    // args[1] must be a file system path to a services.rdb
    public static void main(String[] args) throws Exception {
        TestBed t = new TestBed();
        t.execute(new Provider(t, toFileUrl(args[0]), toFileUrl(args[1])), true,
                  Client.class, 0);
    }

    private static String toFileUrl(String path) throws MalformedURLException {
        String url = new File(path).toURL().toString();
        String prefix = "file:/";
        if (url.startsWith(prefix)
            && (url.length() == prefix.length()
                || url.charAt(prefix.length()) != '/')) {
            url = url.substring(0, prefix.length()) + "//"
                + url.substring(prefix.length());
        }
        return url;
    }

    public static final class Client extends TestBed.Client {
        public static void main(String[] args) {
            new Client().execute();
        }

        protected boolean run(XBridge bridge) throws Throwable {
            XTestFrame f = (XTestFrame) UnoRuntime.queryInterface(
                XTestFrame.class, bridge.getInstance("TestFrame"));
            XComponentContext context
                = Bootstrap.createInitialComponentContext(null);
            XMultiComponentFactory factory = context.getServiceManager();
            XBridgeFactory bridgeFactory
                = (XBridgeFactory) UnoRuntime.queryInterface(
                    XBridgeFactory.class,
                    factory.createInstanceWithContext(
                        "com.sun.star.bridge.BridgeFactory", context));
            XAcceptor acceptor = (XAcceptor) UnoRuntime.queryInterface(
                XAcceptor.class,
                factory.createInstanceWithContext(
                    "com.sun.star.connection.Acceptor", context));
            System.out.println("Client, 2nd connection: Accepting...");
            XInstanceProvider prov = new Provider();
            f.notifyAccepting(new Done(), prov.getInstance(INSTANCE1),
                              prov.getInstance(INSTANCE2));
            XConnection connection = acceptor.accept(CONNECTION_DESCRIPTION);
            System.out.println("Client, 2nd connection: ...connected...");
            XBridge bridge2 = bridgeFactory.createBridge("",
                                                         PROTOCOL_DESCRIPTION,
                                                         connection, prov);
            System.out.println("Client, 2nd connection: ...bridged.");
            synchronized (lock) {
                while (!done) {
                    lock.wait();
                }
            }
            return true;
        }

        private static final class Provider implements XInstanceProvider {
            public Object getInstance(String instanceName) {
                synchronized (map) {
                    Object o = map.get(instanceName);
                    if (o == null) {
                        o = new XDerived() {};
                        map.put(instanceName, o);
                    }
                    return o;
                }
            }

            private final HashMap map = new HashMap();
        }

        private final class Done implements XDone {
            public void notifyDone() {
                synchronized (lock) {
                    done = true;
                    lock.notifyAll();
                }
            }
        }

        private final Object lock = new Object();
        private boolean done = false;
    }

    private static final class Provider implements XInstanceProvider {
        public Provider(TestBed testBed, String unoTypes, String unoServices) {
            this.testBed = testBed;
            this.unoTypes = unoTypes;
            this.unoServices = unoServices;
        }

        public Object getInstance(String instanceName) {
            return new XTestFrame() {
                    public void notifyAccepting(XDone done, Object object1,
                                                Object object2) throws Exception
                    {
                        Object test1Aa = object1;
                        XBase test1Ab = (XBase) UnoRuntime.queryInterface(
                            XBase.class, test1Aa);
                        XDerived test1Ac = (XDerived) UnoRuntime.queryInterface(
                            XDerived.class, test1Aa);
                        Object test2Aa = object2;
                        XBase test2Ab = (XBase) UnoRuntime.queryInterface(
                            XBase.class, test2Aa);
                        XDerived test2Ac = (XDerived) UnoRuntime.queryInterface(
                            XDerived.class, test2Aa);

                        Hashtable params = new Hashtable();
                        params.put("UNO_TYPES", unoTypes);
                        params.put("UNO_SERVICES", unoServices);
                        XComponentContext context = Bootstrap.
                            defaultBootstrap_InitialComponentContext(null,
                                                                     params);
                        XMultiComponentFactory factory
                            = context.getServiceManager();
                        XImplementationLoader loader = (XImplementationLoader)
                            UnoRuntime.queryInterface(
                                XImplementationLoader.class,
                                factory.createInstanceWithContext(
                                    "com.sun.star.loader.SharedLibrary",
                                    context));
                        XSingleComponentFactory factory2
                            = (XSingleComponentFactory)
                            UnoRuntime.queryInterface(
                                XSingleComponentFactory.class,
                                loader.activate(
                                    "com.sun.star.test.bridges.testequals.impl",
                                    "", "testequals.uno", null));
                        XTestInterface test = (XTestInterface)
                            UnoRuntime.queryInterface(
                                XTestInterface.class,
                                factory2.createInstanceWithContext(context));
                        // allow client to start accepting:
                        Thread.sleep(3000);
                        test.connect(CONNECTION_DESCRIPTION,
                                     PROTOCOL_DESCRIPTION);

                        Object test1Ba = test.get(INSTANCE1);
                        XBase test1Bb = (XBase) UnoRuntime.queryInterface(
                            XBase.class, test1Ba);
                        XDerived test1Bc = (XDerived) UnoRuntime.queryInterface(
                            XDerived.class, test1Ba);
                        Object test2Ba = test.get(INSTANCE2);
                        XBase test2Bb = (XBase) UnoRuntime.queryInterface(
                            XBase.class, test2Ba);
                        XDerived test2Bc = (XDerived) UnoRuntime.queryInterface(
                            XDerived.class, test2Ba);

                        boolean success = true;

                        success &= test("UnoRumtime.areSame(null, null)",
                                        UnoRuntime.areSame(null, null));
                        success &= test("!UnoRumtime.areSame(null, test1Aa)",
                                        !UnoRuntime.areSame(null, test1Aa));
                        success &= test("!UnoRumtime.areSame(null, test1Ab)",
                                        !UnoRuntime.areSame(null, test1Ab));
                        success &= test("!UnoRumtime.areSame(null, test1Ac)",
                                        !UnoRuntime.areSame(null, test1Ac));
                        success &= test("!UnoRumtime.areSame(null, test1Ba)",
                                        !UnoRuntime.areSame(null, test1Ba));
                        success &= test("!UnoRumtime.areSame(null, test1Bb)",
                                        !UnoRuntime.areSame(null, test1Bb));
                        success &= test("!UnoRumtime.areSame(null, test1Bc)",
                                        !UnoRuntime.areSame(null, test1Bc));
                        success &= test("!UnoRumtime.areSame(null, test2Aa)",
                                        !UnoRuntime.areSame(null, test2Aa));
                        success &= test("!UnoRumtime.areSame(null, test2Ab)",
                                        !UnoRuntime.areSame(null, test2Ab));
                        success &= test("!UnoRumtime.areSame(null, test2Ac)",
                                        !UnoRuntime.areSame(null, test2Ac));
                        success &= test("!UnoRumtime.areSame(null, test2Ba)",
                                        !UnoRuntime.areSame(null, test2Ba));
                        success &= test("!UnoRumtime.areSame(null, test2Bb)",
                                        !UnoRuntime.areSame(null, test2Bb));
                        success &= test("!UnoRumtime.areSame(null, test2Bc)",
                                        !UnoRuntime.areSame(null, test2Bc));

                        success &= test("!test1Aa.equals(null)",
                                        !test1Aa.equals(null));
                        success &= test("!UnoRuntime.areSame(test1Aa, null)",
                                        !UnoRuntime.areSame(test1Aa, null));
                        success &= test("test1Aa.equals(test1Aa)",
                                        test1Aa.equals(test1Aa));
                        success &= test("UnoRuntime.areSame(test1Aa, test1Aa)",
                                        UnoRuntime.areSame(test1Aa, test1Aa));
                        success &= test("test1Aa.equals(test1Ab)",
                                        test1Aa.equals(test1Ab));
                        success &= test("UnoRuntime.areSame(test1Aa, test1Ab)",
                                        UnoRuntime.areSame(test1Aa, test1Ab));
                        success &= test("test1Aa.equals(test1Ac)",
                                        test1Aa.equals(test1Ac));
                        success &= test("UnoRuntime.areSame(test1Aa, test1Ac)",
                                        UnoRuntime.areSame(test1Aa, test1Ac));
                        success &= test("test1Aa.equals(test1Ba)",
                                        test1Aa.equals(test1Ba));
                        success &= test("UnoRuntime.areSame(test1Aa, test1Ba)",
                                        UnoRuntime.areSame(test1Aa, test1Ba));
                        success &= test("test1Aa.equals(test1Bb)",
                                        test1Aa.equals(test1Bb));
                        success &= test("UnoRuntime.areSame(test1Aa, test1Bb)",
                                        UnoRuntime.areSame(test1Aa, test1Bb));
                        success &= test("test1Aa.equals(test1Bc)",
                                        test1Aa.equals(test1Bc));
                        success &= test("UnoRuntime.areSame(test1Aa, test1Bc)",
                                        UnoRuntime.areSame(test1Aa, test1Bc));
                        success &= test("!test1Aa.equals(test2Aa)",
                                        !test1Aa.equals(test2Aa));
                        success &= test("!UnoRuntime.areSame(test1Aa, test2Aa)",
                                        !UnoRuntime.areSame(test1Aa, test2Aa));
                        success &= test("!test1Aa.equals(test2Ab)",
                                        !test1Aa.equals(test2Ab));
                        success &= test("!UnoRuntime.areSame(test1Aa, test2Ab)",
                                        !UnoRuntime.areSame(test1Aa, test2Ab));
                        success &= test("!test1Aa.equals(test2Ac)",
                                        !test1Aa.equals(test2Ac));
                        success &= test("!UnoRuntime.areSame(test1Aa, test2Ac)",
                                        !UnoRuntime.areSame(test1Aa, test2Ac));
                        success &= test("!test1Aa.equals(test2Ba)",
                                        !test1Aa.equals(test2Ba));
                        success &= test("!UnoRuntime.areSame(test1Aa, test2Ba)",
                                        !UnoRuntime.areSame(test1Aa, test2Ba));
                        success &= test("!test1Aa.equals(test2Bb)",
                                        !test1Aa.equals(test2Bb));
                        success &= test("!UnoRuntime.areSame(test1Aa, test2Bb)",
                                        !UnoRuntime.areSame(test1Aa, test2Bb));
                        success &= test("!test1Aa.equals(test2Bc)",
                                        !test1Aa.equals(test2Bc));
                        success &= test("!UnoRuntime.areSame(test1Aa, test2Bc)",
                                        !UnoRuntime.areSame(test1Aa, test2Bc));

                        success &= test("!test1Ab.equals(null)",
                                        !test1Ab.equals(null));
                        success &= test("!UnoRuntime.areSame(test1Ab, null)",
                                        !UnoRuntime.areSame(test1Ab, null));
                        success &= test("test1Ab.equals(test1Aa)",
                                        test1Ab.equals(test1Aa));
                        success &= test("UnoRuntime.areSame(test1Ab, test1Aa)",
                                        UnoRuntime.areSame(test1Ab, test1Aa));
                        success &= test("test1Ab.equals(test1Ab)",
                                        test1Ab.equals(test1Ab));
                        success &= test("UnoRuntime.areSame(test1Ab, test1Ab)",
                                        UnoRuntime.areSame(test1Ab, test1Ab));
                        success &= test("test1Ab.equals(test1Ac)",
                                        test1Ab.equals(test1Ac));
                        success &= test("UnoRuntime.areSame(test1Ab, test1Ac)",
                                        UnoRuntime.areSame(test1Ab, test1Ac));
                        success &= test("test1Ab.equals(test1Ba)",
                                        test1Ab.equals(test1Ba));
                        success &= test("UnoRuntime.areSame(test1Ab, test1Ba)",
                                        UnoRuntime.areSame(test1Ab, test1Ba));
                        success &= test("test1Ab.equals(test1Bb)",
                                        test1Ab.equals(test1Bb));
                        success &= test("UnoRuntime.areSame(test1Ab, test1Bb)",
                                        UnoRuntime.areSame(test1Ab, test1Bb));
                        success &= test("test1Ab.equals(test1Bc)",
                                        test1Ab.equals(test1Bc));
                        success &= test("UnoRuntime.areSame(test1Ab, test1Bc)",
                                        UnoRuntime.areSame(test1Ab, test1Bc));
                        success &= test("!test1Ab.equals(test2Aa)",
                                        !test1Ab.equals(test2Aa));
                        success &= test("!UnoRuntime.areSame(test1Ab, test2Aa)",
                                        !UnoRuntime.areSame(test1Ab, test2Aa));
                        success &= test("!test1Ab.equals(test2Ab)",
                                        !test1Ab.equals(test2Ab));
                        success &= test("!UnoRuntime.areSame(test1Ab, test2Ab)",
                                        !UnoRuntime.areSame(test1Ab, test2Ab));
                        success &= test("!test1Ab.equals(test2Ac)",
                                        !test1Ab.equals(test2Ac));
                        success &= test("!UnoRuntime.areSame(test1Ab, test2Ac)",
                                        !UnoRuntime.areSame(test1Ab, test2Ac));
                        success &= test("!test1Ab.equals(test2Ba)",
                                        !test1Ab.equals(test2Ba));
                        success &= test("!UnoRuntime.areSame(test1Ab, test2Ba)",
                                        !UnoRuntime.areSame(test1Ab, test2Ba));
                        success &= test("!test1Ab.equals(test2Bb)",
                                        !test1Ab.equals(test2Bb));
                        success &= test("!UnoRuntime.areSame(test1Ab, test2Bb)",
                                        !UnoRuntime.areSame(test1Ab, test2Bb));
                        success &= test("!test1Ab.equals(test2Bc)",
                                        !test1Ab.equals(test2Bc));
                        success &= test("!UnoRuntime.areSame(test1Ab, test2Bc)",
                                        !UnoRuntime.areSame(test1Ab, test2Bc));

                        success &= test("!test1Ac.equals(null)",
                                        !test1Ac.equals(null));
                        success &= test("!UnoRuntime.areSame(test1Ac, null)",
                                        !UnoRuntime.areSame(test1Ac, null));
                        success &= test("test1Ac.equals(test1Aa)",
                                        test1Ac.equals(test1Aa));
                        success &= test("UnoRuntime.areSame(test1Ac, test1Aa)",
                                        UnoRuntime.areSame(test1Ac, test1Aa));
                        success &= test("test1Ac.equals(test1Ab)",
                                        test1Ac.equals(test1Ab));
                        success &= test("UnoRuntime.areSame(test1Ac, test1Ab)",
                                        UnoRuntime.areSame(test1Ac, test1Ab));
                        success &= test("test1Ac.equals(test1Ac)",
                                        test1Ac.equals(test1Ac));
                        success &= test("UnoRuntime.areSame(test1Ac, test1Ac)",
                                        UnoRuntime.areSame(test1Ac, test1Ac));
                        success &= test("test1Ac.equals(test1Ba)",
                                        test1Ac.equals(test1Ba));
                        success &= test("UnoRuntime.areSame(test1Ac, test1Ba)",
                                        UnoRuntime.areSame(test1Ac, test1Ba));
                        success &= test("test1Ac.equals(test1Bb)",
                                        test1Ac.equals(test1Bb));
                        success &= test("UnoRuntime.areSame(test1Ac, test1Bb)",
                                        UnoRuntime.areSame(test1Ac, test1Bb));
                        success &= test("test1Ac.equals(test1Bc)",
                                        test1Ac.equals(test1Bc));
                        success &= test("UnoRuntime.areSame(test1Ac, test1Bc)",
                                        UnoRuntime.areSame(test1Ac, test1Bc));
                        success &= test("!test1Ac.equals(test2Aa)",
                                        !test1Ac.equals(test2Aa));
                        success &= test("!UnoRuntime.areSame(test1Ac, test2Aa)",
                                        !UnoRuntime.areSame(test1Ac, test2Aa));
                        success &= test("!test1Ac.equals(test2Ab)",
                                        !test1Ac.equals(test2Ab));
                        success &= test("!UnoRuntime.areSame(test1Ac, test2Ab)",
                                        !UnoRuntime.areSame(test1Ac, test2Ab));
                        success &= test("!test1Ac.equals(test2Ac)",
                                        !test1Ac.equals(test2Ac));
                        success &= test("!UnoRuntime.areSame(test1Ac, test2Ac)",
                                        !UnoRuntime.areSame(test1Ac, test2Ac));
                        success &= test("!test1Ac.equals(test2Ba)",
                                        !test1Ac.equals(test2Ba));
                        success &= test("!UnoRuntime.areSame(test1Ac, test2Ba)",
                                        !UnoRuntime.areSame(test1Ac, test2Ba));
                        success &= test("!test1Ac.equals(test2Bb)",
                                        !test1Ac.equals(test2Bb));
                        success &= test("!UnoRuntime.areSame(test1Ac, test2Bb)",
                                        !UnoRuntime.areSame(test1Ac, test2Bb));
                        success &= test("!test1Ac.equals(test2Bc)",
                                        !test1Ac.equals(test2Bc));
                        success &= test("!UnoRuntime.areSame(test1Ac, test2Bc)",
                                        !UnoRuntime.areSame(test1Ac, test2Bc));

                        success &= test("!test1Ba.equals(null)",
                                        !test1Ba.equals(null));
                        success &= test("!UnoRuntime.areSame(test1Ba, null)",
                                        !UnoRuntime.areSame(test1Ba, null));
                        success &= test("test1Ba.equals(test1Aa)",
                                        test1Ba.equals(test1Aa));
                        success &= test("UnoRuntime.areSame(test1Ba, test1Aa)",
                                        UnoRuntime.areSame(test1Ba, test1Aa));
                        success &= test("test1Ba.equals(test1Ab)",
                                        test1Ba.equals(test1Ab));
                        success &= test("UnoRuntime.areSame(test1Ba, test1Ab)",
                                        UnoRuntime.areSame(test1Ba, test1Ab));
                        success &= test("test1Ba.equals(test1Ac)",
                                        test1Ba.equals(test1Ac));
                        success &= test("UnoRuntime.areSame(test1Ba, test1Ac)",
                                        UnoRuntime.areSame(test1Ba, test1Ac));
                        success &= test("test1Ba.equals(test1Ba)",
                                        test1Ba.equals(test1Ba));
                        success &= test("UnoRuntime.areSame(test1Ba, test1Ba)",
                                        UnoRuntime.areSame(test1Ba, test1Ba));
                        success &= test("test1Ba.equals(test1Bb)",
                                        test1Ba.equals(test1Bb));
                        success &= test("UnoRuntime.areSame(test1Ba, test1Bb)",
                                        UnoRuntime.areSame(test1Ba, test1Bb));
                        success &= test("test1Ba.equals(test1Bc)",
                                        test1Ba.equals(test1Bc));
                        success &= test("UnoRuntime.areSame(test1Ba, test1Bc)",
                                        UnoRuntime.areSame(test1Ba, test1Bc));
                        success &= test("!test1Ba.equals(test2Aa)",
                                        !test1Ba.equals(test2Aa));
                        success &= test("!UnoRuntime.areSame(test1Ba, test2Aa)",
                                        !UnoRuntime.areSame(test1Ba, test2Aa));
                        success &= test("!test1Ba.equals(test2Ab)",
                                        !test1Ba.equals(test2Ab));
                        success &= test("!UnoRuntime.areSame(test1Ba, test2Ab)",
                                        !UnoRuntime.areSame(test1Ba, test2Ab));
                        success &= test("!test1Ba.equals(test2Ac)",
                                        !test1Ba.equals(test2Ac));
                        success &= test("!UnoRuntime.areSame(test1Ba, test2Ac)",
                                        !UnoRuntime.areSame(test1Ba, test2Ac));
                        success &= test("!test1Ba.equals(test2Ba)",
                                        !test1Ba.equals(test2Ba));
                        success &= test("!UnoRuntime.areSame(test1Ba, test2Ba)",
                                        !UnoRuntime.areSame(test1Ba, test2Ba));
                        success &= test("!test1Ba.equals(test2Bb)",
                                        !test1Ba.equals(test2Bb));
                        success &= test("!UnoRuntime.areSame(test1Ba, test2Bb)",
                                        !UnoRuntime.areSame(test1Ba, test2Bb));
                        success &= test("!test1Ba.equals(test2Bc)",
                                        !test1Ba.equals(test2Bc));
                        success &= test("!UnoRuntime.areSame(test1Ba, test2Bc)",
                                        !UnoRuntime.areSame(test1Ba, test2Bc));

                        success &= test("!test1Bb.equals(null)",
                                        !test1Bb.equals(null));
                        success &= test("!UnoRuntime.areSame(test1Bb, null)",
                                        !UnoRuntime.areSame(test1Bb, null));
                        success &= test("test1Bb.equals(test1Aa)",
                                        test1Bb.equals(test1Aa));
                        success &= test("UnoRuntime.areSame(test1Bb, test1Aa)",
                                        UnoRuntime.areSame(test1Bb, test1Aa));
                        success &= test("test1Bb.equals(test1Ab)",
                                        test1Bb.equals(test1Ab));
                        success &= test("UnoRuntime.areSame(test1Bb, test1Ab)",
                                        UnoRuntime.areSame(test1Bb, test1Ab));
                        success &= test("test1Bb.equals(test1Ac)",
                                        test1Bb.equals(test1Ac));
                        success &= test("UnoRuntime.areSame(test1Bb, test1Ac)",
                                        UnoRuntime.areSame(test1Bb, test1Ac));
                        success &= test("test1Bb.equals(test1Ba)",
                                        test1Bb.equals(test1Ba));
                        success &= test("UnoRuntime.areSame(test1Bb, test1Ba)",
                                        UnoRuntime.areSame(test1Bb, test1Ba));
                        success &= test("test1Bb.equals(test1Bb)",
                                        test1Bb.equals(test1Bb));
                        success &= test("UnoRuntime.areSame(test1Bb, test1Bb)",
                                        UnoRuntime.areSame(test1Bb, test1Bb));
                        success &= test("test1Bb.equals(test1Bc)",
                                        test1Bb.equals(test1Bc));
                        success &= test("UnoRuntime.areSame(test1Bb, test1Bc)",
                                        UnoRuntime.areSame(test1Bb, test1Bc));
                        success &= test("!test1Bb.equals(test2Aa)",
                                        !test1Bb.equals(test2Aa));
                        success &= test("!UnoRuntime.areSame(test1Bb, test2Aa)",
                                        !UnoRuntime.areSame(test1Bb, test2Aa));
                        success &= test("!test1Bb.equals(test2Ab)",
                                        !test1Bb.equals(test2Ab));
                        success &= test("!UnoRuntime.areSame(test1Bb, test2Ab)",
                                        !UnoRuntime.areSame(test1Bb, test2Ab));
                        success &= test("!test1Bb.equals(test2Ac)",
                                        !test1Bb.equals(test2Ac));
                        success &= test("!UnoRuntime.areSame(test1Bb, test2Ac)",
                                        !UnoRuntime.areSame(test1Bb, test2Ac));
                        success &= test("!test1Bb.equals(test2Ba)",
                                        !test1Bb.equals(test2Ba));
                        success &= test("!UnoRuntime.areSame(test1Bb, test2Ba)",
                                        !UnoRuntime.areSame(test1Bb, test2Ba));
                        success &= test("!test1Bb.equals(test2Bb)",
                                        !test1Bb.equals(test2Bb));
                        success &= test("!UnoRuntime.areSame(test1Bb, test2Bb)",
                                        !UnoRuntime.areSame(test1Bb, test2Bb));
                        success &= test("!test1Bb.equals(test2Bc)",
                                        !test1Bb.equals(test2Bc));
                        success &= test("!UnoRuntime.areSame(test1Bb, test2Bc)",
                                        !UnoRuntime.areSame(test1Bb, test2Bc));

                        success &= test("!test1Bc.equals(null)",
                                        !test1Bc.equals(null));
                        success &= test("!UnoRuntime.areSame(test1Bc, null)",
                                        !UnoRuntime.areSame(test1Bc, null));
                        success &= test("test1Bc.equals(test1Aa)",
                                        test1Bc.equals(test1Aa));
                        success &= test("UnoRuntime.areSame(test1Bc, test1Aa)",
                                        UnoRuntime.areSame(test1Bc, test1Aa));
                        success &= test("test1Bc.equals(test1Ab)",
                                        test1Bc.equals(test1Ab));
                        success &= test("UnoRuntime.areSame(test1Bc, test1Ab)",
                                        UnoRuntime.areSame(test1Bc, test1Ab));
                        success &= test("test1Bc.equals(test1Ac)",
                                        test1Bc.equals(test1Ac));
                        success &= test("UnoRuntime.areSame(test1Bc, test1Ac)",
                                        UnoRuntime.areSame(test1Bc, test1Ac));
                        success &= test("test1Bc.equals(test1Ba)",
                                        test1Bc.equals(test1Ba));
                        success &= test("UnoRuntime.areSame(test1Bc, test1Ba)",
                                        UnoRuntime.areSame(test1Bc, test1Ba));
                        success &= test("test1Bc.equals(test1Bb)",
                                        test1Bc.equals(test1Bb));
                        success &= test("UnoRuntime.areSame(test1Bc, test1Bb)",
                                        UnoRuntime.areSame(test1Bc, test1Bb));
                        success &= test("test1Bc.equals(test1Bc)",
                                        test1Bc.equals(test1Bc));
                        success &= test("UnoRuntime.areSame(test1Bc, test1Bc)",
                                        UnoRuntime.areSame(test1Bc, test1Bc));
                        success &= test("!test1Bc.equals(test2Aa)",
                                        !test1Bc.equals(test2Aa));
                        success &= test("!UnoRuntime.areSame(test1Bc, test2Aa)",
                                        !UnoRuntime.areSame(test1Bc, test2Aa));
                        success &= test("!test1Bc.equals(test2Ab)",
                                        !test1Bc.equals(test2Ab));
                        success &= test("!UnoRuntime.areSame(test1Bc, test2Ab)",
                                        !UnoRuntime.areSame(test1Bc, test2Ab));
                        success &= test("!test1Bc.equals(test2Ac)",
                                        !test1Bc.equals(test2Ac));
                        success &= test("!UnoRuntime.areSame(test1Bc, test2Ac)",
                                        !UnoRuntime.areSame(test1Bc, test2Ac));
                        success &= test("!test1Bc.equals(test2Ba)",
                                        !test1Bc.equals(test2Ba));
                        success &= test("!UnoRuntime.areSame(test1Bc, test2Ba)",
                                        !UnoRuntime.areSame(test1Bc, test2Ba));
                        success &= test("!test1Bc.equals(test2Bb)",
                                        !test1Bc.equals(test2Bb));
                        success &= test("!UnoRuntime.areSame(test1Bc, test2Bb)",
                                        !UnoRuntime.areSame(test1Bc, test2Bb));
                        success &= test("!test1Bc.equals(test2Bc)",
                                        !test1Bc.equals(test2Bc));
                        success &= test("!UnoRuntime.areSame(test1Bc, test2Bc)",
                                        !UnoRuntime.areSame(test1Bc, test2Bc));

                        success &= test("!test2Aa.equals(null)",
                                        !test2Aa.equals(null));
                        success &= test("!UnoRuntime.areSame(test2Aa, null)",
                                        !UnoRuntime.areSame(test2Aa, null));
                        success &= test("!test2Aa.equals(test1Aa)",
                                        !test2Aa.equals(test1Aa));
                        success &= test("!UnoRuntime.areSame(test2Aa, test1Aa)",
                                        !UnoRuntime.areSame(test2Aa, test1Aa));
                        success &= test("!test2Aa.equals(test1Ab)",
                                        !test2Aa.equals(test1Ab));
                        success &= test("!UnoRuntime.areSame(test2Aa, test1Ab)",
                                        !UnoRuntime.areSame(test2Aa, test1Ab));
                        success &= test("!test2Aa.equals(test1Ac)",
                                        !test2Aa.equals(test1Ac));
                        success &= test("!UnoRuntime.areSame(test2Aa, test1Ac)",
                                        !UnoRuntime.areSame(test2Aa, test1Ac));
                        success &= test("!test2Aa.equals(test1Ba)",
                                        !test2Aa.equals(test1Ba));
                        success &= test("!UnoRuntime.areSame(test2Aa, test1Ba)",
                                        !UnoRuntime.areSame(test2Aa, test1Ba));
                        success &= test("!test2Aa.equals(test1Bb)",
                                        !test2Aa.equals(test1Bb));
                        success &= test("!UnoRuntime.areSame(test2Aa, test1Bb)",
                                        !UnoRuntime.areSame(test2Aa, test1Bb));
                        success &= test("!test2Aa.equals(test1Bc)",
                                        !test2Aa.equals(test1Bc));
                        success &= test("!UnoRuntime.areSame(test2Aa, test1Bc)",
                                        !UnoRuntime.areSame(test2Aa, test1Bc));
                        success &= test("test2Aa.equals(test2Aa)",
                                        test2Aa.equals(test2Aa));
                        success &= test("UnoRuntime.areSame(test2Aa, test2Aa)",
                                        UnoRuntime.areSame(test2Aa, test2Aa));
                        success &= test("test2Aa.equals(test2Ab)",
                                        test2Aa.equals(test2Ab));
                        success &= test("UnoRuntime.areSame(test2Aa, test2Ab)",
                                        UnoRuntime.areSame(test2Aa, test2Ab));
                        success &= test("test2Aa.equals(test2Ac)",
                                        test2Aa.equals(test2Ac));
                        success &= test("UnoRuntime.areSame(test2Aa, test2Ac)",
                                        UnoRuntime.areSame(test2Aa, test2Ac));
                        success &= test("test2Aa.equals(test2Ba)",
                                        test2Aa.equals(test2Ba));
                        success &= test("UnoRuntime.areSame(test2Aa, test2Ba)",
                                        UnoRuntime.areSame(test2Aa, test2Ba));
                        success &= test("test2Aa.equals(test2Bb)",
                                        test2Aa.equals(test2Bb));
                        success &= test("UnoRuntime.areSame(test2Aa, test2Bb)",
                                        UnoRuntime.areSame(test2Aa, test2Bb));
                        success &= test("test2Aa.equals(test2Bc)",
                                        test2Aa.equals(test2Bc));
                        success &= test("UnoRuntime.areSame(test2Aa, test2Bc)",
                                        UnoRuntime.areSame(test2Aa, test2Bc));

                        success &= test("!test2Ab.equals(null)",
                                        !test2Ab.equals(null));
                        success &= test("!UnoRuntime.areSame(test2Ab, null)",
                                        !UnoRuntime.areSame(test2Ab, null));
                        success &= test("!test2Ab.equals(test1Aa)",
                                        !test2Ab.equals(test1Aa));
                        success &= test("!UnoRuntime.areSame(test2Ab, test1Aa)",
                                        !UnoRuntime.areSame(test2Ab, test1Aa));
                        success &= test("!test2Ab.equals(test1Ab)",
                                        !test2Ab.equals(test1Ab));
                        success &= test("!UnoRuntime.areSame(test2Ab, test1Ab)",
                                        !UnoRuntime.areSame(test2Ab, test1Ab));
                        success &= test("!test2Ab.equals(test1Ac)",
                                        !test2Ab.equals(test1Ac));
                        success &= test("!UnoRuntime.areSame(test2Ab, test1Ac)",
                                        !UnoRuntime.areSame(test2Ab, test1Ac));
                        success &= test("!test2Ab.equals(test1Ba)",
                                        !test2Ab.equals(test1Ba));
                        success &= test("!UnoRuntime.areSame(test2Ab, test1Ba)",
                                        !UnoRuntime.areSame(test2Ab, test1Ba));
                        success &= test("!test2Ab.equals(test1Bb)",
                                        !test2Ab.equals(test1Bb));
                        success &= test("!UnoRuntime.areSame(test2Ab, test1Bb)",
                                        !UnoRuntime.areSame(test2Ab, test1Bb));
                        success &= test("!test2Ab.equals(test1Bc)",
                                        !test2Ab.equals(test1Bc));
                        success &= test("!UnoRuntime.areSame(test2Ab, test1Bc)",
                                        !UnoRuntime.areSame(test2Ab, test1Bc));
                        success &= test("test2Ab.equals(test2Aa)",
                                        test2Ab.equals(test2Aa));
                        success &= test("UnoRuntime.areSame(test2Ab, test2Aa)",
                                        UnoRuntime.areSame(test2Ab, test2Aa));
                        success &= test("test2Ab.equals(test2Ab)",
                                        test2Ab.equals(test2Ab));
                        success &= test("UnoRuntime.areSame(test2Ab, test2Ab)",
                                        UnoRuntime.areSame(test2Ab, test2Ab));
                        success &= test("test2Ab.equals(test2Ac)",
                                        test2Ab.equals(test2Ac));
                        success &= test("UnoRuntime.areSame(test2Ab, test2Ac)",
                                        UnoRuntime.areSame(test2Ab, test2Ac));
                        success &= test("test2Ab.equals(test2Ba)",
                                        test2Ab.equals(test2Ba));
                        success &= test("UnoRuntime.areSame(test2Ab, test2Ba)",
                                        UnoRuntime.areSame(test2Ab, test2Ba));
                        success &= test("test2Ab.equals(test2Bb)",
                                        test2Ab.equals(test2Bb));
                        success &= test("UnoRuntime.areSame(test2Ab, test2Bb)",
                                        UnoRuntime.areSame(test2Ab, test2Bb));
                        success &= test("test2Ab.equals(test2Bc)",
                                        test2Ab.equals(test2Bc));
                        success &= test("UnoRuntime.areSame(test2Ab, test2Bc)",
                                        UnoRuntime.areSame(test2Ab, test2Bc));

                        success &= test("!test2Ac.equals(null)",
                                        !test2Ac.equals(null));
                        success &= test("!UnoRuntime.areSame(test2Ac, null)",
                                        !UnoRuntime.areSame(test2Ac, null));
                        success &= test("!test2Ac.equals(test1Aa)",
                                        !test2Ac.equals(test1Aa));
                        success &= test("!UnoRuntime.areSame(test2Ac, test1Aa)",
                                        !UnoRuntime.areSame(test2Ac, test1Aa));
                        success &= test("!test2Ac.equals(test1Ab)",
                                        !test2Ac.equals(test1Ab));
                        success &= test("!UnoRuntime.areSame(test2Ac, test1Ab)",
                                        !UnoRuntime.areSame(test2Ac, test1Ab));
                        success &= test("!test2Ac.equals(test1Ac)",
                                        !test2Ac.equals(test1Ac));
                        success &= test("!UnoRuntime.areSame(test2Ac, test1Ac)",
                                        !UnoRuntime.areSame(test2Ac, test1Ac));
                        success &= test("!test2Ac.equals(test1Ba)",
                                        !test2Ac.equals(test1Ba));
                        success &= test("!UnoRuntime.areSame(test2Ac, test1Ba)",
                                        !UnoRuntime.areSame(test2Ac, test1Ba));
                        success &= test("!test2Ac.equals(test1Bb)",
                                        !test2Ac.equals(test1Bb));
                        success &= test("!UnoRuntime.areSame(test2Ac, test1Bb)",
                                        !UnoRuntime.areSame(test2Ac, test1Bb));
                        success &= test("!test2Ac.equals(test1Bc)",
                                        !test2Ac.equals(test1Bc));
                        success &= test("!UnoRuntime.areSame(test2Ac, test1Bc)",
                                        !UnoRuntime.areSame(test2Ac, test1Bc));
                        success &= test("test2Ac.equals(test2Aa)",
                                        test2Ac.equals(test2Aa));
                        success &= test("UnoRuntime.areSame(test2Ac, test2Aa)",
                                        UnoRuntime.areSame(test2Ac, test2Aa));
                        success &= test("test2Ac.equals(test2Ab)",
                                        test2Ac.equals(test2Ab));
                        success &= test("UnoRuntime.areSame(test2Ac, test2Ab)",
                                        UnoRuntime.areSame(test2Ac, test2Ab));
                        success &= test("test2Ac.equals(test2Ac)",
                                        test2Ac.equals(test2Ac));
                        success &= test("UnoRuntime.areSame(test2Ac, test2Ac)",
                                        UnoRuntime.areSame(test2Ac, test2Ac));
                        success &= test("test2Ac.equals(test2Ba)",
                                        test2Ac.equals(test2Ba));
                        success &= test("UnoRuntime.areSame(test2Ac, test2Ba)",
                                        UnoRuntime.areSame(test2Ac, test2Ba));
                        success &= test("test2Ac.equals(test2Bb)",
                                        test2Ac.equals(test2Bb));
                        success &= test("UnoRuntime.areSame(test2Ac, test2Bb)",
                                        UnoRuntime.areSame(test2Ac, test2Bb));
                        success &= test("test2Ac.equals(test2Bc)",
                                        test2Ac.equals(test2Bc));
                        success &= test("UnoRuntime.areSame(test2Ac, test2Bc)",
                                        UnoRuntime.areSame(test2Ac, test2Bc));

                        success &= test("!test2Ba.equals(null)",
                                        !test2Ba.equals(null));
                        success &= test("!UnoRuntime.areSame(test2Ba, null)",
                                        !UnoRuntime.areSame(test2Ba, null));
                        success &= test("!test2Ba.equals(test1Aa)",
                                        !test2Ba.equals(test1Aa));
                        success &= test("!UnoRuntime.areSame(test2Ba, test1Aa)",
                                        !UnoRuntime.areSame(test2Ba, test1Aa));
                        success &= test("!test2Ba.equals(test1Ab)",
                                        !test2Ba.equals(test1Ab));
                        success &= test("!UnoRuntime.areSame(test2Ba, test1Ab)",
                                        !UnoRuntime.areSame(test2Ba, test1Ab));
                        success &= test("!test2Ba.equals(test1Ac)",
                                        !test2Ba.equals(test1Ac));
                        success &= test("!UnoRuntime.areSame(test2Ba, test1Ac)",
                                        !UnoRuntime.areSame(test2Ba, test1Ac));
                        success &= test("!test2Ba.equals(test1Ba)",
                                        !test2Ba.equals(test1Ba));
                        success &= test("!UnoRuntime.areSame(test2Ba, test1Ba)",
                                        !UnoRuntime.areSame(test2Ba, test1Ba));
                        success &= test("!test2Ba.equals(test1Bb)",
                                        !test2Ba.equals(test1Bb));
                        success &= test("!UnoRuntime.areSame(test2Ba, test1Bb)",
                                        !UnoRuntime.areSame(test2Ba, test1Bb));
                        success &= test("!test2Ba.equals(test1Bc)",
                                        !test2Ba.equals(test1Bc));
                        success &= test("!UnoRuntime.areSame(test2Ba, test1Bc)",
                                        !UnoRuntime.areSame(test2Ba, test1Bc));
                        success &= test("test2Ba.equals(test2Aa)",
                                        test2Ba.equals(test2Aa));
                        success &= test("UnoRuntime.areSame(test2Ba, test2Aa)",
                                        UnoRuntime.areSame(test2Ba, test2Aa));
                        success &= test("test2Ba.equals(test2Ab)",
                                        test2Ba.equals(test2Ab));
                        success &= test("UnoRuntime.areSame(test2Ba, test2Ab)",
                                        UnoRuntime.areSame(test2Ba, test2Ab));
                        success &= test("test2Ba.equals(test2Ac)",
                                        test2Ba.equals(test2Ac));
                        success &= test("UnoRuntime.areSame(test2Ba, test2Ac)",
                                        UnoRuntime.areSame(test2Ba, test2Ac));
                        success &= test("test2Ba.equals(test2Ba)",
                                        test2Ba.equals(test2Ba));
                        success &= test("UnoRuntime.areSame(test2Ba, test2Ba)",
                                        UnoRuntime.areSame(test2Ba, test2Ba));
                        success &= test("test2Ba.equals(test2Bb)",
                                        test2Ba.equals(test2Bb));
                        success &= test("UnoRuntime.areSame(test2Ba, test2Bb)",
                                        UnoRuntime.areSame(test2Ba, test2Bb));
                        success &= test("test2Ba.equals(test2Bc)",
                                        test2Ba.equals(test2Bc));
                        success &= test("UnoRuntime.areSame(test2Ba, test2Bc)",
                                        UnoRuntime.areSame(test2Ba, test2Bc));

                        success &= test("!test2Bb.equals(null)",
                                        !test2Bb.equals(null));
                        success &= test("!UnoRuntime.areSame(test2Bb, null)",
                                        !UnoRuntime.areSame(test2Bb, null));
                        success &= test("!test2Bb.equals(test1Aa)",
                                        !test2Bb.equals(test1Aa));
                        success &= test("!UnoRuntime.areSame(test2Bb, test1Aa)",
                                        !UnoRuntime.areSame(test2Bb, test1Aa));
                        success &= test("!test2Bb.equals(test1Ab)",
                                        !test2Bb.equals(test1Ab));
                        success &= test("!UnoRuntime.areSame(test2Bb, test1Ab)",
                                        !UnoRuntime.areSame(test2Bb, test1Ab));
                        success &= test("!test2Bb.equals(test1Ac)",
                                        !test2Bb.equals(test1Ac));
                        success &= test("!UnoRuntime.areSame(test2Bb, test1Ac)",
                                        !UnoRuntime.areSame(test2Bb, test1Ac));
                        success &= test("!test2Bb.equals(test1Ba)",
                                        !test2Bb.equals(test1Ba));
                        success &= test("!UnoRuntime.areSame(test2Bb, test1Ba)",
                                        !UnoRuntime.areSame(test2Bb, test1Ba));
                        success &= test("!test2Bb.equals(test1Bb)",
                                        !test2Bb.equals(test1Bb));
                        success &= test("!UnoRuntime.areSame(test2Bb, test1Bb)",
                                        !UnoRuntime.areSame(test2Bb, test1Bb));
                        success &= test("!test2Bb.equals(test1Bc)",
                                        !test2Bb.equals(test1Bc));
                        success &= test("!UnoRuntime.areSame(test2Bb, test1Bc)",
                                        !UnoRuntime.areSame(test2Bb, test1Bc));
                        success &= test("test2Bb.equals(test2Aa)",
                                        test2Bb.equals(test2Aa));
                        success &= test("UnoRuntime.areSame(test2Bb, test2Aa)",
                                        UnoRuntime.areSame(test2Bb, test2Aa));
                        success &= test("test2Bb.equals(test2Ab)",
                                        test2Bb.equals(test2Ab));
                        success &= test("UnoRuntime.areSame(test2Bb, test2Ab)",
                                        UnoRuntime.areSame(test2Bb, test2Ab));
                        success &= test("test2Bb.equals(test2Ac)",
                                        test2Bb.equals(test2Ac));
                        success &= test("UnoRuntime.areSame(test2Bb, test2Ac)",
                                        UnoRuntime.areSame(test2Bb, test2Ac));
                        success &= test("test2Bb.equals(test2Ba)",
                                        test2Bb.equals(test2Ba));
                        success &= test("UnoRuntime.areSame(test2Bb, test2Ba)",
                                        UnoRuntime.areSame(test2Bb, test2Ba));
                        success &= test("test2Bb.equals(test2Bb)",
                                        test2Bb.equals(test2Bb));
                        success &= test("UnoRuntime.areSame(test2Bb, test2Bb)",
                                        UnoRuntime.areSame(test2Bb, test2Bb));
                        success &= test("test2Bb.equals(test2Bc)",
                                        test2Bb.equals(test2Bc));
                        success &= test("UnoRuntime.areSame(test2Bb, test2Bc)",
                                        UnoRuntime.areSame(test2Bb, test2Bc));

                        success &= test("!test2Bc.equals(null)",
                                        !test2Bc.equals(null));
                        success &= test("!UnoRuntime.areSame(test2Bc, null)",
                                        !UnoRuntime.areSame(test2Bc, null));
                        success &= test("!test2Bc.equals(test1Aa)",
                                        !test2Bc.equals(test1Aa));
                        success &= test("!UnoRuntime.areSame(test2Bc, test1Aa)",
                                        !UnoRuntime.areSame(test2Bc, test1Aa));
                        success &= test("!test2Bc.equals(test1Ab)",
                                        !test2Bc.equals(test1Ab));
                        success &= test("!UnoRuntime.areSame(test2Bc, test1Ab)",
                                        !UnoRuntime.areSame(test2Bc, test1Ab));
                        success &= test("!test2Bc.equals(test1Ac)",
                                        !test2Bc.equals(test1Ac));
                        success &= test("!UnoRuntime.areSame(test2Bc, test1Ac)",
                                        !UnoRuntime.areSame(test2Bc, test1Ac));
                        success &= test("!test2Bc.equals(test1Ba)",
                                        !test2Bc.equals(test1Ba));
                        success &= test("!UnoRuntime.areSame(test2Bc, test1Ba)",
                                        !UnoRuntime.areSame(test2Bc, test1Ba));
                        success &= test("!test2Bc.equals(test1Bb)",
                                        !test2Bc.equals(test1Bb));
                        success &= test("!UnoRuntime.areSame(test2Bc, test1Bb)",
                                        !UnoRuntime.areSame(test2Bc, test1Bb));
                        success &= test("!test2Bc.equals(test1Bc)",
                                        !test2Bc.equals(test1Bc));
                        success &= test("!UnoRuntime.areSame(test2Bc, test1Bc)",
                                        !UnoRuntime.areSame(test2Bc, test1Bc));
                        success &= test("test2Bc.equals(test2Aa)",
                                        test2Bc.equals(test2Aa));
                        success &= test("UnoRuntime.areSame(test2Bc, test2Aa)",
                                        UnoRuntime.areSame(test2Bc, test2Aa));
                        success &= test("test2Bc.equals(test2Ab)",
                                        test2Bc.equals(test2Ab));
                        success &= test("UnoRuntime.areSame(test2Bc, test2Ab)",
                                        UnoRuntime.areSame(test2Bc, test2Ab));
                        success &= test("test2Bc.equals(test2Ac)",
                                        test2Bc.equals(test2Ac));
                        success &= test("UnoRuntime.areSame(test2Bc, test2Ac)",
                                        UnoRuntime.areSame(test2Bc, test2Ac));
                        success &= test("test2Bc.equals(test2Ba)",
                                        test2Bc.equals(test2Ba));
                        success &= test("UnoRuntime.areSame(test2Bc, test2Ba)",
                                        UnoRuntime.areSame(test2Bc, test2Ba));
                        success &= test("test2Bc.equals(test2Bb)",
                                        test2Bc.equals(test2Bb));
                        success &= test("UnoRuntime.areSame(test2Bc, test2Bb)",
                                        UnoRuntime.areSame(test2Bc, test2Bb));
                        success &= test("test2Bc.equals(test2Bc)",
                                        test2Bc.equals(test2Bc));
                        success &= test("UnoRuntime.areSame(test2Bc, test2Bc)",
                                        UnoRuntime.areSame(test2Bc, test2Bc));

                        success &= test(
                            "test1Aa.hashCode() == test1Ab.hashCode()",
                            test1Aa.hashCode() == test1Ab.hashCode());
                        success &= test(
                            "test1Aa.hashCode() == test1Ac.hashCode()",
                            test1Aa.hashCode() == test1Ac.hashCode());
                        success &= test(
                            "test1Aa.hashCode() == test1Ba.hashCode()",
                            test1Aa.hashCode() == test1Ba.hashCode());
                        success &= test(
                            "test1Aa.hashCode() == test1Bb.hashCode()",
                            test1Aa.hashCode() == test1Bb.hashCode());
                        success &= test(
                            "test1Aa.hashCode() == test1Bc.hashCode()",
                            test1Aa.hashCode() == test1Bc.hashCode());
                        success &= test(
                            "test2Aa.hashCode() == test2Ab.hashCode()",
                            test2Aa.hashCode() == test2Ab.hashCode());
                        success &= test(
                            "test2Aa.hashCode() == test2Ac.hashCode()",
                            test2Aa.hashCode() == test2Ac.hashCode());
                        success &= test(
                            "test2Aa.hashCode() == test2Ba.hashCode()",
                            test2Aa.hashCode() == test2Ba.hashCode());
                        success &= test(
                            "test2Aa.hashCode() == test2Bb.hashCode()",
                            test2Aa.hashCode() == test2Bb.hashCode());
                        success &= test(
                            "test2Aa.hashCode() == test2Bc.hashCode()",
                            test2Aa.hashCode() == test2Bc.hashCode());

                        done.notifyDone();
                        testBed.serverDone(success);
                    }

                    private boolean test(String message, boolean condition) {
                        if (!condition) {
                            System.err.println("Failed: " + message);
                        }
                        return condition;
                    }
                };
        }

        private final TestBed testBed;
        private final String unoTypes;
        private final String unoServices;
    }

    public interface XDone extends XInterface {
        void notifyDone();

        TypeInfo[] UNOTYPEINFO = { new MethodTypeInfo("notifyDone", 0, 0) };
    }

    public interface XTestFrame extends XInterface {
        void notifyAccepting(XDone done, Object object1, Object object2)
            throws Exception;

        TypeInfo[] UNOTYPEINFO = {
            new MethodTypeInfo("notifyAccepting", 0, TypeInfo.ONEWAY) };
    }

    private static final String CONNECTION_DESCRIPTION
    = "socket,host=localhost,port=12346";
    private static final String PROTOCOL_DESCRIPTION = "urp";

    private static final String INSTANCE1 = "instance1";
    private static final String INSTANCE2 = "instance2";
}
