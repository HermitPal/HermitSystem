-- xmake.lua
set_project("DisplayApp")
set_version("1.0.0")

-- Set C++ standard
set_languages("c++17")

-- Add target
target("Aug")
    set_kind("binary")
    
    add_files("src/*.cpp")
    add_files("src/**/*.cpp")
    
    if is_plat("windows") then
        add_syslinks("user32", "gdi32", "opengl32")
    end