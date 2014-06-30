package org.apache.cocoon.samples.parentcm;

import org.apache.avalon.framework.component.Component;
import org.apache.avalon.framework.thread.ThreadSafe;

import java.util.Date;

/**
 * Implementing class for the parent component manager sample's 
 * <code>org.apache.cocoon.samples.parentcm.Time</code> component.
 */
public class TimeComponent implements Component, Time, ThreadSafe {
    public Date getTime () {
        return new Date();
    }
}

