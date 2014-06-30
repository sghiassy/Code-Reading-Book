package org.apache.cocoon.transformation;

import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.Constants;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.Request;
import org.apache.cocoon.environment.Session;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.xml.EmbeddedXMLPipe;
import org.apache.cocoon.xml.dom.DOMStreamer;
import org.w3c.dom.Node;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.util.Map;


/**
 * With this transformer, a DOM-object that is stored in the session, can be inserted
 * in the SAX stream at a given position.
 *
 * Usage in sitemap:
 *    &lt;map:transform type="readDOMsession"&gt;
 *      &lt;map:parameter name="dom-name" value="companyInfo"/&gt;
 *      &lt;map:parameter name="trigger-element" value="company"/&gt;
 *      &lt;map:parameter name="position" value="after"/&gt;
 *    &lt;/map:transform&gt;
 *
 * where: 
 *  dom-name is the name of the DOM object in the session
 *  trigger-element is the element that we need to insert the SAX events
 *  postion is the actual place where the stream will be inserted, ie before, after or in
 *  the trigger-element
 *
 * @author <a href="mailto:sven.beauprez@the-ecorp.com">Sven Beauprez</a>
 * @version CVS $Revision: 1.1 $ $Date: 2002/01/03 12:31:20 $ $Author: giacomo $
 */

public class ReadDOMSessionTransformer extends AbstractTransformer implements Poolable {

    public static final String DOM_NAME = "dom-name";
    public static final String TRIGGER_ELEMENT = "trigger-element";
    /*
      position where the sax events from the dom should be insterted
      this can be: 'before', 'after' or 'in'
    */
    public static final String POSITION = "position";

    Node node;
    DOMStreamer streamer;
    Session session;

    String DOMName;
    String trigger;
    String position;

    /** BEGIN SitemapComponent methods **/
    public void setup(SourceResolver resolver, Map objectModel,
                      String source, Parameters parameters)
    throws ProcessingException, SAXException, IOException {
      Request request = (Request)(objectModel.get(Constants.REQUEST_OBJECT));
      session = request.getSession(false);
      if (session != null) {
        getLogger().debug("ReadSessionTransformer: SessioID="+session.getId());
        getLogger().debug("ReadSessionTransformer: Session available, try to read from it");
        DOMName = parameters.getParameter(ReadDOMSessionTransformer.DOM_NAME,null);
        trigger = parameters.getParameter(ReadDOMSessionTransformer.TRIGGER_ELEMENT,null);
        position = parameters.getParameter(ReadDOMSessionTransformer.POSITION,"in");
        getLogger().debug("ReadSessionTransformer: " + ReadDOMSessionTransformer.DOM_NAME + "="+
                          DOMName + " " + ReadDOMSessionTransformer.TRIGGER_ELEMENT + "=" +
                          trigger + " " + ReadDOMSessionTransformer.POSITION + "=" +
                          position);
      } else  {
        getLogger().error("ReadSessionTransformer: no session object");
      }
    }
    /** END SitemapComponent methods **/

    /** BEGIN SAX ContentHandler handlers **/
    public void startElement(String uri, String name, String raw, Attributes attributes)
    throws SAXException {
      //start streaming after certain startelement is encountered
      if (name.equalsIgnoreCase(trigger)) {
        getLogger().debug("ReadSessionTransformer: trigger encountered");
        if (position.equalsIgnoreCase("before"))  {
          streamDOM();
          super.contentHandler.startElement(uri,name,raw,attributes);
        } else if (position.equalsIgnoreCase("in"))  {
          super.contentHandler.startElement(uri,name,raw,attributes);
          streamDOM();
        } else if (position.equalsIgnoreCase("after"))  {
          super.contentHandler.startElement(uri,name,raw,attributes);
        }
      } else {
        super.contentHandler.startElement(uri,name,raw,attributes);
      }
    }

    public void endElement(String uri,String name,String raw)
    throws SAXException  {
      super.contentHandler.endElement(uri,name,raw);
      if (name.equalsIgnoreCase(trigger)) {
        if (position.equalsIgnoreCase("after"))  {
          streamDOM();
        }
      }
    }
    /** END SAX ContentHandler handlers **/

    /** own methods **/
    private void streamDOM()
    throws SAXException  {
      if (DOMName!=null)  {
        node = (Node)session.getAttribute(DOMName);
      } else  {
        getLogger().error("ReadSessionTransformer: no "+ ReadDOMSessionTransformer.DOM_NAME
                          +" parameter specified");
      }
      if (node!=null)  {
        getLogger().debug("ReadSessionTransformer: start streaming");
        EmbeddedXMLPipe pipe = new EmbeddedXMLPipe(super.contentHandler);
        streamer = new DOMStreamer(pipe,super.lexicalHandler);
        streamer.stream(node);
      }else  {
        getLogger().error("ReadSessionTransformer: no Document in session");
      }
    }

}
