/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.sitemap;

import org.apache.avalon.excalibur.component.RoleManager;
import org.apache.avalon.excalibur.logger.LogKitManager;
import org.apache.avalon.framework.activity.Disposable;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.logger.AbstractLoggable;
import org.apache.avalon.framework.thread.ThreadSafe;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.acting.Action;
import org.apache.cocoon.components.CocoonComponentManager;
import org.apache.cocoon.components.classloader.RepositoryClassLoader;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.components.url.URLFactory;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.generation.Generator;
import org.apache.cocoon.matching.Matcher;
import org.apache.cocoon.reading.Reader;
import org.apache.cocoon.selection.Selector;
import org.apache.cocoon.serialization.Serializer;
import org.apache.cocoon.transformation.Transformer;
import org.apache.cocoon.util.ClassUtils;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Iterator;

/**
 * Base class for generated <code>Sitemap</code> classes
 *
 * @author <a href="mailto:Giacomo.Pati@pwr.ch">Giacomo Pati</a>
 * @version CVS $Revision: 1.4 $ $Date: 2002/01/30 10:14:48 $
 */
public abstract class AbstractSitemap extends AbstractLoggable implements Sitemap, Disposable, ThreadSafe {
    private Context context;
    private static final int BYTE_ARRAY_SIZE = 1024;
    private static RoleManager roleManager;
    private static LogKitManager logKitManager;
    private static Configuration defaultConfig;

    /** The component manager instance */
    protected CocoonComponentManager manager;

    /** The sitemap manager instance */
    protected Manager sitemapManager;

    /** The URLFactory instance */
    protected URLFactory urlFactory;

    /** The creation date */
    protected static long dateCreated = -1L;
    protected SitemapComponentSelector generators;
    protected SitemapComponentSelector transformers;
    protected SitemapComponentSelector serializers;
    protected SitemapComponentSelector readers;
    protected SitemapComponentSelector actions;
    protected SitemapComponentSelector matchers;
    protected SitemapComponentSelector selectors;

    /**
     * Set the role manager
     */
    public static void setRoleManager(RoleManager roles, Configuration config) {
        AbstractSitemap.roleManager = roles;
        AbstractSitemap.defaultConfig = config;
    }

    /**
     * Set the logkit manager
     */
    public static void setLogKitManager(LogKitManager logkit) {
        AbstractSitemap.logKitManager = logkit;
    }

    /**
     * Set the current <code>ComponentManager</code> instance used by this
     * <code>Composable</code>.
     */
    public void compose(ComponentManager manager) throws ComponentException {
        this.manager = new CocoonComponentManager(manager);
        this.manager.setLogger(getLogger());
        this.manager.contextualize(this.context);
        this.manager.setRoleManager(AbstractSitemap.roleManager);
        this.manager.setLogKitManager(AbstractSitemap.logKitManager);

        try {
            if (AbstractSitemap.defaultConfig != null) {
                this.manager.configure(AbstractSitemap.defaultConfig);
            }

            this.urlFactory = (URLFactory)manager.lookup(URLFactory.ROLE);

            // Create Sitemap Selectors
            this.generators = new SitemapComponentSelector();
            this.transformers = new SitemapComponentSelector();
            this.serializers = new SitemapComponentSelector();
            this.readers = new SitemapComponentSelector();
            this.actions = new SitemapComponentSelector();
            this.matchers = new SitemapComponentSelector();
            this.selectors = new SitemapComponentSelector();

            // Set Parent Sitemap Selectors
            try {
                this.generators.setParentSelector((SitemapComponentSelector) this.manager.lookup(Generator.ROLE + "Selector"));
                this.transformers.setParentSelector((SitemapComponentSelector) this.manager.lookup(Transformer.ROLE + "Selector"));
                this.serializers.setParentSelector((SitemapComponentSelector) this.manager.lookup(Serializer.ROLE  + "Selector"));
                this.readers.setParentSelector((SitemapComponentSelector) this.manager.lookup(Reader.ROLE + "Selector"));
                this.actions.setParentSelector((SitemapComponentSelector) this.manager.lookup(Action.ROLE + "Selector"));
                this.matchers.setParentSelector((SitemapComponentSelector) this.manager.lookup(Matcher.ROLE + "Selector"));
                this.selectors.setParentSelector((SitemapComponentSelector) this.manager.lookup(Selector.ROLE + "Selector"));
            } catch (ComponentException ce) {
                /* we are ignoring those exceptions.  We just want
                 * to ensure that the selectors get initialized
                 * properly
                 */
            }

            // Setup the Selectors
            this.setupSelector(this.generators);
            this.setupSelector(this.transformers);
            this.setupSelector(this.serializers);
            this.setupSelector(this.readers);
            this.setupSelector(this.actions);
            this.setupSelector(this.matchers);
            this.setupSelector(this.selectors);

            // Add the Selectors
            this.manager.addComponentInstance(Generator.ROLE + "Selector", this.generators);
            this.manager.addComponentInstance(Transformer.ROLE + "Selector", this.transformers);
            this.manager.addComponentInstance(Serializer.ROLE + "Selector", this.serializers);
            this.manager.addComponentInstance(Reader.ROLE + "Selector", this.readers);
            this.manager.addComponentInstance(Action.ROLE + "Selector", this.actions);
            this.manager.addComponentInstance(Matcher.ROLE + "Selector", this.matchers);
            this.manager.addComponentInstance(Selector.ROLE + "Selector", this.selectors);
        } catch (Exception e) {
            getLogger().error("cannot obtain the Component", e);
            throw new ComponentException("cannot obtain the URLFactory", e);
        }
    }

    private void setupSelector(SitemapComponentSelector selector) throws Exception {
        selector.setLogger(getLogger());
        selector.contextualize(this.context);
        selector.setRoleManager(AbstractSitemap.roleManager);
        selector.setLogKitManager(AbstractSitemap.logKitManager);
        selector.compose(this.manager);
    }

    public void contextualize(Context context) throws ContextException {
        this.context = context;
    }

    /**
     * Return the component manager for this sitemap
     */
    public ComponentManager getComponentManager() {
        return this.manager;
    }

    /**
     * Determines whether this generator's source files have changed
     *
     * @return Whether any of the files this sitemap depends on has changed
     * since it was created
     */
    public final boolean modifiedSince(long date) {
        return (dateCreated < date);
    }

    /**
     * Determines whether generated content has changed since
     * last invocation. Users may override this method to take
     * advantage of SAX event cacheing
     *
     * @param request The request whose data must be inspected to assert whether
     * dynamically generated content has changed
     * @return Whether content has changes for this request's data
     */

    /*
    public boolean hasContentChanged(Request request) {
    return true;
    }
    */

    /**
    * Loads a class specified in a sitemap component definition and
    * initialize it
    */

    public void load_component(int type, Object hint, String classURL, Configuration configuration,
        String mime_type) throws Exception {
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
            switch (type) {
                case Sitemap.GENERATOR:
                    this.generators.addComponent(hint, clazz, configuration);
                    break;
                case Sitemap.TRANSFORMER:
                    this.transformers.addComponent(hint, clazz, configuration);
                    break;
                case Sitemap.SERIALIZER:
                    this.serializers.addSitemapComponent(hint, clazz, configuration, mime_type);
                    break;
                case Sitemap.READER:
                    this.readers.addSitemapComponent(hint, clazz, configuration, mime_type);
                    break;
                case Sitemap.ACTION:
                    this.actions.addComponent(hint, clazz, configuration);
                    break;
                case Sitemap.MATCHER:
                    this.matchers.addComponent(hint, clazz, configuration);
                    break;
                case Sitemap.SELECTOR:
                    this.selectors.addComponent(hint, clazz, configuration);
                    break;
            }
    }

    private byte[] getByteArrayFromStream(InputStream stream) {
        List list = new ArrayList();
        byte[] b = new byte[BYTE_ARRAY_SIZE];
        int last = 0;
        try {
            while ((last = stream.read(b)) == BYTE_ARRAY_SIZE) {
                list.add(b);
                b = new byte[BYTE_ARRAY_SIZE];
            }
        } catch (IOException ioe) {
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

     /**
      * Replaces occurences of xpath like expressions in an argument String
      * with content from a List of Maps
      */
    protected String substitute(List list, String expr) throws PatternException, NumberFormatException {
        if (expr == null) {
            return null;
        }
        StringBuffer result = new StringBuffer();
        String s = null;
        int j = 0;
        int k = 0;
        int l = 0;
        int m = 0;
        int ii = 0;
        int i = -1;
        try {
            while (ii <= expr.length() && (i = expr.indexOf('{', ii)) != -1) {
                result.append(expr.substring(ii, i));
                j = expr.indexOf('}', i);
                if (j < i) {
                    throw new PatternException("invalid expression in \"" + expr + "\"");
                }
                ii = j + 1;
                k = list.size() - 1;
                s = expr.substring(i + 1, j);
                l = -3;
                m = -1;
                while ((l = s.indexOf("../", l + 3)) != -1) {
                    k--;
                    m = l;
                }
                if (m != -1) {
                    s = s.substring(m + 3);
                }
                Object value = ((Map)list.get(k)).get(s);
                if (value != null){
                    result.append(value.toString());
                    getLogger().debug("Substitute evaluated value for " + s + " as " + value);
                }else{
                    getLogger().warn("Substitute: value not found for " + s + " while evaluating " + expr);
                }
            }
            if (ii < expr.length()) {
                result.append(expr.substring(ii));
            }
            return (result.toString());
        } catch (Exception e) {
            getLogger().error("AbstractSitemap:substitute()", e);
            throw new PatternException("error occurred during evaluation of expression \"" + expr + "\" at position " +
                (i + 1) + "\n" + e.getMessage());
        }
    }

    /**
     * Dumps all sitemap parameters to log
     */
    protected void dumpParameters(List list) {
        if (getLogger().isDebugEnabled()) {
            StringBuffer sb=new StringBuffer();
            if (!list.isEmpty()) {
                sb.append("\nCurrent Sitemap Parameters:\n");
                String path="";
                for (int i=list.size()-1; i>=0; i--) {
                    Map map=(Map)list.get(i);
                    Iterator keys = map.keySet().iterator();
                    while (keys.hasNext()) {
                        String key = (String)keys.next();
                        sb.append("PARAM: '").append(path).append(key)
                            .append("' VALUE: '").append(map.get(key)).append("'\n");
                    }
                    path="../"+path;
                }
            }
            getLogger().debug(sb.toString());
        }
    }

    /**
     * Constructs a resource for the <code>Environment</code> arguments.
     * This method is supplied by the generated Sitemap.
     */
    public abstract boolean process(Environment environment) throws Exception;

    /**
     * Constructs a resource for the <code>Environment</code> arguments.
     * This method is supplied by the generated Sitemap.
     */
    public abstract boolean process(Environment environment, StreamPipeline pipeline,
        EventPipeline eventPipeline) throws Exception;

    /**
     * Invokes a method of this class using Java Reflection
     *
     * @param methodName The name of the method to be invoked
     * @param argTypes The array of class types of the method attributes
     * @param argTypes The array of values of the method attributes
     *
     * @exception Exception If an error occurs during method invocation
     */
    protected boolean invokeMethod(String methodName, Class[] argTypes, Object[] argValues)
    throws Exception {
        try {
            if ( this.getLogger().isDebugEnabled() )
                this.getLogger().debug("Dynamically invoking " + methodName);

            java.lang.reflect.Method method = this.getClass().getDeclaredMethod(methodName, argTypes);
            return ((Boolean)method.invoke(this, argValues)).booleanValue();
        } catch (NoSuchMethodException e) {
            getLogger().error("AbstractSitemap:invokeMethod()", e);
            int prefix = methodName.indexOf("_");
            throw new ProcessingException("Sitemap: " + methodName.substring(0, prefix) + " '" +
                methodName.substring(prefix + 1) + "' not found", e);
        } catch (InvocationTargetException e) {
            final Throwable t = e.getTargetException();

            if (t == null) {
                throw e;
            } else if (t instanceof Error) {
                throw (Error) t;
            } else if (t instanceof Exception) {
                throw (Exception) t;
            } else {
                getLogger().error("Unknown target exception type: " + t.getClass().getName());
                throw e;
            }
        } catch (Exception e) {
            getLogger().error("AbstractSitemap:invokeMethod()", e);
            throw e;
        }
    }

    /**
     * dispose
     */
    public void dispose() {
        if (this.urlFactory != null) {
            manager.release((Component)this.urlFactory);
        }
        if (this.generators != null) {
            manager.release((Component)this.generators);
        }
        if (this.transformers != null) {
            manager.release((Component)this.transformers);
        }
        if (this.serializers != null) {
            manager.release((Component)this.serializers);
        }
        if (this.readers != null) {
            manager.release((Component)this.readers);
        }
        if (this.actions != null) {
            manager.release((Component)this.actions);
        }
        if (this.matchers != null) {
            manager.release((Component)this.matchers);
        }
        if (this.selectors != null) {
            manager.release((Component)this.selectors);
        }
    }

    /**
      Reset the response if possible. This allows error handlers to have
      a higher chance to produce clean output if the pipeline that raised
      the error has already output some data.

      @param objectModel the object model
      @return true if the response was successfully reset
      @deprecated Use the tryResetResponse on the Environment instead.
      */

    protected boolean tryResetResponse(Map objectModel)
    {
        getLogger().warn("tryResetResponse in AbstractSitemap is deprecated!");
        return false;
    }
}
