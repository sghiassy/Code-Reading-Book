/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.components.language.markup.xsp;
 
import org.apache.avalon.excalibur.datasource.DataSourceComponent;

/**
 * This is the Cocoon2 specific part of an EsqlConnection.
 * This should only be in the C2 codebase
 *
 * @author <A HREF="mailto:tcurdt@dff.st">Torsten Curdt</A>
 * based on the orginal esql.xsl
 */
 
public class EsqlConnectionCocoon2 extends EsqlConnection {
  public DataSourceComponent datasource = null;
}
