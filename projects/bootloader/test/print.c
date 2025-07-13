#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>


typedef uint8_t         CHAR8;
typedef uint8_t         BOOLEAN;
#ifndef IN
    #define IN
    #define OUT
    #define OPTIONAL
#endif
#ifndef CONST
   #define CONST const
#endif
#ifndef TRUE
    #define TRUE    ((BOOLEAN) 1)
    #define FALSE   ((BOOLEAN) 0)
#endif
#ifndef __BOOTLOADER_BOOLEAN_MACRO_HEADER__
#define __BOOTLOADER_BOOLEAN_MACRO_HEADER__

#ifndef blboolean
#   define blboolean(arg) (!!(arg))
#endif

#endif /* __BOOTLOADER_BOOLEAN_MACRO_HEADER__ */
#ifndef __BOOTLOADER_C_DEFINITION_PRINT_FORMAT__
#define __BOOTLOADER_C_DEFINITION_PRINT_FORMAT__
#include <efi.h>
#include <efilib.h>

/* 
	* prints string to UEFI->Bootservices->ConOut
	* format Specifier is %
	* format types:
	* d - int32_t
	* u - uint32_t
	* l - int64_t
	* z - uint64_t
	* c - char_t
	* w - char16_t (not supported YET)
	* s - string   (array of chars)
	* x - hex number      (max_size = uint32_t)
	* X - hex number      (max_size = uint64_t)
	* p - address pointer (max_size = uint64_t)
	* b - binary		  (max_size = uint64_t)
*/
void vprintb(const CHAR16* format, va_list args);


void printb(const CHAR16* format, ...);


// void     printb_align_format(uint16_t padding_amount);
// uint16_t printb_get_align(void);


#endif /* __BOOTLOADER_C_DEFINITION_PRINT_FORMAT__ */


#include <stdarg.h>


static void putbc(wchar_t _char);
static void memsetzerovoid(void* addr, uint32_t size);


static void format_ascii_to_utf16(
	IN  const char* asciibuf, 
	IN  uint64_t 	utf16_maxsize,
	OUT wchar_t* 	utf16_out
);
static wchar_t* formatd(
	IN  const int64_t  value,
	IN  const uint64_t size, 
	OUT wchar_t*  	   buffer
);
static wchar_t* formatu (IN uint32_t value, IN uint64_t size, OUT wchar_t* buffer);
static wchar_t* formatl (IN int64_t  value, IN uint64_t size, OUT wchar_t* buffer);
static wchar_t* formatul(IN uint64_t value, IN uint64_t size, OUT wchar_t* buffer);
static wchar_t* formatx(
	IN  uint64_t value, 
	IN  uint64_t size, 
	IN  BOOLEAN  is64,
	IN  BOOLEAN  uselower,
	OUT wchar_t*  buffer
);
static wchar_t* formatp (IN uint64_t value, IN uint64_t size, OUT wchar_t* buffer);
static wchar_t* formatb (
	IN  const uint64_t value, 
	IN  const uint64_t size, 
	IN  const uint32_t amountOfBits,
	OUT wchar_t*  	   buffer
);



/* 
	* prints string to UEFI->Bootservices->ConOut
	* format Specifier is %
	* format types:
	* d - int32_t
	* u - uint32_t
	* l - int64_t
	* z - uint64_t
	* c - wchar_t_t
	* s - string   (array of chars)
	* x - hex number      (max_size = uint32_t)
	* X - hex number      (max_size = uint64_t)
	* p - address pointer (max_size = uint64_t)
	* b - binary		  (max_size = uint32_t)
	* B - binary		  (max_size = uint64_t)
*/
enum formatTypeList {
	ENDOFLINE     = L'\0', 
	NEWLINE       = L'\n', 
	DIGIT 	      = L'd', 
	UNSIGNED      = L'u', 
	LONG_DIGIT    = L'l',
	LONG_UNSIGNED = L'z', 
	CHAR          = L'c', 
	STRING 		  = L's', 
	UTF8_STRING   = L'a',
	HEX32         = L'x', 
	HEX64 		  = L'X', 
	POINTER64     = L'p', 
	BINARY32      = L'b', 
	BINARY64      = L'B',
    SPECIFIER     = L'%'
};


void vprintb(const wchar_t* format, va_list args)
{
	static const uint32_t formatbufsize = 512U;
	static const wchar_t   eol 		   	= ENDOFLINE;
	static const wchar_t   fmtspecifier  = SPECIFIER;
	static wchar_t   	  utf16fmtbuf[512U] = {};
	static char   	      utf8fmtbuf [512U] = {};

	int32_t  argument_i32;
	int64_t  argument_i64;
	uint32_t argument_u32;
	uint64_t argument_u64;
	uint32_t argument_h32;
	uint64_t argument_h64;
	uint64_t argument_p64;
	uint64_t argument_b32;
	uint64_t argument_b64;
	wchar_t   argument_ch16;
	BOOLEAN  requiresFmtString = FALSE;
	const wchar_t* utf16fmt = (wchar_t*)NULL;
	const char*   utf8fmt  = (char*)NULL;


	
	memsetzerovoid(utf16fmtbuf, sizeof(wchar_t) * formatbufsize);
	memsetzerovoid(utf8fmtbuf , sizeof( char ) * formatbufsize);
	for(const wchar_t* character = &format[0]; *character != eol; ++character) 
	{
		while(*character != fmtspecifier && *character != eol) {
			putbc(*character);
			++character;
		}
		if(*character == eol) {
			--character;
			continue;
		}

		requiresFmtString = FALSE;
		utf16fmt	  	  = (const wchar_t*)NULL;
		utf8fmt	 	  	  = (const char*  )NULL;
		/* we found a fmtspecifier */
		switch( *(character + 1) ) {
			
			case ENDOFLINE: /* L'\0\ */
			putbc(L'%');
			continue;
			break;

			case NEWLINE: /* L'\n' */
			putbc(L'%');
			putbc(L'\n');
			++character;
			continue;
			break;

			case DIGIT: /* L'd' */
			argument_i32 = va_arg(args, int32_t);
			utf16fmt = formatd(argument_i32, formatbufsize, &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;
			
			case UNSIGNED: /* L'u' */
			argument_u32 = va_arg(args, uint32_t);
			utf16fmt = formatu(argument_u32, formatbufsize, &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;

			case LONG_DIGIT: /* L'l' */
			argument_i64 = va_arg(args, int64_t);
			utf16fmt = formatl(argument_i64, formatbufsize, &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;
			
			case LONG_UNSIGNED: /* L'z' */
			argument_u64 = va_arg(args, uint64_t);
			utf16fmt = formatul(argument_u64, formatbufsize, &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;
			
			case CHAR: /* L'c' */
			argument_ch16 = (wchar_t)va_arg(args, int32_t);
			putbc(argument_ch16);
			++character;
			break;
			
			case STRING: /* L's' */
			utf16fmt = (const wchar_t*)va_arg(args, const wchar_t*);
			++character;
			requiresFmtString = TRUE;
			break;

			case UTF8_STRING: /* L'a' */
			utf8fmt = va_arg(args, const char*);
			format_ascii_to_utf16(utf8fmt, formatbufsize, &utf16fmtbuf[0]);
			utf16fmt = &utf16fmtbuf[0];
			++character;
			requiresFmtString = TRUE;
			break;
			
			case HEX32: /* L'x' */
			argument_h32 = va_arg(args, uint32_t);
			utf16fmt = formatx(argument_h32, formatbufsize, FALSE, FALSE, &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;
			
			case HEX64: /* L'X' */
			argument_h64 = va_arg(args, uint64_t);
			utf16fmt = formatx(argument_h64, formatbufsize, TRUE, FALSE, &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;
			
			case POINTER64: /* L'p' */
			argument_p64 = va_arg(args, uint64_t);
			utf16fmt = formatp(argument_p64, formatbufsize, &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;
			
			case BINARY32: /* L'b' */
			argument_b32 = va_arg(args, uint32_t);
			utf16fmt = formatb(argument_b32, formatbufsize, 8 * sizeof(uint32_t), &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;

			case BINARY64: /* L'B' */
			argument_b64 = va_arg(args, uint64_t);
			utf16fmt = formatb(argument_b64, formatbufsize, 8 * sizeof(uint64_t), &utf16fmtbuf[0]);
			++character;
			requiresFmtString = TRUE;
			break;
			
			default:
			putbc( *( character + 0 ) );
			putbc( *( character + 1 ) );
			break;
		}
		if(blboolean(requiresFmtString)) {
			for(; *utf16fmt != eol; ++utf16fmt) { putbc(*utf16fmt); }
		}
	}
}


void printb(const wchar_t* format, ...)
{
	va_list args; 
	va_start(args, format);
	vprintb(format, args);
	va_end(args);
	return;
}


static void putbc(wchar_t _char)
{
	static wchar_t single_char_str[2] = { L'\0', L'\0' };

	single_char_str[0] = _char;
    putwchar(single_char_str[0]);
	return;
}


static void memsetzerovoid(void* addr, uint32_t size)
{
	memset(addr, 0x00, size);
	return;
}




static void format_ascii_to_utf16(
	IN  const char* asciibuf, 
	IN  uint64_t 	utf16_maxsize,
	OUT wchar_t* 	utf16_out
) {
	static const uint8_t ascii_max = 128U;
	static const wchar_t ascii_to_utf16[128] = {
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
	while(i < utf16_maxsize && asciibuf[i] != '\0') {
		invalid = blboolean(asciibuf[i] < 0 || (unsigned char)asciibuf[i] >= ascii_max); 
		utf16_out[i] = blboolean(invalid) ? L'\ufffd' : ascii_to_utf16[(unsigned char)asciibuf[i]]; // 0xfffd (�)
		++i;
	}
	if(i < utf16_maxsize) {
		utf16_out[i] = L'\0';
	} else {
		utf16_out[i - 1] = L'\0'; /* i - 1 = utf16_maxsize - 1 */
	}
    return;
}


static wchar_t* formatd(
	IN  const int64_t  value,
	IN  const uint64_t size, 
	OUT wchar_t*  	   buffer
) {
	static const wchar_t  Representation[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9' };
	static const int64_t digitBase         = 10;
	const BOOLEAN ltz = value < 0;
	uint32_t digitCount = 0;
	uint32_t tmp  	    = 0;

	if(value == 0) {
		*(buffer + 0) = L'0';
		*(buffer + 1) = L'\0';
		return buffer;
	}


	*buffer = ltz ? L'-' : '+';
	for(int64_t num = value * (ltz ? -1 : 1); num > 0; num /= digitBase, ++digitCount) {}
	
	tmp = digitCount;
	if(digitCount > size)
		return (wchar_t*)NULL;


	for(int64_t num = value * (ltz ? -1 : 1); num > 0; num /= digitBase) {
		buffer[tmp] = Representation[num % digitBase];
		--tmp;
	}
	buffer[digitCount + 1] = L'\0';

	return buffer;
}


static wchar_t* formatu(
	IN  uint32_t value, 
	IN  uint64_t size, 
	OUT wchar_t*  buffer
) {
	return formatd((int32_t)value, size, buffer);
}


static wchar_t* formatl (IN int64_t  value, IN uint64_t size, OUT wchar_t* buffer) {
	return formatd(value, size, buffer);
}
static wchar_t* formatul(IN uint64_t value, IN uint64_t size, OUT wchar_t* buffer) {
	return formatd((int64_t)value, size, buffer);
}

static wchar_t* formatx(
	IN  uint64_t value, 
	IN  uint64_t size, 
	IN  BOOLEAN  is64,
	IN  BOOLEAN  uselower,
	OUT wchar_t*  buffer
)
{
	static const wchar_t  RepresentationLower[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'a', L'b', L'c', L'd', L'e', L'f' };
	static const wchar_t  RepresentationUpper[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };
	static const int64_t  digitBase        	    = 16;
	const wchar_t* 		  Representation	    = uselower ? 
		RepresentationLower : RepresentationUpper;
	const int64_t 		 totalDigits            = is64 ? 16 : 8;
	uint32_t 	  		 digitCount 	        = 0;
	uint32_t 	  		 tmp  	    	        = 0;


	if(value == 0) {
		*buffer = L'0'; /* totalDigits will pad the rest of the digits required */
	}


	for(int64_t num = value; num > 0; num /= digitBase, ++digitCount) {}
	
	tmp = digitCount;
	if(digitCount > size)
		return (wchar_t*)NULL;

	
	for(int64_t num = 0; num < totalDigits - digitCount; ++num) {
		buffer[num] = L'0';
	}
	tmp = totalDigits;
	for(int64_t num = value; num > 0; num /= digitBase) {
		--tmp;
		buffer[tmp] = Representation[num % digitBase];
	}
	buffer[totalDigits] = L'\0';

	return buffer;
}


static wchar_t* formatp(
	IN  uint64_t value, 
	IN  uint64_t size, 
	OUT wchar_t* buffer
	)
{
	if(size < 2) {
		return (wchar_t*)NULL;
	}


	buffer[0] = L'0';
	buffer[1] = L'x';
	formatx(value, size - 2, TRUE, TRUE, buffer + 2);
	return buffer;
}


// static uint64_t round2(uint64_t n);
// static uint64_t log2ul(uint64_t bits); 


static wchar_t* formatb (
	IN  const uint64_t value, 
	IN  const uint64_t size, 
	IN  const uint32_t amountOfBits,
	OUT wchar_t*  buffer
) {
	static const wchar_t  Representation[] = { L'0', L'1' };
	static const int64_t digitBase        = 2;
	const int64_t 		 totalDigits      = amountOfBits + 2;
	uint32_t 	  		 digitCount		  = 0;
	uint32_t 	  		 tmp  	   		  = 0;


	if(amountOfBits + 2 > size)
		return (wchar_t*)NULL;

	buffer[0] = L'0';
	buffer[1] = L'b';
	for(int64_t num = (int64_t)value; num > 0; num /= digitBase, ++digitCount) {}	

	tmp = digitCount;
	for(int64_t num = 2; num < totalDigits - digitCount + 2; ++num) {
		buffer[num] = L'0';
	}
	tmp = totalDigits;
	for(int64_t num = (int64_t)value; num > 0; num /= digitBase) {
		--tmp;
		buffer[tmp] = Representation[num % digitBase];
	}
	buffer[totalDigits] = L'\0';

	return buffer;
}


int main()
{
	printb(L"hello from kernel entry!!!\n");
    printb(L"Abruhb%ruhb%druh\n\n\n", -5151);
    printb(L"Abruhb%ruhb%druh\n\n\n", 313512);
    printb(L"worked? ");
    printb(L"ad ");
    printb(L"bd  ");
    printb(L"cd   %l", 0x50000000000000);
    printb(L"ed    %\n%\ndada\n\n\d\da");
    printb(L"testing string %s", L"bruh your mom");
    printb(L"test %u\n", 0xDEADBEEF);
    printb(L"test %l\n", 0xDEADBEEFEEEE);
    printb(L"test %l\n", 0xAFFFFFFFFFFFFFFF);
    printb(L"test %d\n", -5151);
    printb(L"test %x\n", 0xDEAD);
    printb(L"test %x\n", 0xDEADBEEF);
    printb(L"test %x\n", 0xDEADBEEFFFF);
    printb(L"test %X\n", 0xDEADBEEFFFF);
    printb(L"test %p\n", 0x90000000);
    printb(L"test %p\n", 0x00);
    printb(L"test %b\n", 0b01010100101);
    printb(L"test %x\n", 0);
    printb(L"%s <--- this is a string.\n", L"your mom");
    printb(L"%c <--- this is a character.\n", L'S');
    printb(L"Abruhb%ruhb%druh\n\n\n\r\r\r\r", 787873120);
    printb(L"this is a > %s < string.\n", L"your mom");
    printb(L"this is a > %c < character.\n", L'S');
    printb(L"%a <--- this is an ascii string.\n", "your mom");
    printb(L"%a <--- this is an ascii character.\n", "S");
    printb(L"\
    PT_LOAD Section Loaded at %p (%z Bytes). Offset %z\n\r\
        Offset: %z\n\r\
        Virtual Address : %p\n\r\
        Physical Address: %p\n\r\
        File Size  : %X (Bytes)\n\r\
        Memory Size: %X (Bytes)\n\r\
        Alignment  : %u (Bytes)\n\r\
        pages allocated at: %p\n\r\
    ",
        0x7777f807, 0x37, 333333338981235,
        0x5000,
        0xfffffffff99fe,
        0x50505030,
        0xfffff,
        0xf3fbd,
        4096,
        0x8000000000
    );
    printb(L" virtual_addr : %p\n\r", 0xffffff8000000000);
}