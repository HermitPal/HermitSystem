-- xmake.lua
set_project("DisplayApp")
set_version("1.0.0")

-- Set C++ standard
set_languages("c++17")

-- Add packages required for testing
add_requires("gtest")

-- 1. Compile your source code (EXCEPT main.cpp) into a static library
target("CoreLib")
    set_kind("static")
    -- Add all source files from subdirectories
    add_files("src/Renderer/*.cpp", "src/System/*.cpp", "src/Math/*.cpp")
    add_includedirs("src", {public = true})

    if is_plat("windows") then
        add_syslinks("user32", "gdi32", "d3d11")
    end

-- 2. Your main application now compiles main.cpp and links to the library
target("Game")
    set_kind("binary")
    add_files("src/main.cpp")
    add_deps("CoreLib")

-- 3. Define the test target for the System library
target("SystemTests")
    set_kind("binary")
    add_files("tests/System/*.cpp") -- Point to System test files
    add_deps("CoreLib")
    add_packages("gtest")
    add_rules("test")

-- 4. Define the test target for the Math library
target("MathTests")
    set_kind("binary")
    add_files("tests/Math/*.cpp") -- Point to Math test files
    add_deps("CoreLib")
    add_packages("gtest")
    add_rules("test")

-- 5. Define the test target for the Renderer library
target("RendererTests")
    set_kind("binary")
    add_files("tests/Renderer/*.cpp") -- Point to Renderer test files
    add_deps("CoreLib")
    add_packages("gtest")
    add_rules("test")

-- 6. Define the custom rule that tells xmake how to run our tests
rule("test")
    on_run(function(target)
        print("Executing test: %s", target:name())
        os.exec(target:targetfile())
    end)

-- 7. Define a group to run all tests at once
target("AllTests")
    set_kind("phony")
    add_deps("SystemTests", "MathTests", "RendererTests")
    on_run(function(target)
        print("Running all tests...")
        os.exec("xmake run SystemTests")
        os.exec("xmake run MathTests")
        os.exec("xmake run RendererTests")
    end)