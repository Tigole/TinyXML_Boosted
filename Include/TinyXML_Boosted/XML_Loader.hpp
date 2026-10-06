#ifndef _XML_FILE_LOADER_HPP
#define _XML_FILE_LOADER_HPP 1

#include <functional>
#include <map>
#include <stack>
#include <string>
#include <vector>

class TiXmlElement;
class TiXmlNode;
class TiXmlDocument;

class XML_Element
{
public:
    XML_Element(const TiXmlElement& target);
    bool Get_XML_Attribute(const std::string& attribute_name, std::string& attribute_value) const;
    bool Get_XML_Attribute(const std::string& attribute_name, int& attribute_value) const;
    bool Get_XML_Attribute(const std::string& attribute_name, unsigned int& attribute_value) const;
    bool Get_XML_Attribute(const std::string& attribute_name, float& attribute_value) const;
    bool Get_XML_Attribute(const std::string& attribute_name, bool& attribute_value) const;
    template<typename T>
    bool Get_XML_Attribute(const std::string& attribute_name, T& t) const
    {
        int i;
        bool l_b_Ret;

        l_b_Ret = Get_XML_Attribute(attribute_name, i);
        if (l_b_Ret == true)
        {
            t = static_cast<T>(i);
        }

        return l_b_Ret;
    }
    template<typename T>
    bool Get_XML_Attribute(const std::string& attribute_name, T& attribute_value, T (*pfn_StringToEnum)(const std::string&)) const
    {
        bool l_b_ret;
        std::string l_str;

        l_b_ret = Get_XML_Attribute(attribute_name, l_str);

        if (l_b_ret == true)
        {
            attribute_value = pfn_StringToEnum(l_str);
        }

        return l_b_ret;
    }
    template<typename T, typename Callback>
    bool Get_XML_Attribute(const std::string& attribute_name, T& attribute_value, Callback callback) const
    {
        std::string l_Str;
        if (Get_XML_Attribute(attribute_name, l_Str) == false)
        {
            return false;
        }
        return callback(l_Str, attribute_value);
    }
    bool Get_XML_Text(std::string& element_Text, bool maybe_empty = true) const;

    template<typename T>
    bool Load_Custom_Type(std::function<bool(const TiXmlElement&, T&)> load, T& object) const
    {
        return load(*m_target, object);
    }

private:
    const TiXmlElement* m_target;
};

using XML_Callback          = std::function<bool(const XML_Element&)>;
using XML_CallbackContainer = std::map<std::string, XML_Callback>;
struct XML_FileHandlerData
{
    XML_FileHandlerData() : m_Element_Count(0), m_On_Entry_Callbacks(), m_On_Exit_Callbacks() {}
    size_t m_Element_Count;
    XML_CallbackContainer m_On_Entry_Callbacks;
    XML_CallbackContainer m_On_Exit_Callbacks;
};

class XML_Loader
{
public:
    XML_Loader();

    template<class C>
    void Add_On_Entry_Callback(const std::string& xml_path, bool (C::*callback)(const XML_Element&), C* object)
    {
        Add_Callback(m_File_Data.m_On_Entry_Callbacks, xml_path, callback, object);
    }

    template<class C>
    void Add_On_Exit_Callback(const std::string& xml_path, bool (C::*callback)(const XML_Element&), C* object)
    {
        Add_Callback(m_File_Data.m_On_Exit_Callbacks, xml_path, callback, object);
    }

    void Add_On_Entry_Callback(const std::string& xml_path, std::function<bool(const XML_Element&)> callback)
    {
        Add_Callback(m_File_Data.m_On_Entry_Callbacks, xml_path, callback);
    }

    void Add_On_Exit_Callback(const std::string& xml_path, std::function<bool(const XML_Element&)> callback)
    {
        Add_Callback(m_File_Data.m_On_Exit_Callbacks, xml_path, callback);
    }

    bool Load_From_File(const std::string& file_path);
    bool Load_From_Memory(const std::string& string_content);
    bool Load_From_Memory(const char* string_content);

    template<class C>
    void Add_Progression_Callback(void (C::*pmt_Callback)(const std::string&, int, int), C* obj)
    {
        Add_Progression_Callback(std::bind(pmt_Callback, obj));
    }

    void Add_Progression_Callback(std::function<void(const std::string&, int, int)> callback)
    {
        m_Progression_Callback.push_back(callback);
    }

    void Reset_Progression_Callback(void)
    {
        m_Progression_Callback.clear();
    }

    const std::string& Get_Error_Description(void) const
    {
        return m_Error_Description;
    }

protected:
    template<class C>
    bool Add_Callback(XML_CallbackContainer& callback_container, const std::string& xml_path, bool (C::*callback)(const XML_Element&),
                      C* object)
    {
        return callback_container.emplace(xml_path, std::bind(callback, object, std::placeholders::_1)).second;
    }
    bool Add_Callback(XML_CallbackContainer& callback_container, const std::string& xml_path,
                      std::function<bool(const XML_Element&)> callback)
    {
        return callback_container.emplace(xml_path, callback).second;
    }
    struct LoadingStructure
    {
        LoadingStructure() : m_Error(), m_Element_Count(0) {}
        std::string m_Error;
        size_t m_Element_Count;
    };

    bool Count_File_Elements(TiXmlDocument& document);

    bool Explore_Document(TiXmlElement& root, LoadingStructure& loading_struct, XML_CallbackContainer& on_entry_callbacks,
                          XML_CallbackContainer& on_exit_callbacks);

    bool Recursive_Exploration(const TiXmlElement* current_element, LoadingStructure& loading_struct,
                               XML_CallbackContainer& on_entry_callbacks, XML_CallbackContainer& on_exit_callbacks);

    bool Manage_Callback(const TiXmlElement& element, const std::string& path, XML_CallbackContainer& callbacks);

    bool Load(TiXmlDocument& document);

    std::string Get_Path(const TiXmlNode* element);

    XML_FileHandlerData m_File_Data;
    LoadingStructure m_Loading_Struct;

    std::string m_File_Path;
    std::vector<std::function<void(const std::string&, int, int)>> m_Progression_Callback;

    std::string m_Error_Description;
};

#endif  // _XML_FILE_LOADER_HPP
