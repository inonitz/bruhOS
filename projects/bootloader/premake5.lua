project "bootloader"
    toolset "clang"

    -- Target Info    
    kind "SharedLib"
    pic "Off"
    
    -- Target Info
    targetprefix    ""
    targetname      "bootx64"
    targetextension (".efi")

    -- Build Directories
    SetupBuildDirectories()

    -- Project Structure
    files { 
        "source/**.c"
    }
    -- Specify Include Headers
    includedirs { 
        "include"
    }
    IncludeProjectHeader("kernel", "include")
    IncludeDependencyHeaders("gnu-efi", "inc")
    IncludeDependencyHeaders("gnu-efi", "inc/x86_64")
    IncludeDependencyHeaders("gnu-efi", "inc/protocol")

    -- Build Options
    -- buildoptions {
    --     "-v",
    --     "-ffreestanding", 
    --     "-fshort-wchar",
    --     "-mno-red-zone",
    --     "-Wno-missing-noreturn",
    --     "-Wno-assign-enum",
    --     "-Wno-sign-conversion",
    --     "-Wno-declaration-after-statement",
    --     "-Wno-unused-variable"
    -- }
    buildoptions {
        "-v",
        "-O0",
        "-target x86_64-unknown-windows",
        "-ffreestanding",
        "-fshort-wchar",
        "-mno-red-zone"
    }


    -- Linking Options
    links {}
    linkoptions {
        "-target x86_64-unknown-windows",
        "-nostdlib",
        "-Wl,-entry:efi_main",
        "-Wl,-subsystem:efi_application",
        "-fuse-ld=lld-link"
    }
    -- llvm-pdbutil dump -all ./build/bin/Debug_amd64_bootloader/bootx64.pdb &> out.txt
    -- Examine if the .pdb debug data is actually any good
    -- More info: https://stackoverflow.com/questions/2040132/reading-a-pdb-file
    SetupLinkingFlagsForDebugInfo()

    -- Macros
    defines {}

    -- Custom Pre &// Post build Actions
    PreBuildCopyBuildTargetCompileCommandsToFolder()