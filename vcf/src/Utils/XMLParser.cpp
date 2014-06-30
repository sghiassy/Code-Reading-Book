//XMLParser.h

#include "FoundationKit.h"

#include "XMLParser.h"
#include <algorithm>


using namespace VCF;

String XMLAttr::toString()
{
	String result = " " + getName() + "=\"" + getValue() + "\" "; 
	return result;
}


XMLNode::XMLNode( const String& name, XMLNode* parentNode )
{
	m_name = name;
	m_parentNode = parentNode;
	m_attrsContainer.initContainer( m_attrs );
	m_childNodesContainer.initContainer( m_childNodes );
}


XMLNode::XMLNode( const XMLNode& node ) 
{
	m_name = node.m_name;
	m_parentNode = node.m_parentNode;
	m_CDATA = node.m_CDATA;
	m_attrs = node.m_attrs;
	m_childNodes = node.m_childNodes;
	m_attrsContainer.initContainer( m_attrs );
	m_childNodesContainer.initContainer( m_childNodes );
}

XMLNode::~XMLNode()
{
	clearChildNodes();
}

XMLAttr* XMLNode::getAttrByName( const String& name )
{
	XMLAttr* result = NULL;
	std::vector<XMLAttr>::iterator it = m_attrs.begin();
	while ( it != m_attrs.end() ){
		if ( name == (*it).getName() ) {
			result = &(*it);
			break;
		}
		it ++;
	}
	return result;
}

XMLAttr* XMLNode::getAttrByIndex( const long& index )
{
	XMLAttr* result = NULL;
	if ( index < m_attrs.size() ) {
		result = &m_attrs.at(index);
	}
	return result;
}

void XMLNode::removeNode( XMLNode* node )
{
	std::vector<XMLNode*>::iterator found = std::find( m_childNodes.begin(), m_childNodes.end(), node );
	if ( found != m_childNodes.end() ){
		m_childNodes.erase( found );
	}
}

XMLNode* XMLNode::getNodeByName( const String& name )
{
	XMLNode* result = NULL;
	
	std::vector<XMLNode*>::iterator it = m_childNodes.begin();
	while ( it != m_childNodes.end() ) {
		XMLNode* node = *it;
		if ( node->getName() == name ) {
			result = node;
			break;
		}
		it ++;
	}

	return result;
}

XMLNode* XMLNode::getNodeByIndex( const long& index )
{
	XMLNode* result = NULL;
	if ( (index >= 0) && (index < m_childNodes.size()) ) {
		result = m_childNodes[index];
	}

	return result;
}

void XMLNode::clearChildNodes()
{
	std::vector<XMLNode*>::iterator it = m_childNodes.begin();
	while ( it != m_childNodes.end() ) {
		XMLNode* node = *it;
		delete node;
		node = NULL;
		it ++;
	}
}

long XMLNode::getDepth()
{
	long result = 0;

	XMLNode* parent = getParentNode();
	while ( NULL != parent ) {
		result ++;
		parent = parent->getParentNode();
	}
		
	return result;
}

String XMLNode::toString()
{
	String result;

	String tab;
	int tabsize = getDepth();
	for ( int i=0;i<tabsize;i++) {
		tab += "\t";
	}
	String nodeString = tab + "<" + getName() + " ";
	result += nodeString;

	std::vector<XMLAttr>::iterator attrIt = m_attrs.begin();
	while ( attrIt != m_attrs.end() ){
		XMLAttr* attr = &(*attrIt);
		result += attr->toString();
		attrIt ++;
	}
	
	if ( true == m_childNodes.empty() ) {
		nodeString = "/>\n";
		result += nodeString;
	}
	else {
		nodeString = ">\n";
		result += nodeString;		

		std::vector<XMLNode*>::iterator nodeIt = m_childNodes.begin();
		while ( nodeIt != m_childNodes.end() ) {
			XMLNode* node = *nodeIt;
			result += node->toString();	
			nodeIt ++;
		}
		nodeString = tab + "</" + getName() + ">\n";
		result += nodeString;
	}

	return result;
}





XMLParser::XMLParser()
{
	INIT_EVENT_HANDLER_LIST(NodeFound);
	INIT_EVENT_HANDLER_LIST(NodeCDATAFound);

	m_parsedNodesContainer.initContainer( m_parsedNodes );
	m_currentNode = NULL;
}

XMLParser::~XMLParser()
{
	clearNodes();
}

Enumerator<XMLNode*>* XMLParser::getParsedNodes()
{
	return m_parsedNodesContainer.getEnumerator();
}

void XMLParser::clearNodes()
{
	std::vector<XMLNode*>::iterator it = m_parsedNodes.begin();
	while ( it != m_parsedNodes.end() ) {
		XMLNode* node = *it;
		delete node;
		node = NULL;
		it ++;
	}
	m_parsedNodes.clear();
}

void XMLParser::parse( const String& xmlString ) 
{
	m_currentNode = NULL;
	clearNodes();
	if ( ! xmlString.empty() ) {
		m_sourcePtr = xmlString.c_str();
		m_xmlBufferStart = m_sourcePtr;
		m_sourceSize = xmlString.size();
		
		while ( nextNode() ) {
			
		}
	}
}

void XMLParser::parse( InputStream* stream )
{	
	int sz = stream->getSize();

	if ( sz == 0 ) { //nothing to parse
		return;
	}

	VCFChar* tmpBuffer = new VCFChar[sz+1];
	memset( tmpBuffer, 0, sz+1);
	
	stream->seek( 0, SEEK_FROM_START );
	stream->read( tmpBuffer, sz );

	String xmlText = tmpBuffer;
	
	delete [] tmpBuffer;

	parse( xmlText );
}

bool XMLParser::nextNode() 
{		
	if ( m_sourceSize <= (m_sourcePtr - m_xmlBufferStart) ) {
		return false;
	}
	
	const VCFChar* P = m_sourcePtr;
	const VCFChar* tokenStart;
	while ( (*P != 0) && ((' ' == *P) || (13 == *P) || (10 == *P) || (9 == *P)) ) {
		P++;
	}
	
	m_tokenPtr = P;
	
	switch( *P ) {

		case XMLParser::TagOpen : {
			P++;
			tokenStart = P;
			//check for comments				
			if ( *P == XMLParser::TagComment ) {
				String commentTag;
				commentTag.append( P, 3 );	 
				if ( commentTag == "!--" ) {
					P += 3;
					const VCFChar* endComments = parseComments( P );
					P = endComments;
					P++;
					m_sourcePtr = P;
					return true;
				}
				else {
					return false; //invalid XML
				}
			}
			else {
				
				while ( (*P != XMLParser::TagClose) && (*P != 0) ) {
					P++;
				}
				
				//check for end tag
				const VCFChar* endTag = P;
				endTag--;
				bool endTagFound = false;
				if ( *endTag == XMLParser::TagEnd ) {
					endTagFound = true;
					P --; //back up our position
				}
				
				if ( *tokenStart != XMLParser::TagEnd ) {
					//parse the node for attributes
					
					XMLNode* node = new XMLNode("", m_currentNode ); 

					m_parsedNodes.push_back( node );
					m_currentNode = node;
					parseNode( tokenStart, P );				
				}
				//skip past the closing node
				P++; 
				if ( true == endTagFound ) {
					P++; 
					m_sourcePtr = P;
					if ( NULL != m_currentNode ) {
						m_currentNode = m_currentNode->getParentNode();
					}
					return true;
				}
				else {
					if ( *tokenStart == XMLParser::TagEnd ) {
						m_sourcePtr = P;
						if ( NULL != m_currentNode ) {
							m_currentNode = m_currentNode->getParentNode();
						}
						return true;
					}
				}
			}
		}
		break;

		default : {
			m_tokenString = "";
			//check for character data
			while ( (*P != '<') && ( (P - m_xmlBufferStart ) < m_sourceSize) ) {
				P++;
			}

			m_tokenString.append( m_tokenPtr, P - m_tokenPtr );
			if ( NULL != m_currentNode ) {
				m_currentNode->setCDATA( m_tokenString );
				XMLParserEvent event( this, m_currentNode );
				fireOnNodeCDATAFound( &event );
			}
		}
		break;

	}
	
	m_sourcePtr = P;
	return m_sourceSize > ( m_sourcePtr - m_xmlBufferStart );
}

const VCFChar* XMLParser::parseComments( const VCFChar* commentPtrStart ) 
{
	const VCFChar* result = NULL;
	bool commentsFinished = false;
	while ( (false == commentsFinished) && (m_sourceSize > ( commentPtrStart - m_xmlBufferStart )) ) {
		switch ( *commentPtrStart ) {
			case '-' : {
				const VCFChar* tmp = commentPtrStart;
				tmp++;
				if ( *tmp == '-' ) {
					tmp ++;
					if ( *tmp == XMLParser::TagClose ) {
						//poof - Elvis has left the building ! comments are finished
						commentPtrStart = tmp;
						commentsFinished = true;
					}
				}
			}
			break;
		}
		commentPtrStart++;
	}

	result = commentPtrStart;

	return result;
}

void XMLParser::parseNode( const VCFChar* nodePtrStart, const VCFChar* nodePtrEnd ) 
{				
	const VCFChar* P = nodePtrStart;
	
	while ( (*P != ' ') && (P < nodePtrEnd)  ) {
		P++;
	}
	
	String s;	
	s.append( nodePtrStart, P - nodePtrStart );
	m_currentNode->setName( s );	

	while ( (*P == ' ') && (P < nodePtrEnd)  ) {
		P++;
	}

	parseAttrs( P, nodePtrEnd );

	XMLParserEvent event( this, m_currentNode );
	fireOnNodeFound( &event );
}

void XMLParser::parseAttrs( const VCFChar* attrPtrStart, const VCFChar* attrPtrEnd ) 
{
	m_currentNode->clearAttrs();

	const VCFChar* P = attrPtrStart;

	bool nextAttr = P < attrPtrEnd;
	while ( true == nextAttr ) {
		const VCFChar* nameStart = P;
		while ( (*P != ' ') && (*P != XMLParser::TagEquals) && (P < attrPtrEnd)  ) {
			P++;
		}
		String name;
		String value;
		name.append( nameStart, P - nameStart );

		bool assignmentOperatorFound = false;
		switch ( *P ) {
			case XMLParser::TagEquals : {
				P++;
				assignmentOperatorFound = true;
			}
			break;

			case 9 : case ' ' : case 10 : case 13: {
				while ( ((' ' == *P) || (13 == *P) || (10 == *P) || (9 == *P)) && (P < attrPtrEnd)  ) {
					P++;
				}
				if ( *P == XMLParser::TagEquals ) {
					P++;	
				}
				assignmentOperatorFound = true;
			}
			break;
		}

		if ( true == assignmentOperatorFound ) {				
			//goto the first quote
			while ( (*P != XMLParser::TagQuote) && (P < attrPtrEnd)  ) {
				P++;
			}
			if ( *P != XMLParser::TagQuote ) {
				throw RuntimeException( "Malformed XML Attribute - string literal expected, but no beginning quote (\") found." );	
			}
			P++;
			const VCFChar* valueStart = P;

			//goto end quote
			while ( (*P != XMLParser::TagQuote) && (P < attrPtrEnd)  ) {
				P++;
			}

			if ( *P != XMLParser::TagQuote ) {
				throw RuntimeException( "Malformed XML Attribute - string literal expected, but no beginning quote (\") found." );	
			}

			value.append( valueStart, P - valueStart );

			m_currentNode->addAttr( XMLAttr(name,value,m_currentNode) );

			P++;

			//get rid of the white space
			while ( (*P == ' ') && (P < attrPtrEnd)  ) {
				P++;
			}
		}
		else {
			throw RuntimeException( "Malformed XML Attribute" );
		}			
		
		nextAttr = P < attrPtrEnd;
	}
}
