-- PROJECT_LIST inside premake5.lua
getProjectNames = function()
    name_folder_pair = {}
    for _, projpath in ipairs(PROJECT_LIST) do
        local i, j = string.find(projpath, '/')
        local projname = string.sub(projpath, i + 1) 
        table.insert(name_folder_pair, { projname, projpath })
    end
    return name_folder_pair
end

newoption {
    trigger     = "proj",
    description = "Set this when using cleanproj",
    allowed     = getProjectNames()
}


newaction {
    trigger     = "cleanbuild",
    description = "Delete All Files Generated during the build process",
    execute     = function ()
        local dirs_to_delete = { "./build", "./.vs" }
        local ok, err
        printf("----------------------------------------")
        for _, path in ipairs(dirs_to_delete) do
            ok, err = os.rmdir(path)
            if ok then
                printf("%-30s: %s", "Removed Directory", path)
            else
                printf("%-30s: %s", "Got Error", err)
            end
        end
        printf("----------------------------------------")
        print("Done")
    end
}


newaction {
    trigger     = "cleanproj",
    description = "Delete All Files Generated for a specific project",
    allowed = getProjectNames(),
    execute = function()
        if _OPTIONS["proj"] then
            local specific_bin_dirs = os.matchdirs( "build/bin/*" .. _OPTIONS["proj"] .. "*" )
            local specific_obj_dirs = os.matchdirs( "build/obj/*" .. _OPTIONS["proj"] .. "*" )
            local status, maybeError
            for key, value in pairs(specific_bin_dirs) do
                printf("----------------------------------------")
                status, maybeError = os.rmdir(value)
                if status then
                    printf("%-30s: %s", "Removed Directory", value)
                else
                    printf("%-30s: %s", "Got Error", maybeError)
                end
                printf("----------------------------------------")
            end
            for key, value in pairs(specific_obj_dirs) do
                printf("----------------------------------------")
                status, maybeError = os.rmdir(value)
                if status then
                    printf("%-30s: %s", "Removed Directory", value)
                else
                    printf("%-30s: %s", "Got Error", maybeError)
                end
                printf("----------------------------------------")
            end
        else
            printf("--proj option wasn't specified - Can't Continue!")
        end
    end
}


newaction {
    trigger     = "cleancfg",
    description = "Delete All Build-System-Related files Generated (Makefile, sln, ninja, ...)",
    execute     = function ()
        local build_extensions = { "/Makefile", "/**.make", "/**.sln", "/**.vcxproj", "/**.vcxproj.filters", "/**.vcxproj.user", "/**.ninja", "/.ninja_deps", "/.ninja_log", "/.ninja_lock" }
        local ok, err
        local temp_prj_list = PROJECT_LIST 
        table.insert(temp_prj_list, ".")
        for _, path in ipairs(temp_prj_list) do
            local flag = true
            printf("%s", path)

            for __, ext in ipairs(build_extensions) do
                ok, err = os.remove{ path .. ext }
                if ok then
                    printf("%-30s: %s", "Removed Project Config Files", path .. ext)
                else
                    printf("%-30s: %s", "Got Error", err)
                end
            end
            printf("----------------------------------------")
        end
        -- table.remove(PROJECTS)
        print("Done")
    end
}


-- Delete whatever was generated using export-compile-commands
newaction {
    trigger     = "cleanclangd",
    description = "Delete the compile_commands/* Directory",
    execute     = function ()
        local ok, err, path
        printf("----------------------------------------")
        path = "./.vscode/compile_commands"
        ok, err = os.rmdir(path)
        if ok then
            printf("%-30s: %s", "Removed Directory", path)
        else
            printf("%-30s: %s", "Got Error", err)
        end
        printf("----------------------------------------")
        print("Done")
    end
}


newaction {
    trigger     = "cleanall",
    description = "Trigger the following actions: cleanprojectconfigs, cleanclangd, cleanbuild",
    execute = function()
        print("[ACTION] = [cleanll] Begin\n")
        os.execute("premake5 cleanbuild")
        os.execute("premake5 cleancfg")
        os.execute("premake5 cleanclangd")
        print("[ACTION] = [cleanll] End\n")
    end
}


newaction {
    trigger     = "buildall",
    description = "Trigger the following actions: export-compile-commands, gmake",
    execute = function()
        print("[ACTION] = [buildall] Begin\n")
        os.execute("premake5 export-compile-commands")
        os.execute("premake5 gmake")
        os.execute("make config=debuglib_amd64 -j 16")
        print("[ACTION] = [buildall] End\n")
    end
}


newaction {
    trigger     = "runqemu",
    description = "[NOT WORKING!] Run The QEMU Emulator with the specified kernel image",
    execute = function()
        local KernelImageAbsPath = AbsoluteProjectBinaryPath("kernel") .. "/kernel.img"
        os.execute('\
            qemu-system-x86_64.exe \
            -cpu max,+ssse3,+sse4.1,+sse4.2,+tsc-deadline \
            -vga vmware \
            -monitor stdio \
            -drive format=raw,file=' .. KernelImageAbsPath ..
            '\
            -m 1G \
            -smp 12 \
            -net none \
            -drive if=pflash,format=raw,unit=0,file=\
            '
                .. AbsoluteDependencyDirectory("ovmf/CODE_EFI.fd") 
                .. ',readonly=on' 
                ..
            '-drive if=pflash,format=raw,unit=0,file='
                .. AbsoluteDependencyDirectory("ovmf/VARS_EFI.fd")
        )
    end
}
