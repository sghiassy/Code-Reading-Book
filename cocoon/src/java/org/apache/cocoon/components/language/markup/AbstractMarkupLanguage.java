/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup;

import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.excalibur.pool.Recyclable;

import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.components.language.programming.ProgrammingLanguage;
import org.apache.cocoon.components.store.Store;
import org.apache.cocoon.components.url.URLFactory;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.environment.URLFactorySourceResolver;

import org.xml.sax.helpers.XMLFilterImpl;
import org.xml.sax.helpers.XMLReaderFactory;
import org.xml.sax.SAXException;
import org.xml.sax.XMLFilter;
import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;
import org.xml.sax.Attributes;

import java.io.IOException;
import java.net.MalformedURLException;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.ListIterator;
import java.util.Iterator;
import java.util.Map;
import java.util.List;
import java.util.ArrayList;

/**
 * Base implementation of <code>MarkupLanguage</code>. This class uses
 * logicsheets as the only means of code generation. Code generation
 * should be decoupled from this context!!!
 *
 * @author <a href="mailto:ricardo@apache.org">Ricardo Rocha</a>
 * @author <a href="mailto:dims@yahoo.com">Davanum Srinivas</a>
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @version CVS $Revision: 1.6 $ $Date: 2002/01/29 15:46:08 $
 */
public abstract class AbstractMarkupLanguage extends AbstractLoggable
    implements MarkupLanguage, Composable, Configurable, Recyclable, Disposable
{
    /** The 'file' URL protocol. */
    private static final String FILE = "file:";
  
    /** Prefix for cache keys to avoid name clash with the XSLTProcessor */
    private static final String CACHE_PREFIX = "logicsheet:";

    /** The supported language table */
    protected Hashtable languages;

    /** The code-generation logicsheet cache */
    protected Store logicsheetCache;

    /** The markup language's namespace uri */
    protected String uri;

    /** The markup language's namespace prefix */
    protected String prefix;

    /** The component manager */
    protected ComponentManager manager;
    
    /** The URL factory to create source resolver */
    private URLFactory urlFactory;

    /** The URL factory source resolver used to resolve URIs */
    private URLFactorySourceResolver urlResolver;

    private final LinkedList logicSheetList = new LinkedList();


    /** The default constructor. */
    public AbstractMarkupLanguage() throws SAXException, IOException {
        // Initialize language table
        this.languages = new Hashtable();
    }

    /**
     * Process additional configuration. Load supported programming
     * language definitions
     *
     * @param conf The language configuration
     * @exception ConfigurationException If an error occurs loading logichseets
     */
    public void configure(Configuration conf) throws ConfigurationException {
        try {
            // Set up each target-language
            Configuration[] l = conf.getChildren("target-language");
            for (int i = 0; i < l.length; i++) {
                LanguageDescriptor language = new LanguageDescriptor();
                language.setName(l[i].getAttribute("name"));

                // Create & Store the core logicsheet
                Logicsheet logicsheet = createLogicsheet(l[i], false);
                language.setLogicsheet(logicsheet.getSystemId());

                // Set up each built-in logicsheet
                Configuration[] n = l[i].getChildren("builtin-logicsheet");
                for (int j = 0; j < n.length; j++) {
                    // Create & Store the named logicsheets
                    NamedLogicsheet namedLogicsheet =
                        (NamedLogicsheet) createLogicsheet(n[j], true);

                    // FIXME: Logicsheets should be found by uri--not prefix.
                    language.addNamedLogicsheet(
                        namedLogicsheet.getPrefix(),
                        namedLogicsheet.getSystemId());
                }

                this.languages.put(language.getName(), language);
            }
        } catch (Exception e) {
          getLogger().warn("Configuration Error: " + e.getMessage(), e);
          throw new ConfigurationException("AbstractMarkupLanguage: "
                                           + e.getMessage(), e);
        }
    }

    /**
     * Abstract out the Logicsheet creation.  Handles both Named and regular logicsheets.
     */
    private Logicsheet createLogicsheet(Configuration configuration, boolean named)
            throws Exception
    {
        Parameters params = Parameters.fromConfiguration(configuration);

        Logicsheet logicsheet;
        if (named) {
            String location = params.getParameter("href", null);
            String prefix = params.getParameter("prefix", null);

            NamedLogicsheet namedLogicsheet =
                new NamedLogicsheet(location, manager, urlResolver);
            namedLogicsheet.setLogger(getLogger());
            namedLogicsheet.setPrefix(prefix);
            logicsheet = namedLogicsheet;
        } else {
            String location = params.getParameter("core-logicsheet", null);
            logicsheet = new Logicsheet(location, manager, urlResolver);
            logicsheet.setLogger(getLogger());
        }

        String logicsheetName = logicsheet.getSystemId();
        logicsheetCache.store(CACHE_PREFIX + logicsheetName, logicsheet);

        return logicsheet;
    }

    /**
     * Set the global component manager.
     * @param manager The sitemap-specified component manager
     */
    public void compose(ComponentManager manager) throws ComponentException {
        this.manager = manager;

        // Initialize logicsheet cache
        this.logicsheetCache = (Store) manager.lookup(Store.ROLE);

        // Initialize the URL factory source resolver
        this.urlFactory = (URLFactory)this.manager.lookup(URLFactory.ROLE);
        this.urlResolver = new URLFactorySourceResolver(this.urlFactory, manager);
    }

    public void recycle()
    {
        this.logicSheetList.clear();
    }

    public void dispose() {
        if (this.logicsheetCache != null)
            this.manager.release(this.logicsheetCache);
        this.logicsheetCache = null;

        if (this.urlFactory != null)
            this.manager.release(this.urlFactory);
        this.urlFactory = null;
        this.urlResolver = null;
        this.manager = null;
        this.languages.clear();
    }

    /**
     * Initialize the (required) markup language namespace definition.
     * @param params The sitemap-supplied parameters
     * @exception Exception Not actually thrown
     */
    protected void setParameters(Parameters params) throws Exception {
        this.uri = params.getParameter("uri", null);
        this.prefix = params.getParameter("prefix", null);
    }


    /**
     * Return the source document's encoding. This can be <code>null</code> for
     * the platform's default encoding. The default implementation returns
     * <code>null</code>, but derived classes may override it if encoding applies to
     * their concrete languages.
     * FIXME: There should be a way to get the
     * XML document's encoding as seen by the parser; unfortunately, this
     * information is not returned by current DOM or SAX parsers...
     * @return The document-specified encoding
     */
    public String getEncoding() {
        return null;
    }
    
    /**
     * Returns a filter that chains on the fly the requested
     * transformers for source code generation. This method scans the
     * input SAX events for built-in logicsheet declared as namespace
     * attribute on the root element. Derived class should overide
     * this method and the public inner class in order to add more
     * specif action and to build a more specific transformer chain.
     *
     * @param logicsheetMarkupGenerator the logicsheet markup generator
     * @param resolver the entity resolver
     * @return XMLFilter the filter that build on the fly the transformer chain
     */
    protected TransformerChainBuilderFilter getTransformerChainBuilder(
        LogicsheetCodeGenerator logicsheetMarkupGenerator,
        SourceResolver resolver)
    {
        return new TransformerChainBuilderFilter(logicsheetMarkupGenerator,
                                                 resolver);
    }

    /**
     * Prepare the input source for logicsheet processing and code
     * generation with a preprocess filter.  The return
     * <code>XMLFilter</code> object is the first filter on the
     * transformer chain.  The default implementation does nothing by
     * returning a identity filter, but derived classes should (at
     * least) use the passed programming language to quote
     * <code>Strings</code>
     *
     * @param filename The source filename
     * @param language The target programming language
     * @return The preprocess filter
     */
    protected XMLFilter getPreprocessFilter(String filename,
                                            ProgrammingLanguage language)
    {
        return new XMLFilterImpl();
    }

    /**
     * Add a dependency on an external file to the document for inclusion in
     * generated code. This is used to populate a list of <code>File</code>'s
     * tested for change on each invocation; this information is used to assert whether regeneration is necessary.
     * @param location The file path of the dependent file
     * @see <code>AbstractMarkupLanguage</code>, <code>ServerPagesGenerator</code> and <code>AbstractServerPage</code>
     */
    protected abstract void addDependency(String location);

    /**
     * Generate source code from the input document for the target
     * <code>ProgrammingLanguage</code>. After preprocessing the input
     * document, this method applies logicsheets in the following
     * order:
     *
     * <ul>
     * <li>User-defined logicsheets</li>
     * <li>Namespace-mapped logicsheets</li>
     * <li>Language-specific logicsheet</li>
     * </ul>
     *
     * @param input The input source
     * @param filename The input document's original filename
     * @param programmingLanguage The target programming language
     * @return The generated source code
     * @exception Exception If an error occurs during code generation
     */
    public String generateCode(InputSource input, String filename, ProgrammingLanguage programmingLanguage,
                               SourceResolver resolver) throws Exception {
        String languageName = programmingLanguage.getLanguageName();
        LanguageDescriptor language = (LanguageDescriptor)this.languages.get(languageName);
        if (language == null) {
            throw new IllegalArgumentException("Unsupported programming language: " + languageName);
        }
        // Create a XMLReader
        XMLReader reader = XMLReaderFactory.createXMLReader();
        // Get the needed preprocess filter
        XMLFilter preprocessFilter = this.getPreprocessFilter(filename, programmingLanguage);
        preprocessFilter.setParent(reader);
        // Create code generator
        LogicsheetCodeGenerator codeGenerator = new LogicsheetCodeGenerator();
        codeGenerator.setLogger(getLogger());
        codeGenerator.initialize();
        // set the transformer chain builder filter
        TransformerChainBuilderFilter tranBuilder = 
            getTransformerChainBuilder(codeGenerator, resolver);
        tranBuilder.setLanguageDescriptor(language);
        tranBuilder.setParent(preprocessFilter);
        return codeGenerator.generateCode(tranBuilder, input, filename);
    }

    /**
     * Add logicsheet list to the code generator.
     * @param codeGenerator The code generator
     */
    protected void addLogicsheetsToGenerator(LogicsheetCodeGenerator codeGenerator)
        throws MalformedURLException, IOException, SAXException, ProcessingException {

        if (codeGenerator == null) {
            getLogger().debug("This should never happen: codeGenerator is null");
            throw new SAXException("codeGenerator must never be null.");
        }

        // Walk backwards and remove duplicates.
        LinkedList newLogicSheetList = new LinkedList();
        for(int i = logicSheetList.size()-1; i>=0; i--) {
            Logicsheet logicsheet = (Logicsheet) logicSheetList.get(i);
            if(newLogicSheetList.indexOf(logicsheet) == -1)
                newLogicSheetList.addFirst(logicsheet);
        }

        // Add the list of logicsheets now.
        Iterator iterator = newLogicSheetList.iterator();
        while(iterator.hasNext()) {
            Logicsheet logicsheet = (Logicsheet) iterator.next();
            codeGenerator.addLogicsheet(logicsheet);
        }
    }

    /**
     * Add a logicsheet to the code generator.
     * @param logicsheetLocation Location of the logicsheet to be added
     * @param document The input document
     * @exception MalformedURLException If location is invalid
     * @exception IOException IO Error
     * @exception SAXException Logicsheet parse error
     */
    protected void addLogicsheetToList(LanguageDescriptor language,
                                       String logicsheetLocation,
                                       SourceResolver resolver)
        throws IOException, SAXException, ProcessingException
    {
        Logicsheet logicsheet = (Logicsheet)logicsheetCache.get(CACHE_PREFIX + logicsheetLocation);
        if (logicsheet == null) {
            // Logicsheet is reusable (across multiple XSPs) object,
            // and it is resolved via urlResolver, and not via per-request 
            // temporary resolver. 
            Source inputSource = this.urlResolver.resolve(logicsheetLocation);

            // Resolver (local) could not be used as it is temporary
            // (per-request) object, yet Logicsheet is being cached and reused
            // across multiple requests. "Global" url-factory-based resolver
            // passed to the Logicsheet.
            logicsheet = new Logicsheet(inputSource, manager, this.urlResolver);
            logicsheetCache.store(CACHE_PREFIX + logicsheet.getSystemId(), logicsheet);
        }
        String logicsheetName = logicsheet.getSystemId();

        if (getLogger().isDebugEnabled()) {
            getLogger().debug("addLogicsheetToList: "
                + "name: " + logicsheetName
                + ", location: " + logicsheetLocation
                + ", instance: " + logicsheet);
        }

        if (logicsheetName.startsWith(FILE)) {
            String filename = logicsheetName.substring(FILE.length());
            addDependency(filename);
            getLogger().debug("addLogicsheetToList: "
                + "adding dependency on file " + filename);
        }

        logicSheetList.add(logicsheet);

        Map namespaces = logicsheet.getNamespaces();
        if(!logicsheetLocation.equals(language.getLogicsheet())) {
            if(namespaces != null && namespaces.size() > 0) {
                Iterator iter = namespaces.keySet().iterator();
                while(iter.hasNext()) {
                    String namespace = (String) iter.next();
                    String namedLogicsheetName = language.getNamedLogicsheet(namespace);
                    if(namedLogicsheetName!= null
                        && !logicsheetLocation.equals(namedLogicsheetName)) {
                        getLogger().debug("Adding embedded logic sheet for "
                            + namespace + ":" + namedLogicsheetName);
                        // Add embedded logic sheets too.
                        addLogicsheetToList(language, namedLogicsheetName, resolver);
                    }
                }
            }
        }
    }

    //
    // Inner classes
    //

    /** This class holds transient information about a target programming language. */
    protected class LanguageDescriptor {
        /** The progamming language name */
        protected String name;

        /** The progamming language core logicsheet */
        protected String logicsheet;

        /** The list of built-in logicsheets defined for this target language */
        protected Hashtable namedLogicsheets;

        /** The default constructor */
        protected LanguageDescriptor() {
            this.namedLogicsheets = new Hashtable();
        }

        /**
         * Set the programming language's name
         * @param name The programming language's name
         */
        protected void setName(String name) {
            this.name = name;
        }

        /**
         * Return the programming language's name
         * @return The programming language's name
         */
        protected String getName() {
            return this.name;
        }

        /**
         * Set the programming language's core logichseet location
         * @param logicsheet The programming language's core logichseet location
         */
        protected void setLogicsheet(String logicsheet) {
            this.logicsheet = logicsheet;
        }

        /**
         * Return the programming language's core logichseet location
         * @return The programming language's core logichseet location
         */
        protected String getLogicsheet() {
            return this.logicsheet;
        }

        /**
         * Add a namespace-mapped logicsheet to this language
         * @param prefix The logichseet's namespace prefix
         * @param uri The logichseet's namespace uri
         * @param namedLogicsheet The logichseet's location
         */
        protected void addNamedLogicsheet(String prefix, String namedLogicsheet) {
            this.namedLogicsheets.put(prefix, namedLogicsheet);
        }

        /**
         * Return a namespace-mapped logicsheet given its name
         * @return The namespace-mapped logicsheet
         */
        protected String getNamedLogicsheet(String prefix) {
            return (String)this.namedLogicsheets.get(prefix);
        }
    }


    /**
     * An XMLFilter that build the chain of transformers on the fly.
     * Each time a stylesheet is found, a call to the code generator is done
     * to add the new transformer at the end of the current transformer chain.
     */
    public class TransformerChainBuilderFilter extends XMLFilterImpl {
        /** The markup generator */
        protected LogicsheetCodeGenerator logicsheetMarkupGenerator;

        /** the language description */
        protected LanguageDescriptor language;

        /** the entity resolver */
        protected SourceResolver resolver;
        private boolean isRootElem;
        private List startPrefixes;

        /**
         * the constructor depends on the code generator, and the entity resolver
         * @param logicsheetMarkupGenerator The code generator
         * @param resolver
         */
        protected TransformerChainBuilderFilter(LogicsheetCodeGenerator logicsheetMarkupGenerator, SourceResolver resolver) {
            this.logicsheetMarkupGenerator = logicsheetMarkupGenerator;
            this.resolver = resolver;
        }

        /**
         * This method should be called prior to receiving any SAX event.
         * Indeed the language information is needed to get the core stylesheet.
         * @language the language in used
         */
        protected void setLanguageDescriptor(LanguageDescriptor language) {
            this.language = language;
        }

        /** @see org.xml.sax.ContentHandler */
        public void startDocument() throws SAXException {
            isRootElem = true;
            startPrefixes = new ArrayList();
        }

        /** @see org.xml.sax.ContentHandler */
        public void startPrefixMapping(String prefix, String uri) throws SAXException {
            if (!isRootElem) {
                super.startPrefixMapping(prefix, uri);
            } else {
                // Cache the prefix mapping
                String[] prefixNamingArray = new String[2];
                prefixNamingArray[0] = prefix;
                prefixNamingArray[1] = uri;
                this.startPrefixes.add(prefixNamingArray);
            }
        }

        /** @see org.xml.sax.ContentHandler */
        public void startElement(String namespaceURI, String localName, String qName, Attributes atts) throws SAXException {
            if (isRootElem) {
                isRootElem = false;
                try {
                    // Add namespace-mapped logicsheets
                    int prefixesCount = this.startPrefixes.size();
                    for (int i = 0; i < prefixesCount; i++) {
                        String[] prefixNamingArray = (String[]) this.startPrefixes.get(i);
                        String namedLogicsheetName = this.language.getNamedLogicsheet(prefixNamingArray[0]);
                        if (namedLogicsheetName != null) {
                            AbstractMarkupLanguage.this.addLogicsheetToList(language, namedLogicsheetName, resolver);
                        }
                    }

                    // Add the language stylesheet (Always the last one)
                    AbstractMarkupLanguage.this.addLogicsheetToList(language, this.language.getLogicsheet(), resolver);
                    AbstractMarkupLanguage.this.addLogicsheetsToGenerator(this.logicsheetMarkupGenerator);
                } catch (ProcessingException pe) {
                    throw new SAXException (pe);
                } catch (IOException ioe) {
                    throw new SAXException(ioe);
                }

                // All stylesheet have been configured and correctly setup.
                // Starts firing SAX events, especially the startDocument event,
                // and the cached prefixNaming.
                super.startDocument();
                int prefixesCount = this.startPrefixes.size();
                for (int i = 0; i < prefixesCount; i++) {
                    String[] prefixNamingArray = (String[]) this.startPrefixes.get(i);
                    super.startPrefixMapping(prefixNamingArray[0], prefixNamingArray[1]);
                }
            }
            // Call super method
            super.startElement(namespaceURI, localName, qName, atts);
        }
    }
}
