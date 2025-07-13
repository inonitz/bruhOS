workspace (WORKSPACE_NAME)
    startproject (START_PROJECT)
    -- Platforms & Build Configurations
    configurations {
        "Debug",
        "Release",
        "RelWithDebInfo",
    }

    platforms { "amd64" }
    filter "platforms:amd64"
        architecture "x86_64"
    filter {}


    filter { "files:**.c or files:**.h" }
        cdialect "C11"
    filter { "files:**.cpp or files:**.hpp" }
        cppdialect "C++17"
    filter {}


    -- Compiler Flags
    -- Debug Configuration Across Multiple Platforms
    filter { "configurations:Debug*" }
        defines { "_DEBUG" }
        optimize "Off"
        symbols  "On"
        buildoptions { 
            "-g", 
            "-gcolumn-info" 
        }
    filter { "configurations:Debug*", "toolset:*gcc*" }
        buildoptions { "-ggdb" }
    filter { "configurations:Debug*", "toolset:*clang*" }
        buildoptions { "-glldb" }
    filter {}

    -- RelWithDebInfo Configuration
    filter { "configurations:RelWithDebInfo*" }
        defines { "_DEBUG" }
        optimize "On"
        symbols  "On"
        buildoptions { 
            "-g", 
            "-gcolumn-info" 
        }
    filter { "configurations:RelWithDebInfo*", "toolset:*gcc*" }
        buildoptions { "-gcc" }
    filter { "configurations:RelWithDebInfo*", "toolset:*clang*" }
        buildoptions { "-glldb" }
    filter {}


    -- Release Configuration
    filter { "configurations:Release*" }
        defines { "NDEBUG" }
        optimize "On"
        symbols  "Off"
    filter {}


    buildoptions {
        "-fno-exceptions",
        "-Wshadow",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-Wno-unsafe-buffer-usage",
        "-fvisibility=hidden"
    }


    -- Specify Address Sanitizer for Debug/RelWithDebInfo
    -- Integration with ASan & UBSan is possible but takes tinkering - not for now. 
    -- filter { "configurations:Debug* or configurations:RelWithDebInfo*", "toolset:gcc or toolset:clang" }
    --     buildoptions { 
    --         "-fsanitize=address",
    --         "-fsanitize=undefined",
    --     }
    --     linkoptions { 
    --         "-fsanitize=address",
    --         "-fsanitize=undefined",
    --     }
    -- filter {}