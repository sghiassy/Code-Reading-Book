package org.apache.cocoon.scheme.sitemap;

import java.io.InputStream;
import java.util.Date;
import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.avalon.framework.configuration.DefaultConfigurationBuilder;
import org.apache.avalon.framework.context.Context;
import org.apache.avalon.framework.context.ContextException;
import org.apache.avalon.framework.context.Contextualizable;
import org.apache.cocoon.Constants;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;
import org.apache.cocoon.sitemap.AbstractSitemap;
import org.apache.cocoon.sitemap.Sitemap;
import sisc.Interpreter;
import sisc.data.ImmutableString;
import sisc.data.Procedure;
import sisc.data.Symbol;
import sisc.data.Value;
import sisc.modules.J2S;

/**
 *
 * This is the main driver for the Scheme sitemap implementation. It
 * is setup by the {@link SchemeSitemapHandler} class. It reads the
 * <file>components.xconf</file> file, which describes what are the
 * components that can be used in a sitemap. It sets up a pool of
 * Scheme interpreters that handle the actual sitemap invocation.
 *
 * <p>The actual sitemap handling is done in the Scheme code, look for
 * it in the <file>sitemap.scm</file> file.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since January 4, 2002
 */
public class SchemeSitemap
  extends AbstractSitemap
  implements Configurable, Sitemap, Contextualizable, Composable
{
  protected String defaultGenerator;
  protected String defaultTransformer;
  protected String defaultReader;
  protected String defaultSerializer;
  protected String defaultSelector;

  Context cocoonContext;

  public void configure(Configuration confs)
    throws ConfigurationException
  {
    String componentsFileName = confs.getAttribute("components");
    if (componentsFileName == null)
      throw new ConfigurationException("Components file not specified");
    setupSitemap(componentsFileName);
  }

  /**
   * Reads the components file and sets up the component manager
   * accordingly. For each component category we use a
   * ComponentSelector that stores the appropriate components using
   * their names.
   *
   * @param componentsFileName a <code>String</code> value
   */
  protected void setupSitemap(String componentsFileName)
    throws ConfigurationException
  {
    try {
      org.apache.cocoon.environment.Context context =
        (org.apache.cocoon.environment.Context)cocoonContext.get(Constants.CONTEXT_ENVIRONMENT_CONTEXT);

      InputStream is = context.getResource(componentsFileName).openStream();
      DefaultConfigurationBuilder builder = new DefaultConfigurationBuilder();
      Configuration conf = builder.build(is);
      loadComponents(conf);
    } catch (Exception ex) {
      ex.printStackTrace();
      throw new ConfigurationException("Cannot load sitemap components file "
                                       + componentsFileName
                                       + ": " + ex.toString());
    }
  }

  /**
   * Initialize the sitemap components according to the configuration
   * in components.xconf.
   *
   * @param confs a <code>Configurations</code> value
   */
  public void loadComponents(Configuration confs)
    throws ConfigurationException
  {
    System.out.print("Loading the sitemap components... ");
    System.out.flush();

    try {
      Configuration generatorsConf = confs.getChild("generators");
      defaultGenerator = generatorsConf.getAttribute("default");
      Configuration[] gConf = generatorsConf.getChildren("generator");
      for (int i = 0; i < gConf.length; i++)        
        load_component(Sitemap.GENERATOR,
                       gConf[i].getAttribute("name", ""),
                       gConf[i].getAttribute("src"),
                       gConf[i],
                       gConf[i].getAttribute("mime-type", null));

      Configuration readersConf = confs.getChild("readers");
      defaultReader = readersConf.getAttribute("default");
      Configuration[] rConf = readersConf.getChildren("reader");
      for (int i = 0; i < rConf.length; i++)
        load_component(Sitemap.READER,
                       rConf[i].getAttribute("name", ""),
                       rConf[i].getAttribute("src"),
                       rConf[i],
                       rConf[i].getAttribute("mime-type", null));

      Configuration transformersConf = confs.getChild("transformers");
      defaultTransformer = transformersConf.getAttribute("default");
      Configuration[] tConf = transformersConf.getChildren("transformer");
      for (int i = 0; i < tConf.length; i++)
        load_component(Sitemap.TRANSFORMER,
                       tConf[i].getAttribute("name", ""),
                       tConf[i].getAttribute("src"),
                       tConf[i],
                       tConf[i].getAttribute("mime-type", null));

      Configuration serializersConf = confs.getChild("serializers");
      defaultSerializer = serializersConf.getAttribute("default");
      Configuration[] sConf = serializersConf.getChildren("serializer");
      for (int i = 0; i < sConf.length; i++)
        load_component(Sitemap.SERIALIZER,
                       sConf[i].getAttribute("name", ""),
                       sConf[i].getAttribute("src"),
                       sConf[i],
                       sConf[i].getAttribute("mime-type", null));

      Configuration selectorsConf = confs.getChild("selectors");
      defaultSelector = selectorsConf.getAttribute("default");
      Configuration[] slConf = selectorsConf.getChildren("selector");
      for (int i = 0; i < slConf.length; i++)
        load_component(Sitemap.SELECTOR,
                       slConf[i].getAttribute("name", ""),
                       slConf[i].getAttribute("src"),
                       slConf[i],
                       slConf[i].getAttribute("mime-type", null));
    }
    catch (Exception ex) {
      ex.printStackTrace();
      throw new ConfigurationException(ex.toString());
    }

    System.out.println("done");
  }

  public void compose(ComponentManager manager)
    throws ComponentException
  {
    super.compose(manager);
  }

  public void contextualize(Context context)
    throws ContextException
  {
    this.cocoonContext = context;
  }

  public StreamPipeline getStreamPipeline()
    throws Exception
  {
    EventPipeline eventPipeline
      = (EventPipeline)manager.lookup(EventPipeline.ROLE);
    eventPipeline.recompose(manager);

    StreamPipeline pipeline
      = (StreamPipeline)manager.lookup(StreamPipeline.ROLE);
    pipeline.setEventPipeline(eventPipeline);
    pipeline.recompose(manager);

    return pipeline;
  }

  public void releasePipeline(StreamPipeline pipeline)
  {
    EventPipeline eventPipeline = pipeline.getEventPipeline();
    manager.release((Component)eventPipeline);
    manager.release((Component)pipeline);
  }

  public String getDefaultGeneratorType()
  {
    return defaultGenerator;
  }

  public String getDefaultTransformerType()
  {
    return defaultTransformer;
  }

  public String getDefaultReaderType()
  {
    return defaultReader;
  }

  public String getDefaultSerializerType()
  {
    return defaultSerializer;
  }

  public String getDefaultSelectorType()
  {
    return defaultSelector;
  }

  /**
   * Translate the XML sitemap definition into a Scheme
   * representation, and pass it to the Scheme interpreter for
   * evaluation. This method is invoked via the {@link
   * SchemeSitemapFunctions#parse(sisc.data.Value, sisc.data.Value)}
   * method, from the Scheme layer (see <file>sitemap.scm</file>).
   *
   * @param environment an <code>Environment</code> value
   * @exception Exception if an error occurs
   */
  public void parseSitemap(Source sitemapSource)
    throws Exception
  {
    System.out.print("Reading the sitemap... ");
    System.out.flush();

    Date start = new Date();

    J2S.JavaObject sManager = new J2S.JavaObject(manager);
    J2S.JavaObject ssource = new J2S.JavaObject(sitemapSource);
    Value[] args = new Value[] { sManager, ssource };

    SchemeInterpreter interpreters
      = (SchemeInterpreter)manager.lookup(SchemeInterpreter.ROLE);
    Interpreter interp = interpreters.getInterpreter();
    Symbol sitemapParseFunction = interpreters.getSitemapParseFunction();

    try {
      interp.eval((Procedure)interp.ctx.toplevel_env.lookup(sitemapParseFunction),
                  args);
    }
    finally {
      interpreters.releaseInterpreter(interp);
    }

    Date end = new Date();
    Date duration = new Date(end.getTime() - start.getTime());

    System.out.println("done (" + ((double)duration.getTime())/1000 + " sec)");

    // Set the creation date
    dateCreated = (new Date()).getTime();
  }

  /**
   * This method invokes the main Scheme function responsible with
   * dispatching the request according to the sitemap definition.
   *
   * @param environment an <code>Environment</code> value
   * @return a <code>boolean</code> value
   * @exception Exception if an error occurs
   */
  public boolean process(Environment environment)
    throws Exception
  {
    String requestedURI = environment.getURI();

    J2S.JavaObject ssitemap = new J2S.JavaObject(this);
    J2S.JavaObject senv = new J2S.JavaObject(environment);
    ImmutableString servletPath = new ImmutableString(requestedURI);
    Value[] args = new Value[] {servletPath, ssitemap, senv};

    SchemeInterpreter interpreters
      = (SchemeInterpreter)manager.lookup(SchemeInterpreter.ROLE);
    Interpreter interp = interpreters.getInterpreter();
    Symbol mainFunction = interpreters.getMainFunction();
    Value result = sisc.Util.FALSE;

    try {
      result
        = interp.eval((Procedure)interp.ctx.toplevel_env.lookup(mainFunction),
                      args);
    }
    finally {
      interpreters.releaseInterpreter(interp);
    }

    return !result.eq(sisc.Util.FALSE);
  }

  public boolean process(Environment environment,
                         StreamPipeline pipeline,
                         EventPipeline eventPipeline)
    throws Exception
  {
    System.out.println("process(environment, pipeline, eventPipeline)");
    return true;
  }
}
