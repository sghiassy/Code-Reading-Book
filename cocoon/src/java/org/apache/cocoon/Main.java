/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 2001 The Apache Software Foundation.  All rights
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
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Axis" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
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
 */

package org.apache.cocoon;

import org.apache.avalon.excalibur.cli.CLArgsParser;
import org.apache.avalon.excalibur.cli.CLOption;
import org.apache.avalon.excalibur.cli.CLOptionDescriptor;
import org.apache.avalon.excalibur.cli.CLUtil;
import org.apache.avalon.excalibur.logger.DefaultLogKitManager;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.DefaultConfigurationBuilder;
import org.apache.avalon.framework.context.DefaultContext;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.commandline.CommandlineContext;
import org.apache.cocoon.environment.commandline.FileSavingEnvironment;
import org.apache.cocoon.environment.commandline.LinkSamplingEnvironment;
import org.apache.cocoon.util.IOUtils;
import org.apache.cocoon.util.MIMEUtils;
import org.apache.cocoon.util.NetUtils;
import org.apache.log.Hierarchy;
import org.apache.log.Logger;
import org.apache.log.Priority;

import java.io.BufferedReader;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * Command line entry point.
 *
 * @author <a href="mailto:stefano@apache.org">Stefano Mazzocchi</a>
 * @version CVS $Revision: 1.12 $ $Date: 2002/01/22 00:17:11 $
 */
public class Main {

    /** The <code>Logger</code> */
    private static Logger log = null;

    protected static final int HELP_OPT =         'h';
    protected static final int VERSION_OPT =      'v';
    protected static final int LOG_KIT_OPT =      'k';
    protected static final int LOGGER_OPT =       'l';
    protected static final int LOG_LEVEL_OPT =    'u';
    protected static final int CONTEXT_DIR_OPT =  'c';
    protected static final int DEST_DIR_OPT =     'd';
    protected static final int WORK_DIR_OPT =     'w';
    protected static final int XSP_OPT =          'x';
    protected static final int AGENT_OPT =        'a';
    protected static final int ACCEPT_OPT =       'p';
    protected static final int URI_FILE =         'f';
    protected static final int FOLLOW_LINKS_OPT = 'r';
    protected static final int CONFIG_FILE =      'C';
    protected static final int BROKEN_LINK_FILE = 'b';

    protected static final String DEFAULT_USER_AGENT = Constants.COMPLETE_NAME;
    protected static final String DEFAULT_ACCEPT = "text/html, */*";

    protected static final CLOptionDescriptor [] OPTIONS = new CLOptionDescriptor [] {
        new CLOptionDescriptor("brokenLinkFile",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               BROKEN_LINK_FILE,
                               "send a list of broken links to a file (1 uri per line)"),
        new CLOptionDescriptor("uriFile",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               URI_FILE,
                               "use a text file with uris to process (1 uri per line)"),
        new CLOptionDescriptor("help",
                               CLOptionDescriptor.ARGUMENT_DISALLOWED,
                               HELP_OPT,
                               "print this message and exit"),
        new CLOptionDescriptor("version",
                               CLOptionDescriptor.ARGUMENT_DISALLOWED,
                               VERSION_OPT,
                               "print the version information and exit"),
        new CLOptionDescriptor("logKitconfig",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               LOG_KIT_OPT,
                               "use given file for LogKit Management configuration"),
        new CLOptionDescriptor("Logger",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               LOGGER_OPT,
                               "use given logger category as default logger for the Cocoon engine"),
        new CLOptionDescriptor("logLevel",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               LOG_LEVEL_OPT,
                               "choose the minimum log level for logging (DEBUG, INFO, WARN, ERROR, FATAL_ERROR) for startup logging"),
        new CLOptionDescriptor("contextDir",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               CONTEXT_DIR_OPT,
                               "use given dir as context"),
        new CLOptionDescriptor("destDir",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               DEST_DIR_OPT,
                               "use given dir as destination"),
        new CLOptionDescriptor("workDir",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               WORK_DIR_OPT,
                               "use given dir as working directory"),
        new CLOptionDescriptor("xspOnly",
                               CLOptionDescriptor.ARGUMENT_DISALLOWED,
                               XSP_OPT,
                               "generate java code for xsp files"),
        new CLOptionDescriptor("userAgent",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               AGENT_OPT,
                               "use given string for user-agent header"),
        new CLOptionDescriptor("accept",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               ACCEPT_OPT,
                               "use given string for accept header"),
        new CLOptionDescriptor("followLinks",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               FOLLOW_LINKS_OPT,
                               "process pages linked from starting page or not"
                               + " (boolean argument is expected, default is true)"),
        new CLOptionDescriptor("configFile",
                               CLOptionDescriptor.ARGUMENT_REQUIRED,
                               CONFIG_FILE,
                               "specify alternate locatin of the configuration"
                               + "file (default is ${contextDir}/cocoon.xconf)")
    };

    /**
     * <code>processFile</code> method.
     *
     * @param filename a <code>String</code> value
     * @param uris a <code>List</code> of URIs
     */
    public static void processFile(String filename, List uris) {
        try {
            BufferedReader uriFile = new BufferedReader(new FileReader(filename));
            boolean eof = false;

            while (!eof) {
                String uri = uriFile.readLine();

                if (null == uri) {
                    eof = true;
                } else {
                    uris.add(NetUtils.normalize(uri.trim()));
                }
            }

            uriFile.close();
        } catch (Exception e) {
            // ignore errors.
        }
    }


    /**
     * The <code>main</code> method.
     *
     * @param args a <code>String[]</code> of arguments
     * @exception Exception if an error occurs
     */
    public static void main(String[] args) throws Exception {

        String destDir = Constants.DEFAULT_DEST_DIR;
        String contextDir = Constants.DEFAULT_CONTEXT_DIR;
        String configFile = null;
        File brokenLinkFile = null;
        String workDir = Constants.DEFAULT_WORK_DIR;
        List targets = new ArrayList();
        CLArgsParser parser = new CLArgsParser(args, OPTIONS);
        String logKit = null;
        String logger = null;
        String logLevel = "DEBUG";
        boolean xspOnly = false;
        String userAgent = DEFAULT_USER_AGENT;
        String accept = DEFAULT_ACCEPT;
        boolean followLinks = true;

        List clOptions = parser.getArguments();
        int size = clOptions.size();

        for (int i = 0; i < size; i++) {
            CLOption option = (CLOption) clOptions.get(i);

            switch (option.getId()) {
                case 0:
                    targets.add(NetUtils.normalize(option.getArgument()));
                    break;

                case Main.CONFIG_FILE:
                     configFile = option.getArgument();
                     break;

                case Main.HELP_OPT:
                    printUsage();
                    break;

                case Main.VERSION_OPT:
                    printVersion();
                    break;

                case Main.DEST_DIR_OPT:
                    destDir = option.getArgument();
                    break;

                case Main.WORK_DIR_OPT:
                    workDir = option.getArgument();
                    break;

                case Main.CONTEXT_DIR_OPT:
                    contextDir = option.getArgument();
                    break;

                case Main.LOG_KIT_OPT:
                    logKit = option.getArgument();
                    break;

                case Main.LOGGER_OPT:
                    logger = option.getArgument();
                    break;

                case Main.LOG_LEVEL_OPT:
                    logLevel = option.getArgument();
                    break;

                case Main.XSP_OPT:
                    xspOnly = true;
                    break;

                case Main.AGENT_OPT:
                    userAgent = option.getArgument();
                    break;

                case Main.ACCEPT_OPT:
                    accept = option.getArgument();
                    break;

                case Main.URI_FILE:
                    Main.processFile(option.getArgument(), targets);
                    break;

                case Main.FOLLOW_LINKS_OPT:
                    followLinks = "yes".equals(option.getArgument())
                        || "true".equals(option.getArgument());
                    break;

                case Main.BROKEN_LINK_FILE:
                    brokenLinkFile = new File(option.getArgument());
                    break;
            }
        }

        final Priority priority = Priority.getPriorityForName(logLevel);
        Hierarchy.getDefaultHierarchy().setDefaultPriority(priority);
        log = Hierarchy.getDefaultHierarchy().getLoggerFor("");

        if (destDir.equals("")) {
            String error = "Careful, you must specify a destination dir when using the -d/--destDir argument";
            log.fatalError(error);
            System.out.println(error);
            System.exit(1);
        }

        if (contextDir.equals("")) {
            String error = "Careful, you must specify a configuration file when using the -c/--contextDir argument";
            log.error(error);
            System.out.println(error);
            System.exit(1);
        }

        if (workDir.equals("")) {
            String error = "Careful, you must specify a destination dir when using the -w/--workDir argument";
            log.error(error);
            System.out.println(error);
            System.exit(1);
        }

        if (targets.size() == 0 && !xspOnly) {
            String error = "Please, specify at least one starting URI.";
            log.error(error);
            System.out.println(error);
            System.exit(1);
        }

        try {
            File dest = null;
            if (!xspOnly) {
                dest = getDir(destDir, "destination");
            }
            File work = getDir(workDir, "working");
            File context = getDir(contextDir, "context");
            File conf = null;

            if ( null == configFile ) {
                conf = getConfigurationFile(context);
            } else {
                conf = new File(context, configFile);
            }

            DefaultContext appContext = new DefaultContext();
            appContext.put(Constants.CONTEXT_CLASS_LOADER, Main.class.getClassLoader());
            CommandlineContext clContext = new CommandlineContext(contextDir);
            clContext.setLogger(log);
            appContext.put(Constants.CONTEXT_ENVIRONMENT_CONTEXT, clContext);
            DefaultLogKitManager logKitManager = null;
            if (logKit != null) {
                final FileInputStream fis = new FileInputStream(logKit);
                final DefaultConfigurationBuilder builder = new DefaultConfigurationBuilder();
                final Configuration logKitConf = builder.build(fis);
                logKitManager = new DefaultLogKitManager(Hierarchy.getDefaultHierarchy());
                logKitManager.setLogger(log);
                final DefaultContext subcontext = new DefaultContext(appContext);
                subcontext.put("context-root", contextDir);
                logKitManager.contextualize(subcontext);
                logKitManager.configure(logKitConf);
                logKitManager = logKitManager;
                if (logger != null) {
                    log = logKitManager.getLogger(logger);
                } else {
                    log = logKitManager.getLogger("cocoon");
                }
            }
            appContext.put(Constants.CONTEXT_CLASSPATH, getClassPath(contextDir));
            appContext.put(Constants.CONTEXT_WORK_DIR, work);
            appContext.put(Constants.CONTEXT_UPLOAD_DIR, contextDir + "upload-dir");
            File cacheDir = getDir(workDir + File.separator + "cache-dir", "cache");
            appContext.put(Constants.CONTEXT_CACHE_DIR, cacheDir);
            appContext.put(Constants.CONTEXT_CONFIG_URL, conf.toURL());
            Cocoon c = new Cocoon();
            c.setLogger(log);
            c.contextualize(appContext);
            c.setLogKitManager(logKitManager);
            c.initialize();
            Main main = new Main(c, context, dest, brokenLinkFile);
            main.userAgent = userAgent;
            main.accept = accept;
            main.followLinks = followLinks;
            main.warmup();
            if (main.process(targets, xspOnly) == 0) {
                main.recursivelyProcessXSP(context, context);
            }
            c.dispose();
            log.info("Done");
        } catch (Exception e) {
            log.fatalError("Exception caught ", e);
            System.exit(1);
        }
        System.exit(0);
    }

    /** Print the version string and exit */
    private static void printVersion() {
        System.out.println(Constants.VERSION);
        System.exit(0);
    }

    /** Print the usage message and exit */
    private static void printUsage() {
        String lSep = System.getProperty("line.separator");
        StringBuffer msg = new StringBuffer();
        msg.append("------------------------------------------------------------------------ ").append(lSep);
        msg.append(Constants.NAME).append(" ").append(Constants.VERSION).append(lSep);
        msg.append("Copyright (c) ").append(Constants.YEAR).append(" Apache Software Foundation. All rights reserved.").append(lSep);
        msg.append("------------------------------------------------------------------------ ").append(lSep).append(lSep);
        msg.append("Usage: java org.apache.cocoon.Main [options] [targets]").append(lSep).append(lSep);
        msg.append("Options: ").append(lSep);
        msg.append(CLUtil.describeOptions(Main.OPTIONS).toString());
        msg.append("Note: the context directory defaults to '").append(Constants.DEFAULT_CONTEXT_DIR + "'").append(lSep);
        System.out.println(msg.toString());
        System.exit(0);
    }

    /**
     * Check for a configuration file in a specific directory.
     *
     * @param dir a <code>File</code> where to look for configuration files
     * @return a <code>File</code> representing the configuration
     * @exception Exception if an error occurs
     */
    private static File getConfigurationFile(File dir) throws Exception {

        if (log.isDebugEnabled()) {
            log.debug("Trying configuration file at: " + dir + File.separator + Constants.DEFAULT_CONF_FILE);
        }
        File f = new File(dir, Constants.DEFAULT_CONF_FILE);
        if (f.canRead()) {
            return f;
        }

        if (log.isDebugEnabled()) {
            log.debug("Trying configuration file at: " + System.getProperty("user.dir") + File.separator + Constants.DEFAULT_CONF_FILE);
        }
        f = new File(System.getProperty("user.dir") + File.separator + Constants.DEFAULT_CONF_FILE);
        if (f.canRead()) {
            return f;
        }

        if (log.isDebugEnabled()) {
            log.debug("Trying configuration file at: /usr/local/etc/" + Constants.DEFAULT_CONF_FILE);
        }
        f = new File("/usr/local/etc/", Constants.DEFAULT_CONF_FILE);
        if (f.canRead()) {
            return f;
        }

        log.error("Could not find the configuration file.");
        throw new FileNotFoundException("The configuration file could not be found.");
    }

    /**
     * Get a <code>File</code> representing a directory.
     *
     * @param dir a <code>String</code> with a directory name
     * @param type a <code>String</code> describing the type of directory
     * @return a <code>File</code> value
     * @exception Exception if an error occurs
     */
    private static File getDir(String dir, String type) throws Exception {

        if (log.isDebugEnabled()) {
            log.debug("Getting handle to " + type + " directory '" + dir + "'");
        }
        File d = new File(dir);

        if (!d.exists()) {
            if (!d.mkdirs()) {
                log.error("Error creating " + type + " directory '" + d + "'");
                throw new IOException("Error creating " + type + " directory '" + d + "'");
            }
        }

        if (!d.isDirectory()) {
            log.error("'" + d + "' is not a directory.");
            throw new IOException("'" + d + "' is not a directory.");
        }

        if (!(d.canRead() && d.canWrite())) {
            log.error("Directory '" + d + "' is not readable/writable");
            throw new IOException("Directory '" + d + "' is not readable/writable");
        }

        return d;
    }

    // -----------------------------------------------------------------------

    private Cocoon cocoon;
    private File destDir;
    private File context;
    private PrintWriter brokenLinkWriter;
    private Map attributes;
    private HashMap empty;
    private Map allProcessedLinks;
    private Map allTranslatedLinks;
    private String userAgent;
    private String accept;
    private boolean followLinks;

    /**
     * Creates the Main class
     * @param cocoon a <code>Cocoon</code> instance
     * @param context a <code>File</code> for the context directory
     * @param destDir a <code>File</code> for the destination directory
     */
    public Main(Cocoon cocoon, File context, File destDir, File brokenLinks) {
        this.cocoon = cocoon;
        this.context = context;
        this.destDir = destDir;
        if (brokenLinks != null) {
            try {
                this.brokenLinkWriter = new PrintWriter(new FileWriter(brokenLinks), true);
            } catch (IOException ioe) {
                log.error("File does not exist: " + brokenLinks.getAbsolutePath());
            }
        }
        this.attributes = new HashMap();
        this.empty = new HashMap();
        this.allProcessedLinks = new HashMap();
        this.allTranslatedLinks = new HashMap();
    }

    /**
     * Warms up the engine by accessing the root.
     * @exception Exception if an error occurs
     */
    public void warmup() throws Exception {
        log.info("Warming up...");
        log.info(" [Cocoon might need to compile the sitemaps, this might take a while]");
        //cocoon.process(new LinkSamplingEnvironment("/", context, attributes, null));
        cocoon.generateSitemap(new LinkSamplingEnvironment("/", context, attributes, null, this.log));
    }

    /**
     * Process the URI list and process them all independently.
     * @param uris a <code>Collection</code> of URIs
     * @param xspOnly a <code>boolean</code> denoting to process XSP only
     * @return an <code>int</code> value with the number of links processed
     * @exception Exception if an error occurs
     */
    public int process(Collection uris, boolean xspOnly) throws Exception {
        int nCount = 0;
        log.info("...ready, let's go:");

        ArrayList links = new java.util.ArrayList();
        Iterator i = uris.iterator();
        String next;
        while (i.hasNext()) {
            next = (String)i.next();
            if (!links.contains(next)) {
                links.add(next);
            }
        }
        while (links.size() > 0) {
            String url = (String)links.get(0);

            try {
                if (allProcessedLinks.get(url) == null) {
                    if (xspOnly) {
                        this.processXSP(url);
                    } else if (this.followLinks) {
                        i = this.processURI(url).iterator();
                        while (i.hasNext()) {
                            next = (String)i.next();
                            if (!links.contains(next)) {
                                links.add(next);
                            }
                        }
                    } else {
                        this.processURI(url);
                    }
                }
            } catch (ResourceNotFoundException rnfe) {
                log.warn("  [broken link]--> " + url);
                if (null != this.brokenLinkWriter) {
                    this.brokenLinkWriter.println(url);
                }
            }
            links.remove(url);
            nCount++;

            if (log.isInfoEnabled()) {
                log.info("  Memory used: " + (Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory()));
                log.info("  Processed, Translated & Left: " + allProcessedLinks.size() + ", "  + allTranslatedLinks.size() + ", " + links.size());
            }
        }
        return nCount;
    }

    /**
     * Recurse the directory hierarchy and process the XSP's.
     * @param contextDir a <code>File</code> value for the context directory
     * @param file a <code>File</code> value for a single XSP file or a directory to scan recursively
     */
    public void recursivelyProcessXSP(File contextDir, File file) {
        if (file.isDirectory()) {
            String entries[] = file.list();
            for (int i = 0; i < entries.length; i++) {
                recursivelyProcessXSP(contextDir, new File(file, entries[i]));
            }
        } else if (file.getName().toLowerCase().endsWith(".xsp")) {
            try {
                this.processXSP(IOUtils.getContextFilePath(contextDir.getCanonicalPath(),file.getCanonicalPath()));
            } catch (Exception e){
                //Ignore for now.
            }
        }
    }

    /**
     * Process a single XSP file
     *
     * @param uri a <code>String</code> pointing to an xsp URI
     * @exception Exception if an error occurs
     */
    public void processXSP(String uri) throws Exception {
        Environment env = new LinkSamplingEnvironment("/", context, attributes, null, this.log);
        cocoon.generateXSP(uri, env);
    }

    /**
     * Processes the given URI and return all links. The algorithm is the following:
     *
     * <ul>
     *  <li>file name for the URI is generated. URI MIME type is checked for
     *      consistency with the URI and, if the extension is inconsistent
     *      or absent, the file name is changed</li>
     *  <li>the link view of the given URI is called and the file names for linked
     *      resources are generated and stored.</li>
     *  <li>for each link, absolute file name is translated to relative path.</li>
     *  <li>after the complete list of links is translated, the link-translating
     *      view of the resource is called to obtain a link-translated version
     *      of the resource with the given link map</li>
     *  <li>list of absolute URI is returned, for every URI which is not yet
     *      present in list of all translated URIs</li>
     * </ul>
     * @param uri a <code>String</code> URI to process
     * @return a <code>Collection</code> containing all links found
     * @exception Exception if an error occurs
     */
    public Collection processURI(String uri) throws Exception {
        log.info("Processing URI: " + uri);

        // Get parameters, deparameterized URI and path from URI
        final TreeMap parameters = new TreeMap();
        final String deparameterizedURI = NetUtils.deparameterize(uri, parameters);
        final String path = NetUtils.getPath(uri);
        final String suri = NetUtils.parameterize(deparameterizedURI, parameters);
        parameters.put("user-agent", userAgent);
        parameters.put("accept", accept);

        // Get file name from URI (without path)
        String pageURI = deparameterizedURI;
        if (pageURI.indexOf("/") != -1) {
            pageURI = pageURI.substring(pageURI.lastIndexOf("/") + 1);
            if (pageURI.length() == 0) {
                pageURI = "./";
            }
        }

        String filename = (String)allTranslatedLinks.get(suri);
        if (filename == null) {
            filename = mangle(suri);
            final String type = getType(deparameterizedURI, parameters);
            final String ext = NetUtils.getExtension(filename);
            final String defaultExt = MIMEUtils.getDefaultExtension(type);
            if ((ext == null) || (!ext.equals(defaultExt))) {
                filename += defaultExt;
            }
            allTranslatedLinks.put(suri, filename);
        }
        // Store processed URI list to avoid eternal loop
        allProcessedLinks.put(suri, filename);

        if ("".equals(filename)) {
            return new ArrayList();
        }

        // Process links
        final ArrayList absoluteLinks = new ArrayList();
        final HashMap translatedLinks = new HashMap();
        final Iterator i = this.getLinks(deparameterizedURI, parameters).iterator();
        while (i.hasNext()) {
            String link = (String) i.next();
            // Fix relative links starting with "?"
            String relativeLink = link;
            if (relativeLink.startsWith("?")) {
                relativeLink = pageURI + relativeLink;
            }

            String absoluteLink = NetUtils.normalize(NetUtils.absolutize(path, relativeLink));
            {
                final TreeMap p = new TreeMap();
                absoluteLink = NetUtils.parameterize(NetUtils.deparameterize(absoluteLink, p), p);
            }
            String translatedAbsoluteLink = (String)allTranslatedLinks.get(absoluteLink);
            if (translatedAbsoluteLink == null) {
                try {
                    translatedAbsoluteLink = this.translateURI(absoluteLink);
                    log.info("  Link translated: " + absoluteLink);
                    allTranslatedLinks.put(absoluteLink, translatedAbsoluteLink);
                    absoluteLinks.add(absoluteLink);
                } catch (ResourceNotFoundException rnfe) {
                    log.warn("  [broken link]--> " + absoluteLink);

                    if (null != this.brokenLinkWriter) {
                        this.brokenLinkWriter.println(absoluteLink);
                    }
                    continue;
                }
            }

            final String translatedRelativeLink = NetUtils.relativize(path, translatedAbsoluteLink);
            translatedLinks.put(link, translatedRelativeLink);
        }

        try {
            // Process URI
            File file = IOUtils.createFile(destDir, NetUtils.decodePath(filename));
            OutputStream output = new BufferedOutputStream(new FileOutputStream(file));
            String type = getPage(deparameterizedURI, parameters, translatedLinks, output);
            output.close();

            if (type == null) {
                log.warn("  [broken link]--> " + filename);
                if (null != this.brokenLinkWriter) {
                    this.brokenLinkWriter.println(filename);
                }
                resourceUnavailable(file);
            } else {
                log.info("  [" + type + "]--> " + filename);
            }
        } catch (ResourceNotFoundException rnfe) {
            this.log.warn("Could not process URI: " + deparameterizedURI);
        }

        return absoluteLinks;
    }

    /**
     * Translate an URI into a file name.
     *
     * @param uri a <code>String</code> value to map
     * @return a <code>String</code> vlaue for the file
     * @exception Exception if an error occurs
     */
    public String translateURI(String uri) throws Exception {
        if (null == uri || "".equals(uri)) {
            log.warn("translate empty uri");
            return "";
        }
        HashMap parameters = new HashMap();
        parameters.put("user-agent", userAgent);
        parameters.put("accept", accept);
        String deparameterizedURI = NetUtils.deparameterize(uri, parameters);

        String path = NetUtils.getPath(uri);
        String filename = mangle(uri);
        String type = getType(deparameterizedURI, parameters);
        String ext = NetUtils.getExtension(filename);
        String defaultExt = MIMEUtils.getDefaultExtension(type);
        if ((ext == null) || (!ext.equals(defaultExt))) {
            filename += defaultExt;
        }

        return filename;
    }

    /**
     * Generate a <code>resourceUnavailable</code> message.
     *
     * @param file being unavailable
     * @exception IOException if an error occurs
     */
    private void resourceUnavailable(File file) throws IOException {
        PrintStream out = new PrintStream(new BufferedOutputStream(new FileOutputStream(file)));
        out.println(
            new StringBuffer("<html><head><title>Page Not Available</title></head>")
                .append("<body><h1 align=\"center\">Page Not Available</h1>")
                .append("<body><p align=\"center\">Generated by ")
                .append(Constants.COMPLETE_NAME)
                .append("</p></body></html>")
        );
        out.close();
    }

    /**
     * Mangle an URI.
     *
     * @param uri a URI to mangle
     * @return a mangled URI
     */
    private String mangle(String uri) {
        if (log.isDebugEnabled()) {
            log.debug("mangle(\"" + uri + "\")");
        }
        if (uri.charAt(uri.length() - 1) == '/') {
            uri += Constants.INDEX_URI;
        }
        uri = uri.replace('"', '\'');
        uri = uri.replace('?', '_');
        uri = uri.replace(':', '_');
        if (log.isDebugEnabled()) {
            log.debug(uri);
        }
        return uri;
    }

    /**
     * Generate the string for a leaf in ascii art for a tree view log output.
     *
     * @param level of nesting
     * @return a <code>String</code> value to print the levels requested
     */
    private String leaf(int level) {
        if (level == 0) {
            return "";
        }
        return tree(level - 2) + "+--";
    }

    /**
     * Generate the string for a tree in ascii art for a tree view log output.
     *
     * @param level of nesting
     * @return a <code>String</code> value to print the levels requested
     */
    protected String tree(int level) {
        StringBuffer buffer = new StringBuffer();
        for (int i = 0; i <= level; i++) {
            buffer.append("|  ");
        }
        return buffer.toString();
    }

    /**
     * Samples an URI for its links.
     *
     * @param deparameterizedURI a <code>String</code> value of an URI to start sampling from
     * @param parameters a <code>Map</code> value containing request parameters
     * @return a <code>Collection</code> of links
     * @exception Exception if an error occurs
     */
    protected Collection getLinks(String deparameterizedURI, Map parameters) throws Exception {
        LinkSamplingEnvironment env = new LinkSamplingEnvironment(deparameterizedURI,
                  context,
                  attributes,
                  parameters,
                  this.log);
        cocoon.process(env);
        return env.getLinks();
    }

    /**
     * Processes an URI for its content.
     *
     * @param deparameterizedURI a <code>String</code> value of an URI to start sampling from
     * @param parameters a <code>Map</code> value containing request parameters
     * @param links a <code>Map</code> value
     * @param stream an <code>OutputStream</code> to write the content to
     * @return a <code>String</code> value for the content
     * @exception Exception if an error occurs
     */
    protected String getPage(String deparameterizedURI, Map parameters, Map links, OutputStream stream) throws Exception {
        FileSavingEnvironment env = new FileSavingEnvironment(deparameterizedURI,
                        context,
                        attributes,
                        parameters,
                        links,
                        stream,
                        this.log);
        cocoon.process(env);
        return env.getContentType();
    }

    /** Class <code>NullOutputStream</code> here. */
    static class NullOutputStream extends OutputStream
    {
        public void write(int b) throws IOException { }
        public void write(byte b[]) throws IOException { }
        public void write(byte b[], int off, int len) throws IOException { }
    }

    /**
     * Analyze the type of content for an URI.
     *
     * @param deparameterizedURI a <code>String</code> value to analyze
     * @param parameters a <code>Map</code> value for the request
     * @return a <code>String</code> value denoting the type of content
     * @exception Exception if an error occurs
     */
    protected String getType(String deparameterizedURI, Map parameters) throws Exception {
        FileSavingEnvironment env = new FileSavingEnvironment(deparameterizedURI,
                     context,
                     attributes,
                     parameters,
                     empty,
                     new NullOutputStream(),
                     this.log);
        cocoon.process(env);
        return env.getContentType();
    }

    /**
     * This builds the important ClassPath used by this class.  It
     * does so in a neutral way.
     * It iterates in alphabetical order through every file in the
     * lib directory and adds it to the classpath.
     *
     * Also, we add the files to the ClassLoader for the Cocoon system.
     * In order to protect ourselves from skitzofrantic classloaders,
     * we need to work with a known one.
     *
     * @param context  The context path
     * @return a <code>String</code> value
     */
    protected static String getClassPath(final String context) {
        StringBuffer buildClassPath = new StringBuffer();
        String classDir = context + "/WEB-INF/classes";
        File root = new File(context + "/WEB-INF/lib");

        buildClassPath.append(classDir);

        if (root.isDirectory()) {
            File[] libraries = root.listFiles();
            Arrays.sort(libraries);
            for (int i = 0; i < libraries.length; i++) {
                buildClassPath.append(File.pathSeparatorChar)
                              .append(IOUtils.getFullFilename(libraries[i]));
            }
        }

        buildClassPath.append(File.pathSeparatorChar)
                      .append(System.getProperty("java.class.path"));

//        buildClassPath.append(File.pathSeparatorChar)
//                      .append(getExtraClassPath(context));

        if (log.isDebugEnabled()) {
            log.debug("Context classpath: " + buildClassPath.toString());
        }
        return buildClassPath.toString();
     }
}

