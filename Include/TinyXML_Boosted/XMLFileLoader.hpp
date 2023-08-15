#ifndef _XML_FILE_LOADER_HPP
#define _XML_FILE_LOADER_HPP 1

#include <map>
#include <string>
#include <functional>
#include <stack>
#include <vector>

class TiXmlElement;
class TiXmlNode;

class XML_Element
{
public:
	XML_Element(const TiXmlElement& target);
	bool mt_Get_Attribute(const std::string& attribute_name, std::string& attribute_value) const;
	bool mt_Get_Attribute(const std::string& attribute_name, int& attribute_value) const;
	bool mt_Get_Attribute(const std::string& attribute_name, unsigned int& attribute_value) const;
	bool mt_Get_Attribute(const std::string& attribute_name, float& attribute_value) const;
	bool mt_Get_Attribute(const std::string& attribute_name, bool& attribute_value) const;
	template<typename T>
	bool mt_Get_Attribute(const std::string& attribute_name, T& t) const
	{
		int i;
		bool l_b_Ret;

		l_b_Ret = mt_Get_Attribute(attribute_name, i);
		if (l_b_Ret == true)
		{
			t = static_cast<T>(i);
		}

		return l_b_Ret;
	}
	template<typename T>
	bool mt_Get_Attribute(const std::string& attribute_name, T& attribute_value, T(*pfn_StringToEnum)(const std::string&)) const
	{
		bool l_b_ret;
		std::string l_str;

		l_b_ret = mt_Get_Attribute(attribute_name, l_str);

		if (l_b_ret == true)
		{
			attribute_value = pfn_StringToEnum(l_str);
		}


		return l_b_ret;
	}
	bool mt_Get_Text(std::string& element_Text, bool maybe_empty = true) const;

	template<typename T>
	bool mt_Load_Custom_Type(std::function<bool(const TiXmlElement&, T&)> load, T& object) const
	{
	    return load(*m_target, object);
	}

private:
	const TiXmlElement* m_target;
};

using XML_Callback = std::function<bool(const XML_Element&)>;
using XML_CallbackContainer = std::map<std::string, XML_Callback>;
struct XML_FileHandlerData
{
	XML_FileHandlerData() : m_Element_Count(0), m_On_Entry_Callbacks(), m_On_Exit_Callbacks(){}
	size_t m_Element_Count;
	XML_CallbackContainer m_On_Entry_Callbacks;
	XML_CallbackContainer m_On_Exit_Callbacks;
};

class XMLFileLoader
{
public:
	XMLFileLoader();

	template<class C>
	void mt_Add_On_Entry_Callback(const std::string& xml_path, bool(C::*callback)(const XML_Element&), C* object)
	{
	    mt_Add_Callback(m_File_Data.m_On_Entry_Callbacks, xml_path, callback, object);
	}

	template<class C>
	void mt_Add_On_Exit_Callback(const std::string& xml_path, bool(C::*callback)(const XML_Element&), C* object)
	{
	    mt_Add_Callback(m_File_Data.m_On_Exit_Callbacks, xml_path, callback, object);
	}

	void mt_Add_On_Entry_Callback(const std::string& xml_path, std::function<bool(const XML_Element&)> callback)
	{
	    mt_Add_Callback(m_File_Data.m_On_Entry_Callbacks, xml_path, callback);
	}

	void mt_Add_On_Exit_Callback(const std::string& xml_path, std::function<bool(const XML_Element&)> callback)
	{
	    mt_Add_Callback(m_File_Data.m_On_Exit_Callbacks, xml_path, callback);
	}

	bool mt_Load(const std::string& file_path);

	template<class C>
	void mt_Add_Progression_Callback(void (C::*pmt_Callback)(const std::string&, int, int), C* obj)
	{
	    mt_Add_Progression_Callback(std::bind(pmt_Callback, obj));
	}

	void mt_Add_Progression_Callback(std::function<void(const std::string&, int, int)> callback)
	{
	    m_Progression_Callback.push_back(callback);
	}

	void mt_Reset_Progression_Callback(void)
	{
	    m_Progression_Callback.clear();
	}

	const std::string& mt_Get_Error_Description(void) const
	{
	    return m_Error_Description;
	}

protected:
	template<class C>
	bool mt_Add_Callback(XML_CallbackContainer& callback_container, const std::string& xml_path, bool(C::*callback)(const XML_Element&), C* object)
	{
		return callback_container.emplace(xml_path, std::bind(callback, object, std::placeholders::_1)).second;
	}
	bool mt_Add_Callback(XML_CallbackContainer& callback_container, const std::string& xml_path, std::function<bool(const XML_Element&)> callback)
	{
	    return callback_container.emplace(xml_path, callback).second;
	}
	struct LoadingStructure
	{
		LoadingStructure():m_Error(), m_Element_Count(0){}
		std::string m_Error;
		size_t m_Element_Count;
	};

	bool mt_Count_File_Elements(void);

	bool mt_Explore_Document(TiXmlElement& root, LoadingStructure& loading_struct, XML_CallbackContainer& on_entry_callbacks, XML_CallbackContainer& on_exit_callbacks);

	bool mt_Recursive_Exploration(const TiXmlElement* current_element, LoadingStructure& loading_struct, XML_CallbackContainer& on_entry_callbacks, XML_CallbackContainer& on_exit_callbacks);

	bool mt_Manage_Callback(const TiXmlElement& element, const std::string& path, XML_CallbackContainer& callbacks);

	std::string mt_Get_Path(const TiXmlNode* element);

	XML_FileHandlerData m_File_Data;
	LoadingStructure m_Loading_Struct;

	std::string m_File_Path;
	std::vector<std::function<void(const std::string&, int, int)>> m_Progression_Callback;

	std::string m_Error_Description;
};

#endif // _XML_FILE_LOADER_HPP
