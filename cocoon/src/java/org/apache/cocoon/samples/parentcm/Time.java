package org.apache.cocoon.samples.parentcm;

import org.apache.avalon.framework.component.Component;

import java.util.Date;

/**
 * Interface for a simple time-keeping component.
 */
public interface Time extends Component {

    String ROLE = "org.apache.cocoon.samples.parentcm.Time";
   
    /**
     * Gets the current time.
     */
    Date getTime ();
}

