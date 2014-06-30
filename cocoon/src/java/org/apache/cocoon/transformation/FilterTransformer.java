package org.apache.cocoon.transformation;

import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.caching.Cacheable;
import org.apache.cocoon.caching.CacheValidity;
import org.apache.cocoon.caching.NOPCacheValidity;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.util.HashUtil;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.util.Map;

/**
 * The filter transformer can be used to let only an amount of elements through in
 * a given block.
 *
 * Usage in sitemap
 *    &lt;map:transform type="filter"&gt;
 *     &lt;map:parameter name="element-name" value="row"/&gt;
 *     &lt;map:parameter name="count" value="5"/&gt;
 *     &lt;map:parameter name="blocknr" value="3"/&gt;
 *    &lt;/map:transform&gt;
 *
 * Only the 3th block will be shown, containing only 5 row elements.
 *
 * @author <a href="mailto:sven.beauprez@the-ecorp.com">Sven Beauprez</a>
 * @author <a href="mailto:cziegeler@apache.org">Carsten Ziegeler</a>
 * @version CVS $Revision: 1.2 $ $Date: 2002/01/08 10:47:46 $ $Author: cziegeler $
 */

public class FilterTransformer
extends AbstractTransformer
implements Poolable, Cacheable {

    private static final String ELEMENT = "element-name";
    private static final String COUNT = "count";
    private static final String BLOCKNR = "blocknr";
    private static final String BLOCK = "block";
    private static final String BLOCKID = "id";
    private static final int DEFAULT_COUNT = 10;
    private static final int DEFAULT_BLOCK = 1;

    protected int counter;
    protected int count;
    protected int blocknr;
    protected int currentBlocknr;
    protected String elementName;
    protected String parentName;
    protected boolean skip;
    protected boolean foundIt;

    /** BEGIN SitemapComponent methods **/
    public void setup(SourceResolver resolver,
                      Map objectModel,
                      String source,
                      Parameters parameters)
    throws ProcessingException, SAXException, IOException {
        this.counter=0;
        this.currentBlocknr=0;
        this.skip=false;
        this.foundIt=false;
        this.parentName=null;
        this.elementName = parameters.getParameter(FilterTransformer.ELEMENT, "");
        this.count = parameters.getParameterAsInteger(FilterTransformer.COUNT, FilterTransformer.DEFAULT_COUNT);
        this.blocknr = parameters.getParameterAsInteger(FilterTransformer.BLOCKNR, FilterTransformer.DEFAULT_BLOCK);
        if (this.elementName == null || this.elementName.equals("") || this.count == 0)  {
            throw new ProcessingException("FilterTransformer: both "+ FilterTransformer.ELEMENT + " and " +
                          FilterTransformer.COUNT + " parameters need to be specified");
        }
    }

    /**
     * Generate the unique key.
     * This key must be unique inside the space of this component.
     * This method must be invoked before the generateValidity() method.
     *
     * @return The generated key or <code>0</code> if the component
     *              is currently not cacheable.
     */
    public long generateKey() {
        final StringBuffer hash = new StringBuffer(this.elementName);
        hash.append('<').append(this.count).append('>').append(this.blocknr);
        return HashUtil.hash(hash);
    }

    /**
     * Generate the validity object.
     * Before this method can be invoked the generateKey() method
     * must be invoked.
     *
     * @return The generated validity object or <code>null</code> if the
     *         component is currently not cacheable.
     */
    public CacheValidity generateValidity() {
        return NOPCacheValidity.CACHE_VALIDITY;
    }

    /** BEGIN SAX ContentHandler handlers **/
    public void startElement(String uri, String name, String raw, Attributes attributes)
    throws SAXException {
        if (name.equalsIgnoreCase(elementName)) {
            this.foundIt = true;
            this.counter++;
            if (this.counter <= (this.count*(this.blocknr)) && this.counter > (this.count*(this.blocknr-1))) {
                this.skip = false;
            } else  {
                this.skip = true;
            }
            if (this.currentBlocknr != (int)Math.ceil((float)this.counter/this.count)) {
                this.currentBlocknr = (int)Math.ceil((float)this.counter/this.count);
                AttributesImpl attr = new AttributesImpl();
                attr.addAttribute(uri,FilterTransformer.BLOCKID,FilterTransformer.BLOCKID,"CDATA",String.valueOf(this.currentBlocknr));
                if (this.counter < this.count)  {
                    super.contentHandler.startElement(uri,FilterTransformer.BLOCK,FilterTransformer.BLOCK,attr);
                } else  {
                    super.contentHandler.endElement(uri,FilterTransformer.BLOCK,FilterTransformer.BLOCK);
                    super.contentHandler.startElement(uri,FilterTransformer.BLOCK,FilterTransformer.BLOCK,attr);
                }
            }
        } else if (!this.foundIt)  {
            this.parentName = name;
        }
        if (!this.skip)  {
            super.contentHandler.startElement(uri,name,raw,attributes);
        }
    }

    public void endElement(String uri,String name,String raw)
    throws SAXException  {
        if (this.foundIt && name.equals(this.parentName)) {
            super.contentHandler.endElement(uri,FilterTransformer.BLOCK,FilterTransformer.BLOCK);
            super.contentHandler.endElement(uri,name,raw);
            this.foundIt = false;
            this.skip = false;
        } else if (!this.skip)  {
            super.contentHandler.endElement(uri,name,raw);
        }
    }

    public void characters(char c[], int start, int len)
    throws SAXException {
        if (!this.skip)  {
            super.contentHandler.characters(c,start,len);
        }
    }

    public void processingInstruction(String target, String data)
    throws SAXException {
        if (!this.skip)  {
            super.contentHandler.processingInstruction(target, data);
        }
    }

    public void startEntity(String name)
    throws SAXException {
        if (!this.skip)  {
            super.lexicalHandler.startEntity(name);
        }
    }

    public void endEntity(String name)
    throws SAXException {
        if (!this.skip)  {
            super.lexicalHandler.endEntity( name);
        }
    }

    public void startCDATA()
    throws SAXException {
        if (!this.skip)  {
            super.lexicalHandler.startCDATA();
        }
    }

    public void endCDATA()
    throws SAXException {
        if (!this.skip)  {
            super.lexicalHandler.endCDATA();
        }
    }

    public void comment(char ch[], int start, int len)
    throws SAXException {
        if (!this.skip)  {
            super.lexicalHandler.comment(ch, start, len);
        }
    }

}
