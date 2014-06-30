/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/

package org.apache.cocoon.generation;

import com.ibm.bsf.BSFException;
import com.ibm.bsf.BSFManager;
import com.ibm.bsf.util.IOUtils;
import org.apache.avalon.excalibur.pool.Recyclable;
import org.apache.avalon.framework.configuration.Configurable;
import org.apache.avalon.framework.configuration.Configuration;
import org.apache.avalon.framework.configuration.ConfigurationException;
import org.apache.cocoon.ProcessingException;
import org.apache.cocoon.ResourceNotFoundException;
import org.apache.cocoon.components.parser.Parser;
import org.apache.cocoon.environment.Source;
import org.xml.sax.InputSource;

import java.io.FileNotFoundException;
import java.io.Reader;
import java.io.StringReader;

/**
 * The Scriptgenerator executes arbitraty scripts using the BSF framework
 * and additional interpreter (Rhino, Jython, etc.) as a Cocoon Generator.
 *
 * Additional language support can be added during configuration, eg
 * using:
 *
 * <pre>
 *   &lt;add-languages&gt;
 *     &lt;language name="potatoscript" src="edu.purdue.cs.bsf.engines.Potatoscript"&gt;
 *       &lt;extension map:value="pos"/&gt;
 *       &lt;extension map:value="psc"/&gt;
 *     &lt;language&gt;
 *     &lt;language name="kawa-scheme" src="org.gnu.kawa.bsf.engines.KawaEngine"&gt;
 *       &lt;extension map:value="scm"/&gt;
 *     &lt;language&gt;
 *   &lt;/add-languages&gt;
 * </pre>
 *
 * @author <a href="mailto:jafoster@engmail.uwaterloo.ca">Jason Foster</a>
 * @version CVS $Revision: 1.3 $ $Date: 2002/01/25 03:48:30 $
 */
public class ScriptGenerator extends ComposerGenerator implements Configurable,
Recyclable {

    protected class BSFLanguage
    {
        public String name;
        public String engineSrc;
        public String[] extensions;
    }

    protected BSFLanguage[] additionalLanguages;

    /** The source */
    private Source inputSource;

    public void configure(Configuration conf) throws ConfigurationException
    {
        if (conf != null)
        {
            //add optional support for additional languages
            Configuration languagesToAdd = conf.getChild("add-languages");

            Configuration[] languages = languagesToAdd.getChildren("language");
            this.additionalLanguages = new BSFLanguage[languages.length];


            for (int i = 0; i < languages.length; ++i)
            {
                Configuration language = languages[i];
                BSFLanguage bsfLanguage = new BSFLanguage();

                bsfLanguage.name = language.getAttribute("name");
                bsfLanguage.engineSrc = language.getAttribute("src");

                getLogger().debug("Configuring ScriptGenerator with additional BSF language " + bsfLanguage.name);
                getLogger().debug("Configuring ScriptGenerator with BSF engine " + bsfLanguage.engineSrc);


                Configuration[] extensions = language.getChildren("extension");
                bsfLanguage.extensions = new String[extensions.length];

                for (int j = 0; j < extensions.length; ++j)
                {
                    bsfLanguage.extensions[i] = extensions[i].getValue();
                    getLogger().debug("Configuring ScriptGenerator with lang extension " + bsfLanguage.extensions[i]);
                }

                this.additionalLanguages[i] = bsfLanguage;
            }
        }
    }

    public void recycle() {
        super.recycle();
        if (this.inputSource != null) {
            this.inputSource.recycle();
            this.inputSource = null;
        }
    }

    public void generate() throws ProcessingException {
        Parser parser = null;
        try {
            // Figure out what file to open and do so
            getLogger().debug("processing file [" + super.source + "]");
            this.inputSource = this.resolver.resolve(super.source);

            getLogger().debug("file resolved to [" + this.inputSource.getSystemId() + "]");

            // TODO: why doesn't this work?
            // Reader in = src.getCharacterStream();
            Reader in = new java.io.InputStreamReader(this.inputSource.getInputStream());

            // Set up the BSF manager and register relevant helper "beans"
            BSFManager mgr = new BSFManager();

            // add support for additional languages

            if (this.additionalLanguages != null)
            {
                for (int i = 0; i < this.additionalLanguages.length; ++i)
                {
                    getLogger().debug("adding BSF language " + this.additionalLanguages[i].name + " with engine " + this.additionalLanguages[i].engineSrc);

                    mgr.registerScriptingEngine(this.additionalLanguages[i].name,
                                                this.additionalLanguages[i].engineSrc,
                                                this.additionalLanguages[i].extensions);
                }
            }

            StringBuffer output = new StringBuffer();

            mgr.registerBean("resolver", this.resolver);
            mgr.registerBean("source", super.source);
            mgr.registerBean("objectModel", this.objectModel);
            mgr.registerBean("parameters", this.parameters);
            mgr.registerBean("output", output);
            mgr.registerBean("logger", getLogger());

            getLogger().debug("BSFManager execution begining");

            // Execute the script

            mgr.exec(BSFManager.getLangFromFilename(this.inputSource.getSystemId()),
                     this.inputSource.getSystemId(), 0, 0, IOUtils.getStringFromReader(in));

            getLogger().debug("BSFManager execution complete");
            getLogger().debug("output = [" + output.toString() + "]");

            // Extract the XML string from the BSFManager and parse it

            InputSource xmlInput =
                    new InputSource(new StringReader(output.toString()));
            parser = (Parser)(this.manager.lookup(Parser.ROLE));
            parser.setConsumer(this.xmlConsumer);
            parser.parse(xmlInput);
        } catch (ProcessingException e) {
            throw e;
        } catch (FileNotFoundException e) {
            throw new ResourceNotFoundException(
                "Could not load script " + this.inputSource.getSystemId(), e);
        } catch (BSFException e) {
            throw new ProcessingException(
                    "Exception in ScriptGenerator.generate()", e);
        } catch (Exception e) {
            throw new ProcessingException(
                    "Exception in ScriptGenerator.generate()", e);
        } finally {
            if (parser != null) this.manager.release(parser);
        }
    }
}
