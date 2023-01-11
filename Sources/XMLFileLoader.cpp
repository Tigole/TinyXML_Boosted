#include "XMLFileLoader.hpp"
#include "tinyxml.h"
#include <unordered_map>
#include <vector>

XML_Element::XML_Element(const TiXmlElement& target)
 :	m_target(&target)
{}

bool XML_Element::mt_Get_Attribute(const std::string& attribute_name, std::string& attribute_value) const
{
	return m_target->QueryStringAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::mt_Get_Attribute(const std::string& attribute_name, int& attribute_value) const
{
	return m_target->QueryIntAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::mt_Get_Attribute(const std::string& attribute_name, unsigned int& attribute_value) const
{
	return m_target->QueryUnsignedAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::mt_Get_Attribute(const std::string& attribute_name, float& attribute_value) const
{
	return m_target->QueryFloatAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::mt_Get_Attribute(const std::string& attribute_name, bool& attribute_value) const
{
	return m_target->QueryBoolAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::mt_Get_Text(std::string& element_value) const
{
    element_value = "";
    if (m_target->Value() != nullptr)
    {
        element_value = m_target->ValueStr();
    }
	return true;
}






XMLFileLoader::XMLFileLoader() :
    m_File_Data(),
    m_Loading_Struct(),
    m_File_Path(),
	m_Progression_Callback()
{}

bool XMLFileLoader::mt_Load(const std::string& file_path)
{
	TiXmlDocument l_document;

	if (mt_Count_File_Elements() == false)
    {
        return false;
    }

    if (l_document.LoadFile(file_path))
    {
        return mt_Explore_Document(*l_document.RootElement(), m_Loading_Struct, m_File_Data.m_On_Entry_Callbacks, m_File_Data.m_On_Exit_Callbacks);
    }
    std::cerr << l_document.ErrorDesc() << '\n';

	return false;
}

bool XMLFileLoader::mt_Count_File_Elements(void)
{
	LoadingStructure l_loading_struct;
	TiXmlDocument l_document;
	TiXmlElement* l_root;
	XML_FileHandlerData l_xml_file_handler_data;

	if (l_document.LoadFile(m_File_Path) == true)
	{
		l_root = l_document.RootElement();
		if (l_root != nullptr)
		{
            bool l_b_ret = mt_Explore_Document(*l_root, l_loading_struct, l_xml_file_handler_data.m_On_Entry_Callbacks, l_xml_file_handler_data.m_On_Exit_Callbacks);
            if (l_b_ret == true)
            {
                m_File_Data.m_Element_Count = l_loading_struct.m_Element_Count;

                return true;
            }
		}
		else
        {
            std::cerr << "No root\n";
        }
	}
	else
    {
        std::cerr << l_document.ErrorDesc() << '\n';
    }

    return false;
}

bool XMLFileLoader::mt_Manage_Callback(const TiXmlElement& element, const std::string& path, XML_CallbackContainer& callbacks)
{
	bool l_b_ret(true);
	XML_CallbackContainer::const_iterator l_it(callbacks.find(path));

	if (l_it != callbacks.end())
	{
		l_b_ret = (l_it->second)(XML_Element(element));
	}

	return l_b_ret;
}

std::string XMLFileLoader::mt_Get_Path(const TiXmlNode* element)
{
	std::string l_ret;
	std::stack<const TiXmlNode*> l_nodes;

	while (element != nullptr)
	{
		l_nodes.push(element);
		element = element->Parent();
	}

	l_nodes.pop();/** Remove document : we don't care **/

	while (l_nodes.size())
	{
		l_ret += "/" + l_nodes.top()->ValueStr();
		l_nodes.pop();
	}

	return l_ret;
}

bool XMLFileLoader::mt_Recursive_Exploration(const TiXmlElement* current_element, LoadingStructure& loading_struct, XML_CallbackContainer& on_entry_callbacks, XML_CallbackContainer& on_exit_callbacks)
{
	bool l_b_ret;
	std::string l_path;

	l_b_ret = true;
	for (const TiXmlElement* l_Element = current_element->FirstChildElement(); (l_Element != nullptr) && (l_b_ret == true); l_Element = l_Element->NextSiblingElement())
    {
        l_path = mt_Get_Path(l_Element);
        l_b_ret = mt_Manage_Callback(*l_Element, l_path, on_entry_callbacks);

        if (l_b_ret == true)
        {
            l_b_ret = mt_Recursive_Exploration(l_Element, loading_struct, on_entry_callbacks, on_exit_callbacks);
        }

        if (l_b_ret == true)
        {
            l_b_ret = mt_Manage_Callback(*l_Element, l_path, on_exit_callbacks);
        }

        loading_struct.m_Element_Count++;
    }
	for (std::size_t ii = 0; ii < m_Progression_Callback.size(); ii++)
    {
        m_Progression_Callback[ii](m_File_Path,
                                   static_cast<int>(loading_struct.m_Element_Count),
                                   static_cast<int>(m_File_Data.m_Element_Count));
    }

    return l_b_ret;
}

bool XMLFileLoader::mt_Explore_Document(TiXmlElement& root, LoadingStructure& loading_struct, XML_CallbackContainer& on_entry_callbacks, XML_CallbackContainer& on_exit_callbacks)
{
	bool l_ret(true);

	l_ret = mt_Recursive_Exploration(&root, loading_struct, on_entry_callbacks, on_exit_callbacks);

	return l_ret;
}
