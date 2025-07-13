#include "bootloader/string.h"
#include "efibind.h"
#include <bootloader/bool_macro.h>

#include <bootloader/pause.h>

#include <bootloader/print.h>

#include <stdarg.h>


static void putc(CHAR16 _char);


static void format_ascii_to_utf16(
	IN  const char* asciibuf, 
	IN  uint64_t 	utf16_maxsize,
	OUT CHAR16* 	utf16_out
);
static CHAR16* formatd(
	IN  const uint64_t value,
	IN  const uint64_t size,
	IN  const BOOLEAN  neg,
	OUT CHAR16*  	   buffer
);
static CHAR16* formatu (IN uint32_t value, IN uint64_t size, OUT CHAR16* buffer);
static CHAR16* formatl (IN int64_t  value, IN uint64_t size, OUT CHAR16* buffer);
static CHAR16* formatul(IN uint64_t value, IN uint64_t size, OUT CHAR16* buffer);
static CHAR16* formatx(
	IN  uint64_t value, 
	IN  uint64_t size, 
	IN  BOOLEAN  is64,
	IN  BOOLEAN  uselower,
	OUT CHAR16*  buffer
);
static CHAR16* formatp (IN uint64_t value, IN uint64_t size, OUT CHAR16* buffer);
static CHAR16* formatb (
	IN  const uint64_t value, 
	IN  const uint64_t size, 
	IN  const uint32_t amountOfBits,
	OUT CHAR16*  	   buffer
);



/* 
	* prints string to UEFI->Bootservices->ConOut
	* format Specifier is %
	* format types:
	* d - int32_t
	* u - uint32_t
	* l - int64_t
	* z - uint64_t
	* c - CHAR16_t
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


void vprintf(const CHAR16* format, va_list args)
{
	static const uint32_t formatbufsize = 512U;
	static const CHAR16   eol 		   	= ENDOFLINE;
	static const CHAR16   fmtspecifier  = SPECIFIER;
	static CHAR16   	  utf16fmtbuf[512U] = {};
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
	CHAR16   argument_ch16;
	BOOLEAN  requiresFmtString = FALSE;
	const CHAR16* utf16fmt = (CHAR16*)NULL;
	const char*   utf8fmt  = (char*)NULL;


	
	memsetzerovoid(utf16fmtbuf, sizeof(CHAR16) * formatbufsize);
	memsetzerovoid(utf8fmtbuf , sizeof( char ) * formatbufsize);
	for(const CHAR16* character = &format[0]; *character != eol; ++character) 
	{
		while(*character != fmtspecifier && *character != eol) {
			putc(*character);
			++character;
		}
		if(*character == eol) {
			--character;
			continue;
		}

		requiresFmtString = FALSE;
		utf16fmt	  	  = (const CHAR16*)NULL;
		utf8fmt	 	  	  = (const char*  )NULL;
		/* we found a fmtspecifier */
		switch( *(character + 1) ) {
			
			case ENDOFLINE: /* L'\0\ */
			putc(L'%');
			continue;
			break;

			case NEWLINE: /* L'\n' */
			putc(L'%');
			putc(L'\n');
			++character;
			continue;
			break;

			case DIGIT: /* L'd' */
			argument_i32 = va_arg(args, int32_t);
			utf16fmt = formatl((int64_t)argument_i32, formatbufsize, &utf16fmtbuf[0]);
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
			argument_ch16 = (CHAR16)va_arg(args, int32_t);
			putc(argument_ch16);
			++character;
			break;
			
			case STRING: /* L's' */
			utf16fmt = (const CHAR16*)va_arg(args, const CHAR16*);
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
			putc( *( character + 0 ) );
			putc( *( character + 1 ) );
			break;
		}
		if(blboolean(requiresFmtString)) {
			for(; *utf16fmt != eol; ++utf16fmt) { putc(*utf16fmt); }
		}
	}
}


void printf(const CHAR16* format, ...)
{
	va_list args; 
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	return;
}


static void putc(CHAR16 _char)
{
	static CHAR16 single_char_str[2] = { L'\0', L'\0' };

	single_char_str[0] = _char;
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, single_char_str);
	return;
}




static void format_ascii_to_utf16(
	IN  const char* asciibuf, 
	IN  uint64_t 	utf16_maxsize,
	OUT CHAR16* 	utf16_out
) {
	static const uint8_t ascii_max = 128U;
	static const CHAR16 ascii_to_utf16[128] = {
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


static CHAR16* formatd(
	IN  const uint64_t value,
	IN  const uint64_t size,
	IN  const BOOLEAN  neg,
	OUT CHAR16*  	   buffer
) {
	static const CHAR16  Representation[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9' };
	static const int64_t digitBase         = 10;
	uint32_t digitCount = 0;
	uint32_t tmp  	    = 0;

	if(__builtin_expect(value == 0, FALSE)) {
		*(buffer + 0) = L'0';
		*(buffer + 1) = L'\0';
		return buffer;
	}


	*buffer = neg ? L'-' : '+';
	for(uint64_t num = value; num > 0; num /= digitBase, ++digitCount) {}
	
	tmp = digitCount;
	if(digitCount > size)
		return (CHAR16*)NULL;


	for(uint64_t num = value; num > 0; num /= digitBase) {
		buffer[tmp] = Representation[num % digitBase];
		--tmp;
	}
	buffer[digitCount + 1] = L'\0';

	return buffer;
}


static CHAR16* formatu(
	IN  uint32_t value, 
	IN  uint64_t size, 
	OUT CHAR16*  buffer
) {
	return formatd((uint64_t)value, size, FALSE, buffer);
}

static CHAR16* formatl (
	IN int64_t  value, 
	IN uint64_t size, 
	OUT CHAR16* buffer
) {
	const BOOLEAN ltz = blboolean(value < 0);
	uint64_t val = (uint64_t)( value * (ltz ? -1 : 1) );
	return formatd(val, size, ltz, buffer);
}

static CHAR16* formatul(
	IN uint64_t value, 
	IN uint64_t size, 
	OUT CHAR16* buffer
) {
	return formatd(value, size, FALSE, buffer);
}

static CHAR16* formatx(
	IN  uint64_t value, 
	IN  uint64_t size, 
	IN  BOOLEAN  is64,
	IN  BOOLEAN  uselower,
	OUT CHAR16*  buffer
) {
	static const CHAR16  RepresentationLower[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'a', L'b', L'c', L'd', L'e', L'f' };
	static const CHAR16  RepresentationUpper[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };
	static const int64_t  digitBase        	    = 16;
	const CHAR16* 		  Representation	    = uselower ? 
		RepresentationLower : RepresentationUpper;
	const int64_t 		 totalDigits            = is64 ? 16 : 8;
	uint32_t 	  		 digitCount 	        = 0;
	uint32_t 	  		 tmp  	    	        = 0;


	if(value == 0) {
		*buffer = L'0'; /* totalDigits will pad the rest of the digits required */
	}


	for(uint64_t num = value; num > 0; num /= digitBase, ++digitCount) {}
	
	tmp = digitCount;
	if(digitCount > size)
		return (CHAR16*)NULL;

	
	for(int64_t num = 0; num < totalDigits - digitCount; ++num) {
		buffer[num] = L'0';
	}
	tmp = totalDigits;
	for(uint64_t num = value; num > 0; num /= digitBase) {
		--tmp;
		buffer[tmp] = Representation[num % digitBase];
	}
	buffer[totalDigits] = L'\0';

	return buffer;
}


static CHAR16* formatp(
	IN  uint64_t value, 
	IN  uint64_t size, 
	OUT CHAR16* buffer
	)
{
	if(size < 2) {
		return (CHAR16*)NULL;
	}


	buffer[0] = L'0';
	buffer[1] = L'x';
	formatx(value, size - 2, TRUE, TRUE, buffer + 2);
	return buffer;
}


// static uint64_t round2(uint64_t n);
// static uint64_t log2ul(uint64_t bits); 


static CHAR16* formatb (
	IN  const uint64_t value, 
	IN  const uint64_t size, 
	IN  const uint32_t amountOfBits,
	OUT CHAR16*  buffer
) {
	static const CHAR16  Representation[] = { L'0', L'1' };
	static const int64_t digitBase        = 2;
	const int64_t 		 totalDigits      = amountOfBits + 2;
	uint32_t 	  		 digitCount		  = 0;
	uint32_t 	  		 tmp  	   		  = 0;


	if(amountOfBits + 2 > size)
		return (CHAR16*)NULL;

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