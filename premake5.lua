include ".vscode/premake5-lua/searchlocal.lua"
require(".vscode/export-compile-comms")
require(".vscode/premake-ninja")


WORKSPACE_NAME = "Workspace_bruhOS"
PROJECT_LIST = {
    "projects/bootloader",
    "projects/kernel"
}
START_PROJECT = "bootloader"


include "workspace.lua"
include ".vscode/premake5-lua/toolchain.lua"
include ".vscode/premake5-lua/dir.lua"
for _, path in ipairs(PROJECT_LIST) do
    include(path .. "/premake5.lua")
end
include ".vscode/premake5-lua/cmd.lua"
