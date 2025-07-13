#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <wchar.h>


static const char ascii_map[128] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31,
    ' ', '!', '"', '#', '$', '%', '&', '\'',
    '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
    'x', 'y', 'z', '{', '|', '}', '~', 127,
};


static const wchar_t utf16_map[128] = {
    L'\x00', L'\x01', L'\x02', L'\x03', L'\x04', L'\x05', L'\x06', L'\x07',
    L'\b'  , L'\t'  , L'\n'  , L'\v'  , L'\f'  , L'\r'  , L'\x0E', L'\x0F',
    L'\x10', L'\x11', L'\x12', L'\x13', L'\x14', L'\x15', L'\x16', L'\x17',
    L'\x18', L'\x19', L'\x1A', L'\x1B', L'\x1C', L'\x1D', L'\x1E', L'\x1F',
    L' ', L'!', L'"', L'#', L'$', L'%', L'&', L'\'', L'(', L')', L'*', L'+',
    L',', L'-', L'.', L'/', L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
    L'8', L'9', L':', L';', L'<', L'=', L'>', L'?', L'@', L'A', L'B', L'C',
    L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O',
    L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'[',
    L'\\', L']', L'^', L'_', L'`', L'a', L'b', L'c', L'd', L'e', L'f', L'g',
    L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r', L's',
    L't', L'u', L'v', L'w', L'x', L'y', L'z', L'{', L'|', L'}', L'~', L'\x7F'
};


wchar_t ascii_to_utf16(char ascii_char) {
    if (ascii_char < 0 || (unsigned char)ascii_char >= 128) {
        return L'\ufffd'; // Unicode replacement character (�)
    }
    return utf16_map[(unsigned char)ascii_char];
}


uint8_t run_tests() {
    uint8_t condition = 1;

    char ascii;
    wchar_t expected, actual;
    for (int i = 0; i < 128; ++i) {
        ascii    = ascii_map[i];
        expected = utf16_map[i];
        actual   = ascii_to_utf16(ascii);
        condition = condition && (expected == actual);
        printf("Test passed(? = %s) for ASCII: '%c' -> UTF-16: L'%lc'\n", condition ? "YES" : "NO", ascii, actual);
    }
    return condition;
}


int main() {
    if(run_tests())
        printf("All tests passed!\n");
    return 0;
}
