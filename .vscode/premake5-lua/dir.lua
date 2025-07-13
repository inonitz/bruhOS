-- Build Configuration
BUILD_BINARY_DIRECTORY_GENERIC = "/build/bin/%{cfg.buildcfg}_%{cfg.platform}_%{prj.name}"
BUILD_OBJECT_DIRECTORY_GENERIC = "/build/obj/%{cfg.buildcfg}_%{cfg.platform}_%{prj.name}"
BUILD_BINARY_DIRECTORY         = "/build/bin/%{cfg.buildcfg}_%{cfg.platform}"
BUILD_OBJECT_DIRECTORY         = "/build/obj/%{cfg.buildcfg}_%{cfg.platform}"
WORKSPACE_DEPENDENCY_DIRECTORY = _MAIN_SCRIPT_DIR .. "/dependencies/"
COMPILE_COMMANDS_DIRECTORY     = _MAIN_SCRIPT_DIR .. "/.vscode/compile_commands"


AbsoluteProjectBinaryPath = function(projname)
    return _MAIN_SCRIPT_DIR .. BUILD_BINARY_DIRECTORY .. "_" .. projname
end

AbsoluteProjectObjectPath = function(projname)
    return _MAIN_SCRIPT_DIR .. BUILD_OBJECT_DIRECTORY .. "_" .. projname
end

AbsoluteDependencyDirectory = function(DependencyName) 
    return WORKSPACE_DEPENDENCY_DIRECTORY .. DependencyName
end

GetRelativeDependencyDirectory = function(DependencyName) 
    return path.getrelative(_MAIN_SCRIPT_DIR, AbsoluteDependencyDirectory(DependencyName))
end

AbsoluteProjectFolderPath = function(ProjectName, folder)
    return _MAIN_SCRIPT_DIR .. "/projects/" .. ProjectName .. "/" .. folder
end



-- Thanks to: https://stackoverflow.com/questions/1426954/split-string-in-lua
function SplitStringOnSeparator(inputstr, sep)
    if sep == nil then
        sep = "%s"
    end
    local t = {}
    for str in string.gmatch(inputstr, "([^".. sep .."]+)") do
        table.insert(t, str)
    end
    return t
end


SetupBuildDirectories = function()
    targetdir (_MAIN_SCRIPT_DIR .. BUILD_BINARY_DIRECTORY_GENERIC)
    objdir    (_MAIN_SCRIPT_DIR .. BUILD_OBJECT_DIRECTORY_GENERIC)
end


IncludeProjectHeader = function(ProjectName, HeaderDir)
    includedirs { _MAIN_SCRIPT_DIR .. "/projects/" .. ProjectName .. "/" .. HeaderDir }
end

IncludeDependencyHeaders = function(DependencyName, DependencyDir)
    includedirs { AbsoluteDependencyDirectory(DependencyName) .. "/" .. DependencyDir }
end


-- The Idea behind this is that the debug configuration on 'workspace.lua'
-- is not enough for lldb on windows. I need to manually tell the linker to generate pdb files
-- (makes sense all in all, gcc doesn't need pdb files to debug programs compiled with -g)
-- Needs Testing with gdb on linux / WSL
SetupLinkingFlagsForDebugInfo = function()
    local pdbpath = '"' .. _MAIN_SCRIPT_DIR .. BUILD_BINARY_DIRECTORY_GENERIC ..  "/%{prj.name}.pdb" .. '"'


    filter { "toolset:*clang*", "configurations:Debug* or configurations:Production*", "action:gmake or action:ninja" }
        linkoptions( "-g -Wl,--pdb=-Wl,--pdb=" .. pdbpath )
    filter {}
    -- Incomplete and untested, when working with vs2022 figure it out
    filter { "configurations:Debug* or configurations:Production*", "action:vs2022" }
        symbolspath( pdbpath )
    filter {}
end


PreBuildCopyBuildTargetCompileCommandsToFolder = function()
    prebuildcommands { 
        -- "copy /B /Y" .. COMPILE_COMMANDS_DIRECTORY .. "/%{cfg.shortname}.json " .. COMPILE_COMMANDS_DIRECTORY .. "/compile_commands.json"
        "cp -rf " .. COMPILE_COMMANDS_DIRECTORY .. "/%{cfg.shortname}.json " .. COMPILE_COMMANDS_DIRECTORY .. "/compile_commands.json"
        -- "{copyfile} %[%{!COMPILE_COMMANDS_DIRECTORY}/%{cfg.shortname}.json] %[%{!COMPILE_COMMANDS_DIRECTORY}/compile_commands.json]"
    }
end


PostBuildCommmandsForExecutable = function()
    -- Post build commands
    filter { "action:gmake or action:ninja", "configurations:*Dll" }
        local collectiveBinaryDir = _MAIN_SCRIPT_DIR .. BUILD_BINARY_DIRECTORY
        if not os.isdir(collectiveBinaryDir) then
            postbuildcommands{ "mkdir -p " .. collectiveBinaryDir }
        end
        for _, projpath in ipairs(PROJECT_LIST) do
            local projname = SplitStringOnSeparator(projpath, "/")
            projname = projname[#projname] -- Last String in the projname list 
            postbuildcommands{ "cp -rf " .. AbsoluteProjectBinaryPath(projname) .. "/* " .. collectiveBinaryDir }
        end
            -- -- postbuildcommands{ "cp -rf %[../../%{BUILD_BINARY_DIRECTORY}_googletest/*   ] %[../../%{BUILD_BINARY_DIRECTORY}]" }
            -- -- postbuildcommands{ "cp -rf %[../../%{BUILD_BINARY_DIRECTORY}_benchmark194/* ] %[../../%{BUILD_BINARY_DIRECTORY}]" }
            -- -- postbuildcommands{ "cp -rf %[%{cfg.buildtarget.directory}] %[../../%{BUILD_BINARY_DIRECTORY}]" }
            -- postbuildcommands{ "cp -rf ../.." .. "%{BUILD_BINARY_DIRECTORY}_googletest/*   " .. "../.." .. "%{BUILD_BINARY_DIRECTORY}" }
            -- postbuildcommands{ "cp -rf ../.." .. "%{BUILD_BINARY_DIRECTORY}_benchmark194/* " .. "../.." .. "%{BUILD_BINARY_DIRECTORY}" }
            -- postbuildcommands{ "cp -rf %{cfg.buildtarget.directory}/* "                      .. "../.." .. "%{BUILD_BINARY_DIRECTORY}" }
    filter {}
end


PostBuildGenerateKernelImage = function(projectName)
	postbuildcommands{ 
        "dd if=/dev/zero of=" .. AbsoluteProjectBinaryPath(projectName) .. "/" .. projectName .. ".img bs=512 count=93750" 
    }
    postbuildcommands {
        "mformat -i " .. AbsoluteProjectBinaryPath(projectName) .. "/" .. projectName .. ".img 1440 ::"
    }
    postbuildcommands { "mmd -i   " .. AbsoluteProjectBinaryPath(projectName) .. "/%{cfg.linktarget.name}.img " .. " ::/EFI" }
    postbuildcommands { "mmd -i   " .. AbsoluteProjectBinaryPath(projectName) .. "/%{cfg.linktarget.name}.img " .. " ::/EFI/BOOT" }
    postbuildcommands { "mcopy -i " .. AbsoluteProjectBinaryPath(projectName) .. "/%{cfg.linktarget.name}.img " .. AbsoluteProjectBinaryPath("bootloader")    .. "/* ::/EFI/BOOT" }
    postbuildcommands { "mcopy -i " .. AbsoluteProjectBinaryPath(projectName) .. "/%{cfg.linktarget.name}.img " .. AbsoluteDependencyDirectory("startup.nsh") .. " ::"            }
    postbuildcommands { "mcopy -i " .. AbsoluteProjectBinaryPath(projectName) .. "/%{cfg.linktarget.name}.img " .. AbsoluteProjectBinaryPath(projectName)     .. "/%{cfg.linktarget.name}.elf ::"}
end






rule "NASMAssembly"
    display "Compile Assembly Files"
    fileextension ".s"

    propertydefinition {
        name = "optimize",
        display = "Optimization Level",
        values = {
            [0] = "None",
            [1] = "Size",
            [2] = "Speed",
        },
        switch = {
            [0] = "-O0",
            [1] = "-O1",
            [2] = "-O3",
        },
        value = 0,
    }

    buildmessage  'Compiling %{file.name} with nasm'
    buildcommands 'nasm -O0 -f elf64 %{!file.abspath} -o %{!cfg.objdir}/%{file.basename}.o'
    buildoutputs  '%{!cfg.objdir}/%{file.basename}.o'
