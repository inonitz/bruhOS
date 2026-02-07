#include <gpcs2/console/console_instance.h>


/**
 * Output a character to a custom device like UART, used by the printf() function
 * This function is declared here only. You have to write your custom implementation somewhere
 * \param character Character to output
 */
void _putchar(char character) {
    if(unlikely(character == '\n')) {
        localConsoleNewline();
        return;
    }
    localConsoleWriteChar(character);
    return;
}