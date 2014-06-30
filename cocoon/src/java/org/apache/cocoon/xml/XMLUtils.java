/*****************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.        *
 * ------------------------------------------------------------------------- *
 * This software is published under the terms of the Apache Software License *
 * version 1.1, a copy of which has been included  with this distribution in *
 * the LICENSE file.                                                         *
 *****************************************************************************/
package org.apache.cocoon.xml;

import java.util.ArrayList;

import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;
import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;

/**
 * XML utility methods
 *
 * @author <a href="mailto:barozzi@nicolaken.com">Nicola Ken Barozzi</a>
 * @created 18 January 2002
 */
public class XMLUtils{

   //using parent because some dom implementations like jtidy are bugged, 
   //cannot get parent or delete child
   public static void stripDuplicateAttributes(Node node, Node parent) {
        // The output depends on the type of the node
        switch(node.getNodeType()) {
        case Node.DOCUMENT_NODE: {       
            Document doc = (Document)node;
            Node child = doc.getFirstChild();   
            while(child != null) {              
                stripDuplicateAttributes(child, node);          
                child = child.getNextSibling(); 
            }
            break;
        } 
        case Node.ELEMENT_NODE: {        
            Element elt = (Element) node;
            NamedNodeMap attrs = elt.getAttributes();  
           
            ArrayList nodesToRemove = new ArrayList();
            int nodesToRemoveNum = 0;
            
            for(int i = 0; i < attrs.getLength(); i++) {  
                Node a = attrs.item(i);
        
              for(int j = 0; j < attrs.getLength(); j++) {  
                  Node b = attrs.item(j);
                     
                  //if there are two attributes with same name
                  if(i!=j&&(a.getNodeName().equals(b.getNodeName())))
                  {
                    nodesToRemove.add(b);
                    nodesToRemoveNum++;
                  }

              }
   
            }
            
            for(int i=0;i<nodesToRemoveNum;i++)
            {
              org.w3c.dom.Attr nodeToDelete = (org.w3c.dom.Attr) nodesToRemove.get(i);
              org.w3c.dom.Element nodeToDeleteParent =  (org.w3c.dom.Element)node; //nodeToDelete.getParentNode();          
              nodeToDeleteParent.removeAttributeNode(nodeToDelete);
            }
            
            nodesToRemove.clear();

            Node child = elt.getFirstChild();        
            while(child != null) {                      
                stripDuplicateAttributes(child, node);                
                child = child.getNextSibling();        
            }

            break;
        }
        default:   
            //do nothing
            break;
        }
    }
}
