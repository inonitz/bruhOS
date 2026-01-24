
function(generate_vscode_launchjson full_launch_json_path bootloader_binary bootloader_pdbsymbols kernel_elf_binary)
    file(GENERATE
        OUTPUT ${script_full_path}
        CONTENT
        "
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "lldb",
            "request": "launch",
            "name": "LLDB Wait For Qemu (Bootloader)",
            "targetCreateCommands": [
                "target create --no-dependents --arch x86_64 ${bootloader_binary} --symfile ${bootloader_pdbsymbols}",
                "target modules load --file bootx64.efi .text 0xEA5f000"
            ],
            "processCreateCommands": [
                "gdb-remote localhost:1234",
                "watchpoint set expression -w modify -s 8 -- 0x10000"
            ],
            // Useful Procedures:
            // target modules ... ADDR = 
            //     1. StartBin = starting address (link addr) of the binary
            //     2. TextAddr = starting address of the .text section.
            //     3. offset_to_text = addr(.text) - addr(start) [ TextAddr - StartBin ]
            //     4. right 'continue' in the lldb console, wait for the loaded_image_base
            //          to be printed on screen.
            //     * ADDR = loaded_image_base + offset_to_text
            // 
            // "breakpoint set -a ADDR_TO_BREAK_ON" - 
            //      * ADDR_TO_BREAK_ON = same procedure as above, replace addr(.text) with desired instruction addr
            // 
            // memory read --size 1 --format x --count 16 ADDR
        },

        {
            "type": "lldb",
            "request": "launch",
            "name": "LLDB Wait For Qemu (Kernel)",
            "targetCreateCommands": [
                "target create --no-dependents --arch x86_64 ${kernel_elf_binary}",
                // target modules load --file kernel.elf 0xffffff8000000000
                // Not really necessary ^
            ],
            "processCreateCommands": [
                "gdb-remote localhost:1234",
                // "breakpoint set --name _start",
                "breakpoint set --name actual_start"
            ]
        }
    ]
}
        "
    )