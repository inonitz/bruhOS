-- Thanks to: https://stackoverflow.com/questions/75490124/how-to-require-a-file-with-a-dot-in-the-name
local function __custom_searcher(module_name)
    -- Use "/" instead of "." as directory separator
    local path, err = package.searchpath(module_name, package.path, "/")
    if path then
        return assert(loadfile(path))
    end
    return err
end


table.insert(package.searchers, __custom_searcher)