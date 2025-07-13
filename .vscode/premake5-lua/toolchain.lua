local function add_custom_gcc_toolset (name, prefix)
    local gcc                         = premake.tools.gcc
    local new_toolset                 = {}  
    new_toolset.getcflags             = gcc.getcflags
    new_toolset.getcxxflags           = gcc.getcxxflags
    new_toolset.getcppflags           = gcc.getcppflags
    new_toolset.getdefines            = gcc.getdefines
    new_toolset.getundefines          = gcc.getundefines
    new_toolset.getforceincludes      = gcc.getforceincludes
    new_toolset.getincludedirs        = gcc.getincludedirs
    new_toolset.getldflags            = gcc.getldflags
    new_toolset.getLibraryDirectories = gcc.getLibraryDirectories
    new_toolset.getlinks              = gcc.getlinks
    new_toolset.getmakesettings       = gcc.getmakesettings
    new_toolset.getrunpathdirs        = gcc.getrunpathdirs
    new_toolset.toolset_prefix        = prefix
    new_toolset.shared                = gcc.shared
    new_toolset.shared.compileas      = gcc.shared.compileas

    function new_toolset.gettoolname (cfg, tool)
        if tool == "cc" then
            name = new_toolset.toolset_prefix .. "x86_64-elf-gcc"  
        elseif tool == "cxx" then
            name = new_toolset.toolset_prefix .. "x86_64-elf-g++"
        elseif tool == "ar" then
            name = new_toolset.toolset_prefix .. "x86_64-elf-ar"
        elseif tool == "ld" then
            name = new_toolset.toolset_prefix .. "x86_64-elf-ld"
        end
        return name
    end  

    premake.tools[name] = new_toolset
end

add_custom_gcc_toolset("gcc_cross_elf64", string.gsub( os.getenv("ELF64_CROSS_GCC_PATH"), "\\", "/" ) .. "//")