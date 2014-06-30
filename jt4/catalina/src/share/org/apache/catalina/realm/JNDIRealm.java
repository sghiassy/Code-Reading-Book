/*
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowlegement:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowlegement may appear in the software itself,
 *    if and wherever such third-party acknowlegements normally appear.
 *
 * 4. The names "The Jakarta Project", "Tomcat", and "Apache Software
 *    Foundation" must not be used to endorse or promote products derived
 *    from this software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 * [Additional notices, if required by prior licensing conditions]
 *
 */


package org.apache.catalina.realm;


import java.security.Principal;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import javax.naming.Context;
import javax.naming.NameNotFoundException;
import javax.naming.NamingEnumeration;
import javax.naming.NamingException;
import javax.naming.directory.Attribute;
import javax.naming.directory.Attributes;
import javax.naming.directory.DirContext;
import javax.naming.directory.InitialDirContext;
import javax.naming.directory.SearchControls;
import javax.naming.directory.SearchResult;
import org.apache.catalina.LifecycleException;
import org.apache.catalina.Realm;
import org.apache.catalina.util.StringManager;


/**
 * <p>Implementation of <strong>Realm</strong> that works with a directory
 * server accessed via the Java Naming and Directory Interface (JNDI) APIs.
 * The following constraints are imposed on the data structure in the
 * underlying directory server:</p>
 * <ul>
 * <li>Each user that can be authenticated is represented by an individual
 *     element in the top level <code>DirContext</code> that is accessed
 *     via the <code>connectionURL</code> property.  This element has the
 *     following characteristics:
 *     <ul>
 *     <li>The distinguished name (<code>dn</code>) attribute of this element
 *         contains the username that is being presented for authentication.
 *         </li>
 *     <li>The distinguished name can be represented by a pattern passed to
 *         an instance of <code>MessageFormat</code>, where the string "{0}"
 *         in the pattern is replaced by the username being presented.</li>
 *     <li>The element for this user contains an attribute named by the
 *         <code>userPassword</code> property.  The value of this attribute
 *         is retrieved for use in authentication.</li>
 *     <li>The value of the user password attribute is either a cleartext
 *         String, or the result of passing a cleartext String through the
 *         <code>RealmBase.digest()</code> method (using the standard digest
 *         support included in <code>RealmBase</code>).
 *     <li>The user is considered to be authenticated if the presented
 *         credentials (after being passed through
 *         <code>RealmBase.digest()</code>) are equal to the retrieved value
 *         for the user password attribute.</li>
 *     </ul></li>
 * <li>Each group of users that has been assigned a particular role is
 *     represented by an individual element in the top level
 *     <code>DirContext</code> that is accessed via the
 *     <code>connectionURL</code> property.  This element has the following
 *     characteristics:
 *     <ul>
 *     <li>The set of all possible groups of interest can be selected by a
 *         search pattern configured by the <code>roleSearch</code>
 *         property.</li>
 *     <li>The <code>roleSearch</code> pattern optionally includes pattern
 *         replacements "{0}" for the distinguished name, and/or "{1}" for
 *         the username, of the authenticated user for which roles will be
 *         retrieved.</li>
 *     <li>The <code>roleBase</code> property can be set to the element that
 *         is the base of the search for matching roles.  If not specified,
 *         the entire context will be searched.</li>
 *     <li>The <code>roleSubtree</code> property can be set to
 *         <code>true</code> if you wish to search the entire subtree of the
 *         directory context.  The default value of <code>false</code>
 *         requests a search of only the current level.</li>
 *     <li>The element includes an attribute (whose name is configured by
 *         the <code>roleName</code> property) containing the name of the
 *         role represented by this element.</li>
 *     </ul></li>
 * <li>Note that the standard <code>&lt;security-role-ref&gt;</code> element in
 *     the web application deployment descriptor allows applications to refer
 *     to roles programmatically by names other than those used in the
 *     directory server itself.</li>
 * </ul>
 *
 * <p><strong>TODO</strong> - Support connection pooling (including message
 * format objects) so that <code>authenticate()</code> does not have to be
 * synchronized.</p>
 *
 * @author John Holman
 * @author Craig R. McClanahan
 * @version $Revision: 1.5 $ $Date: 2001/09/07 20:45:12 $
 */

public class JNDIRealm extends RealmBase {


    // ----------------------------------------------------- Instance Variables


    /**
     * The connection username for the server we will contact.
     */
    protected String connectionName = null;


    /**
     * The connection password for the server we will contact.
     */
    protected String connectionPassword = null;


    /**
     * The connection URL for the server we will contact.
     */
    protected String connectionURL = null;


    /**
     * The directory context linking us to our directory server.
     */
    protected DirContext context = null;


    /**
     * The JNDI context factory used to acquire our InitialContext.  By
     * default, assumes use of an LDAP server using the standard JNDI LDAP
     * provider.
     */
    protected String contextFactory = "com.sun.jndi.ldap.LdapCtxFactory";


    /**
     * Descriptive information about this Realm implementation.
     */
    protected static final String info =
        "org.apache.catalina.realm.JNDIRealm/1.0";


    /**
     * Descriptive information about this Realm implementation.
     */
    protected static final String name = "JNDIRealm";


    /**
     * The base element for role searches.
     */
    protected String roleBase = "";


    /**
     * The MessageFormat object associated with the current
     * <code>roleSearch</code>.
     */
    protected MessageFormat roleFormat = null;


    /**
     * The name of the attribute containing the role name.
     */
    protected String roleName[] = null;


    /**
     * The message format used to select roles for a user, with "{0}" marking
     * the spot where the distinguished name of the user goes.
     */
    protected String roleSearch = null;


    /**
     * Should we search the entire subtree for matching memberships?
     */
    protected boolean roleSubtree = false;


    /**
     * The MessageFormat object associated with the current
     * <code>userPattern</code>.
     */
    protected MessageFormat userFormat = null;


    /**
     * The attribute name used to retrieve the user password.
     */
    protected String userPassword[] = null;


    /**
     * The message format used to select a user, with "{0}" marking the
     * spot where the specified username goes.
     */
    protected String userPattern = null;


    // ------------------------------------------------------------- Properties


    /**
     * Return the connection username for this Realm.
     */
    public String getConnectionName() {

        return (this.connectionName);

    }


    /**
     * Set the connection username for this Realm.
     *
     * @param connectionName The new connection username
     */
    public void setConnectionName(String connectionName) {

        this.connectionName = connectionName;

    }


    /**
     * Return the connection password for this Realm.
     */
    public String getConnectionPassword() {

        return (this.connectionPassword);

    }


    /**
     * Set the connection password for this Realm.
     *
     * @param connectionPassword The new connection password
     */
    public void setConnectionPassword(String connectionPassword) {

        this.connectionPassword = connectionPassword;

    }


    /**
     * Return the connection URL for this Realm.
     */
    public String getConnectionURL() {

        return (this.connectionURL);

    }


    /**
     * Set the connection URL for this Realm.
     *
     * @param connectionURL The new connection URL
     */
    public void setConnectionURL(String connectionURL) {

        this.connectionURL = connectionURL;

    }


    /**
     * Return the JNDI context factory for this Realm.
     */
    public String getContextFactory() {

        return (this.contextFactory);

    }


    /**
     * Set the JNDI context factory for this Realm.
     *
     * @param contextFactory The new context factory
     */
    public void setContextFactory(String contextFactory) {

        this.contextFactory = contextFactory;

    }


    /**
     * Return the base element for role searches.
     */
    public String getRoleBase() {

        return (this.roleBase);

    }


    /**
     * Set the base element for role searches.
     *
     * @param roleBase The new base element
     */
    public void setRoleBase(String roleBase) {

        this.roleBase = roleBase;

    }


    /**
     * Return the role name attribute name for this Realm.
     */
    public String getRoleName() {

        if (this.roleName != null)
            return (this.roleName[0]);
        else
            return (null);

    }


    /**
     * Set the role name attribute name for this Realm.
     *
     * @param roleName The new role name attribute name
     */
    public void setRoleName(String roleName) {

        if (roleName != null)
            this.roleName = new String[] { roleName };
        else
            this.roleName = null;

    }


    /**
     * Return the message format pattern for selecting roles in this Realm.
     */
    public String getRoleSearch() {

        return (this.roleSearch);

    }


    /**
     * Set the message format pattern for selecting roles in this Realm.
     *
     * @param roleSearch The new role search pattern
     */
    public void setRoleSearch(String roleSearch) {

        this.roleSearch = roleSearch;
        if (roleSearch == null)
            roleFormat = null;
        else
            roleFormat = new MessageFormat(roleSearch);

    }


    /**
     * Return the "search subtree for roles" flag.
     */
    public boolean getRoleSubtree() {

        return (this.roleSubtree);

    }


    /**
     * Set the "search subtree for roles" flag.
     *
     * @param roleSubtree The new search flag
     */
    public void setRoleSubtree(boolean roleSubtree) {

        this.roleSubtree = roleSubtree;

    }


    /**
     * Return the password attribute used to retrieve the user password.
     */
    public String getUserPassword() {

        if (this.userPassword != null)
            return (this.userPassword[0]);
        else
            return (null);

    }


    /**
     * Set the password attribute used to retrieve the user password.
     *
     * @param userPassword The new password attribute
     */
    public void setUserPassword(String userPassword) {

        if (userPassword != null)
            this.userPassword = new String[] { userPassword };
        else
            this.userPassword = null;

    }


    /**
     * Return the message format pattern for selecting users in this Realm.
     */
    public String getUserPattern() {

        return (this.userPattern);

    }


    /**
     * Set the message format pattern for selecting users in this Realm.
     *
     * @param userPattern The new user pattern
     */
    public void setUserPattern(String userPattern) {

        this.userPattern = userPattern;
        if (userPattern == null)
            userFormat = null;
        else
            userFormat = new MessageFormat(userPattern);

    }


    // ---------------------------------------------------------- Realm Methods


    /**
     * Return the Principal associated with the specified username and
     * credentials, if there is one; otherwise return <code>null</code>.
     *
     * If there are any errors with the JDBC connection, executing
     * the query or anything we return null (don't authenticate). This
     * event is also logged, and the connection will be closed so that
     * a subsequent request will automatically re-open it.
     *
     * @param username Username of the Principal to look up
     * @param credentials Password or other credentials to use in
     *  authenticating this username
     */
    public Principal authenticate(String username, String credentials) {

        DirContext context = null;

        try {

            // Ensure that we have a directory context available
            context = open();

            // Authenticate the specified username if possible
            Principal principal = authenticate(context,
                                               username, credentials);

            // Release this context
            release(context);

            // Return the authenticated Principal (if any)
            return (principal);

        } catch (NamingException e) {

            // Log the problem for posterity
            log(sm.getString("jndiRealm.exception"), e);

            // Close the connection so that it gets reopened next time
            if (context != null)
                close(context);

            // Return "not authenticated" for this request
            return (null);

        }

    }


    // -------------------------------------------------------- Package Methods


    // ------------------------------------------------------ Protected Methods


    /**
     * Return the Principal associated with the specified username and
     * credentials, if there is one; otherwise return <code>null</code>.
     *
     * @param username Username of the Principal to look up
     * @param credentials Password or other credentials to use in
     *  authenticating this username
     *
     * @exception NamingException if a directory server error occurs
     */
    public synchronized Principal authenticate(DirContext context,
                                               String username,
                                               String credentials)
        throws NamingException {

        // Authenticate the specified username if possible
        String dn = getUserDN(context, username, credentials);
        if (dn == null)
            return (null);

        // Look up the associated roles
        List roles = getRoles(context, username, dn);

        // Create and return a suitable Principal for this user
        return (new GenericPrincipal(this, username, credentials, roles));

    }


    /**
     * Close any open connection to the directory server for this Realm.
     *
     * @param context The directory context to be closed
     */
    protected void close(DirContext context) {

        // Do nothing if there is no opened connection
        if (context == null)
            return;

        // Close our opened connection
        try {
            if (debug >= 1)
                log("Closing directory context");
            context.close();
        } catch (NamingException e) {
            log(sm.getString("jndiRealm.close"), e);
        }
        this.context = null;

    }


    /**
     * Return a short name for this Realm implementation.
     */
    protected String getName() {

        return (this.name);

    }


    /**
     * Return the password associated with the given principal's user name.
     */
    protected String getPassword(String username) {

        return (null);

    }


    /**
     * Return the Principal associated with the given user name.
     */
    protected Principal getPrincipal(String username) {

        return (null);

    }


    /**
     * Return a List of roles associated with the user with the specified
     * distinguished name.  If no roles are associated with this user, a
     * zero-length List is returned.
     *
     * @param context The directory context we are searching
     * @param username The username of the user to be checked
     * @param dn Distinguished name of the user to be checked
     *
     * @exception NamingException if a directory server error occurs
     */
    protected List getRoles(DirContext context,
                            String username, String dn)
        throws NamingException {

        if (debug >= 2)
            log("getRoles(" + dn + ")");

        // Are we configured to do role searches?
        ArrayList list = new ArrayList();
        if ((roleFormat == null) || (roleName == null))
            return (list);

        // Set up parameters for an appropriate search
        String filter = roleFormat.format(new String[] { dn, username });
        SearchControls controls = new SearchControls();
        if (roleSubtree)
            controls.setSearchScope(SearchControls.SUBTREE_SCOPE);
        else
            controls.setSearchScope(SearchControls.ONELEVEL_SCOPE);
        controls.setReturningAttributes(roleName);

        // Perform the configured search and process the results
        if (debug >= 3) {
            log("  Searching role base '" + roleBase + "' for attribute '" +
                roleName[0] + "'");
            log("  With filter expression '" + filter + "'");
        }
        NamingEnumeration results =
            context.search(roleBase, filter, controls);
        if (results == null)
            return (list);  // Should never happen, but just in case ...
        while (results.hasMore()) {
            SearchResult result = (SearchResult) results.next();
            Attributes attrs = result.getAttributes();
            if (attrs == null)
                continue;
            Attribute attr = attrs.get(roleName[0]);
            if (attr != null) {
                String role = (String) attr.get();
                if (debug >= 3)
                    log("  Found role '" + role + "'");
                list.add(role);
            }
        }

        // Return the completed list of roles
        if (debug >= 2)
            log("  Returning " + list.size() + " roles");
        return (list);

    }


    /**
     * Return the distinguished name of an authenticated user (if successful)
     * or <code>null</code> if authentication is unsuccessful.
     *
     * @param context The directory context we are accessing
     * @param username Username to be authenticated
     * @param credentials Authentication credentials
     *
     * @exception NamingException if a directory server error occurs
     */
    protected String getUserDN(DirContext context,
                               String username, String credentials)
        throws NamingException {

        if (debug >= 2)
            log("getUserDN(" + username + ")");
        if (username == null)
            return (null);
        if ((userFormat == null) || (userPassword == null))
            return (null);

        // Retrieve the user password attribute for this user
        String dn = userFormat.format(new String[] { username });
        if (debug >= 3)
            log("  dn=" + dn);
        Attributes attrs = null;
        try {
            attrs = context.getAttributes(dn, userPassword);
        } catch (NameNotFoundException e) {
            return (null);
        }
        if (attrs == null)
            return (null);
        if (debug >= 3)
            log("  retrieving attribute " + userPassword[0]);
        Attribute attr = attrs.get(userPassword[0]);
        if (attr == null)
            return (null);
        if (debug >= 3)
            log("  retrieving value");
        Object value = attr.get();
        if (value == null)
            return (null);
        String valueString = null;
        if (value instanceof byte[])
            valueString = new String((byte[]) value);
        else
            valueString = value.toString();

        // Validate the credentials specified by the user
        if (debug >= 3)
            log("  validating credentials");

        boolean validated = false;
        if (hasMessageDigest()) {
            // Hex hashes should be compared case-insensitive
            validated = (digest(credentials).equalsIgnoreCase(valueString));
        } else
            validated = (digest(credentials).equals(valueString));

        if (validated) {
            if (debug >= 2)
                log(sm.getString("jndiRealm.authenticateSuccess",
                                 username));
        } else {
            if (debug >= 2)
                log(sm.getString("jndiRealm.authenticateFailure",
                                 username));
            return (null);
        }
        return (dn);

    }


    /**
     * Open (if necessary) and return a connection to the configured
     * directory server for this Realm.
     *
     * @exception NamingException if a directory server error occurs
     */
    protected DirContext open() throws NamingException {

        // Do nothing if there is a directory server connection already open
        if (context != null)
            return (context);

        // Establish a connection and retrieve the initial context
        if (debug >= 1)
            log("Connecting to URL " + connectionURL);
        Hashtable env = new Hashtable();
        env.put(Context.INITIAL_CONTEXT_FACTORY, contextFactory);
        if (connectionName != null)
            env.put(Context.SECURITY_PRINCIPAL, connectionName);
        if (connectionPassword != null)
            env.put(Context.SECURITY_CREDENTIALS, connectionPassword);
        if (connectionURL != null)
            env.put(Context.PROVIDER_URL, connectionURL);
        context = new InitialDirContext(env);
        return (context);

    }


    /**
     * Release our use of this connection so that it can be recycled.
     *
     * @param context The directory context to release
     */
    protected void release(DirContext context) {

        ; // NO-OP since we are not pooling anything

    }


    // ------------------------------------------------------ Lifecycle Methods


    /**
     * Prepare for active use of the public methods of this Component.
     *
     * @exception IllegalStateException if this component has already been
     *  started
     * @exception LifecycleException if this component detects a fatal error
     *  that prevents it from being started
     */
    public void start() throws LifecycleException {

        // Validate that we can open our connection
        try {
            open();
        } catch (NamingException e) {
            throw new LifecycleException(sm.getString("jndiRealm.open"), e);
        }

        // Perform normal superclass initialization
        super.start();

    }


    /**
     * Gracefully shut down active use of the public methods of this Component.
     *
     * @exception IllegalStateException if this component has not been
     *  started
     * @exception LifecycleException if this component detects a fatal error
     *  that needs to be reported
     */
    public void stop() throws LifecycleException {

        // Perform normal superclass finalization
        super.stop();

        // Close any open directory server connection
        close(this.context);

    }



}
