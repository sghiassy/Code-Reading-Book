/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.transformation;

import org.apache.avalon.excalibur.datasource.DataSourceComponent;
import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.*;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.Tokenizer;
import org.apache.log.Logger;
import org.xml.sax.Attributes;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.lang.reflect.Field;
import java.sql.*;
import java.util.*;

/**
 *
 * @author <a href="mailto:balld@webslingerZ.com">Donald Ball</a>
 * @author <a href="mailto:giacomo.pati@pwr.ch">Giacomo Pati</a>
 *         (PWR Organisation & Entwicklung)
 * @author <a href="mailto:sven.beauprez@the-ecorp.com">Sven Beauprez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:20 $ $Author: giacomo $
 */

public class SQLTransformer extends AbstractTransformer implements Composable, Recyclable, Disposable, Configurable {

    /** The SQL namespace **/
    public static final String my_uri = "http://apache.org/cocoon/SQL/2.0";
    public static final String my_name = "SQLTransformer";

    /** The SQL namespace element names **/
    public static final String MAGIC_EXECUTE_QUERY = "execute-query";
    public static final String MAGIC_CONNECTION = "use-connection";
    public static final String MAGIC_DBURL = "dburl";
    public static final String MAGIC_USERNAME = "username";
    public static final String MAGIC_PASSWORD = "password";
    public static final String MAGIC_NR_OF_ROWS = "show-nr-of-rows";
    public static final String MAGIC_QUERY = "query";
    public static final String MAGIC_VALUE = "value";
    public static final String MAGIC_DOC_ELEMENT = "doc-element";
    public static final String MAGIC_ROW_ELEMENT = "row-element";
    public static final String MAGIC_IN_PARAMETER = "in-parameter";
    public static final String MAGIC_IN_PARAMETER_NR_ATTRIBUTE = "nr";
    public static final String MAGIC_IN_PARAMETER_VALUE_ATTRIBUTE = "value";
    public static final String MAGIC_OUT_PARAMETER = "out-parameter";
    public static final String MAGIC_OUT_PARAMETER_NAME_ATTRIBUTE = "name";
    public static final String MAGIC_OUT_PARAMETER_NR_ATTRIBUTE = "nr";
    public static final String MAGIC_OUT_PARAMETER_TYPE_ATTRIBUTE = "type";
    public static final String MAGIC_ESCAPE_STRING = "escape-string";
    public static final String MAGIC_ERROR = "error";

    public static final String MAGIC_NS_URI_ELEMENT = "namespace-uri";
    public static final String MAGIC_NS_PREFIX_ELEMENT = "namespace-prefix";

    public static final String MAGIC_ANCESTOR_VALUE = "ancestor-value";
    public static final String MAGIC_ANCESTOR_VALUE_LEVEL_ATTRIBUTE = "level";
    public static final String MAGIC_ANCESTOR_VALUE_NAME_ATTRIBUTE = "name";
    public static final String MAGIC_SUBSTITUTE_VALUE = "substitute-value";
    public static final String MAGIC_SUBSTITUTE_VALUE_NAME_ATTRIBUTE = "name";
    public static final String MAGIC_NAME_ATTRIBUTE = "name";
    public static final String MAGIC_STORED_PROCEDURE_ATTRIBUTE = "isstoredprocedure";
    public static final String MAGIC_UPDATE_ATTRIBUTE = "isupdate";

    /** The states we are allowed to be in **/
    public static final int STATE_OUTSIDE = 0;
    public static final int STATE_INSIDE_EXECUTE_QUERY_ELEMENT = 1;
    public static final int STATE_INSIDE_VALUE_ELEMENT = 2;
    public static final int STATE_INSIDE_QUERY_ELEMENT = 3;
    public static final int STATE_INSIDE_ANCESTOR_VALUE_ELEMENT = 4;
    public static final int STATE_INSIDE_SUBSTITUTE_VALUE_ELEMENT = 5;
    public static final int STATE_INSIDE_IN_PARAMETER_ELEMENT = 6;
    public static final int STATE_INSIDE_OUT_PARAMETER_ELEMENT = 7;
    public static final int STATE_INSIDE_ESCAPE_STRING = 8;

    /** Default parameters that might apply to all queries **/
    protected Properties default_properties;

    /** The list of queries that we're currently working on **/
    protected Vector queries;

    /** The offset of the current query in the queries list **/
    protected int current_query_index;

    /** The name of the value element we're currently receiving **/
    protected String current_name;

    /** The current state of the event receiving FSM **/
    protected int current_state;

    /** The value of the value element we're currently receiving **/
    protected StringBuffer current_value;

    /** Check if nr of rows need to be written out. **/
    protected String showNrOfRows;

    /** Is the old-driver turned on? (default is off) */
    private boolean oldDriver = false;

    /** Namespace prefix to output */
    protected String outPrefix;

    /** Namespace uri to output */
    protected String outUri;

    /** The prefix of our namespace to listen to */
    protected String inPrefix;

    protected ComponentSelector dbSelector = null;
    protected ComponentManager manager;

    protected Parameters parameters;
    protected Map objectModel;

    public void compose( ComponentManager manager ) {
        try {
            this.manager = manager;
            this.dbSelector = (ComponentSelector) manager.lookup( DataSourceComponent.ROLE + "Selector" );
        } catch ( ComponentException cme ) {
            getLogger().warn( "Could not get the DataSource Selector", cme );
        }
    }

    /**
     * Recycle this component
     */
    public void recycle() {
        super.recycle();
        this.queries = null;
        this.current_value = null;
        this.current_name = null;
        this.default_properties = null;
    }

    /**
     * dispose
     */
    public void dispose() {
        if ( this.dbSelector != null ) this.manager.release( (Component) this.dbSelector );
    }

    /**
     * configure
     */
    public void configure( Configuration conf ) throws ConfigurationException {
        if ( conf != null ) {
            Configuration child = conf.getChild( "old-driver" );
            this.oldDriver = child.getValueAsBoolean( false );
            getLogger().debug( "old-driver is " + this.oldDriver + " for " + this );
        }
    }

    /** BEGIN SitemapComponent methods **/

    public void setup( SourceResolver resolver, Map objectModel,
                       String source, Parameters parameters )
            throws ProcessingException, SAXException, IOException {
        // setup instance variables
        this.queries = new Vector();
        this.current_value = new StringBuffer();
        this.current_name = null;
        this.current_query_index = -1;
        this.default_properties = new Properties();
        this.current_state = SQLTransformer.STATE_OUTSIDE;

        this.parameters = parameters;
        this.objectModel = objectModel;

        current_state = SQLTransformer.STATE_OUTSIDE;

        // Check for connection
        String parameter = parameters.getParameter( SQLTransformer.MAGIC_CONNECTION, null );
        if ( parameter != null ) {
            getLogger().debug( "CONNECTION: " + parameter );
            default_properties.setProperty( SQLTransformer.MAGIC_CONNECTION, parameter );
        } else {
            // Check the dburl
            parameter = parameters.getParameter( SQLTransformer.MAGIC_DBURL, null );
            if ( parameter != null ) {
                getLogger().debug( "DBURL: " + parameter );
                default_properties.setProperty( SQLTransformer.MAGIC_DBURL, parameter );
            }
            // Check the username
            parameter = parameters.getParameter( SQLTransformer.MAGIC_USERNAME, null );
            if ( parameter != null ) {
                getLogger().debug( "USERNAME: " + parameter );
                default_properties.setProperty( SQLTransformer.MAGIC_USERNAME, parameter );
            }

            // Check the password
            parameter = parameters.getParameter( SQLTransformer.MAGIC_PASSWORD, null );
            if ( parameter != null ) {
                default_properties.setProperty( SQLTransformer.MAGIC_PASSWORD, parameter );
            }
        }
        showNrOfRows = parameters.getParameter( SQLTransformer.MAGIC_NR_OF_ROWS, null );

        // Check for row-element and doc-element
        parameter = parameters.getParameter( SQLTransformer.MAGIC_DOC_ELEMENT, null );
        if ( parameter != null ) {
            getLogger().debug( "DOC-ELEMENT: " + parameter );
            default_properties.setProperty( SQLTransformer.MAGIC_DOC_ELEMENT, parameter );
        }
        parameter = parameters.getParameter( SQLTransformer.MAGIC_ROW_ELEMENT, null );
        if ( parameter != null ) {
            getLogger().debug( "ROW-ELEMENT: " + parameter );
            default_properties.setProperty( SQLTransformer.MAGIC_ROW_ELEMENT, parameter );
        }

        // Check namespace-prefix and namespace-uri
        parameter = parameters.getParameter( SQLTransformer.MAGIC_NS_URI_ELEMENT, null );
        if ( parameter != null ) {
            getLogger().debug( "NS-URI: " + parameter );
            default_properties.setProperty( SQLTransformer.MAGIC_NS_URI_ELEMENT, parameter );
        }

        parameter = parameters.getParameter( SQLTransformer.MAGIC_NS_PREFIX_ELEMENT, null );
        if ( parameter != null ) {
            getLogger().debug( "NS-PREFIX: " + parameter );
            default_properties.setProperty( SQLTransformer.MAGIC_NS_PREFIX_ELEMENT, parameter );
        }
    }

    /** END SitemapComponent methods **/

    /** BEGIN my very own methods **/

    /**
     * This will be the meat of SQLTransformer, where the query is run.
     */
    protected void executeQuery( int index ) throws SAXException {


        // First set up the namespace handling
        if ( getCurrentQuery().properties.getProperty( SQLTransformer.MAGIC_NS_URI_ELEMENT ) != null ) {
            outUri = getCurrentQuery().properties.getProperty( SQLTransformer.MAGIC_NS_URI_ELEMENT );
        } else {
            outUri = my_uri;
        }

        if ( getCurrentQuery().properties.getProperty( SQLTransformer.MAGIC_NS_PREFIX_ELEMENT ) != null ) {
            outPrefix = getCurrentQuery().properties.getProperty( SQLTransformer.MAGIC_NS_PREFIX_ELEMENT );
        } else {
            outPrefix = "";
        }
        if ( !"".equals( outPrefix ) && !"".equals( outUri ) ) {
            this.contentHandler.startPrefixMapping( outPrefix, outUri );
        }

        getLogger().debug( "SQLTransformer executing query nr " + index );
        AttributesImpl attr = new AttributesImpl();
        Query query = (Query) queries.elementAt( index );
        boolean query_failure = false;
        try {
            try {
                query.execute();
            } catch ( SQLException e ) {
                getLogger().debug( "SQLTransformer:.executeQuery() query.execute failed ", e );
                AttributesImpl my_attr = new AttributesImpl();
                this.start( query.rowset_name, my_attr );
                this.start( MAGIC_ERROR, my_attr);
                this.data( e.getMessage());
                this.end( MAGIC_ERROR );
                this.end( query.rowset_name );
                query_failure = true;
            }
            if ( !query_failure ) {

                if ( showNrOfRows != null && showNrOfRows.equalsIgnoreCase( "true" ) ) {
                    attr.addAttribute( my_uri, query.nr_of_rows, query.nr_of_rows, "CDATA",
                       String.valueOf( query.getNrOfRows() ) );
                }
                String name = query.getName();
                if ( name != null ) {
                    attr.addAttribute( my_uri, query.name_attribute, query.name_attribute, "CDATA",
                       name );
                }
                this.start( query.rowset_name, attr );
                attr = new AttributesImpl();
                if ( !query.isStoredProcedure() ) {
                    while ( query.next() ) {
                        this.start( query.row_name, attr );
                        query.serializeRow();
                        if ( index + 1 < queries.size() ) {
                            executeQuery( index + 1 );
                        }
                        this.end( query.row_name );
                    }
                } else {
                    query.serializeStoredProcedure();
                }
                this.end( query.rowset_name );
            }
        } catch ( SQLException e ) {
            getLogger().debug( "SQLTransformer.executeQuery()", e );
            throw new SAXException( e );
        } finally {
            try {
                query.close();
            } catch ( SQLException e ) {
                getLogger().warn( "SQLTransformer: Could not close JDBC connection", e );
            }
        }
        if ( !"".equals( outPrefix ) && !"".equals( outUri ) ) {
            this.contentHandler.endPrefixMapping( outPrefix );
        }
    }

    protected static void throwIllegalStateException( String message ) {
        throw new IllegalStateException( my_name + ": " + message );
    }

    protected void startExecuteQueryElement() {
        switch ( current_state ) {
            case SQLTransformer.STATE_OUTSIDE:
            case SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT:
                current_query_index = queries.size();
                Query query = new Query( this, current_query_index );
                queries.addElement( query );
                current_state = SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT;
                break;
            default:
                throwIllegalStateException( "Not expecting a start execute query element" );
        }
    }

    protected void startValueElement( String name ) {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT:
                current_name = name;
                current_value.setLength( 0 );
                current_state = SQLTransformer.STATE_INSIDE_VALUE_ELEMENT;
                break;
            default:
                throwIllegalStateException( "Not expecting a start value element: " +
                                            name );
        }
    }

    protected void startQueryElement( Attributes attributes ) {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT:
                current_value.setLength( 0 );
                Query q = getCurrentQuery();
                current_state = SQLTransformer.STATE_INSIDE_QUERY_ELEMENT;
                String isupdate =
                        attributes.getValue( "", SQLTransformer.MAGIC_UPDATE_ATTRIBUTE );
                if ( isupdate != null && !isupdate.equalsIgnoreCase( "false" ) )
                    q.setUpdate( true );
                String isstoredprocedure =
                        attributes.getValue( "", SQLTransformer.MAGIC_STORED_PROCEDURE_ATTRIBUTE );
                if ( isstoredprocedure != null && !isstoredprocedure.equalsIgnoreCase( "false" ) )
                    q.setStoredProcedure( true );
                String name =
                        attributes.getValue( "", SQLTransformer.MAGIC_NAME_ATTRIBUTE );
                if ( name != null ) {
                    q.setName( name );
                }
                break;
            default:
                throwIllegalStateException( "Not expecting a start query element" );
        }
    }

    protected void endQueryElement() {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_QUERY_ELEMENT:
                if ( current_value.length() > 0 ) {
                    getCurrentQuery().addQueryPart(
                            current_value.toString() );
                    getLogger().debug( "QUERY IS \"" +
                                       current_value.toString() + "\"" );
                    current_value.setLength( 0 );
                }
                current_state = SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT;
                break;
            default:
                throwIllegalStateException( "Not expecting a stop query element" );
        }
    }

    protected void endValueElement() {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_VALUE_ELEMENT:
                getCurrentQuery().setParameter( current_name,
                                                current_value.toString() );
                getLogger().debug( "SETTING VALUE ELEMENT name {" +
                                   current_name + "} value {" +
                                   current_value.toString() + "}" );

                current_state = SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT;
                break;
            default:
                throwIllegalStateException( "Not expecting an end value element" );
        }
    }

    protected void endExecuteQueryElement() throws SAXException {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT:
                if ( current_query_index == 0 ) {
                    executeQuery( 0 );
                    queries.removeAllElements();
                    current_state = SQLTransformer.STATE_OUTSIDE;
                } else {
                    current_query_index--;
                    current_state = SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT;
                }
                break;
            default:
                throwIllegalStateException( "Not expecting an end execute query element" );
        }
    }

    protected void startAncestorValueElement( Attributes attributes ) {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_QUERY_ELEMENT:
                int level = 0;
                try {
                    level = Integer.parseInt( attributes.getValue( my_uri,
                                                                   SQLTransformer.MAGIC_ANCESTOR_VALUE_LEVEL_ATTRIBUTE ) );
                } catch ( Exception e ) {
                    getLogger().debug( "SQLTransformer", e );
                    throwIllegalStateException( "Ancestor value elements must have a " +
                                                SQLTransformer.MAGIC_ANCESTOR_VALUE_LEVEL_ATTRIBUTE + " attribute" );
                }
                String name = attributes.getValue( my_uri,
                                                   SQLTransformer.MAGIC_ANCESTOR_VALUE_NAME_ATTRIBUTE );
                if ( name == null ) {
                    throwIllegalStateException( "Ancestor value elements must have a " +
                                                SQLTransformer.MAGIC_ANCESTOR_VALUE_NAME_ATTRIBUTE + " attribute" );
                }
                AncestorValue av = new AncestorValue( level, name );
                getLogger().debug( "ANCESTOR VALUE " + level + " " + name );

                if ( current_value.length() > 0 ) {
                    getCurrentQuery().addQueryPart(
                            current_value.toString() );
                    getLogger().debug( "QUERY IS \"" +
                                       current_value.toString() + "\"" );

                    current_value.setLength( 0 );
                }
                getCurrentQuery().addQueryPart( av );
                current_state = SQLTransformer.STATE_INSIDE_ANCESTOR_VALUE_ELEMENT;
                break;
            default:
                throwIllegalStateException( "Not expecting a start ancestor value element" );
        }
    }

    protected void endAncestorValueElement() {
        current_state = SQLTransformer.STATE_INSIDE_QUERY_ELEMENT;
    }

    protected void startSubstituteValueElement( Attributes attributes ) {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_QUERY_ELEMENT:
                String name = attributes.getValue( my_uri,
                                                   SQLTransformer.MAGIC_SUBSTITUTE_VALUE_NAME_ATTRIBUTE );
                if ( name == null ) {
                    throwIllegalStateException( "Substitute value elements must have a " +
                                                SQLTransformer.MAGIC_SUBSTITUTE_VALUE_NAME_ATTRIBUTE + " attribute" );
                }
                String substitute = parameters.getParameter( name, null );
                //escape single quote
                substitute = replaceCharWithString( substitute, '\'', "''" );
                getLogger().debug( "SUBSTITUTE VALUE " + substitute );
                if ( current_value.length() > 0 ) {
                    getCurrentQuery().addQueryPart( current_value.toString() );
                    getLogger().debug( "QUERY IS \"" +
                                       current_value.toString() + "\"" );

                    current_value.setLength( 0 );
                }
                getCurrentQuery().addQueryPart( substitute );
                current_state = SQLTransformer.STATE_INSIDE_SUBSTITUTE_VALUE_ELEMENT;
                break;
            default:
                throwIllegalStateException( "Not expecting a start substitute value element" );
        }
    }

    protected void endSubstituteValueElement() {
        current_state = SQLTransformer.STATE_INSIDE_QUERY_ELEMENT;
    }

    protected void startEscapeStringElement( Attributes attributes ) {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_QUERY_ELEMENT:
                if ( current_value.length() > 0 ) {
                    getCurrentQuery().addQueryPart( current_value.toString() );
                    getLogger().debug( "QUERY IS \"" +
                                       current_value.toString() + "\"" );
                    current_value.setLength( 0 );
                }

                current_state = SQLTransformer.STATE_INSIDE_ESCAPE_STRING;
                break;
            default:
                throwIllegalStateException( "Not expecting a start escape-string element" );
        }
    }

    protected void endEscapeStringElement() {
        switch ( current_state) {
        case SQLTransformer.STATE_INSIDE_ESCAPE_STRING:
            if ( current_value.length() > 0 ) {
                String escape = current_value.toString();
                escape = replaceCharWithString( escape, '\'', "''" );
                escape = replaceCharWithString( escape, '\\', "\\\\" );
                getCurrentQuery().addQueryPart( escape );
                getLogger().debug( "QUERY IS \"" +
                   current_value.toString() + "\"" );
                current_value.setLength( 0 );
            }
            current_state = SQLTransformer.STATE_INSIDE_QUERY_ELEMENT;
            break;
        default:
                throwIllegalStateException( "Not expecting a end escape-string element" );
        }
    }

    protected void startInParameterElement( Attributes attributes ) {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT:
                String nr = attributes.getValue( my_uri,
                                                 SQLTransformer.MAGIC_IN_PARAMETER_NR_ATTRIBUTE );
                String value = attributes.getValue( my_uri,
                                                    SQLTransformer.MAGIC_IN_PARAMETER_VALUE_ATTRIBUTE );
                getLogger().debug( "IN PARAMETER NR " + nr + "; VALUE " + value );
                int position = Integer.parseInt( nr );
                getCurrentQuery().setInParameter( position, value );
                current_state = SQLTransformer.STATE_INSIDE_IN_PARAMETER_ELEMENT;
                break;
            default:
                throwIllegalStateException( "Not expecting an in-parameter element" );
        }
    }

    protected void endInParameterElement() {
        current_state = SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT;
    }

    protected void startOutParameterElement( Attributes attributes ) {
        switch ( current_state ) {
            case SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT:
                String name = attributes.getValue( my_uri,
                                                   SQLTransformer.MAGIC_OUT_PARAMETER_NAME_ATTRIBUTE );
                String nr = attributes.getValue( my_uri,
                                                 SQLTransformer.MAGIC_OUT_PARAMETER_NR_ATTRIBUTE );
                String type = attributes.getValue( my_uri,
                                                   SQLTransformer.MAGIC_OUT_PARAMETER_TYPE_ATTRIBUTE );
                getLogger().debug( "OUT PARAMETER NAME" + name + ";NR " + nr + "; TYPE " + type );
                int position = Integer.parseInt( nr );
                getCurrentQuery().setOutParameter( position, type, name );
                current_state = SQLTransformer.STATE_INSIDE_OUT_PARAMETER_ELEMENT;
                break;
            default:
                throwIllegalStateException( "Not expecting an out-parameter element" );
        }
    }

    protected void endOutParameterElement() {
        current_state = SQLTransformer.STATE_INSIDE_EXECUTE_QUERY_ELEMENT;
    }

    protected Query getCurrentQuery() {
        return (Query) queries.elementAt( current_query_index );
    }

    protected Query getQuery( int i ) {
        return (Query) queries.elementAt( i );
    }

    private String replaceCharWithString( String in, char c, String with ) {
        Tokenizer tok;
        StringBuffer replaced = null;
        if ( in.indexOf( c ) > -1 ) {
            tok = new Tokenizer( in, c );
            replaced = new StringBuffer();
            while ( tok.hasMoreTokens() ) {
                replaced.append( tok.nextToken() );
                if ( tok.hasMoreTokens() )
                    replaced.append( with );
            }
        }
        if ( replaced != null ) {
            return replaced.toString();
        } else {
            return in;
        }
    }

    /**
     Qualifies an element name by giving it a prefix.
     @param name the element name
     @param prefix the prefix to qualify with
     @return a namespace qualified name that is correct
     */
    protected String nsQualify( String name, String prefix ) {
        if ( name == null || "".equals( name ) ) {
            return name;
        }
        if ( prefix != null && !"".equals( prefix ) ) {
            return new StringBuffer( prefix ).append( ":" ).append( name ).toString();
        } else {
            return name;
        }
    }

    /** END my very own methods **/

    /** BEGIN SAX ContentHandler handlers **/

    public void startPrefixMapping( String prefix, String uri ) throws SAXException {
        if ( uri.equals( my_uri ) ) {
            inPrefix = prefix;
        } else {
            super.contentHandler.startPrefixMapping( prefix, uri );
        }
    }

    public void endPrefixMapping( String prefix ) throws SAXException {
        if ( !prefix.equals( inPrefix ) ) {
            super.contentHandler.endPrefixMapping( prefix );
        }
    }


    public void setDocumentLocator( Locator locator ) {
        if (getLogger().isDebugEnabled()) {
            getLogger().debug( "PUBLIC ID: " + locator.getPublicId() );
            getLogger().debug( "SYSTEM ID: " + locator.getSystemId() );
        }
        if ( super.contentHandler != null )
            super.contentHandler.setDocumentLocator( locator );
    }

    public void startElement( String uri, String name, String raw,
                              Attributes attributes ) throws SAXException {
        if ( !uri.equals( my_uri ) ) {
            super.startElement( uri, name, raw, attributes );
            return;
        }
        getLogger().debug( "RECEIVED START ELEMENT " + name );

        if ( name.equals( SQLTransformer.MAGIC_EXECUTE_QUERY ) ) {
            startExecuteQueryElement();
        } else if ( name.equals( SQLTransformer.MAGIC_QUERY ) ) {
            startQueryElement( attributes );
        } else if ( name.equals( SQLTransformer.MAGIC_ANCESTOR_VALUE ) ) {
            startAncestorValueElement( attributes );
        } else if ( name.equals( SQLTransformer.MAGIC_SUBSTITUTE_VALUE ) ) {
            startSubstituteValueElement( attributes );
        } else if ( name.equals( SQLTransformer.MAGIC_IN_PARAMETER ) ) {
            startInParameterElement( attributes );
        } else if ( name.equals( SQLTransformer.MAGIC_OUT_PARAMETER ) ) {
            startOutParameterElement( attributes );
        } else if ( name.equals( SQLTransformer.MAGIC_ESCAPE_STRING ) ) {
            startEscapeStringElement( attributes );
        } else {
            startValueElement( name );
        }
    }

    public void endElement( String uri, String name,
                            String raw ) throws SAXException {
        if ( !uri.equals( my_uri ) ) {
            super.endElement( uri, name, raw );
            return;
        }
        getLogger().debug( "RECEIVED END ELEMENT " + name + "(" + uri + ")" );

        if ( name.equals( SQLTransformer.MAGIC_EXECUTE_QUERY ) ) {
            endExecuteQueryElement();
        } else if ( name.equals( SQLTransformer.MAGIC_QUERY ) ) {
            endQueryElement();
        } else if ( name.equals( SQLTransformer.MAGIC_ANCESTOR_VALUE ) ) {
            endAncestorValueElement();
        } else if ( name.equals( SQLTransformer.MAGIC_SUBSTITUTE_VALUE ) ) {
            endSubstituteValueElement();
        } else if ( name.equals( SQLTransformer.MAGIC_IN_PARAMETER ) ) {
            endInParameterElement();
        } else if ( name.equals( SQLTransformer.MAGIC_OUT_PARAMETER ) ) {
            endOutParameterElement();
        } else if ( name.equals( SQLTransformer.MAGIC_VALUE ) || current_state == SQLTransformer.STATE_INSIDE_VALUE_ELEMENT ) {
            endValueElement();
        } else  if ( name.equals( SQLTransformer.MAGIC_ESCAPE_STRING ) ) {
            endEscapeStringElement();
        } else {
            super.endElement( uri, name, raw );
        }
    }

    public void characters( char ary[], int start,
                            int length ) throws SAXException {
        if ( current_state != SQLTransformer.STATE_INSIDE_VALUE_ELEMENT &&
             current_state != SQLTransformer.STATE_INSIDE_QUERY_ELEMENT &&
             current_state != SQLTransformer.STATE_INSIDE_ESCAPE_STRING ) {
            super.characters( ary, start, length );
        }
        getLogger().debug( "RECEIVED CHARACTERS: " +
                           new String( ary, start, length ) );
        current_value.append( ary, start, length );
    }

    private void attribute( AttributesImpl attr, String name, String value ) {
        attr.addAttribute( outUri, name, nsQualify( name, outPrefix ), "CDATA", value );
    }

    private void start( String name, AttributesImpl attr ) throws SAXException {
        super.contentHandler.startElement( outUri, name, nsQualify( name, outPrefix ), attr );
        attr.clear();
    }

    private void end( String name ) throws SAXException {
        super.contentHandler.endElement( outUri, name, nsQualify( name, outPrefix ) );
    }

    private void data( String data ) throws SAXException {
        if ( data != null ) {
            super.contentHandler.characters( data.toCharArray(), 0, data.length() );
        }
    }

    protected static String getStringValue( Object object ) {
        if ( object instanceof byte[] ) {
            return new String( (byte[]) object );
        } else if ( object instanceof char[] ) {
            return new String( (char[]) object );
        } else if ( object != null ) {
            return object.toString();
        } else {
            return "";
        }
    }

    public final Logger getTheLogger() {
        return getLogger();
    }


    class Query {

        /** Who's your daddy? **/
        protected SQLTransformer transformer;

        /** What index are you in daddy's queries list **/
        protected int query_index;

        /** SQL configuration information **/
        protected Properties properties;

        /** Dummy static variables for the moment **/
        protected String rowset_name = "rowset";
        protected String nr_of_rows = "nrofrows";
        protected String row_name = "row";
        protected String name_attribute = "name";

        /** The connection, once opened **/
        protected Connection conn;

        /** And the statements **/
        protected PreparedStatement pst;
        protected CallableStatement cst;

        /** The results, of course **/
        protected ResultSet rs = null;

        /** And the results' metadata **/
        protected ResultSetMetaData md = null;

        /** If this query is actually an update (insert, update, delete) **/
        protected boolean isupdate = false;

        /** If this query is actually a stored procedure **/
        protected boolean isstoredprocedure = false;

        protected String name = null;

        /** If it is an update/etc, the return value (num rows modified) **/
        protected int rv = -1;

        /** The parts of the query **/
        protected Vector query_parts = new Vector();

        /** In parameters **/
        protected HashMap inParameters = null;

        /** Out parameters **/
        protected HashMap outParameters = null;

        /** Mapping out parameters - objectModel **/
        protected HashMap outParametersNames = null;

        protected Query( SQLTransformer transformer, int query_index ) {
            this.transformer = transformer;
            this.query_index = query_index;
            this.properties = new Properties( transformer.default_properties );
        }

        protected void setParameter( String name, String value ) {
            properties.setProperty( name, value );
        }

        protected void setUpdate( boolean flag ) {
            isupdate = flag;
        }

        protected void setStoredProcedure( boolean flag ) {
            isstoredprocedure = flag;
        }

        protected boolean isStoredProcedure() {
            return isstoredprocedure;
        }

        protected void setName( String name ) {
            this.name = name;
        }

        protected String getName() {
            return name;
        }

        protected void setInParameter( int pos, String val ) {
            if ( inParameters == null ) {
                inParameters = new HashMap();
            }
            inParameters.put( new Integer( pos ), val );
        }

        protected void setOutParameter( int pos, String type, String name ) {
            if ( outParameters == null ) {
                outParameters = new HashMap();
                outParametersNames = new HashMap();
            }
            outParameters.put( new Integer( pos ), type );
            outParametersNames.put( new Integer( pos ), name );
        }

        private void registerInParameters( PreparedStatement pst ) throws SQLException {
            if ( inParameters == null )
                return;
            Iterator itInKeys = inParameters.keySet().iterator();
            Integer counter;
            String value;
            while ( itInKeys.hasNext() ) {
                counter = (Integer) itInKeys.next();
                value = (String) inParameters.get( counter );
                try {
                    pst.setObject( counter.intValue(), value );
                } catch ( SQLException e ) {
                    transformer.getTheLogger().error( "Caught a SQLException", e );
                    throw e;
                }
            }
        }

        private void registerOutParameters( CallableStatement cst ) throws SQLException {
            if ( outParameters == null )
                return;
            Iterator itOutKeys = outParameters.keySet().iterator();
            Integer counter;
            int index;
            String type, className, fieldName;
            Class clss;
            Field fld;
            while ( itOutKeys.hasNext() ) {
                counter = (Integer) itOutKeys.next();
                type = (String) outParameters.get( counter );
                index = type.lastIndexOf( "." );
                if ( index > -1 ) {
                    className = type.substring( 0, index );
                    fieldName = type.substring( index + 1, type.length() );
                } else {
                    transformer.getTheLogger().error( "Invalid SQLType: " + type, null );
                    throw new SQLException( "Wrong SQLType" );
                }
                try {
                    clss = Class.forName( className );
                    fld = clss.getField( fieldName );
                    cst.registerOutParameter( counter.intValue(), fld.getInt( fieldName ) );
                } catch ( Exception e ) {
                    //lots of different exceptions to catch
                    transformer.getTheLogger().error( "Invalid SQLType: " +
                                                      className + "." + fieldName, e );
                }
            }
        }

        /** Get a Connection. Made this a separate method to separate the logic from the actual execution. */
        protected Connection getConnection() throws SQLException {
            Connection result = null;

            String connection = properties.getProperty( SQLTransformer.MAGIC_CONNECTION );
            String dburl = properties.getProperty( SQLTransformer.MAGIC_DBURL );
            String username = properties.getProperty( SQLTransformer.MAGIC_USERNAME );
            String password = properties.getProperty( SQLTransformer.MAGIC_PASSWORD );

            DataSourceComponent datasource = null;
            try {
                if ( connection != null ) {
                    datasource = (DataSourceComponent) dbSelector.select( connection );
                    while ( result == null ) {
                        try {
                            result = datasource.getConnection();
                        } catch ( Exception e ) {
                            long waittime = 5000 * (long) Math.random();
                            getTheLogger().debug( "SQLTransformer$Query: could not acquire a Connection -- waiting " + waittime + " ms to try again." );
                            try {
                                Thread.sleep( waittime );
                            } catch ( InterruptedException ie ) {
                            }
                        }
                    }
                } else {
                    if ( username == null || password == null ) {
                        result = DriverManager.getConnection( dburl );
                    } else {
                        result = DriverManager.getConnection( dburl, username,
                                                              password );
                    }
                }
            } catch ( ComponentException cme ) {
                transformer.getTheLogger().error( "Could not use connection: " + connection, cme );
            } catch ( SQLException e ) {
                transformer.getTheLogger().error( "Caught a SQLException", e );
                throw e;
            } finally {
                if ( datasource != null ) dbSelector.release( datasource );
            }

            return result;
        }

        protected void execute() throws SQLException {
            if ( null != properties.getProperty( SQLTransformer.MAGIC_DOC_ELEMENT ) ) {
                this.rowset_name = properties.getProperty( SQLTransformer.MAGIC_DOC_ELEMENT );
            }
            if ( null != properties.getProperty( SQLTransformer.MAGIC_ROW_ELEMENT ) ) {
                this.row_name = properties.getProperty( SQLTransformer.MAGIC_ROW_ELEMENT );
            }


            Enumeration enum = query_parts.elements();
            StringBuffer sb = new StringBuffer();
            while ( enum.hasMoreElements() ) {
                Object object = enum.nextElement();
                if ( object instanceof String ) {
                    sb.append( (String) object );
                } else if ( object instanceof AncestorValue ) {
                    /** Do a lookup into the ancestors' result's values **/
                    AncestorValue av = (AncestorValue) object;
                    Query query = transformer.getQuery( query_index - av.level );
                    sb.append( query.getColumnValue( av.name ) );
                }
            }
            String query = sb.toString();
            transformer.getTheLogger().debug( "EXECUTING " + query );

            conn = getConnection();

            try {
                if ( !isstoredprocedure ) {
                    if ( oldDriver ) {
                        pst = conn.prepareStatement( query );
                    } else {
                        pst = conn.prepareStatement( query,
                                                     ResultSet.TYPE_SCROLL_INSENSITIVE,
                                                     ResultSet.CONCUR_READ_ONLY );
                    }
                } else {
                    if ( oldDriver ) {
                        cst = conn.prepareCall( query );
                    } else {
                        cst = conn.prepareCall( query,
                                                ResultSet.TYPE_SCROLL_INSENSITIVE,
                                                ResultSet.CONCUR_READ_ONLY );
                    }
                    registerOutParameters( cst );
                    pst = cst;
                }

                registerInParameters( pst );
                boolean result = pst.execute();
                if ( result ) {
                    rs = pst.getResultSet();
                    md = rs.getMetaData();
                } else {
                    rv = pst.getUpdateCount();
                }
            } catch ( SQLException e ) {
                transformer.getTheLogger().error( "Caught a SQLException", e );
                throw e;
            } finally {
                conn.close();
                conn = null;        // To make sure we don't use this connection again.
            }
        }

        protected int getNrOfRows() throws SQLException {
            int nr = 0;
            if ( rs != null ) {
                if ( oldDriver ) {
                    nr = -1;
                } else {
                    try {
                        rs.last();
                        nr = rs.getRow();
                        rs.beforeFirst();
                    } catch ( NullPointerException e ) {
                        // A NullPointerException here crashes a whole lot of C2 -- catching it so it won't do any harm for now, but seems like it should be solved seriously
                        getTheLogger().error( "NPE while getting the nr of rows", e );
                    }
                }
            } else {
                if ( outParameters != null ) {
                    nr = outParameters.size();
                }
            }
            return nr;
        }

        protected String getColumnValue( int i ) throws SQLException {
            return transformer.getStringValue( rs.getObject( i ) );
        }

        protected String getColumnValue( String name ) throws SQLException {
            return transformer.getStringValue( rs.getObject( name ) );
        }

        protected boolean next() throws SQLException {
            // if rv is not -1, then an SQL insert, update, etc, has
            // happened (see JDBC docs - return codes for executeUpdate)
            if ( rv != -1 )
                return true;
            try {
                if ( rs == null || !rs.next() ) {
                    //close();
                    return false;
                }
            } catch ( NullPointerException e ) {
                getTheLogger().debug( "NullPointerException, returning false.", e );
                return false;
            }
            return true;
        }

        protected void close() throws SQLException {
            try {
                if ( rs != null )
                    try {
                        //getTheLogger().debug("Trying to close resultset "+rs.toString());
                        rs.close();
                        rs = null;      // This prevents us from using the resultset again.
                        //250getTheLogger().debug("Really closed the resultset now.");
                    } catch ( NullPointerException e ) {
                        getTheLogger().debug( "NullPointer while closing the resultset.", e );
                    }
                if ( pst != null )
                    pst.close();
                pst = null;        // Prevent using pst again.
                if ( cst != null )
                    cst.close();
                cst = null;        // Prevent using cst again.
            } finally {
                if ( conn != null )
                    conn.close();
                conn = null;
            }
        }

        protected void addQueryPart( Object object ) {
            query_parts.addElement( object );
        }

        protected void serializeRow() throws SQLException, SAXException {
            AttributesImpl attr = new AttributesImpl();
            if ( !isupdate && !isstoredprocedure ) {
                for ( int i = 1; i <= md.getColumnCount(); i++ ) {
                    transformer.start( md.getColumnName( i ).toLowerCase(), attr );
                    transformer.data( getColumnValue( i ) );
                    transformer.end( md.getColumnName( i ).toLowerCase() );
                }
            } else if ( isupdate && !isstoredprocedure ) {
                transformer.start( "returncode", attr );
                transformer.data( String.valueOf( rv ) );
                transformer.end( "returncode" );
                rv = -1; // we only want the return code shown once.
            }
        }

        protected void serializeStoredProcedure() throws SQLException, SAXException {
            if ( outParametersNames == null || cst == null )
                return;
            //make sure output follows order as parameter order in stored procedure
            Iterator itOutKeys = ( new TreeMap( outParameters ) ).keySet().iterator();
            Integer counter;
            AttributesImpl attr = new AttributesImpl();
            try {
                while ( itOutKeys.hasNext() ) {
                    counter = (Integer) itOutKeys.next();
                    try {
                        if ( cst == null ) getTheLogger().debug( "SQLTransformer: cst is null" );
                        if ( counter == null ) getTheLogger().debug( " SQLTransformer: counter is null" );
                        Object obj = cst.getObject( counter.intValue() );
                        if ( !( obj instanceof ResultSet ) ) {
                            transformer.start( (String) outParametersNames.get( counter ), attr );
                            transformer.data( transformer.getStringValue( obj ) );
                            transformer.end( (String) outParametersNames.get( counter ) );
                        } else {
                            ResultSet rs = (ResultSet) obj;
                            try {
                                transformer.start( (String) outParametersNames.get( counter ), attr );
                                ResultSetMetaData md = rs.getMetaData();
                                while ( rs.next() ) {
                                    transformer.start( this.row_name, attr );
                                    for ( int i = 1; i <= md.getColumnCount(); i++ ) {
                                        transformer.start( md.getColumnName( i ).toLowerCase(), attr );
                                        transformer.data( transformer.getStringValue( rs.getObject( i ) ) );
                                        transformer.end( md.getColumnName( i ).toLowerCase() );
                                    }
                                    transformer.end( this.row_name );
                                }
                            } finally {
                                rs.close();
                                rs = null;
                            }
                            transformer.end( (String) outParametersNames.get( counter ) );
                        }
                    } catch ( SQLException e ) {
                        transformer.getTheLogger().error( "Caught a SQLException", e );
                        throw e;
                    }
                }
            } finally {
                //close();
            }
        }
    }

    private class AncestorValue {
        protected int level;
        protected String name;

        protected AncestorValue( int level, String name ) {
            this.level = level;
            this.name = name;
        }
    }

}
