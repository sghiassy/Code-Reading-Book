package org.apache.cocoon.scheme.sitemap;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.component.Composable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.sitemap.AbstractSitemap;
import org.apache.cocoon.sitemap.Sitemap;

/**
 * This class is responsible with configuring the ComponentManager
 * appropriately with the sitemap components to be used in
 * sitemaps. These components are read from the components.xconf file
 * at configuration time by this class.
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since January 4, 2002
 */
public class SitemapComponents extends AbstractSitemap implements Configurable
{
  protected String defaultGenerator;
  protected String defaultTransformer;
  protected String defaultReader;
  protected String defaultSerializer;
  protected String defaultSelector;
  
  public void configure(Configuration confs)
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
      throw new ConfigurationException(ex.toString());
    }

    System.out.println("done");
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

  public boolean process(Environment environment)
    throws Exception
  {
    throw new Exception("Should not be invoked");
  }

  public boolean process(Environment environment, StreamPipeline pipeline,
                         EventPipeline eventPipeline)
    throws Exception
  {
    throw new Exception("Should not be invoked");
  }
}
