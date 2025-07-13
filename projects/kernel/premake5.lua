project "kernel"
    toolset "gcc_cross_elf64"
    rules "NASMAssembly"
    NASMAssemblyVars { 
        optimize = "None" 
    }

    -- Target Info
    targetprefix ""
    targetname "kernel"
    targetextension (".elf")

    -- NASMAssemblyVars{ optimize = "None" }
    

    -- Build Directories
    SetupBuildDirectories()

    -- Project Structure
    files { 
        "include/**.h",
        "include/**.hpp",

        "source/**.s",
        "source/**.c",
        "source/**.cpp",
        "source/kernel.ld"
    }
    -- Specify Include Headers
    includedirs { 
        "include"
    }
    
    -- Build Options
    buildoptions {
        "-mcmodel=large",
        "-ffreestanding",
        "-nostdlib",
        "-fshort-wchar",
        "-mno-red-zone",
        "-mgeneral-regs-only",
        "-Os",
        "-Wno-missing-braces",
        "-Wno-unused-variable",
        "-Wno-unused-function",
        "-Wno-unused-but-set-variable",
        "-Wno-reserved-macro-identifier",
        "-Wno-reserved-identifier",
        "-Wno-declaration-after-statement",
        "-Wno-padded-bitfield",
        "-Wno-padded",
        "-Wno-packed"
    }


    -- Linking Options
    links {}
    -- linkoptions {
    --     "-v ",
    --     "-fuse-ld=lld",
    --     "-target x86_64-unknown-none-elf",
    --     "-mcmodel=large",
    --     "-ffreestanding",
    --     "-nostdlib",
    --     "-static",
    --     -- the linker script flag caused me so much pain: https://forum.osdev.org/viewtopic.php?t=36341
    --     "-Wl,-T " .. AbsoluteProjectFolderPath("kernel", "kernel.ld")
    -- }
    linkoptions {
        "-v",
        '-Wl,--script="C:/CTools/projects/bruhOS/projects/kernel/kernel.ld"',
        "-mcmodel=large",
        "-fPIC",
        "-static",
        "-nostdlib",
        "-ffreestanding"
        -- "-Wl,-z max-page-size=0x1000"
    }

    -- I need to execute LD to get a proper aggregated static library with the linker script, 
    -- instead of a bunch of object files clumped together (looking at you AR!)
    -- kind "SharedLib"
    -- flags {
    --     "noimportlib"
    -- }
    kind "StaticLib"
    makesettings {
        'LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)'
    }

    
    -- Macros
    defines {}


    -- Custom Pre &// Post build Actions
    PreBuildCopyBuildTargetCompileCommandsToFolder()
    -- PostBuildGenerateKernelImage("kernel")



project("kernelimage")
    -- Target Info    
    kind "Makefile"
    dependson { "kernel", "bootloader" }

    -- Build Directories
    SetupBuildDirectories()


	buildcommands{ 
        "dd if=/dev/zero of=" .. AbsoluteProjectBinaryPath("kernel") .. "/" .. "kernel" .. ".img bs=512 count=8192" 
    }
    buildcommands {
        "mformat -i " .. AbsoluteProjectBinaryPath("kernel") .. "/" .. "kernel.img ::"
    }
    buildcommands { "mmd -i   " .. AbsoluteProjectBinaryPath("kernel") .. "/kernel.img " .. " ::/EFI" }
    buildcommands { "mmd -i   " .. AbsoluteProjectBinaryPath("kernel") .. "/kernel.img " .. " ::/EFI/BOOT" }
    buildcommands { "mcopy -i " .. AbsoluteProjectBinaryPath("kernel") .. "/kernel.img " .. AbsoluteProjectBinaryPath("bootloader")    .. "/* ::/EFI/BOOT" }
    buildcommands { "mcopy -i " .. AbsoluteProjectBinaryPath("kernel") .. "/kernel.img " .. AbsoluteDependencyDirectory("startup.nsh") .. " ::"            }
    buildcommands { "mcopy -i " .. AbsoluteProjectBinaryPath("kernel") .. "/kernel.img " .. AbsoluteProjectBinaryPath("kernel")        .. "/kernel.elf ::"}