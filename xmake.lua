set_project("TinyXML_Boosted")
set_version("2.0.0")

add_requires("tinyxml 2.6.2")
add_requires("gtest", {system = false, configs = {main = true, gmock = false, shared = false}})

target("tinyxml-boosted")
    set_kind("static")
    add_files("$(projectdir)/Sources/XML_Loader.cpp")
    add_headerfiles("$(projectdir)/Include/TinyXML_Boosted/XML_Loader.hpp", {public = true})
    add_includedirs("$(projectdir)/Include/TinyXML_Boosted/", {public = true})
    add_packages("tinyxml", {public = true})


for _, testfile in ipairs(os.files("Tests/**.cpp")) do
    local name = path.basename(testfile)
    target(name)
        set_kind("binary")
        set_default(false)
        add_packages("gtest", {main = true, gmok = false})
        add_deps("tinyxml-boosted")
        add_files(testfile)
        add_tests("default")
        set_group("test")
        set_rundir("$(projectdir)/Tests/")
end