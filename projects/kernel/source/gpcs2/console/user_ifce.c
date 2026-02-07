#include <gpcs2/console/user_ifce.h>
#include <gpcs2/console/console_instance.h>
#include <stddef.h>


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
    framebuffer_t* fbInfo = &local_console.canvas.buffers.front;

    uint64_t result = (uint64_t)(fbInfo->m_pixelElementSizeBytes * fbInfo->m_pixelsPerScanLine);
    result *= fbInfo->m_height;
    return result;

}