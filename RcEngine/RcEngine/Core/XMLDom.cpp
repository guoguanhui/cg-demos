#include "Core/XMLDom.h"
#include "Core/Utility.h"
#include "Core/Exception.h"

namespace RcEngine{
namespace Core{

XMLNodePtr XMLNode::NullSharedPtr;
XMLAttributePtr XMLAttribute::NullSharedPtr;
	
rapidxml::node_type MapToRapidxml(XMLNodeType type)
{
	switch(type)
	{
	case XML_Node_Document:
		return rapidxml::node_document;
	case XML_Node_Element:
		return rapidxml::node_element;
	case XML_Node_Data:
		return rapidxml::node_data;
	case XML_Node_CData:
		return rapidxml::node_cdata;
	case XML_Node_Comment:
		return rapidxml::node_comment;
	case XML_Node_Declaration:
		return rapidxml::node_declaration;
	case XML_Node_Doctype:
		return rapidxml::node_doctype;
	case XML_Node_PI:
		return rapidxml::node_pi;
	}

	return rapidxml::node_pi;
}

XMLNodeType UnMapToRapidxml(rapidxml::node_type type)
{
	switch(type)
	{
	case rapidxml::node_document:
		return XML_Node_Document;
	case rapidxml::node_element:
		return XML_Node_Element;
	case rapidxml::node_data:
		return XML_Node_Data;
	case rapidxml::node_cdata:
		return XML_Node_CData;
	case rapidxml::node_comment:
		return XML_Node_Comment;
	case rapidxml::node_declaration:
		return XML_Node_Declaration;
	case rapidxml::node_doctype:
		return XML_Node_Doctype;
	case rapidxml::node_pi:
		return XML_Node_PI;
	}

	return XML_Node_PI;
}


XMLDocument::XMLDocument()
	: mDocument(std::make_shared<rapidxml::xml_document<> >())
{

}

XMLNodePtr XMLDocument::Parse( const std::string& xmlName )
{
	FILE* pFile = NULL;
	int len = 0;

	fopen_s(&pFile, xmlName.c_str() , "r");
	if (pFile == NULL) 
	{
		std::string str = "FILE: " + xmlName + "does't exit.";
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, str, "Application::Initialize" );
	}
	fseek (pFile , 0 , SEEK_END);
	len = static_cast<int>( ftell(pFile) );
	fseek(pFile, 0, SEEK_SET);
	mXMLSrc.resize(len+1, 0);
	fread_s(&mXMLSrc[0], len, sizeof(char), len, pFile);
	fclose(pFile);

	mDocument->parse<0>(&mXMLSrc[0]);
	mRoot = std::make_shared<XMLNode>(mDocument->first_node());

	return mRoot;

}

void XMLDocument::Print( std::ostream& os )
{
	os << "<?xml version=\"1.0\"?>" << std::endl; 
	os << *static_cast<rapidxml::xml_document<>*>(mDocument.get());
}

XMLNodePtr XMLDocument::AllocateNode( XMLNodeType type, const std::string& name )
{
	return std::make_shared<XMLNode>(mDocument.get(), type, name);
}

XMLAttributePtr XMLDocument::AllocateAttributeInt( const std::string& name, int32 value )
{
	return AllocateAttributeString(name, LexicalCast<std::string>(value));
}

XMLAttributePtr XMLDocument::AllocateAttributeUInt(const std::string& name, uint32 value)
{
	return AllocateAttributeString(name, LexicalCast<std::string>(value));
}

XMLAttributePtr XMLDocument::AllocateAttributeFloat(const std::string& name, float value)
{
	return AllocateAttributeString(name, LexicalCast<std::string>(value));
}

XMLAttributePtr XMLDocument::AllocateAttributeString(const std::string& name, const std::string& value)
{
	return std::make_shared<XMLAttribute>(mDocument.get(), name, value);
}

void XMLDocument::RootNode( const XMLNodePtr& newNode )
{
	mDocument->remove_all_nodes();
	mDocument->append_node(newNode->mNode);
	mRoot = newNode;
}



XMLNode::XMLNode( rapidxml::xml_node<>* node )
	: mNode(node)
{
	if(mNode != NULL)
	{
		mName = std::string(mNode->name(), mNode->name_size()); 
	}
}

XMLNode::XMLNode( rapidxml::xml_document<>* doc, XMLNodeType type, const std::string& name )
{
	rapidxml::node_type xtype = MapToRapidxml(type);
	mNode = doc->allocate_node(MapToRapidxml(type), name.c_str());
}

const std::string& XMLNode::NodeName() const
{
	return mName;
}

XMLNodeType XMLNode::NodeType() const
{
	return UnMapToRapidxml(mNode->type());
}

XMLAttributePtr XMLNode::FirstAttribute( const std::string& name  )
{
	rapidxml::xml_attribute<> *attr = mNode->first_attribute(name.c_str());
	if(attr)
	{
		return std::make_shared<XMLAttribute>(attr);
	}
	else
	{
		return XMLAttribute::NullSharedPtr;
	}
}

XMLAttributePtr XMLNode::FirstAttribute()
{
	rapidxml::xml_attribute<> *attr = mNode->first_attribute();
	if(attr)
	{
		return std::make_shared<XMLAttribute>(attr);
	}
	else
	{
		return XMLAttribute::NullSharedPtr;
	}
}

XMLAttributePtr XMLNode::LastAttribute( const std::string& name )
{
	rapidxml::xml_attribute<>* attr = mNode->last_attribute(name.c_str());
	if(attr)
	{
		return std::make_shared<XMLAttribute>(attr);
	}
	else
	{
		return XMLAttribute::NullSharedPtr;
	}
}

XMLAttributePtr XMLNode::LastAttribute()
{
	rapidxml::xml_attribute<>* attr = mNode->last_attribute();
	if(attr)
	{
		return std::make_shared<XMLAttribute>(attr);
	}
	else
	{
		return XMLAttribute::NullSharedPtr;
	}
}

XMLAttributePtr XMLNode::Attribute( const std::string& name )
{
	return FirstAttribute(name);
}

int32 XMLNode::AttributeInt( const std::string& name , int32 defaultVar  )
{
	XMLAttributePtr attr = Attribute(name);
	return attr ? attr->ValueInt() : defaultVar;
}

uint32 XMLNode::AttributeUInt( const std::string& name , uint32 defaultVar  )
{
	XMLAttributePtr attr = Attribute(name);
	return attr ? attr->ValueUInt() : defaultVar;
}

float XMLNode::AttributeFloat( const std::string& name , float defaultVar )
{
	XMLAttributePtr attr = Attribute(name);
	return attr ? attr->ValueFloat() : defaultVar;
}

std::string XMLNode::AttributeString( const std::string& name , std::string defaultVar )
{
	XMLAttributePtr attr = Attribute(name);
	return attr ? attr->ValueString() : defaultVar;
}

XMLNodePtr XMLNode::FirstNode( const std::string& name )
{
	rapidxml::xml_node<>* node = mNode->first_node(name.c_str());
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}

XMLNodePtr XMLNode::LastNode( const std::string& name )
{
	rapidxml::xml_node<>* node = mNode->last_node(name.c_str());
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}

XMLNodePtr XMLNode::FirstNode()
{
	rapidxml::xml_node<>* node = mNode->first_node();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}

XMLNodePtr XMLNode::LastNode()
{
	rapidxml::xml_node<>* node = mNode->last_node();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}	

XMLNodePtr XMLNode::PrevSibling( const std::string& name )
{
	rapidxml::xml_node<>* node = mNode->previous_sibling(name.c_str());
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}

XMLNodePtr XMLNode::PrevSibling()
{
	rapidxml::xml_node<>* node = mNode->previous_sibling();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}

XMLNodePtr XMLNode::NextSibling( const std::string& name )
{
	rapidxml::xml_node<>* node = mNode->next_sibling(name.c_str());
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}

XMLNodePtr XMLNode::NextSibling()
{
	rapidxml::xml_node<>* node = mNode->next_sibling();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}

void XMLNode::InsertNode( const XMLNodePtr& where, const XMLNodePtr& child )
{
	mNode->insert_node(where->mNode, child->mNode);

	// update child node position
	for(size_t i = 0; i < mChildren.size(); i++)
	{
		if(mChildren[i]->mNode == where->mNode)
		{
			mChildren.insert(mChildren.begin() + i, child);
		}
	}
}

void XMLNode::AppendNode( const XMLNodePtr& child )
{
	mNode->append_node(child->mNode);
	mChildren.push_back(child);
}

void XMLNode::AppendAttribute( const XMLAttributePtr& attribute )
{
	mNode->append_attribute(attribute->mAttribute);
	mAttributes.push_back(attribute);
}

void XMLNode::InsertAttribute( const XMLAttributePtr& where, const XMLAttributePtr& attribute )
{
	mNode->insert_attribute(where->mAttribute, attribute->mAttribute);

	// update attribute position
	for(size_t i = 0; i < mAttributes.size(); i++)
	{
		if(mAttributes[i]->mAttribute == where->mAttribute)
		{
			mAttributes.insert(mAttributes.begin() + i, attribute);
		}
	}
}

void XMLNode::RemoveFirstNode()
{
	if(mNode->first_node())
	{
		mNode->remove_first_node();
	}
	// erase first node
	mChildren.erase(mChildren.begin());
}

void XMLNode::RemoveLastNode()
{
	if(mNode->first_node())
	{
		mNode->remove_last_node();
	}

	// erase last node
	mChildren.pop_back();
}

void XMLNode::RemoveNode( const XMLNodePtr& where )
{
	mNode->remove_node(where->mNode);
	for(size_t i = 0; i < mChildren.size(); i++)
	{
		if(mChildren[i] == where)
		{
			mChildren.erase(mChildren.begin() + i);
			break;
		}
	}
}

void XMLNode::RemoveAllNodes()
{
	mNode->remove_all_nodes();
	mChildren.clear();
}

void XMLNode::RemoveFirstAttribute()
{
	if(mNode->first_attribute())
	{
		mNode->remove_first_attribute();
		mAttributes.erase(mAttributes.begin());
	}

}

void XMLNode::RemoveLastAttribute()
{
	if(mNode->first_attribute())
	{
		mNode->remove_last_attribute();
		mAttributes.erase(mAttributes.begin());
	}
}

void XMLNode::RemoveAttribute( const XMLAttributePtr& where )
{
	mNode->remove_attribute(where->mAttribute);

	// update attribute position
	for(size_t i = 0; i < mAttributes.size(); i++)
	{
		if(mAttributes[i] == where)
		{
			//mAttributes.erase(mAttributes.begin() + i, where);
		}
	}

}

void XMLNode::RemoveAllAttributes()
{
	mNode->remove_all_attributes();
	mAttributes.clear();
}

uint32 XMLNode::ValueUInt() const
{
	return LexicalCast<uint32>(ValueString());
}

int32 XMLNode::ValueInt() const
{
	return LexicalCast<int32>(ValueString());
}

float XMLNode::ValueFlaot() const
{
	return LexicalCast<float>(ValueString());
}

std::string XMLNode::ValueString() const
{
	return std::string(mNode->value(), mNode->value_size());
}

XMLNodePtr XMLNode::GetParent()
{
	rapidxml::xml_node<>* node = mNode->parent();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return XMLNode::NullSharedPtr;
	}
}

	
XMLAttribute::XMLAttribute( rapidxml::xml_attribute<>* attr )
	: mAttribute(attr)
{
	if(mAttribute)
	{
		mName = std::string(mAttribute->name(), mAttribute->name_size());
		mValue = std::string(mAttribute->value(), mAttribute->value_size());
	}
}


XMLAttribute::XMLAttribute( rapidxml::xml_document<>* doc, const std::string& name, const std::string& value )
	: mName(name), mValue(value)
{
	mAttribute = doc->allocate_attribute(name.c_str(), value.c_str());
}

const std::string& XMLAttribute::Name() const
{
	return mName;
}

XMLAttributePtr XMLAttribute::PrevAttribute( const std::string& name ) const
{
	rapidxml::xml_attribute<>* attr = mAttribute->previous_attribute(name.c_str());
	if(attr){
		return std::make_shared<XMLAttribute>(attr);
	}
	else{
		return XMLAttribute::NullSharedPtr;
	}
}

XMLAttributePtr XMLAttribute::PrevAttribute() const
{
	rapidxml::xml_attribute<>* attr = mAttribute->previous_attribute();
	if(attr){
		return std::make_shared<XMLAttribute>(attr);
	}
	else{
		return XMLAttribute::NullSharedPtr;
	}
}

XMLAttributePtr XMLAttribute::NextAttribute( const std::string& name ) const
{
	rapidxml::xml_attribute<>* attr = mAttribute->next_attribute(name.c_str());
	if(attr){
		return std::make_shared<XMLAttribute>(attr);
	}
	else{
		return XMLAttribute::NullSharedPtr;
	}
}

XMLAttributePtr XMLAttribute::NextAttribute() const
{
	rapidxml::xml_attribute<>* attr = mAttribute->next_attribute();
	if(attr){
		return std::make_shared<XMLAttribute>(attr);
	}
	else{
		return XMLAttribute::NullSharedPtr;
	}
}

uint32 XMLAttribute::ValueUInt() const
{
	return LexicalCast<uint32>(mValue);	
}

int32  XMLAttribute::ValueInt()    const
{
	return LexicalCast<int32>(mValue);
}

float  XMLAttribute::ValueFloat()  const
{
	return LexicalCast<float>(mValue);
}

std::string XMLAttribute::ValueString() const
{
	return mValue;
}

} // Namespace Core
} // Namespace RcEngine