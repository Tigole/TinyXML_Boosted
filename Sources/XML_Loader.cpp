#include "XML_Loader.hpp"

#include <tinyxml.h>

#include <unordered_map>
#include <vector>

XML_Element::XML_Element(const TiXmlElement& target) : m_target(&target) {}

bool XML_Element::Get_XML_Attribute(const std::string& attribute_name, std::string& attribute_value) const
{
    return m_target->QueryStringAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::Get_XML_Attribute(const std::string& attribute_name, int& attribute_value) const
{
    return m_target->QueryIntAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::Get_XML_Attribute(const std::string& attribute_name, unsigned int& attribute_value) const
{
    return m_target->QueryUnsignedAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::Get_XML_Attribute(const std::string& attribute_name, float& attribute_value) const
{
    return m_target->QueryFloatAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::Get_XML_Attribute(const std::string& attribute_name, bool& attribute_value) const
{
    return m_target->QueryBoolAttribute(attribute_name.c_str(), &attribute_value) == TIXML_SUCCESS;
}

bool XML_Element::Get_XML_Text(std::string& element_value, bool maybe_empty) const
{
    element_value = "";

    if (m_target->GetText() != nullptr)
    {
        element_value = m_target->GetText();
        return true;
    }

    return maybe_empty;
}

XML_Loader::XML_Loader() : m_File_Data(), m_Loading_Struct(), m_File_Path(), m_Progression_Callback() {}

bool XML_Loader::Load_From_File(const std::string& file_path)
{
    TiXmlDocument l_document;
    m_File_Path = file_path;
    l_document.LoadFile(m_File_Path);
    return Load(l_document);
}

bool XML_Loader::Load_From_Memory(const std::string& string_content)
{
    return Load_From_Memory(string_content.c_str());
}

bool XML_Loader::Load_From_Memory(const char* string_content)
{
    TiXmlDocument l_document;
    m_File_Path = "<memory>";
    l_document.Parse(string_content);
    return Load(l_document);
}

bool XML_Loader::Load(TiXmlDocument& document)
{
    if (document.Error() == true)
    {
        m_Error_Description = document.ErrorDesc();
        return false;
    }

    if (Count_File_Elements(document) == false)
    {
        m_Error_Description = "Empty document";
        return false;
    }

    return Explore_Document(*document.RootElement(), m_Loading_Struct, m_File_Data.m_On_Entry_Callbacks, m_File_Data.m_On_Exit_Callbacks);
}

bool XML_Loader::Count_File_Elements(TiXmlDocument& document)
{
    LoadingStructure l_loading_struct;
    TiXmlElement* l_root;
    XML_FileHandlerData l_xml_file_handler_data;

    l_root = document.RootElement();
    if (l_root != nullptr)
    {
        bool l_b_ret = Explore_Document(*l_root, l_loading_struct, l_xml_file_handler_data.m_On_Entry_Callbacks,
                                        l_xml_file_handler_data.m_On_Exit_Callbacks);
        if (l_b_ret == true)
        {
            m_File_Data.m_Element_Count = l_loading_struct.m_Element_Count;

            return true;
        }
    }

    return false;
}

bool XML_Loader::Manage_Callback(const TiXmlElement& element, const std::string& path, XML_CallbackContainer& callbacks)
{
    bool l_b_ret(true);
    XML_CallbackContainer::const_iterator l_it(callbacks.find(path));

    if (l_it != callbacks.end())
    {
        l_b_ret = (l_it->second)(XML_Element(element));

        if (l_b_ret == false)
        {
            m_Error_Description =
                m_File_Path + ": Failed loading element at l." + std::to_string(element.Row()) + " c." + std::to_string(element.Column());
        }
    }

    return l_b_ret;
}

std::string XML_Loader::Get_Path(const TiXmlNode* element)
{
    std::string l_ret;
    std::stack<const TiXmlNode*> l_nodes;

    while (element != nullptr)
    {
        l_nodes.push(element);
        element = element->Parent();
    }

    l_nodes.pop(); /** Remove document : we don't care **/

    while (l_nodes.size())
    {
        l_ret += "/" + l_nodes.top()->ValueStr();
        l_nodes.pop();
    }

    return l_ret;
}

bool XML_Loader::Recursive_Exploration(const TiXmlElement* current_element, LoadingStructure& loading_struct,
                                       XML_CallbackContainer& on_entry_callbacks, XML_CallbackContainer& on_exit_callbacks)
{
    bool l_b_ret;
    std::string l_path;

    l_b_ret = true;
    for (const TiXmlElement* l_Element = current_element->FirstChildElement(); (l_Element != nullptr) && (l_b_ret == true);
         l_Element                     = l_Element->NextSiblingElement())
    {
        l_path  = Get_Path(l_Element);
        l_b_ret = Manage_Callback(*l_Element, l_path, on_entry_callbacks);

        if (l_b_ret == true)
        {
            l_b_ret = Recursive_Exploration(l_Element, loading_struct, on_entry_callbacks, on_exit_callbacks);
        }

        if (l_b_ret == true)
        {
            l_b_ret = Manage_Callback(*l_Element, l_path, on_exit_callbacks);
        }

        loading_struct.m_Element_Count++;
    }
    for (std::size_t ii = 0; ii < m_Progression_Callback.size(); ii++)
    {
        m_Progression_Callback[ii](m_File_Path, static_cast<int>(loading_struct.m_Element_Count),
                                   static_cast<int>(m_File_Data.m_Element_Count));
    }

    return l_b_ret;
}

bool XML_Loader::Explore_Document(TiXmlElement& root, LoadingStructure& loading_struct, XML_CallbackContainer& on_entry_callbacks,
                                  XML_CallbackContainer& on_exit_callbacks)
{
    bool l_ret(true);

    l_ret = Manage_Callback(root, Get_Path(&root), on_entry_callbacks);
    if (l_ret == true)
    {
        l_ret = Recursive_Exploration(&root, loading_struct, on_entry_callbacks, on_exit_callbacks);
    }
    if (l_ret == true)
    {
        l_ret = Manage_Callback(root, Get_Path(&root), on_exit_callbacks);
    }

    return l_ret;
}
