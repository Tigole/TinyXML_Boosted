#include <gtest/gtest.h>

#include <XML_Loader.hpp>

TEST(FileLoading, NoSuchFile)
{
    XML_Loader loader;

    EXPECT_FALSE(loader.Load_From_File("nofile.xml"));
    EXPECT_EQ(loader.Get_Error_Description(), "Failed to open file");  /// From tinyxml
}

TEST(FileLoading, EmptyFile)
{
    XML_Loader loader;

    EXPECT_FALSE(loader.Load_From_File("empty.xml"));
    EXPECT_EQ(loader.Get_Error_Description(), "Error document empty.");  /// From tinyxml
}

TEST(FileLoading, NoRootFile)
{
    XML_Loader loader;

    EXPECT_FALSE(loader.Load_From_File("no_root.xml"));
    EXPECT_EQ(loader.Get_Error_Description(), "Empty document");
}

TEST(FileLoading, NominalObject)
{
    struct CustomLoader
    {
        bool On_Entry(const XML_Element& el)
        {
            onEntryCalled = true;
            return true;
        }
        bool On_Exit(const XML_Element& el)
        {
            onExitCalled = true;
            return true;
        }

        bool onEntryCalled = false;
        bool onExitCalled  = false;
    };
    CustomLoader customLoader;
    XML_Loader loader;

    loader.Add_On_Entry_Callback("/Example", &CustomLoader::On_Entry, &customLoader);
    loader.Add_On_Exit_Callback("/Example", &CustomLoader::On_Exit, &customLoader);

    EXPECT_FALSE(customLoader.onEntryCalled);
    EXPECT_FALSE(customLoader.onExitCalled);
    EXPECT_TRUE(loader.Load_From_File("example.xml"));
    EXPECT_EQ(loader.Get_Error_Description(), "");
    EXPECT_TRUE(customLoader.onEntryCalled);
    EXPECT_TRUE(customLoader.onExitCalled);
}

TEST(FileLoading, NominalFunction)
{
    bool onEntryCalled = false;
    bool onExitCalled  = false;
    XML_Loader loader;

    loader.Add_On_Entry_Callback("/Example", [&](const XML_Element& el)
    {
        onEntryCalled = true;
        return true;
    });
    loader.Add_On_Exit_Callback("/Example", [&](const XML_Element& el)
    {
        onExitCalled = true;
        return true;
    });

    EXPECT_FALSE(onEntryCalled);
    EXPECT_FALSE(onExitCalled);
    EXPECT_TRUE(loader.Load_From_File("example.xml"));
    EXPECT_EQ(loader.Get_Error_Description(), "");
    EXPECT_TRUE(onEntryCalled);
    EXPECT_TRUE(onExitCalled);
}

TEST(FileLoading, NominalNoSlash)
{
    bool onEntryCalled = false;
    bool onExitCalled  = false;
    XML_Loader loader;

    loader.Add_On_Entry_Callback("Example", [&](const XML_Element& el)
    {
        onEntryCalled = true;
        return true;
    });
    loader.Add_On_Exit_Callback("Example", [&](const XML_Element& el)
    {
        onExitCalled = true;
        return true;
    });

    EXPECT_FALSE(onEntryCalled);
    EXPECT_FALSE(onExitCalled);
    EXPECT_TRUE(loader.Load_From_File("example.xml"));
    EXPECT_EQ(loader.Get_Error_Description(), "");
    EXPECT_FALSE(onEntryCalled);
    EXPECT_FALSE(onExitCalled);
}

TEST(MemoryLoading, NoRootFile)
{
    XML_Loader loader;

    EXPECT_FALSE(loader.Load_From_Memory(""));
    EXPECT_EQ(loader.Get_Error_Description(), "Error document empty.");  /// From tinyxml
}

TEST(MemoryLoading, EmptyFile)
{
    XML_Loader loader;

    EXPECT_FALSE(loader.Load_From_Memory(R"(<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>)"));
    EXPECT_EQ(loader.Get_Error_Description(), "Empty document");
}

constexpr const char* NominalContent = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes" ?><Example></Example>)";

TEST(MemoryLoading, NominalObject)
{
    struct CustomLoader
    {
        bool On_Entry(const XML_Element& el)
        {
            onEntryCalled = true;
            return true;
        }
        bool On_Exit(const XML_Element& el)
        {
            onExitCalled = true;
            return true;
        }

        bool onEntryCalled = false;
        bool onExitCalled  = false;
    };
    CustomLoader customLoader;
    XML_Loader loader;

    loader.Add_On_Entry_Callback("/Example", &CustomLoader::On_Entry, &customLoader);
    loader.Add_On_Exit_Callback("/Example", &CustomLoader::On_Exit, &customLoader);

    EXPECT_FALSE(customLoader.onEntryCalled);
    EXPECT_FALSE(customLoader.onExitCalled);
    EXPECT_TRUE(loader.Load_From_Memory(NominalContent));
    EXPECT_EQ(loader.Get_Error_Description(), "");
    EXPECT_TRUE(customLoader.onEntryCalled);
    EXPECT_TRUE(customLoader.onExitCalled);
}

TEST(MemoryLoading, NominalFunction)
{
    bool onEntryCalled = false;
    bool onExitCalled  = false;
    XML_Loader loader;

    loader.Add_On_Entry_Callback("/Example", [&](const XML_Element& el)
    {
        onEntryCalled = true;
        return true;
    });
    loader.Add_On_Exit_Callback("/Example", [&](const XML_Element& el)
    {
        onExitCalled = true;
        return true;
    });

    EXPECT_FALSE(onEntryCalled);
    EXPECT_FALSE(onExitCalled);
    EXPECT_TRUE(loader.Load_From_Memory(NominalContent));
    EXPECT_EQ(loader.Get_Error_Description(), "");
    EXPECT_TRUE(onEntryCalled);
    EXPECT_TRUE(onExitCalled);
}

TEST(MemoryLoading, NominalNoSlash)
{
    bool onEntryCalled = false;
    bool onExitCalled  = false;
    XML_Loader loader;

    loader.Add_On_Entry_Callback("Example", [&](const XML_Element& el)
    {
        onEntryCalled = true;
        return true;
    });
    loader.Add_On_Exit_Callback("Example", [&](const XML_Element& el)
    {
        onExitCalled = true;
        return true;
    });

    EXPECT_FALSE(onEntryCalled);
    EXPECT_FALSE(onExitCalled);
    EXPECT_TRUE(loader.Load_From_Memory(NominalContent));
    EXPECT_EQ(loader.Get_Error_Description(), "");
    EXPECT_FALSE(onEntryCalled);
    EXPECT_FALSE(onExitCalled);
}
