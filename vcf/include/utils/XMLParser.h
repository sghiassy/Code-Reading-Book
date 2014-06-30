//XMLParser.h

#ifndef _XMLPARSER_H__
#define _XMLPARSER_H__



using namespace VCF;


namespace VCF  {

class XMLNode;


/**
*Class XMLAttr documentation
*/
class FRAMEWORK_API XMLAttr : public Object {
public:
	XMLAttr ( const String& name="", const String& value="", XMLNode* node=NULL ) {
		m_name = name;
		m_value = value;
		m_node = node;
	}
	
	XMLAttr ( const XMLAttr& attr ) {
		m_name = attr.m_name;
		m_value = attr.m_value;
		m_node = attr.m_node;
	}

	virtual ~XMLAttr(){};

	XMLAttr& operator = ( const XMLAttr& attr ) {
		m_name = attr.m_name;
		m_value = attr.m_value;
		return *this;
	}

	inline String getName() const {
		return m_name;
	}

	inline void setName( const String& name ) {
		m_name = name;
	}

	inline String getValue() const {
		return m_value;
	}

	inline void setValue( const String& value ) {
		m_value = value;
	}

	inline XMLNode* getNode() const {
		return m_node;
	}

	inline void setNode( XMLNode* node ) {
		m_node = node;
	}

	virtual String toString();

protected:
	String m_name;
	String m_value;
	XMLNode* m_node;
};



/**
*Class XMLNode represents a single node in a
*parsed XML document.
*Nodes have a name, parent node, and 0 or more 
*attributes represented as XMLAttr(s). In addition 
*the node may have a string of characters that 
*represent character data (CDATA)
*/
class FRAMEWORK_API XMLNode : public Object {
public:
	XMLNode( const String& name="", XMLNode* parentNode=NULL );

	XMLNode( const XMLNode& node );

	virtual ~XMLNode();

	/**
	*gets the name of the node
	*/
	inline String getName() {
		return m_name;
	}

	/**
	*sets the name of the node. Generally done during parsing
	*/
	inline void setName( const String& name ) {
		m_name = name;
	}

	/**
	*gets the parent node of this node. May
	*return NULL if this is a root, or
	*top level node of the XML document
	*@return XMLNode the parent node. NULL if this 
	*is a root node.
	*/
	inline XMLNode* getParentNode() {
		return m_parentNode;
	}

	/**
	*sets the parent node. Generally called by the parser
	*during the parsing phase of the XML document
	*/
	inline void setParentNode( XMLNode* parentNode ) {
		m_parentNode = parentNode;
	}

	/**
	*gets the collection of attributes for this node
	*@return Enumerator<XMLAttr>* an enumerator of XMLAttr
	*objects
	*/
	inline Enumerator<XMLAttr>* getAttrs() {
		return m_attrsContainer.getEnumerator();
	}

	XMLAttr* getAttrByName( const String& name );

	XMLAttr* getAttrByIndex( const long& index );

	/**
	*adds a new attribute to the node. Generally called by 
	*parser.
	*/
	inline void addAttr( const XMLAttr& attr ) {
		m_attrs.push_back( attr );
	}
	
	/**
	*adds a new attribute to the node. Generally called by 
	*parser.
	*/
	inline void addAttr( const String& name, const String& value ) {
		XMLAttr attr( name, value, this );
		m_attrs.push_back( attr );
	}


	/**
	*clears all the current attributes
	*/
	inline void clearAttrs() {
		m_attrs.clear();
	}

	/**
	*returns the character data associated with this node.
	*/
	inline String getCDATA() {
		return m_CDATA;
	}
	
	/**
	*sets the character data for this node.
	*/
	inline void setCDATA( const String& CDATA ) {
		m_CDATA = CDATA;
	}

	inline void addChildNode( XMLNode* node ) {
		node->setParentNode( this );
		m_childNodes.push_back( node );
	}

	inline XMLNode* addChildNode( const String& nodeName ) {
		XMLNode* node = new XMLNode( nodeName, this );
		m_childNodes.push_back( node );
		return node;
	}

	void removeNode( XMLNode* node );

	XMLNode* getNodeByName( const String& name );

	XMLNode* getNodeByIndex( const long& index );

	inline Enumerator<XMLNode*>* getChildNodes() {
		return 	m_childNodesContainer.getEnumerator();
	};

	void clearChildNodes();

	virtual String toString();
protected:
	String m_CDATA;
	String m_name;
	XMLNode* m_parentNode;
	
	std::vector<XMLNode*> m_childNodes;
	EnumeratorContainer<std::vector<XMLNode*>,XMLNode*> m_childNodesContainer;

	std::vector<XMLAttr> m_attrs;
	EnumeratorContainer<std::vector<XMLAttr>,XMLAttr> m_attrsContainer;
	long getDepth();
};


/**
*Class XMLParserEvent documentation
*/
class FRAMEWORK_API XMLParserEvent : public Event {
public:
	XMLParserEvent( Object * source, XMLNode* node ):
		Event(source){
		m_node = node;
	};

	
	virtual ~XMLParserEvent(){};

	/**
	*gets the XMLNode for this event
	*/
	XMLNode* getNode() {
		return m_node;
	}
protected:

	XMLNode* m_node;

};

/**
*Class XMLParserHandler documentation
*/
template <class SOURCE_TYPE> class FRAMEWORK_API XMLParserHandler : public EventHandlerInstance<SOURCE_TYPE,XMLParserEvent> {
public:
	XMLParserHandler( SOURCE_TYPE* source, 
		EventHandlerInstance<SOURCE_TYPE,XMLParserEvent>::OnEventHandlerMethod handlerMethod, 
		const String& handlerName="") :
			EventHandlerInstance<SOURCE_TYPE,XMLParserEvent>( source, handlerMethod, handlerName ) {
			
	}

	virtual ~XMLParserHandler(){};
	
};

/**
*XMLParser is a very simple non-validating XML parser. Currently 
*only really handles basic XML documents. Does <I><B>not</B></I>
*handle parsing DTDs yet. Allows outside objects to be notified of 
*parsing events as the XML document is parsed. Events are:
*<ul>
*<li>
*NodeFound events. These are fired whenever a new node is discovered. The node
*is created, it's attributes are parsed, and then the event is fired. When receiving this
*event you can safely assume that any attributes for the node will be correctly filled 
*out at this point, as well as the node having a valid parent node. CDATA may not be filled out 
*at this point, there is a seperate event for it.
*</li>
*<li>
*NodeCDATAFound events. These are fired off when character data is found. 
*The current node's CDATA is set, and then the event is fired. Handlers 
*may safely assume that the node that the event object points to has valid 
*CDATA.
*</li>
*</ul>
*When parsing XML, all XML comments are currently ignored.
*@author Jim Crafton
*@version 1.0
*/
class FRAMEWORK_API XMLParser : public ObjectWithEvents { 
public:
	enum XMLTagType {
		TagCDATA	=	-3,
		TagComment	=	'!',
		TagSymbol	=	-1,
		TagOpen		=	'<',
		TagClose	=	'>',
		TagEnd		=	'/',
		TagEquals	=	'=',
		TagQuote	=	'\"'
	};

	XMLParser();

	virtual ~XMLParser();

	EVENT_HANDLER_LIST(NodeFound);
	ADD_EVENT(NodeFound);
	REMOVE_EVENT(NodeFound);
	FIRE_EVENT(NodeFound,XMLParserEvent);

	EVENT_HANDLER_LIST(NodeCDATAFound);
	ADD_EVENT(NodeCDATAFound);
	REMOVE_EVENT(NodeCDATAFound);
	FIRE_EVENT(NodeCDATAFound,XMLParserEvent);


	/**
	*tries to parse and process the next node. 
	*@return bool true if successful, false if the parsing failed.
	*/
	bool nextNode();

	/**
	*parses the XML data represented by the xmlString argument.
	*@param String the XML data to parse
	*/
	void parse( const String& xmlString );

	void parse( InputStream* stream );

	Enumerator<XMLNode*>* getParsedNodes();
protected:

	const VCFChar* m_tokenPtr;
	const VCFChar* m_sourcePtr;
	const VCFChar* m_xmlBufferStart;
	long m_sourceSize;
	String m_tokenString;	
	
	XMLTagType m_token;
	
	XMLNode* m_currentNode;

	std::vector<XMLNode*> m_parsedNodes;
	EnumeratorContainer<std::vector<XMLNode*>,XMLNode*> m_parsedNodesContainer;

	const VCFChar* parseComments( const VCFChar* commentPtrStart );

	void parseNode( const VCFChar* nodePtrStart, const VCFChar* nodePtrEnd );

	void parseAttrs( const VCFChar* attrPtrStart, const VCFChar* attrPtrEnd );

	void clearNodes();
private:
};


}; //end of namespace VCF

#endif //_XMLPARSER_H__


