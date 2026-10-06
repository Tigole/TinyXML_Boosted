set_project("TinyXML_Boosted")
set_version("1.x")

add_requires("tinyxml 2.6.2")

target("tinyxml-boosted")
    set_kind("static")
    --add_files("$(projectdir)/Sources/**.cpp")
    add_files("$(projectdir)/Sources/XMLFileLoader.cpp")
    --add_headerfiles("$(projectdir)/Include/TinyXML_Boosted/**.h", "$(projectdir)/Include/TinyXML_Boosted/**.hpp")
    add_headerfiles("$(projectdir)/Include/TinyXML_Boosted/XMLFileLoader.hpp", {public = true})
    add_includedirs("$(projectdir)/Include/TinyXML_Boosted/")
    add_packages("tinyxml")