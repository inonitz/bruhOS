#include <gtest/gtest.h>
#include <wchar.h>
#include <stdint.h>


#define IN
#define OUT
#define FALSE 0


// Include the function to be tested
void format_ascii_to_utf16(
    IN  const char* asciibuf,
    IN  uint64_t utf16_maxsize,
    OUT wchar_t* utf16_out
) {
    static const uint8_t ascii_max = 128;
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
    uint8_t invalid = FALSE;
    uint16_t i = 0;
    while (i < utf16_maxsize && asciibuf[i] != '\0') {
        invalid = (asciibuf[i] < 0 || (unsigned char)asciibuf[i] >= ascii_max);
        utf16_out[i] = invalid ? L'\ufffd' : utf16_map[(unsigned char)asciibuf[i]]; // 0xfffd (�)
        ++i;
    }
    if (i < utf16_maxsize) {
        utf16_out[i] = L'\0';
    } else {
        utf16_out[i - 1] = L'\0'; /* i - 1 = utf16_maxsize - 1 */
    }
    return;
}

// Test cases
TEST(FormatAsciiToUtf16Test, StandardAsciiTranslation) {
    const char* asciibuf = "Hello";
    uint64_t utf16_maxsize = 10;
    wchar_t utf16_out[10];

    format_ascii_to_utf16(asciibuf, utf16_maxsize, utf16_out);

    wchar_t expected[] = { L'H', L'e', L'l', L'l', L'o', L'\0' };

    for (int i = 0; i < 6; ++i) {
        ASSERT_EQ(utf16_out[i], expected[i]);
    }
}

TEST(FormatAsciiToUtf16Test, InvalidCharacterHandling) {
    const char* asciibuf = "Hello\x80World";  // '\x80' is invalid
    uint64_t utf16_maxsize = 10;
    wchar_t utf16_out[10];

    format_ascii_to_utf16(asciibuf, utf16_maxsize, utf16_out);

    wchar_t expected[] = { L'H', L'e', L'l', L'l', L'o', L'\ufffd', L'W', L'o', L'r', L'\0' };

    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(utf16_out[i], expected[i]);
    }
}

TEST(FormatAsciiToUtf16Test, StringTruncation) {
    const char* asciibuf = "ThisIsAVeryLongString";
    uint64_t utf16_maxsize = 10;
    wchar_t utf16_out[10];

    format_ascii_to_utf16(asciibuf, utf16_maxsize, utf16_out);

    wchar_t expected[] = { L'T', L'h', L'i', L's', L'I', L's', L'A', L'V', L'e', L'\0' };

    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(utf16_out[i], expected[i]);
    }
}

TEST(FormatAsciiToUtf16Test, EmptyString) {
    const char* asciibuf = "";
    uint64_t utf16_maxsize = 10;
    wchar_t utf16_out[10];

    format_ascii_to_utf16(asciibuf, utf16_maxsize, utf16_out);

    ASSERT_EQ(utf16_out[0], L'\0');
}

TEST(FormatAsciiToUtf16Test, BoundaryCharacter) {
    const char* asciibuf = "\x7F";  // ASCII boundary character
    uint64_t utf16_maxsize = 10;
    wchar_t utf16_out[10];

    format_ascii_to_utf16(asciibuf, utf16_maxsize, utf16_out);

    wchar_t expected[] = { L'\x7F', L'\0' };

    ASSERT_EQ(utf16_out[0], expected[0]);
    ASSERT_EQ(utf16_out[1], expected[1]);
}

TEST(FormatAsciiToUtf16Test, MaxSizeHandling) {
    const char* asciibuf = "MaxSize";
    uint64_t utf16_maxsize = 7;  // Length of the string "MaxSize" is 7
    wchar_t utf16_out[7];

    format_ascii_to_utf16(asciibuf, utf16_maxsize, utf16_out);

    wchar_t expected[] = { L'M', L'a', L'x', L'S', L'i', L'z', L'\0' };

    for (int i = 0; i < 7; ++i) {
        ASSERT_EQ(utf16_out[i], expected[i]);
    }
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); // Initialize GoogleTest
    return RUN_ALL_TESTS(); // Run all tests and return the result
}