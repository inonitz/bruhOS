#include "user_ifce.h"
#include "console_instance.h"



void consoleClearScreen() {
    localConsoleClear();
}


void consoleSetTextColor(uint32_t rgb) {
    localConsoleSetTextColor(rgb & 0x00FFFFFF);
}


void consoleSetBackgroundColor(uint32_t rgb) {
    localConsoleSetBackColor(rgb & 0x00FFFFFF);
}


uint64_t consoleFramebufferSize()
{
    vec2us bdims = local_console.canvas.buffers.front.dims; 
    return sizeof(uint32_t) * (uint64_t)bdims.x * bdims.y;
}