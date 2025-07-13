local p = premake

p.modules.export_compile_commands = {}
local m = p.modules.export_compile_commands

local workspace = p.workspace
local project = p.project

local function esc(s)
  s = s:gsub('\\', '\\\\')
  s = s:gsub('"', '\\"')
  return s
end

local function esc_table(t)
  local res = {}
  for k, v in pairs(t) do
    table.insert(res, esc(v))
  end
  return res
end

local function quote(s)
  return '"' .. esc(s) .. '"'
end

function m.getToolset(cfg)
  return p.tools[cfg.toolset or 'gcc']
end


function m.getCppStandard(cfg)
  if cfg.language ~= 'C++' or cfg.cppdialect == nil or cfg.cppdialect == '' then
    return ''
  end
  -- From premake-core/modules/d/actions/vcxproj.lua
  local standards = {
    ['Default']   = 'c++17',   -- the default C++ dialect for the toolset
    ['C++latest'] = 'c++20',   -- the latest C++ dialect for the toolset or action where available, otherwise the latest C++ dialect supported by Premake
    ['C++98']     = 'c++98',   -- ISO C++98
    ['C++0x']     = 'c++11',   -- ISO C++11 Draft
    ['C++11']     = 'c++11',   -- ISO C++11
    ['C++1y']     = 'c++14',   -- ISO C++14 Draft
    ['C++14']     = 'c++14',   -- ISO C++14
    ['C++1z']     = 'c++17',   -- ISO C++17 Draft
    ['C++17']     = 'c++17',   -- ISO C++17
    ['C++2a']     = 'c++20',   -- ISO C++20 Draft
    ['C++20']     = 'c++20',   -- ISO C++20
    ['gnu++98']   = 'c++98',   -- GNU dialect of ISO C++98
    ['gnu++0x']   = 'c++11',   -- GNU dialect of ISO C++11 Draft
    ['gnu++11']   = 'c++11',   -- GNU dialect of ISO C++11
    ['gnu++1y']   = 'c++14',   -- GNU dialect of ISO C++14 Draft
    ['gnu++14']   = 'c++14',   -- GNU dialect of ISO C++14
    ['gnu++1z']   = 'c++17',   -- GNU dialect of ISO C++17 Draft
    ['gnu++17']   = 'c++17',   -- GNU dialect of ISO C++17
    ['gnu++2a']   = 'c++20',   -- GNU dialect of ISO C++20 Draft
    ['gnu++20']   = 'c++20',   -- GNU dialect of ISO C++20
  }
  if standards[cfg.cppdialect] ~= nil then
    return '-std=' .. standards[cfg.cppdialect]
  else
    return ''
  end
end

-- Also added
function m.getCStandard(cfg)
  if cfg.language ~= 'C' or cfg.cdialect == nil or cfg.cdialect == '' then
    return ''
  end
  -- From https://premake.github.io/docs/cdialect
  local standards = {
    ['Default'] = 'C11'  , -- the default C dialect for the toolset
    ['C89']     = 'C89'  , -- ISO C89
    ['C90']     = 'C90'  , -- ISO C90
    ['C99']     = 'C99'  , -- ISO C99
    ['C11']     = 'C11'  , -- ISO C11
    ['C17']     = 'C17'  , -- ISO C17
    ['gnu89']   = 'gnu89', -- GNU dialect of ISO C89
    ['gnu90']   = 'gnu90', -- GNU dialect of ISO C90
    ['gnu99']   = 'gnu99', -- GNU dialect of ISO C99
    ['gnu11']   = 'gnu11', -- GNU dialect of ISO C11
    ['gnu17']   = 'gnu17', -- GNU dialect of ISO C17
  }
  if standards[cfg.cdialect] ~= nil then
    return '-std=' .. standards[cfg.cdialect]
  else
    return ''
  end
end


function m.getCommonFlags(prj, cfg)
  -- some tools that consumes compile_commands.json have problems with relative include paths
  relative = project.getrelative
  project.getrelative = function(prj, dir) return dir end

  local toolset = m.getToolset(cfg)
  local flags = toolset.getcppflags(cfg)
  flags = table.join(flags, toolset.getdefines(cfg.defines))
  flags = table.join(flags, toolset.getundefines(cfg.undefines))
  flags = table.join(flags, toolset.getincludedirs(cfg, cfg.includedirs, cfg.externalincludedirs))
  flags = table.join(flags, toolset.getforceincludes(cfg))
  if project.iscpp(prj) then
    flags = table.join(flags, toolset.getcxxflags(cfg))
    flags = table.join(flags, m.getCppStandard(cfg))
  elseif project.isc(prj) then
    flags = table.join(flags, toolset.getcflags(cfg))
    flags = table.join(flags, m.getCStandard(cfg))
  end
  flags = table.join(flags, cfg.buildoptions)
  project.getrelative = relative
  return flags
end

function m.getObjectPath(prj, cfg, node)
  return path.join(cfg.objdir, path.appendExtension(node.objname, '.o'))
end

function m.getDependenciesPath(prj, cfg, node)
  return path.join(cfg.objdir, path.appendExtension(node.objname, '.d'))
end

function m.getFileFlags(prj, cfg, node)
  return table.join(m.getCommonFlags(prj, cfg), {
    '-o', quote(m.getObjectPath(prj, cfg, node)),
    '-MF', quote(m.getDependenciesPath(prj, cfg, node)),
    '-c', quote(node.abspath)
  })
end

local function computesystemincludepaths(tool, iscfile)
  local cmd = tool .. " -E -v -fsyntax-only " .. (iscfile and '-x c' or '-x c++') .. ' "' .. _MAIN_SCRIPT .. '"' -- Use script as dummy "c" file
  local s,e = os.outputof(cmd, "both")
  if not s or not e then return {} end
  local s = string.match(s, "#include <...> search starts here:\n(.*)End of search list.\n")
  local includepaths = {}
  for w in string.gmatch(s, " *([^\n]+) *") do
    table.insert(includepaths, path.normalize(w))
  end
  return includepaths
end

local systemincludepathscache = {}

local function getsystemincludepaths(tool, iscfile)
  if not systemincludepathscache[tool] then systemincludepathscache[tool] = {} end
  local toolcache = systemincludepathscache[tool]
  if not toolcache[iscfile] then toolcache[iscfile] = computesystemincludepaths(tool, iscfile) end
  return toolcache[iscfile]
end

local function getsystemflags(tool, iscfile)
  return table.implode(getsystemincludepaths(tool, iscfile), ' -isystem \\"', '\\"', '')
end

local function getlanguageflags(cfg)
  local toolset = m.getToolset(cfg)
  local compileas = toolset.shared and toolset.shared.compileas
  if compileas then
	return compileas[cfg.language] or ""
  end
end

function m.generateCompileCommand(prj, cfg, node)
  local toolset = m.getToolset(cfg)
  local tool = path.iscfile(node.abspath) and 'cc' or 'cxx'
  cfg.gccprefix = cfg.gccprefix or "" -- hack to have gcc.gettoolname return non-nil
  local tool = toolset.gettoolname(cfg, tool) or tool
  local system_flag = getsystemflags(tool, path.iscfile(node.abspath))
  local language_flag = getlanguageflags(cfg) .. " "
  return {
    directory = prj.location,
    file = node.abspath,
    command = (tool .. system_flag .. " " .. language_flag .. table.concat(esc_table(m.getFileFlags(prj, cfg, node)), ' '))
  }
end

function m.includeFile(prj, node, depth)
  return path.iscppfile(node.abspath) or path.iscfile(node.abspath) or path.iscppheader(node.abspath)
end

function m.getProjectCommands(prj, cfg)
  local tr = project.getsourcetree(prj)
  local cmds = {}
  p.tree.traverse(tr, {
    onleaf = function(node, depth)
      if m.includeFile(prj, node, depth) then
        table.insert(cmds, m.generateCompileCommand(prj, cfg, node))
      end
    end
  })
  return cmds
end

function m.onWorkspace(wks)
  local cfgCmds = {}
  for prj in workspace.eachproject(wks) do
    for cfg in project.eachconfig(prj) do
      local cfgKey = string.format('%s', cfg.shortname)
      if not cfgCmds[cfgKey] then
        cfgCmds[cfgKey] = {}
      end
      cfgCmds[cfgKey] = table.join(cfgCmds[cfgKey], m.getProjectCommands(prj, cfg))
    end
  end
  for cfgKey,cmds in pairs(cfgCmds) do
    local outfile = string.format("%s/%s.json", path.getabsolute(".vscode/export-compile-comms/../compile_commands"), cfgKey)
    p.generate(wks, outfile, function(wks)
      p.push('[')
      for i = 1, #cmds do
        local item = cmds[i]
        p.push('{')
        p.x('"directory": "%s",', item.directory)
        p.x('"file": "%s",', item.file)
        p.w('"command": "%s"', item.command)
        if i ~= #cmds then
          p.pop('},')
        else
          p.pop('}')
        end
      end
      p.pop(']')
    end)
  end
end

newaction {
  trigger = 'export-compile-commands',
  description = 'Export compiler commands in JSON Compilation Database Format',
  onWorkspace = m.onWorkspace,
  toolset = "clang",
  valid_kinds = { "ConsoleApp", "WindowedApp", "Makefile", "SharedLib", "StaticLib", "Utility" },
  valid_languages = { "C", "C++" },
  valid_tools = {
    cc = { "gcc", "clang" }
  }
}


newaction {
  trigger = 'ecc',
  description = 'Export compiler commands in JSON Compilation Database Format',
  onWorkspace = m.onWorkspace,
  toolset = "clang",
  valid_kinds = { "ConsoleApp", "WindowedApp", "Makefile", "SharedLib", "StaticLib", "Utility" },
  valid_languages = { "C", "C++" },
  valid_tools = {
    cc = { "gcc", "clang" }
  }
}

return m
