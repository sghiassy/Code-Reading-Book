/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.acting;

import java.io.IOException;
import java.io.InputStream;
import java.lang.Class;
import java.net.URL;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;
import java.util.List;
import java.util.LinkedList;
import java.util.ListIterator;
import java.util.Enumeration;
import java.util.Collections;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;
import java.io.IOException;

import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.DefaultConfiguration;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.avalon.excalibur.component.ExcaliburComponentSelector;
import org.apache.avalon.excalibur.component.ExcaliburComponentManager;
import org.apache.avalon.excalibur.component.RoleManager;
import org.apache.avalon.excalibur.component.DefaultRoleManager;
import org.apache.avalon.excalibur.datasource.DataSourceComponent;

import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.classloader.RepositoryClassLoader;
import org.apache.cocoon.components.url.URLFactory;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Redirector;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.generation.ImageDirectoryGenerator;
import org.apache.cocoon.util.ClassUtils;
import org.apache.cocoon.util.HashMap;
import org.apache.cocoon.selection.Selector;

import org.apache.cocoon.acting.ModularDatabaseAccess.AutoIncrementHelper;
import org.apache.cocoon.acting.ModularDatabaseAccess.AttributeHelper;
import org.apache.cocoon.acting.ModularDatabaseAccess.OutputHelper;

/**
 * Adds record in a database. The action can update one or more tables,
 * and can add more than one row to a table at a time. The form descriptor
 * semantics for this are still in a bit of a state of flux. 
 *
 * The difference to the other Database*Actions is, that it uses
 * additional components for reading and writing parameters. In
 * addition the descriptor format has changed to accomodate the new
 * features.
 *
 * This action is heavily based upon the DatabaseAddAction and relies
 * on the AbstractDatabaseAction.
 *
 * Configuration options:
 * descriptor       file containing database description
 * throw-exception  throw an exception when an error occurs (default: false)
 *
 * Note: Component management will move to sitemap so that other
 * components can utilize the helper components.
 *
 * @author <a href="mailto:haul@apache.org">Christian Haul</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:35 $
 */
public abstract class ModularDatabaseAction extends AbstractDatabaseAction 
    implements Disposable, ThreadSafe, Contextualizable {

    // ========================================================================
    // inner helper classes
    // ========================================================================

    /**
     * Structure that takes all processed data for one column.
     */
    protected class Column {
        boolean isKey = false;
        boolean isSet = false;
        boolean isAutoIncrement = false;
        String mode = null;
        Configuration modeConf = null;
        Configuration columnConf = null;
    }

    /**
     * Structure that takes all processed data for a table depending
     * on current default modes 
     */
    protected class CacheHelper {
        /**
         * Generated query string
         */
        public String queryString = null;
        /**
         * if a set is used, column number which is used to determine
         * the number of rows to insert. 
         */
        public int setMaster = -1;
        public boolean isSet = false;
        public int noOfKeys = 0;
        public Column[] columns = null;
        
        public CacheHelper( int cols ) {
            this(0,cols);
        }
        
        public CacheHelper( int keys, int cols ) {
            noOfKeys = keys;
            columns = new Column[cols];
            for ( int i=0; i<cols; i++ ) {
                columns[i] = new Column();
            }
        }
    }


    /**
     * Structure that takes up both current mode types for database
     * operations and table configuration data. Used to access parsed
     * configuration data.
     */
    protected class LookUpKey {
        public Configuration tableConf = null;
        public Map modeTypes = null;
        
        public LookUpKey( Configuration tableConf, Map modeTypes ) {
            this.tableConf = tableConf;
            this.modeTypes = modeTypes;
        }
    }

    // ========================================================================
    // constants
    // ========================================================================

    private static final String LOCATION = "org.apache.cocoon.acting.ModularDatabaseAddAction";
    private static final int BYTE_ARRAY_SIZE = 1024;

    static final Integer MODE_AUTOINCR = new Integer( 0 );
    static final Integer MODE_OTHERS = new Integer( 1 );
    static final Integer MODE_OUTPUT = new Integer( 2 );

    // ========================================================================
    // instance vars
    // ========================================================================

    // please ignore component management as I know this is ugly and I
    // hope that this duty is taken by the sitemap in short term.
    // please ignore ====>
    protected RoleManager roleManager;
    private Configuration defaultConfig;

    protected Context context;

    /** The component manager instance */
    protected ExcaliburComponentManager manager2;

    /** The URLFactory instance */
    protected URLFactory urlFactory;

    protected ExcaliburComponentSelector  modeMapping = null;
    protected ExcaliburComponentSelector  outputMapping = null;

    // default modes for other / insert operations
    protected HashMap defaultModeNames = new HashMap( 3 );

    // <==== please ignore
    //

    protected final Map cachedQueryData = new HashMap();


    //
    // please ignore ====>

    // ========================================================================
    // setup / class loading
    // (basically copied from AbstractSitemap)
    // This is ugly. Please ignore. Helpers should be handled at sitemap level
    // so that other components can share them.
    // ========================================================================

    /**
     * Set the role manager
     */
    public void setRoleManager( RoleManager roles, Configuration config ) {
        this.roleManager = roles;
        this.defaultConfig = config;
    }


    /**
     * Set the current <code>ComponentManager</code> instance used by this
     * <code>Composable</code>.
     */
    public void compose( ComponentManager manager ) throws ComponentException {

        super.compose( manager );
        if (getLogger().isDebugEnabled()) 
            getLogger().debug("compose");
        this.manager2 = new ExcaliburComponentManager(manager);
        this.manager2.setLogger(getLogger());
        this.manager2.contextualize(this.context);
        this.manager2.setRoleManager(this.roleManager);

        try {
            if (this.defaultConfig != null) {
                this.manager2.configure(this.defaultConfig);
            }
            this.urlFactory = (URLFactory)manager.lookup(URLFactory.ROLE);
            this.modeMapping = new ExcaliburComponentSelector();
            this.outputMapping = new ExcaliburComponentSelector();
            this.setupSelector(this.modeMapping);
            this.setupSelector(this.outputMapping);
            this.manager2.addComponentInstance(Selector.ROLE + "Selector", this.modeMapping);
            this.manager2.addComponentInstance(Selector.ROLE + "Selector", this.outputMapping);
        } catch (Exception e) {
            if (getLogger().isDebugEnabled())
                getLogger().error("cannot obtain the Component", e);
            throw new ComponentException("cannot obtain the URLFactory", e);
        }
    }



    private void setupSelector( ExcaliburComponentSelector selector ) throws Exception {

        selector.setLogger(getLogger());
        selector.contextualize(this.context);
        selector.setRoleManager(this.roleManager);
        selector.compose(this.manager2);
    }



    public void contextualize( Context context ) throws ContextException {

        this.context = context;
    }

    /**
     * Return the component manager for this sitemap
     */
    public ComponentManager getComponentManager() {

        return this.manager2;
    }

    /**
     * Release all components.
     */
    public void dispose() {

        if (this.urlFactory != null) {
            manager.release((Component)this.urlFactory);
        }
        if (this.modeMapping != null) {
            manager2.release((Component)this.modeMapping);
        }
        if (this.outputMapping != null) {
            manager2.release((Component)this.outputMapping);
        }
        manager2.dispose();
    }


    /**
     * Configure modes that are used to delegate database specific methods
     * and other modes.
     */
    public void configure( Configuration conf ) throws ConfigurationException {

        try {
            if (getLogger().isDebugEnabled())
                getLogger().debug("MDAA: configure");
            LinkedList hints = new LinkedList();
            if (conf != null) {
                String val = null;
                String nodeName = null;
                Configuration[] parameters = conf.getChildren();
                this.settings = new HashMap(parameters.length);
                for ( int i = 0; i < parameters.length; i++) {
                    nodeName= parameters[i].getName();
                    if (getLogger().isDebugEnabled())
                        getLogger().debug("configure -- " + nodeName);
                    if ( nodeName != null ) {
                        if ( "mode".equals(nodeName.trim()) || "output".equals(nodeName.trim())) {
                            String modeName = parameters[i].getAttribute("name");
                            String src = parameters[i].getAttribute("src");
                            if (modeName!=null && src!=null) {
                                Configuration modeConfig = parameters[i];
                                if (modeConfig == null) {
                                    modeConfig = this.defaultConfig;
                                }
                                if (getLogger().isDebugEnabled())
                                    getLogger().debug("load -- " + modeName + " -> " + src);
                                if ( "mode".equals(nodeName.trim())) {
                                    this.load_component(modeMapping, modeName, src, modeConfig);
                                    hints.add(modeName);
                                } else {
                                    this.load_component(outputMapping, modeName, src, modeConfig);
                                    if ( !defaultModeNames.containsKey( MODE_OUTPUT ) ) {
                                        defaultModeNames.put( MODE_OUTPUT, modeName );
                                    }
                                }
                            }
                        } else {
                            val = parameters[i].getValue();
                            this.settings.put(nodeName, val);
                        }
                    }
                }
            }
            
            this.modeMapping.initialize();
            this.outputMapping.initialize();
            this.manager2.initialize();
            
            ListIterator li = hints.listIterator();
            while ( li.hasNext() ) {
                String modeName = (String) li.next();
                Component helper = modeMapping.select( modeName );
                if ( !defaultModeNames.containsKey( MODE_OTHERS ) && helper instanceof AttributeHelper ) { 
                    defaultModeNames.put( MODE_OTHERS, modeName );
                    if (getLogger().isDebugEnabled())
                        getLogger().debug(modeName + " default mode for normal columns");
                }
                
                if ( !defaultModeNames.containsKey( MODE_AUTOINCR ) && helper instanceof AutoIncrementHelper ) { 
                    defaultModeNames.put( MODE_AUTOINCR, modeName );
                    if (getLogger().isDebugEnabled())
                        getLogger().debug(modeName + " default mode for autoincrement columns");
                }
                modeMapping.release(helper);
            }
            
            if ( !defaultModeNames.containsKey( MODE_OTHERS ) || 
                 !defaultModeNames.containsKey( MODE_AUTOINCR ) || 
                 !defaultModeNames.containsKey( MODE_OUTPUT ) 
                 ) {
                throw new ConfigurationException("Not all default modes are configured:"
                                                 + ( defaultModeNames.containsKey( MODE_AUTOINCR ) ? " insert mode" : "" )
                                                 + ( defaultModeNames.containsKey( MODE_OTHERS ) ? " others" : "" )
                                                 + ( defaultModeNames.containsKey( MODE_OUTPUT ) ? " output" : "" )
                                                 );
            }
            
        } catch (Exception e) {
            throw new ConfigurationException(e.toString());
        }
    }
    
    
    /**
     * Load a component.
     *
     * @param hint Object to identify this component
     * @param classURL component's class name / URL
     * @param config configuration for this component
     */
    private void load_component( ExcaliburComponentSelector selector, Object hint, 
                                 String classURL, Configuration config ) throws Exception {

        Class clazz = null;
        //FIXME(GP): Is it true that a class name containing a colon should be an URL?
        if (classURL.indexOf(':') > 1) {
            URL url = urlFactory.getURL(classURL);
            byte[] b = getByteArrayFromStream(url.openStream());
            clazz = ((RepositoryClassLoader)ClassUtils.getClassLoader()).defineClass(b);
        } else {
            clazz = ClassUtils.loadClass(classURL);
        }
        if (!Component.class.isAssignableFrom(clazz)) {
            throw new IllegalAccessException("Object " + classURL + " is not a Component");
        }
        selector.addComponent(hint, clazz, config);
    }


    
    /**
     * Helper to read in a class
     */
    private byte[] getByteArrayFromStream( InputStream stream ) {

        List list = new ArrayList();
        byte[] b = new byte[BYTE_ARRAY_SIZE];
        int last = 0;
        try {
            while ((last = stream.read(b)) == BYTE_ARRAY_SIZE) {
                list.add(b);
                b = new byte[BYTE_ARRAY_SIZE];
            }
        } catch (IOException ioe) {
            if (getLogger().isErrorEnabled())
                getLogger().error("cannot read class byte stream", ioe);
        }
        list.add(b);
        int listSize = list.size();
        b = new byte[(listSize - 1) * BYTE_ARRAY_SIZE + last];
        int i;
        for (i = 0; i < listSize - 1; i++) {
            System.arraycopy(list.get(i), 0, b, i * BYTE_ARRAY_SIZE, BYTE_ARRAY_SIZE);
        }
        System.arraycopy(list.get(i), 0, b, i * BYTE_ARRAY_SIZE, last);
        return b;
    }


    // <==== please ignore
    //

    // ========================================================================
    // protected utility methods
    // ========================================================================

    /**
     * override super's method since we prefer to get the datasource
     * from defaults first or from sitemap parameters rather than from 
     * descriptor file.
     */
    protected DataSourceComponent getDataSource( Configuration conf, Parameters parameters )
        throws ComponentException {

        String sourceName = parameters.getParameter( "connection", (String) settings.get( "connection" ) );
        if ( sourceName == null ) {
            return getDataSource( conf );
        } else {
            if (getLogger().isDebugEnabled())
                getLogger().debug("Using datasource: "+sourceName);
            return (DataSourceComponent) this.dbselector.select(sourceName);
        }
    }


    /**
     * Store a key/value pair in the request attributes. We prefix the key
     * with the name of this class to prevent potential name collisions.
     * This method overrides super class' method to allow an OutputHelper 
     * to take care of what to do with the values.
     */
    void setRequestAttribute(Request request, String key, Object value) {

        try {
            OutputHelper oh = (OutputHelper) outputMapping.select( (String) defaultModeNames.get( MODE_OUTPUT ) );
            oh.setAttribute( null, request, key, value );
            outputMapping.release( oh );
        } catch (Exception e) {
                if (getLogger().isWarnEnabled())
                    getLogger()
                        .warn( "Could not select output mode " 
                               + (String) defaultModeNames.get( MODE_OUTPUT ) 
                               + ":" + e.getMessage() );
        }
    }


    /**
     * Retrieve a value from the request attributes.
     * This method overrides super class' method to allow an OutputHelper 
     * to take care of where to get the values.
     */
    Object getRequestAttribute(Request request, String key) {

        Object value = null;
        try {
            OutputHelper oh = (OutputHelper) outputMapping.select( (String) defaultModeNames.get( MODE_OUTPUT ) );
            value = oh.getAttribute( null, request, key );
            outputMapping.release( oh );
        } catch (Exception e) {
            if (getLogger().isWarnEnabled())
                getLogger()
                    .warn( "Could not select output mode "
                           + (String) defaultModeNames.get( MODE_OUTPUT )
                           + ":" + e.getMessage());
        }
        return value;
    }
    

    // ========================================================================
    // main method
    // ========================================================================


    /**
     * Add a record to the database.  This action assumes that
     * the file referenced by the "descriptor" parameter conforms
     * to the AbstractDatabaseAction specifications.
     */
    public Map act( Redirector redirector, SourceResolver resolver, Map objectModel, 
                    String source, Parameters param ) throws Exception {

        DataSourceComponent datasource = null;
        Connection conn = null;
        Map results = new HashMap();

        // read global parameter settings
        boolean reloadable = Constants.DESCRIPTOR_RELOADABLE_DEFAULT;
        Request request = (Request) objectModel.get(Constants.REQUEST_OBJECT);
        
        if (this.settings.containsKey("reloadable"))
            reloadable = Boolean.getBoolean((String) this.settings.get("reloadable"));
        // read local parameter settings
        try {
            Configuration conf = 
                this.getConfiguration(param.getParameter("descriptor", (String) this.settings.get("descriptor")), 
                                      resolver,
                                      param.getParameterAsBoolean("reloadable",reloadable));
            
            datasource = this.getDataSource(conf, param);
            conn = datasource.getConnection();
            if (conn.getAutoCommit() == true) {
                try {
                    conn.setAutoCommit(false);
                } catch (Exception ex) {
                    String tmp = param.getParameter("use-transactions",(String) this.settings.get("use-transactions",null));
                    if (tmp != null &&  (tmp.equalsIgnoreCase("no") || tmp.equalsIgnoreCase("false") || tmp.equalsIgnoreCase("0"))) {
                        if (getLogger().isErrorEnabled()) 
                            getLogger().error("This DB connection does not support transactions. If you want to risk your data's integrity by continuing nonetheless set parameter \"use-transactions\" to \"no\".");
                        throw ex;
                    }
                }
            }

            Configuration[] tables = conf.getChildren("table");
            String tablesetname = param.getParameter("table-set", (String) this.settings.get("table-set"));
            Map set_tables = null; // default to old behaviour
            
            HashMap modeTypes = null;

            if (tablesetname != null) {
                // new set based behaviour
                Configuration[] tablesets = conf.getChildren("table-set");
                String setname = null;
                boolean found = false;
                
                // find tables contained in tableset
                int j = 0;
                for (j=0; j<tablesets.length; j++) {
                    setname = tablesets[j].getAttribute ("name", "");
                    if (tablesetname.trim().equals (setname.trim ())) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    throw new IOException(" given set " + tablesetname + " does not exists in a description file.");
                }
                
                Configuration[] set = tablesets[j].getChildren("table");
        
                // construct a Map that contains the names of the tables
                // contained in the requested tableset
                set_tables = new HashMap(set.length);
                for (int i=0; i<set.length; i++) {
                    // look for alternative modes
                    modeTypes = new HashMap(2);
                    modeTypes.put( MODE_AUTOINCR, set[i].getAttribute( "autoincr-mode", "autoincr" ) );
                    modeTypes.put( MODE_OTHERS, set[i].getAttribute( "others-mode",   "others" ) );
                    set_tables.put(set[i].getAttribute("name",""), modeTypes);
                }
            } else {
                modeTypes = new HashMap(2);
                modeTypes.put( MODE_AUTOINCR, "autoincr" );
                modeTypes.put( MODE_OTHERS, "others" );
            };
            for (int i=0; i<tables.length; i++) {
                if ( set_tables == null || 
                     set_tables.containsKey( tables[i].getAttribute( "name" ) ) ||
                     ( tables[i].getAttribute( "alias", null ) != null && set_tables.containsKey( tables[i].getAttribute( "alias" ) ) )
                     ) {
                    if (tablesetname != null) {
                        modeTypes = (HashMap) set_tables.get(tables[i].getAttribute("name"));
                    }
                    processTable( tables[i], conn, request, results, modeTypes );
                }
            }
            
            if (conn.getAutoCommit()==false)
                conn.commit();
            OutputHelper oh = (OutputHelper) outputMapping.select( (String) defaultModeNames.get( MODE_OUTPUT ) );
            oh.commit( null, request );
            outputMapping.release( oh );
        } catch (Exception e) {
            if ( conn != null ) {
                try {
                    if (getLogger().isDebugEnabled())
                        getLogger().debug( "Rolling back transaction. Caused by " + e.getMessage() );
                    conn.rollback();
                    OutputHelper oh = (OutputHelper) outputMapping.select( (String) defaultModeNames.get( MODE_OUTPUT ) );
                    oh.rollback( null, request, e );
                    outputMapping.release( oh );
                } catch (SQLException se) {
                    if (getLogger().isDebugEnabled())
                        getLogger().debug("There was an error rolling back the transaction", se);
                }
            }

            //throw new ProcessingException("Could not add record :position = " + currentIndex, e);

            // don't throw an exception, an error has been signalled, that should suffice
        
            String throwException = (String) this.settings.get( "throw-exception", 
                                                                param.getParameter( "throw-exception", null ) );
            if ( throwException != null && 
                 ( throwException.equalsIgnoreCase( "true" ) || throwException.equalsIgnoreCase( "yes" ) ) ) {
                throw new ProcessingException("Could not add record",e);
            }
        } finally {
            if (conn != null) {
                try {
                    conn.close();
                } catch (SQLException sqe) {
                    getLogger().warn("There was an error closing the datasource", sqe);
                }
            }
            
            if (datasource != null) 
                this.dbselector.release(datasource);
        }
        
        return Collections.unmodifiableMap(results);
    }



    /**
     * Inserts a row or a set of rows into the given table based on the
     * request parameters
     *
     * @param table the table's configuration
     * @param conn the database connection
     * @param request the request
     */
    protected void processTable( Configuration table, Connection conn, Request request, 
                                 Map results, HashMap modeTypes ) 
        throws SQLException, ConfigurationException, Exception {

        PreparedStatement statement = null;
        try {
            CacheHelper queryData = this.getQuery( table, modeTypes, defaultModeNames );
            if (getLogger().isDebugEnabled())
                getLogger().debug("query: "+queryData.queryString);
            statement = conn.prepareStatement(queryData.queryString);
            
            Object[][] columnValues = this.getColumnValues( table, queryData, request );
            
            int setLength = 1;
            if ( queryData.isSet ) {
                if ( columnValues[ queryData.setMaster ] != null ) {
                    setLength = columnValues[ queryData.setMaster ].length;
                } else {
                    setLength = 0;
                }
            }
            
            for ( int rowIndex = 0; rowIndex < setLength; rowIndex++ ) {
                if (getLogger().isDebugEnabled())
                    getLogger().debug( "====> row no. " + rowIndex );
                processRow( request, conn, statement, table, queryData, columnValues, rowIndex, results );
            }
            
        } finally {
            try {
                if (statement != null) {
                    statement.close();
                }
            } catch (SQLException e) {}
        }
    }

    
    /**
     * Choose a mode configuration based on its name.
     * @param conf Configuration (i.e. a column's configuration) that might have
     * several children configurations named "mode".
     * @param type desired type (i.e. every mode has a type
     * attribute), find the first mode that has a compatible type.
     * Special mode "all" matches all queried types.
     * @return configuration that has desired type or type "all" or null.
     */
    protected Configuration getMode( Configuration conf, String type ) 
        throws ConfigurationException {

        String modeAll = "all";
        Configuration[] modes = conf.getChildren("mode");
        Configuration modeConfig = null;;
        
        for ( int i=0; i<modes.length; i++ ) {
            String modeType = modes[i].getAttribute("type", "others");
            if ( modeType.equals(type) || modeType.equals(modeAll)) {
                if (getLogger().isDebugEnabled())
                    getLogger().debug("requested mode was \""+type+"\" returning \""+modeType+"\"");
                modeConfig = modes[i];
                break;
            };
        }
        
        return modeConfig;
    }


    /**
     * compose name for output a long the lines of "table.column"
     */
    protected String getOutputName ( Configuration tableConf, Configuration columnConf )
        throws ConfigurationException {

        return getOutputName( tableConf, columnConf, -1 );
    }


    /**
     * compose name for output a long the lines of "table.column[row]" or
     * "table.column" if rowIndex is -1.
     */
    protected String getOutputName ( Configuration tableConf, Configuration columnConf, int rowIndex )
        throws ConfigurationException {

        return ( tableConf.getAttribute("alias", tableConf.getAttribute("name") ) 
                 + "." + columnConf.getAttribute("name") 
                 + ( rowIndex == -1 ? "" : "[" + rowIndex + "]" ) );
    }
    

    /*
     * Read all values for a column from an AttributeHelper
     *
     * If the given column is an autoincrement column, an empty array
     * is returned, otherwise if it is part of a set, all available
     * values are fetched, or only the first one if it is not part of
     * a set.
     *
     */
    protected Object[] getColumnValue( Configuration tableConf, Column column, Request request )
        throws ConfigurationException, ComponentException {

        if ( column.isAutoIncrement ) {
            return new Object[1];
        } else {
            Object[] values;
            AttributeHelper dph = (AttributeHelper) modeMapping.select( column.mode );
            String cname = getOutputName( tableConf, column.columnConf );
            if ( column.isSet ){
                if (getLogger().isDebugEnabled())
                    getLogger().debug( "Trying to set column " + cname +" using getAttributeValues method");
                values = dph.getAttributeValues( cname, column.modeConf, request );
            } else {
                if (getLogger().isDebugEnabled())
                    getLogger().debug( "Trying to set column " + cname +" using getAttribute method");
                values = new Object[1];
                values[0] = dph.getAttribute( cname, column.modeConf, request );
            }
            modeMapping.release( dph );
            if ( values != null ) {
                for ( int i = 0; i < values.length; i++ ) {
                    if (getLogger().isDebugEnabled())
                        getLogger().debug( "Setting column " + cname + " [" + i + "] " + values[i] );
                }
            }
            return values;
        }
    }


    /**
     * Setup parsed attribute configuration object
     */
    protected void fillModes ( Configuration[] conf, boolean isKey, HashMap defaultModeNames,
                               HashMap modeTypes, CacheHelper set )
        throws ConfigurationException {

        String setMode = null;
        int setMaster = -1;
        String setMastersMode = null;
        boolean manyrows = false;
        int offset = ( isKey ? 0: set.noOfKeys);
        
        for ( int i = offset; i < conf.length + offset; i++ ) {
            if (getLogger().isDebugEnabled())
                getLogger().debug("i="+i);
            set.columns[i].columnConf =  conf[ i - offset ];
            set.columns[i].isSet = false;
            set.columns[i].isKey = isKey;
            set.columns[i].isAutoIncrement = false;
            if ( isKey & this.honourAutoIncrement() ) {
                String autoIncrement = set.columns[i].columnConf.getAttribute("autoincrement","false");
                if ( autoIncrement.equalsIgnoreCase("yes") || autoIncrement.equalsIgnoreCase("true") ) {
                    set.columns[i].isAutoIncrement = true;
                }
            }
            set.columns[i].modeConf = getMode( set.columns[i].columnConf, 
                                               selectMode( set.columns[i].isAutoIncrement, modeTypes ) );
            set.columns[i].mode = ( set.columns[i].modeConf != null ? 
                                    set.columns[i].modeConf.getAttribute( "name", selectMode( isKey, defaultModeNames ) ) :  
                                    selectMode( isKey, defaultModeNames ) );
            // Determine set mode for a whole column ...
            setMode = set.columns[i].columnConf.getAttribute("set", null);  // master vs slave vs null
            if ( setMode == null && set.columns[i].modeConf != null ) {
                // ... or for each mode individually
                setMode = set.columns[i].modeConf.getAttribute("set", null);
            }
            if ( setMode != null ) {
                manyrows = true;
                set.columns[i].isSet = true;
                set.isSet = true;
                if ( setMode.equals("master") ) {
                    set.setMaster = i;
                }
            } 
        }
    }



    /**
     * Put key values into request attributes.
     */
    protected void storeKeyValue( Configuration tableConf, Column key, int rowIndex, Connection conn, 
                                  Statement statement, Request request, Map results ) 
        throws SQLException, ConfigurationException, ComponentException {

        AutoIncrementHelper dah = (AutoIncrementHelper) modeMapping.select( key.mode );
        if (!dah.includeAsValue()) {
            String keyname = getOutputName( tableConf, key.columnConf, rowIndex );
            Object value = dah.getPostValue( tableConf, key.columnConf, key.modeConf, conn, statement, request );
            if (getLogger().isDebugEnabled())
                getLogger().debug( "Retrieving autoincrement for " + keyname + "as " + value );
            setRequestAttribute( request, keyname, value );
            results.put( keyname, String.valueOf( value ) );
        }
        modeMapping.release(dah);
    }


    /**
     * Sets the key value on the prepared statement for an autoincrement type.
     *
     * @param table the table's configuration object
     * @param column the key's configuration object
     * @param currentIndex the position of the key column
     * @param rowIndex the position in the current row set
     * @param conn the database connection
     * @param statement the insert statement
     * @param request the request object
     * @param result sitemap result object
     * @return the number of columns by which to increment the currentIndex
     */
    protected int setKeyAuto ( Configuration table, Column column, int currentIndex, int rowIndex,
                               Connection conn, PreparedStatement statement, Request request, Map results ) 
        throws ConfigurationException, SQLException, ComponentException, Exception {

        int columnCount = 0;

        AutoIncrementHelper dah = (AutoIncrementHelper) modeMapping.select( column.mode );
        if ( dah.includeInQuery() ) {
            if ( dah.includeAsValue() ) {
                Object value = dah.getPreValue( table, column.columnConf, column.modeConf, conn, request );
                String keyname = this.getOutputName( table, column.columnConf, rowIndex );
                if (getLogger().isDebugEnabled())
                    getLogger().debug( "Setting key " + keyname + " to " + value );
                statement.setObject( currentIndex, value );
                setRequestAttribute( request, keyname, value );
                results.put( keyname, String.valueOf( value ) );
                columnCount = 1;
            }
        } else {
            if (getLogger().isDebugEnabled())
                getLogger().debug( "Automatically setting key" );
        }  
        modeMapping.release( dah );
        return columnCount;
    }

    // ========================================================================
    // abstract methods
    // ========================================================================


    /**
     * set all necessary ?s and execute the query
     *
     * This method is intended to be overridden by classes that
     * implement other operations e.g. delete
     */
    protected abstract void processRow( Request request, Connection conn, PreparedStatement statement,
                                        Configuration table, CacheHelper queryData, Object[][] columnValues,
                                        int rowIndex, Map results )
        throws SQLException, ConfigurationException, Exception;
    
    /**
     * determine which mode to use as default mode
     *
     * This method is intended to be overridden by classes that
     * implement other operations e.g. delete
     */
    protected abstract String selectMode( boolean isAutoIncrement, HashMap modes );


    /**
     * determine whether autoincrement columns should be honoured by
     * this operation. This is usually snsible only for INSERTs.
     *
     * This method is intended to be overridden by classes that
     * implement other operations e.g. delete
     */
    protected abstract boolean honourAutoIncrement();


    /**
     * Fetch all values for all columns that are needed to do the
     * database operation.
     *
     * This method is intended to be overridden by classes that
     * implement other operations e.g. delete
     */
    abstract Object[][] getColumnValues( Configuration tableConf, CacheHelper queryData, Request request )
        throws ConfigurationException, ComponentException;

    /**
     * Get the String representation of the PreparedStatement.  This is
     * mapped to the Configuration object itself, so if it doesn't exist,
     * it will be created.
     *
     * This method is intended to be overridden by classes that
     * implement other operations e.g. delete
     *
     * @param table the table's configuration object
     * @return the insert query as a string
     */
    protected abstract CacheHelper getQuery( Configuration table, HashMap modeTypes, HashMap defaultModeNames ) 
        throws ConfigurationException, ComponentException;

}
