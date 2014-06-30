package org.apache.cocoon.samples.parentcm;

import org.apache.avalon.excalibur.pool.Poolable;
import org.apache.avalon.framework.component.ComponentException;
import org.apache.avalon.framework.parameters.Parameters;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.environment.SourceResolver;
import org.apache.cocoon.generation.ComposerGenerator;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import java.io.IOException;
import java.util.Date;
import java.util.Map;

/**
 * Generator for the parent component manager sample. The generator outputs
 * a single tag <code>&lt;time&gt;<i>current time</i>&lt;/time&gt;</code>.
 * Where <code><i>current time</i></code> is the current time as obtained from the
 * <code>Time</code> component.
 *
 * @author <a href="mailto:leo.sutic@inspireinfrastructure.com">Leo Sutic</a>
 */
public class Generator extends ComposerGenerator implements Poolable {

    /**
     * Current time.
     */
    private Date time;

    /**
     * Looks up a <code>Time</code> component and obtains the current time.
     */
    public void setup(SourceResolver resolver, Map objectModel, String src, Parameters par)
        throws ProcessingException, SAXException, IOException {

        Time timeGiver = null;
        try {
            timeGiver = (Time) manager.lookup(Time.ROLE);
            this.time = timeGiver.getTime ();
        } catch (ComponentException ce) {
            throw new ProcessingException ("Could not obtain current time.", ce);
        } finally {
            manager.release(timeGiver);
        }
    }

    /**
     * Generate XML data.
     */
    public void generate() throws SAXException, ProcessingException {
        AttributesImpl emptyAttributes = new AttributesImpl();
        contentHandler.startDocument();
        contentHandler.startElement("", "time", "time", emptyAttributes);

        char[] text = this.time.toString().toCharArray();

        contentHandler.characters(text, 0, text.length);

        contentHandler.endElement("", "time", "time");
        contentHandler.endDocument();
    }

    /**
     * Prepare this object for another cycle.
     */
    public void recycle () {
        this.time = null;
    }
}


