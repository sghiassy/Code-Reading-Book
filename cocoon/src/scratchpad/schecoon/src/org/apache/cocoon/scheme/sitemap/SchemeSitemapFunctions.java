package org.apache.cocoon.scheme.sitemap;

import org.apache.avalon.framework.component.ComponentManager;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.components.pipeline.EventPipeline;
import org.apache.cocoon.components.pipeline.StreamPipeline;
import org.apache.cocoon.environment.Environment;
import org.apache.cocoon.environment.Source;
import org.xml.sax.InputSource;
import sisc.ContinuationException;
import sisc.Interpreter;
import sisc.ModuleAdapter;
import sisc.data.Pair;
import sisc.data.Symbol;
import sisc.data.Value;
import sisc.modules.J2S;

/**
 * This class contains the definition of the Scheme functions:
 *
 * <ul>
 *
 *  <li><b>sitemap:generate</b> - create a new {@link
 *  org.apache.cocoon.components.pipeline.StreamPipeline}, and adds the
 *  generator specified by the function arguments to it. The prototype
 *  of Scheme function is:
 *  <pre>
 *   sitemap:generate: SchemeSitemap Environment params -> StreamPipeline
 *  </pre>
 *
 *
 *  <li><b>sitemap:read</b> - creates a new {@link
 *  org.apache.cocoon.components.pipeline.StreamPipeline} and a new
 *  {@link org.apache.cocoon.reading.Reader}, adds the reader to the
 *  pipeline, and returns the pipeline. The prototype of the Scheme
 *  function is:
 *  <pre>
 *   sitemap:read: SchemeSitemap Environment params -> StreamPipeline
 *  </pre>
 *
 *  <li><b>sitemap:transform</b> - creates a transformer object of the
 *  type specified by the arguments, adds it to the pipeline passed as
 *  argument, and returns the pipeline. The prototype of the Scheme
 *  function is:
 *  <pre>
 *   sitemap:transform: SchemeSitemap Environment params StreamPipeline
 *                      -> StreamPipeline
 *  </pre>
 *
 *  <li><b>sitemap:serialize</b> - creates a serializer object of the
 *  type specified by the arguments, adds it to the pipeline passed as
 *  argument, and returns the pipeline. The prototype of the Scheme
 *  function is:
 *  <pre>
 *   sitemap:serialize: SchemeSitemap Environment params StreamPipeline
 *                      -> StreamPipeline
 *  </pre>
 *
 *  <li><b>sitemap:process</b> - processes the given pipeline passed
 *  as argument. This has the side effect of processing the pipeline,
 *  as it was specified in the sitemap.
 *  <pre>
 *   sitemap:process: SchemeSitemap Environment params StreamPipeline
 *                    -> void
 *  </pre>
 *
 * </ul>
 *
 * <p>With these Scheme functions, a pipeline processing setup would
 * look like this:
 *
 * <pre>
 * (match "some-url/(.*)" (sitemap env file . args)
 *   (sitemap:process sitemap env (list (cons 'type "xml"))
 *     (sitemap:transform sitemap env (list (cons 'type "xslt")
 *                                          (cons 'src "stylesheet/a.xsl"))
 *       (sitemap:generate sitemap env (list (cons 'src (concat "docs/" file))))
 *     )))
 * </pre>
 *
 * <p>Another function defined in this module is
 * <code>sitemap:parse</code>. This function is used as an auxiliary
 * to translate and XML document to its equivalent SXML format. It
 * takes as arguments an Avalon <code>ComponentManager</code> object
 * and a Cocoon <code>Source</code> object, and returns the SXML
 * representation of the XML document read from the
 * <code>Source</code>.
 *
 * <pre>
 *  sitemap:parse: ComponentManager org.apache.cocoon.environment.Source
 *                 -> Value
 * </pre>
 *
 * @author <a href="mailto:ovidiu@cup.hp.com">Ovidiu Predescu</a>
 * @since December 20, 2001
 *
 * @see org.apache.cocoon.components.pipeline.EventPipeline
 * @see org.apache.cocoon.components.pipeline.StreamPipeline
 * @see org.apache.cocoon.reading.Reader
 */
public class SchemeSitemapFunctions extends ModuleAdapter
{
  public String getModuleName()
  {
    return "Apache Cocoon Sitemap Components";
  }

  public float getModuleVersion()
  {
    return 1.0f;
  }

  public static final int
    GENERATE = 1,
    READER = 2,
    TRANSFORM = 3,
    SERIALIZE = 4,
    PROCESS = 5,
    PARSE = 6;

  protected Parameters emptyParam = new Parameters();

  /**
   * Creates a new <code>SchemeSitemapFunctions</code> instance. Defines
   * the Scheme functions implemented by this module.
   */
  public SchemeSitemapFunctions()
  {
    define("sitemap:generate", GENERATE);
    define("sitemap:read", READER);
    define("sitemap:transform", TRANSFORM);
    define("sitemap:serialize", SERIALIZE);
    define("sitemap:process", PROCESS);
    define("sitemap:parse", PARSE);
  }

  /**
   * The SISC evaluator function for this module. Executes the actual
   * Java code corresponding to the Scheme functions defined by the
   * module.
   *
   * @param primid an <code>int</code> value
   * @param r an <code>Interpreter</code> value
   * @return a <code>Value</code> value
   */
  public Value eval(int primid, Interpreter r)
    throws ContinuationException
  {
    try {
      switch (r.vlr.length) {

        // Two argument functions
      case 2:
        switch (primid) {
        case PARSE:
          return parse(r.vlr[0], r.vlr[1]);
        }

        // Three argument functions
      case 3:
        switch (primid) {
        case GENERATE:
          return generate(r.vlr[0], r.vlr[1], r.vlr[2]);
        case READER:
          return read(r.vlr[0], r.vlr[1], r.vlr[2]);
        }

        // Four argument functions
      case 4:
        switch (primid) {
        case TRANSFORM:
          return transform (r.vlr[0], r.vlr[1], r.vlr[2], r.vlr[3]);
        case SERIALIZE:
          return serialize (r.vlr[0], r.vlr[1], r.vlr[2], r.vlr[3]);
        case PROCESS:
          return process (r.vlr[0], r.vlr[1], r.vlr[2], r.vlr[3]);
        }
      }
    }
    catch (Exception ex) {
      ex.printStackTrace();
      throw new RuntimeException(ex.toString());
    }

    throw new RuntimeException("Invalid number of arguments to function "
                               + r.acc + ", got " + r.vlr.length);
  }

  /**
   * Type cast function from a Scheme wrapper of a SchemeSitemap.
   *
   * @param scm a Scheme wrapper instance of a SchemeSitemap.
   * @return a <code>SchemeSitemap</code> value
   */
  static public SchemeSitemap schemeSitemap(Value scm)
  {
    try {
      return (SchemeSitemap)(((J2S.JavaObject)scm).o);
    }
    catch (ClassCastException ex) {
      typeError("SchemeSitemap", scm);
    }
    return null;
  }

  /**
   * Type cast function from a Scheme wrapper of an Environment
   * instance.
   *
   * @param scm a Scheme wrapper of an Environment instance.
   * @return an <code>Environment</code> value
   */
  static public Environment environment(Value scm)
  {
    try {
      return (Environment)(((J2S.JavaObject)scm).o);
    }
    catch (ClassCastException ex) {
      typeError("Environment", scm);
    }
    return null;
  }

  /**
   * Type cast function from a Scheme wrapper of an StreamPipeline
   * instance.
   *
   * @param scm a Scheme wrapper of an StreamPipeline instance.
   * @return an <code>StreamPipeline</code> value
   */
  static public StreamPipeline streamPipeline(Value scm)
  {
    try {
      return (StreamPipeline)(((J2S.JavaObject)scm).o);
    }
    catch (ClassCastException ex) {
      typeError("StreamPipeline", scm);
    }
    return null;
  }

  /**
   * Type cast function from a Scheme wrapper of
   * <code>ComponentManager</code> instance.
   *
   * @param scm a Scheme wrapper of a Cocoon
   * <code>ComponentManager</code> instance.
   * @return an <code>ComponentManager</code> value
   */
  static public ComponentManager componentManager(Value scm)
  {
    try {
      return (ComponentManager)(((J2S.JavaObject)scm).o);
    }
    catch (ClassCastException ex) {
      typeError("ComponentManager", scm);
    }
    return null;
  }

  /**
   * Type cast function from a Scheme wrapper of a Cocoon
   * <code>Source</code> instance.
   *
   * @param scm a Scheme wrapper of a Cocoon <code>Source</code>
   * instance.
   * @return an <code>Source</code> value
   */
  static public Source source(Value scm)
  {
    try {
      return (Source)(((J2S.JavaObject)scm).o);
    }
    catch (ClassCastException ex) {
      typeError("Source", scm);
    }
    return null;
  }

  /**
   * Retrieve an entry from an association list. Uses eq? to compare
   * the CAR of each entry.
   *
   * @param l the association list
   * @param v the value to be searched for
   * @return a <code>Pair</code> value representing the entry, or
   * <tt>FALSE</tt> if no such entry is present.
   */
  static public Value assq (Value l, Value v)
  {
    Pair list = pair(l);
    while (list != EMPTYLIST) {
      Pair entry = pair(list.car);
      if (entry.car.eq(v))
        return entry;
      list = pair(list.cdr);
    }
    return FALSE;
  }

  /**
   * Assumes the <tt>sparams</tt> is either an association list or the
   * FALSE value. It returns either an empty Avalon
   * <tt>Parameters</tt> instance, or a <tt>Parameters</tt> instance
   * that contains the values extracted from the association list.
   *
   * @param sparams a <code>Value</code> value, either <tt>FALSE</tt>
   * or an association list
   * @return an Avalon <code>Parameters</code> instance
   */
  public Parameters getParameters(Value sparams)
  {
    Parameters params = emptyParam;

    if (!sparams.eq(FALSE)) {
      params = new Parameters();
      Pair sparamValues = pair(pair(sparams).cdr);
      while (sparamValues != EMPTYLIST) {
        Pair entry = pair(sparamValues.car);
        String name = string(entry.car);
        String value = string(entry.cdr);
        params.setParameter(name, value);
        sparamValues = pair(sparamValues.cdr);
      }
    }

    return params;
  }

  /**
   * <p>Creates a new pipeline instance, and a new Generator
   * instance. Adds the Generator to the pipeline.
   *
   * <p>The type of the generator is specified in the <tt>sargs</tt>
   * list, which is a Scheme association list. The parameters to be
   * passed to the generators, if any, should be present in this
   * association list as the value of the <it>params</it> entry. This
   * value should be another association list, where the CAR of each
   * entry is the name of the parameter, and the CDR of the entry is
   * the actual value.
   *
   * <p>The recognized parameters are:
   *
   * <ul>
   *
   *  <li><b>src</b> - (required) the source of the generator
   *
   *  <li><b>type</b> - (optional) the type of generator. If not
   *  specified, the default generator specified using the
   *  <tt>default</tt> attribute of the <tt>generators</tt> XML
   *  element is used.
   *
   *  <li><b>params</b> - (optional) the parameters to be passed to
   *  the generator. The CDR of this association entry should be
   *  another association entry, that contains the names and values of
   *  the parameters.
   *
   * </ul>
   *
   * <p><b>Example:</b>
   *
   * <pre>
   * (match "generate/(.*)" (sitemap env file . args)
   *  (sitemap:generate sitemap env
   *                    (list (cons 'src (concat "doc/samples" file ".xml")))))
   * </pre>
   *
   * @param scm the Scheme wrapper for the SchemeSitemap instance
   * @param senv the Scheme wrapper for the Environment instance
   * @param sargs the Scheme arguments, as list
   * @return a Scheme wrapper for the <tt>StreamPipeline</tt> instance
   * @exception Exception if an error occurs
   *
   * @see SchemeSitemap
   */
  public Value generate(Value scm, Value senv, Value sargs)
    throws Exception
  {
    SchemeSitemap sitemap = schemeSitemap(scm);
    StreamPipeline pipeline = sitemap.getStreamPipeline();
    EventPipeline eventPipeline = pipeline.getEventPipeline();

    // Obtain the 'src' attribute
    Value ssrc = assq(sargs, Symbol.get("src"));
    if (ssrc.eq(FALSE))
      throw new RuntimeException("No 'src' attribute specified for 'generate'!");
    String src = string(pair(ssrc).cdr);

    // Obtain the 'type' attribute
    Value stype = assq(sargs, Symbol.get("type"));
    String type;
    if (!stype.eq(FALSE))
      type = string(pair(stype).cdr);
    else
      type = sitemap.getDefaultGeneratorType();

    // Obtain the parameters
    Value sparams = assq(sargs, Symbol.get("params"));
    Parameters params = getParameters(sparams);

//     System.out.println("generate type " + type + ", src " + src
//                        + " params " + params);
    
    eventPipeline.setGenerator(type, src, params);

    return new J2S.JavaObject(pipeline);
  }

  /**
   * <p>Creates a new <tt>Reader</tt> and a new pipeline. Adds the
   * reader to the pipeline and returns the pipeline newly created.
   *
   * <p>The parameters that describe the reader are specified in the
   * <tt>sargs</tt> list, which is a Scheme association list.
   *
   * <p>The recognized parameters for a reader are:
   *
   * <ul>
   *  <li><b>src</b> - (required) the source for the reader
   *
   *  <li><b>mime-type</b> - (optional) the MIME type associated with
   *  the source. If no MIME type is specified, text/html is assumed.
   *
   *  <li><b>type</b> - (optional) The type of the reader to be
   *  used. If no type is specified, the default reader type is
   *  used. The default reader type is specified using the
   *  <tt>default</tt> attribute of the <tt>readers</tt> XML element.
   *
   *  <li><b>params</b> - (optional) additional parameters to be
   *  specified when configuring the reader. The value of this entry
   *  should be a Scheme association list, which contains the
   *  additional parameters.
   *
   * </ul>
   *
   * @param scm the Scheme wrapper for the SchemeSitemap instance
   * @param senv the Scheme wrapper for the Environment instance
   * @param sargs the Scheme arguments, as list
   * @return a Scheme wrapper for the <tt>StreamPipeline</tt> instance
   * @exception Exception if an error occurs
   *
   * @see SchemeSitemap
   */
  public Value read(Value scm, Value senv, Value sargs)
    throws Exception
  {
    SchemeSitemap sitemap = schemeSitemap(scm);
    StreamPipeline pipeline = sitemap.getStreamPipeline();
    EventPipeline eventPipeline = pipeline.getEventPipeline();

    // Obtain the 'src' attribute
    Value ssrc = assq(sargs, Symbol.get("src"));
    if (ssrc.eq(FALSE))
      throw new RuntimeException("No 'src' attribute specified for 'read'!");
    String src = string(pair(ssrc).cdr);

    // Obtain the 'mime-type' attribute
    Value smimeType = assq(sargs, Symbol.get("mime-type"));
    String mimeType = null;
    if (!smimeType.eq(FALSE))
      mimeType = string(pair(smimeType).cdr);

    // Obtain the 'type' attribute
    Value stype = assq(sargs, Symbol.get("type"));
    String type;
    if (!stype.eq(FALSE))
      type = string(pair(stype).cdr);
    else
      type = sitemap.getDefaultReaderType();

    // Obtain the parameters
    Value sparams = assq(sargs, Symbol.get("params"));
    Parameters params = getParameters(sparams);

    pipeline.setReader(type, src, params, mimeType);

    return new J2S.JavaObject(pipeline);
  }

  /**
   * Creates a new <tt>transformer</tt> and adds it to the pipeline
   * passed as argument. Returns the pipeline object passed as
   * argument.
   *
   * <p>The recognized parameters for a transformer are:
   *
   * <ul> <li><b>src</b> - (required) the location of the XSLT
   * stylesheet to be applied.
   *
   *  <li><b>type</b> - (optional) The type of the transformer to be
   *  used, as defined in the <file>components.xconf</file>. If no
   *  type is specified, the default transformer is used. The default
   *  transformer type is specified using the <tt>default</tt>
   *  attribute of the <tt>transformers</tt> XML element.
   *
   *  <li><b>params</b> - (optional) additional parameters to be
   *  specified when configuring the transformer. The value of this
   *  entry should be a Scheme association list, which contains the
   *  additional parameters.
   *
   * </ul>
   *
   * @param scm the Scheme wrapper for the SchemeSitemap instance
   * @param senv the Scheme wrapper for the Environment instance
   * @param sargs the Scheme arguments, as list
   * @return a Scheme wrapper for the <tt>StreamPipeline</tt> instance
   * @exception Exception if an error occurs
   *
   * @see SchemeSitemap
   */
  public Value transform(Value scm, Value senv, Value sargs, Value spipeline)
    throws Exception
  {
    SchemeSitemap sitemap = schemeSitemap(scm);
    StreamPipeline pipeline = streamPipeline(spipeline);
    EventPipeline eventPipeline = pipeline.getEventPipeline();

    // Obtain the 'src' attribute
    Value ssrc = assq(sargs, Symbol.get("src"));
    if (ssrc.eq(FALSE))
      throw new RuntimeException("No 'src' attribute specified for 'transform'!");
    String src = string(pair(ssrc).cdr);

    // Obtain the 'type' attribute
    Value stype = assq(sargs, Symbol.get("type"));
    String type;
    if (!stype.eq(FALSE))
      type = string(pair(stype).cdr);
    else
      type = sitemap.getDefaultTransformerType();

    // Obtain the parameters
    Value sparams = assq(sargs, Symbol.get("params"));
    Parameters params = getParameters(sparams);

//     System.out.println("transform type " + type + ", src " + src
//                        + " params " + params);
    
    eventPipeline.addTransformer(type, src, params);

    return spipeline;
  }

  /**
   * Creates a new <tt>serializer</tt> and adds it to the pipeline
   * passed as argument. Returns the pipeline object passed as
   * argument.
   *
   * <p>The recognized parameters for a transformer are:
   *
   *  <li><b>type</b> - (optional) The type of the serializer to be
   *  used, as defined in the <file>components.xconf</file>. If no
   *  type is specified, the default serializer is used. The default
   *  serializer type is specified using the <tt>default</tt>
   *  attribute of the <tt>serializers</tt> XML element.
   *
   *  <li><b>mime-type</b> - (optional) the MIME type associated with
   *  the generated result. If no MIME type is specified, the default
   *  used by the particular serializer is used.
   *
   *  <li><b>params</b> - (optional) additional parameters to be
   *  specified when configuring the serializer. The value of this
   *  entry should be a Scheme association list, which contains the
   *  additional parameters.
   *
   * </ul>
   *
   * @param scm the Scheme wrapper for the SchemeSitemap instance
   * @param senv the Scheme wrapper for the Environment instance
   * @param sargs the Scheme arguments, as list
   * @return a Scheme wrapper for the <tt>StreamPipeline</tt> instance
   * @exception Exception if an error occurs
   *
   * @see SchemeSitemap
   */
  public Value serialize(Value scm, Value senv, Value sargs, Value spipeline)
    throws Exception
  {
    SchemeSitemap sitemap = schemeSitemap(scm);
    StreamPipeline pipeline = streamPipeline(spipeline);

    // Obtain the 'type' attribute
    Value stype = assq(sargs, Symbol.get("type"));
    String type;
    if (!stype.eq(FALSE))
      type = string(pair(stype).cdr);
    else
      type = sitemap.getDefaultSerializerType();

    // Obtain the 'mime-type' attribute
    Value smimeType = assq(sargs, Symbol.get("mime-type"));
    String mimeType = null;
    if (!smimeType.eq(FALSE))
      mimeType = string(pair(smimeType).cdr);

    // Obtain the parameters
    Value sparams = assq(sargs, Symbol.get("params"));
    Parameters params = getParameters(sparams);

//     System.out.println("serialize type " + type
//                        + " params " + params + " mime-type " + mimeType);
    
    pipeline.setSerializer(type, null, params, mimeType);

    return spipeline;
  }

  /**
   * Invokes the <tt>process</tt> method of the pipeline instance
   * passed as argument. The pipeline instance passed between all the
   * sitemap Scheme functions is always an instance of {@link
   * org.apache.cocoon.components.pipeline.StreamPipeline}. If you
   * need to obtain the {@link
   * org.apache.cocoon.components.pipeline.EventPipeline} use the
   * appropriate method from the {@link
   * org.apache.cocoon.components.pipeline.StreamPipeline#getEventPipeline()}
   * method to obtain it.
   *
   * <p>This function releases the pipeline, so it must be the last
   * function you call on the pipeline.
   *
   * @param scm the Scheme wrapper for the SchemeSitemap instance
   * @param senv the Scheme wrapper for the Environment instance
   * @param sargs the Scheme arguments, as list
   * @param spipeline the Scheme wrapper for pipeline object
   * @return null
   * @exception Exception if an error occurs
   */
  public Value process(Value scm, Value senv, Value sargs, Value spipeline)
    throws Exception
  {
    SchemeSitemap sitemap = schemeSitemap(scm);
    Environment env = environment(senv);
    StreamPipeline pipeline = streamPipeline(spipeline);

    pipeline.process(env);

    sitemap.releasePipeline(pipeline);

    return null;
  }

  /**
   * Parses the XML document read from the <code>Source</code> object
   * passes as the second argument, and translates it to its SXML
   * representation. Returns the SXML representation of the Source as
   * a Scheme value.
   *
   * @param sManager a <code>Value</code> wrapper for an Avalon
   * <code>ComponentManager</code>
   * @param ssource a <code>Value</code> wrapper for a Cocoon Source
   * object
   * @return a <code>Value</code> value, containing the SXML
   * representation of the XML document read from
   * <code>ssource</code>
   * @exception Exception if an error occurs during parsing.
   */
  public Value parse(Value sManager, Value ssource)
    throws Exception
  {
    ComponentManager manager = componentManager(sManager);
    Source source = source(ssource);

    Parser parser = (Parser)manager.lookup(Parser.ROLE);
    XMLtoSXML handler = new XMLtoSXML();
    parser.setContentHandler(handler);
    InputSource input = new InputSource(source.getInputStream());
    parser.parse(input);

    SchemeInterpreter interpreters
      = (SchemeInterpreter)manager.lookup(SchemeInterpreter.ROLE);
    Symbol mainFunction = interpreters.getMainFunction();
    Interpreter interp = interpreters.getInterpreter();

    try {
      return interp.eval(handler.getContent());
    }
    finally {
      interpreters.releaseInterpreter(interp);
    }
  }
}
