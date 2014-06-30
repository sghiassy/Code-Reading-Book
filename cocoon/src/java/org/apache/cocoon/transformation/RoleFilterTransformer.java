/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.transformation;

import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.SourceResolver;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.util.Map;
import java.util.StringTokenizer;

/**
 * Filter XML fragments based on a user's role.  This will help in
 * the development of smart forms that must only show information to
 * people who are logged in and have the correct role.  The Role is
 * specified by the Request semantics.  You can specify multiple roles
 * by using comma delimiting.
 *
 * <pre>
 *   &lt;root xmlns:roles="http://apache.org/cocoon/role-filter/1.0"&gt;
 *     &lt;textbox name="identifier" roles:restricted="admin,boss"/&gt;
 *     &lt;textbox name="name" roles:read-only="admin,boss"/&gt;
 *   &lt;/root&gt;
 * </pre>
 *
 * The previous example will only show the "identifier" textbox for the
 * roles "admin" and "boss".  It will pass role:read-only="" if the
 * roles "admin" or "boss" are accessing the page.  That way you can
 * specify any special processing by testing for the read-only attribute.
 * This filter does not care about the prefix, only the namespace URI.
 * That means you can reassign the namespace to another prefix and all
 * will work as expected.
 *
 * @author <a href="mailto:bloritsch@apache.org">Berin Loritsch</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/08 10:47:46 $ $Author: cziegeler $
 */
public class RoleFilterTransformer extends FilterTransformer {
    private final static String URI = "http://apache.org/cocoon/role-filter/1.0";
    private final static String RESTRICT = "restricted";
    private final static String VIEW = "read-only";
    Request request = null;

    public RoleFilterTransformer() {
    }

    public final void setup(SourceResolver resolver, Map objectModel, String src, Parameters params)
    throws ProcessingException, SAXException, IOException {
        super.setup(resolver, objectModel, src, params);
        this.request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
    }

    /**
     * Disable caching
     */
    public long generateKey() {
        return 0;
    }

    public final void startElement(String uri, String loc, String raw, Attributes a)
    throws SAXException {
        int roleIndex = a.getIndex(RoleFilterTransformer.URI, RoleFilterTransformer.RESTRICT);
        int viewIndex = a.getIndex(RoleFilterTransformer.URI, RoleFilterTransformer.VIEW);
        boolean propogate = true;
        boolean readOnly = false;

        if (roleIndex >= 0) {
            String roleRestriction = a.getValue(roleIndex);
            StringTokenizer roles = new StringTokenizer(roleRestriction, ",", false);
            propogate = false;

            while ((! propogate) && roles.hasMoreTokens()) {
                if (request.isUserInRole(roles.nextToken())) {
                    propogate = true;
                }
            }
        }

        if (! propogate) {
            super.elementName = loc;
        } else {
            if (viewIndex >= 0) {
                String viewRestriction = a.getValue(viewIndex);
                StringTokenizer roles = new StringTokenizer(viewRestriction, ",", false);

                while ((! readOnly) && roles.hasMoreTokens()) {
                    if (request.isUserInRole(roles.nextToken())) {
                        readOnly = true;
                    }
                }
            }
        }

        super.startElement(uri, loc, raw,
                this.copyAttributes(a, roleIndex, viewIndex, readOnly));
    }

    public final void endElement(String uri, String loc, String raw)
    throws SAXException {
        super.endElement(uri, loc, raw);

        if (! super.skip) {
            super.elementName = "";
        }
    }

    private final Attributes copyAttributes(final Attributes a, final int role,
                                            final int view, boolean readOnly) {
        if (role < 0 && view < 0) {
            return a;
        }

        AttributesImpl attr = new AttributesImpl();
        attr.setAttributes(a);
        if (role >= 0) {
            attr.removeAttribute(role);
        }

        if (view >= 0) {
            if (readOnly) {
                attr.setValue(view, "");
            } else {
                attr.removeAttribute(view);
            }
        }

        return attr;
    }

    public void recycle() {
        this.request = null;
    }
}
