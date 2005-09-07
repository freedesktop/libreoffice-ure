/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: Cache.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-07 19:03:05 $
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

package com.sun.star.lib.uno.protocols.urp;

import java.util.HashMap;

/**
   An LRU cache for arbitrary objects.

   This class is not synchronized, as any necessary synchronization will already
   take place in the client.
*/
final class Cache {
    /**
       Create a cache.

       @param size the maximum cache size, must be between 0, inclusive, and
       NOT_CACHED, exclusive
    */
    public Cache(int size) {
        maxSize = size;
    }

    public int add(boolean[] found, Object content) {
        Entry e = (Entry) map.get(content);
        found[0] = e != null;
        if (e == null) {
            if (map.size() < maxSize) {
                // There is still room for a new entry at the front:
                e = new Entry(content, map.size(), last, null);
                if (first == null) {
                    last = e;
                } else {
                    first.prev = e;
                }
                first = e;
            } else if (last != null) {
                // Take last entry out and recycle as new front:
                map.remove(last.content);
                e = last;
                e.content = content;
                if (first != last) {
                    // Reached only if maxSize > 1:
                    last = last.prev;
                    last.next = null;
                    e.prev = null;
                    e.next = first;
                    first.prev = e;
                    first = e;
                }
            } else {
                // Reached iff maxSize == 0:
                return NOT_CACHED;
            }
            map.put(content, e);
        } else if (e != first) {
            // Move to front (reached only if maxSize > 1):
            e.prev.next = e.next;
            if (e.next == null) {
                last = e.prev;
            } else {
                e.next.prev = e.prev;
            }
            e.prev = null;
            e.next = first;
            first.prev = e;
            first = e;
        }
        return e.index;
    }

    public static final int NOT_CACHED = 0xFFFF;

    private static final class Entry {
        public Entry(Object content, int index, Entry prev, Entry next) {
            this.content = content;
            this.index = index;
            this.prev = prev;
            this.next = next;
        }

        public Object content;
        public int index;
        public Entry prev;
        public Entry next;
    }

    // first/last form a list of 0 to maxSize entries, most recently used first;
    // map contains the same entries; each entry has a unique index in the range
    // 0 to maxSize - 1
    private final int maxSize;
    private final HashMap map = new HashMap(); // from Object to Entry
    private Entry first = null;
    private Entry last = null;
}
