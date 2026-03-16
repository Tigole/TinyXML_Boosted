set_project("TinyXML_Boosted")
set_version("1.x")

target("tinyxml-boosted")
    set_kind("static")

    add_files("$(projectdir)/Sources/**.cpp")
    add_headerfiles("$(projectdir)/Include/TinyXML_Boosted/**.h", "$(projectdir)/Include/TinyXML_Boosted/**.hpp")
    add_includedirs("$(projectdir)/Include/", {public = true})