#include <std/io.h>
#include <std/enable_bool.h>
#include <std/bool.h>
#include <std/string.h>
#include <std/math.h>
#include <gpcs2/console/console_instance.h>
#include <stdarg.h>


// inline definitions
// extern __force_inline void* memset(void* location, uint8_t val, uint64_t size);


// statics
// this array just doesn't want to be initialized to 0, because I'm loading the elf file improperly
// Don't have time to fix that.
static const uint64_t ITSbufSize 		  = 140u;
static 		 char_t   intToStrBuffer[140] = {0};
static       int16_t  format_align_pad     = 0;


// funcs
// uint64_t strlen(char_t* string)
// {
// 	uint32_t r = 0;
// 	while (*string != '\0') { ++string; ++r; }
// 	return r;
// }


// char_t* strcpy(char_t* str1, char_t* str2)
// {
// 	while (*str1 != '\0')
// 	{
// 		*str2 = *str1;
// 		++str1;
// 		++str2;
// 	}
// 	return str2;
// }


// void* memset(void* location, uint8_t val, uint64_t size)
// {
// 	char_t* ptr = (char_t*)location + size - 1;
// 	for (; size != 0; --size)
// 	{
// 		*ptr = val;
// 		--ptr;
// 	}
// 	return location;
// }


// uint8_t memcmp(char_t* a, char_t* b, int64_t size)
// {
// 	--size;
// 	while(size >= 0 && a[size] == b[size]) {
// 		--size;
// 	}
// 	return size+1;
// }


// void memcpy(void* dst, void* src, uint64_t amount)
// {
// 	for(uint64_t i = 0; i < amount; ++i)
// 	{
// 		((uint8_t*)dst)[i] = ((uint8_t*)src)[i]; 
// 	}
// }


// format specifier functions.
static char_t* formatd(
	IN  int32_t  value,
	IN  uint64_t size, 
	OUT char_t* buffer
	)
{
	char_t* ptr = buffer + size - 2;
	if (!value) { 
		*ptr = '0';
		for(int32_t i = 0; i < format_align_pad - 1; ++i)
		{
			--ptr;
			*ptr = ' ';
		} 
		return ptr;
	}
	

	const char_t Representation[] = "0123456789";
	bool_t       ltz 	 = value < 0;
	int16_t 	 padding = 0;
	char_t 		 result  = '\0';


	value *= (-1 * ltz + 1 * !ltz);
	for (; value != 0; value /= 10, --ptr) 
	{ 
		*ptr = Representation[value % 10];
		++padding;
	}

	// padding if format_align_pad > 0
	padding = (padding < (format_align_pad - (int8_t)ltz)) * (format_align_pad - (int8_t)ltz - padding);
	ptr	   -= padding;
	memset(ptr+1, ' ', padding);

	result = ltz * '-';
	result += !ltz * (*ptr);
	*ptr = result;
	ptr += !ltz;

	return ptr;
}


static char_t* formatu(
	IN  uint32_t value, 
	IN  uint64_t size, 
	OUT char_t* buffer
	)
{
	char_t* ptr = buffer + size - 2;
	if (!boolean(value)) { 
		*ptr = '0';
		for(int32_t i = 0; i < format_align_pad - 1; ++i)
		{
			--ptr;
			*ptr = ' ';
		} 
		return ptr;
	}


	const char_t Representation[] = "0123456789";
	int16_t  	 padding 		  = 0;
	for (; value != 0; value /= 10, --ptr) { *ptr = Representation[value % 10]; ++padding; }
	
	// padding if format_align_pad > 0
	padding = (padding < format_align_pad) * (format_align_pad - padding);
	ptr    -= padding;
	memset(ptr+1, ' ', padding);

	return ++ptr;
}


static char_t* formatl(
	IN  int64_t  value, 
	IN  uint64_t size, 
	OUT char_t* buffer
	)
{
	char_t* ptr = buffer + size - 2;
	if (!value) { 
		*ptr = '0';
		for(int32_t i = 0; i < format_align_pad - 1; ++i)
		{
			--ptr;
			*ptr = ' ';
		} 
		return ptr;
	}


	const char_t Representation[] = "0123456789";
	bool_t 	     ltz     = value < 0;
	int16_t 	 padding = 0;
	char_t 		 result  = '\0';

	value *= (-1 * ltz + 1 * !ltz);
	for (; value != 0; value /= 10, --ptr) { *ptr = Representation[value % 10]; ++padding; }
	
	padding = (padding < (format_align_pad - (int8_t)ltz)) * (format_align_pad - (int8_t)ltz - padding);
	ptr	   -= padding;
	memset(ptr+1, ' ', padding);

	
	result = ltz * '-';
	result += !ltz * (*ptr);

	*ptr = result;
	ptr += !ltz;


	return ptr;
}


static char_t* formatul(
	IN  uint64_t   value, 
	IN  uint64_t   size, 
	OUT char_t* buffer
	)
{
	char_t* ptr = buffer + size - 2;
	if (!value) { 
		*ptr = '0';
		for(int32_t i = 0; i < format_align_pad - 1; ++i)
		{
			--ptr;
			*ptr = ' ';
		} 
		return ptr;
	}


	const char_t Representation[] = "0123456789";
	int16_t 	 padding 		  = 0;
	while (value != 0)
	{
		*ptr   = Representation[value % 10];
		value /= 10;
		--ptr;
		++padding;
	}

	padding = (padding < format_align_pad) * (format_align_pad - padding);
	ptr    -= padding;
	memset(ptr+1, ' ', padding);

	return ++ptr;
}


static char_t* formatx(
	IN  uint64_t value, 
	IN  uint64_t size, 
	IN  bool_t   is64,
	OUT char_t* buffer
	)
{
	char_t* ptr = buffer + size - 2;
	// if you want to enable padding by format_align_padding, use this line (in second for loop):
	// padding = (str_size < format_digits - 2) * (format_digits - str_size - 2) + (str_size > format_digits - 2) * (8 * (1 + boolean(is64)))

	// if (!boolean(value))
	// {
	// 	*ptr = '0'; --ptr;
	// 	*ptr = '0'; --ptr;
	// }
	// else 
	{
		const char_t Representation[] = "0123456789ABCDEF";
		uint32_t chars = 0;
		for (; value != 0; value /= 16, --ptr) { *ptr = Representation[value % 16]; ++chars; }
		for (; chars < 8 * (1 + boolean(is64)); ++chars, --ptr) *ptr = '0';
	}
	*ptr   = 'x';
	*--ptr = '0';

	return ptr;
}


static char_t* formatp(
	IN  uint64_t value, 
	IN  uint64_t   size, 
	OUT char_t* buffer
	)
{
	char_t* ptr = buffer + size - 2;
	if (!value) { 
		*ptr = '0';
		for(int32_t i = 0; i < format_align_pad - 1; ++i)
		{
			--ptr;
			*ptr = '0';
		} 
		return ptr;
	}


	const char_t Representation[] = "0123456789ABCDEF";
	int16_t      padding          = 0;

	for (; value != 0; value /= 16, --ptr) { *ptr = Representation[value % 16]; ++padding; }
	padding = (padding > format_align_pad) * (16 - padding) + (padding < format_align_pad) * (format_align_pad - padding);

	for (int32_t i = 0; i < padding; ++i, --ptr) { *ptr = '0'; }

	return ++ptr;
}


static char_t* formatb(
	IN  uint64_t value, 
	IN  uint64_t   size, 
	OUT char_t*  buffer
	)
{
	const char_t Representation[] = "01";
	char_t* ptr                   = buffer + size - 2;
	uint64_t currDigits           = (uint64_t)ptr;
	uint8_t  max_bit              = 0;

	max_bit = (uint8_t)round2(log2ul(value + (value == 0)));
	max_bit = (max_bit < 8) * 8 + (max_bit > 8) * max_bit;

	for (; value != 0; value /= 2, --ptr) { *ptr = Representation[value % 2]; }
	currDigits -= (uint64_t)ptr;
	for (; currDigits < (uint64_t)max_bit; ++currDigits, --ptr) { *ptr = '0'; }
	*ptr   = 'b';
	*--ptr = '0';
	return ptr;
}



void puts(const char_t* string)
{
	char_t val = 0;
	for(; *string != '\0'; ++string)
	{
		// if val is an invalid character like '\n', we print a space instead.
		val  = *string;
		val *=  val > 32;
		val += 32 * !val; 
		localConsoleWriteChar(val);
	}
}


void putln(void)
{
	localConsoleNewline();
}


static void print_internal(const char_t* string, va_list arglist)
{
	uint64_t argval;
	uint32_t start    = 0; 
	uint32_t end 	  = 0;
	bool_t   dontRes  = BOOLEAN_FALSE;
	char_t   endl     = '\0';
	char_t*  intToStr = &endl;


	for(const char_t* c = string; *c != '\0'; ++c)
	{
		dontRes = BOOLEAN_FALSE;

		// find next % or \n
		while(*c != '%' && *c != '\n' && *c != '\0') {
			++c;
			++end;
		}
		// print the string from the start till the end, where there is no % or \n
		localConsoleWriteSubstring(string, start, end);
		start = ++end;


		switch(*c)
		{
			case '\n':
				localConsoleNewline();
				continue;

			case '\0':
				--c; // continue increments var c before going to the top of the loop.
					// since *++c is undefined behaviour, we decrement it to exit the loop. 				
				continue;
			
			case '%':
				// parse the proper format specifier.
				switch(*++c)
				{
					case 'd':
					intToStr = formatd(va_arg(arglist, int32_t), ITSbufSize, intToStrBuffer);
					break;

					case 'x':
					intToStr = formatx(va_arg(arglist, uint32_t), ITSbufSize, BOOLEAN_FALSE, intToStrBuffer);
					break;

					case 'X':
					argval   = va_arg(arglist, uint64_t);
					intToStr = formatx(argval,  ITSbufSize, BOOLEAN_TRUE, intToStrBuffer);
					break;

					case 'p':
					argval   = va_arg(arglist, uint64_t);
					intToStr = formatp(argval,  ITSbufSize, intToStrBuffer);
					break;

					case 'z':
					argval   = va_arg(arglist, uint64_t);
					intToStr = formatul(argval,  ITSbufSize, intToStrBuffer);
					break;

					case 'u':
					intToStr = formatu(va_arg(arglist, uint32_t), ITSbufSize, intToStrBuffer);
					break;

					case 'b':
					argval   = va_arg(arglist, uint64_t);
					intToStr = formatb(argval,  ITSbufSize, intToStrBuffer);
					break;

					case 's':
					localConsoleWriteString((const char_t*)va_arg(arglist, const char_t*));
					dontRes = BOOLEAN_TRUE;
					break;

					case 'c':
					localConsoleWriteChar(va_arg(arglist, int32_t) & 0xFF);
					dontRes = BOOLEAN_TRUE;
					break;

					case 'l':
					intToStr = formatl(va_arg(arglist, int64_t), ITSbufSize, intToStrBuffer);
					break;

					default:
					localConsoleWriteChar('%'); // if this isn't %format, then the % is a regular sign and we should print it 
					dontRes = BOOLEAN_TRUE;		    // we didn't change the buffer, so we don't reset it.
					start = --end; 				    // we dont need to skip format char as it isn't one.
					--c;							// we need to print the char that was tested for format specification
					break;
				}

				localConsoleWriteString(intToStr);
				start = ++end; 						// skip format specifier char
				if(!dontRes) {
					memset(intToStr, 0x00, ITSbufSize - (uint64_t)(intToStr - intToStrBuffer));
				}
				break;
			default:
			// Should never reach here
			break;
		}
	}
}


void printk(const char_t* string, ...)
{
	va_list args; 
	va_start(args, string);
	print_internal(string, args); 

	va_end(args);
	return;
}


void printkcol(uint64_t foreback, const char_t* string, ...)
{
	uint32_t front = local_console.text_col;
	uint32_t back  = local_console.background_col;

	foreback &= 0x00FFFFFF00FFFFFFu; 			     // arg safety - see gpcs2/gdef.h 
	localConsoleSetBackColor(foreback & 0x00FFFFFF); // low  bits
	localConsoleSetTextColor(foreback >> 32);		 // high bits
	
	va_list args; 
	va_start(args, string);
	print_internal(string, args); 

	va_end(args);

	localConsoleSetBackColor(back);
	localConsoleSetTextColor(front);
	return;
}


void 	 printk_align_format(uint16_t padding_amount) {
	format_align_pad = (int16_t)padding_amount;
}
uint16_t printk_get_align   (void) { 
	return (uint16_t)format_align_pad;
}


/* 
Debugging Statements to make sure the printk works correctly:
	printk("hello from kernel entry!!!\n");
    printk("Abruhb%ruhb%druh\n\n\n", -5151);
    printk("Abruhb%ruhb%druh\n\n\n", 313512);
    printk("worked? ");
    printk("ad ");
    printk("bd  ");
    printk("cd   ");
    printk("ed    ");
    printk("testing string %s", "bruh your mom");
    printk("test %u\n", 0xDEADBEEF);
    printk("test %l\n", 0xDEADBEEFEEEE);
    printk("test %l\n", 0xAFFFFFFFFFFFFFFF);
    printk("test %d\n", -5151);
    printk("test %x\n", 0xDEADBEEF);
    printk("test %x\n", 0xDEADBEEFFFF);
    printk("test %X\n", 0xDEADBEEFFFF);
    printk("test %p\n", 0x90000000);
    printk("test %p\n", 0x00);
    printk("test %b\n", 0b01010100101);
    printk("test %x\n", 0);
    printk("%s <--- this is a string.\n", "your mom");
    printk("%c <--- this is a character.\n", 'S');
*/


/* 
void printk(char_t* string, ...)
{
	va_list  arglist;
	uint64_t argval;
	uint32_t start    = 0, 
			 end 	  = 0;
	bool_t   dontRes  = BOOLEAN_FALSE;
	char_t   endl     = '\0';
	char_t*  intToStr = &endl;


	va_start(arglist, string);
	for(char_t* c = string; *c != '\0'; ++c)
	{
		dontRes = BOOLEAN_FALSE;

		// find next % or \n
		while(*c != '%' && *c != '\n' && *c != '\0') {
			++c;
			++end;
		}
		// print the string from the start till the end, where there is no % or \n
		localConsoleWriteSubstring(string, start, end);
		start = ++end;


		switch(*c)
		{
			case '\n':
				localConsoleNewline();
				continue;

			case '\0':
				--c; // continue increments var c before going to the top of the loop.
					 // since *++c is undefined behaviour, we decrement it to exit the loop. 				
				continue;
			
			case '%':
				// parse the proper format specifier.
				switch(*++c)
				{
					case 'd':
					argval   = va_arg(arglist, int32_t);
					intToStr = formatd((int32_t)argval, ITSbufSize, intToStrBuffer);
					break;

					case 'x':
					argval   = va_arg(arglist, uint32_t);
					intToStr = formatx(argval,  ITSbufSize, BOOLEAN_FALSE, intToStrBuffer);
					break;

					case 'X':
					argval   = va_arg(arglist, uint64_t);
					intToStr = formatx(argval,  ITSbufSize, BOOLEAN_TRUE, intToStrBuffer);
					break;

					case 'p':
					argval   = va_arg(arglist, uint64_t);
					intToStr = formatp(argval,  ITSbufSize, intToStrBuffer);
					break;

					case 'z':
					argval   = va_arg(arglist, uint64_t);
					intToStr = formatul(argval,  ITSbufSize, intToStrBuffer);
					break;

					case 'u':
					argval   = va_arg(arglist, uint32_t);
					intToStr = formatu((uint32_t)argval, ITSbufSize, intToStrBuffer);
					break;

					case 'b':
					argval   = va_arg(arglist, uint64_t);
					intToStr = formatb(argval,  ITSbufSize, intToStrBuffer);
					break;

					case 's':
					localConsoleWriteString((const char_t*)va_arg(arglist, const char_t*));
					dontRes = BOOLEAN_TRUE;
					break;

					case 'c':
					localConsoleWriteChar(va_arg(arglist, int32_t) & 0xFF);
					dontRes = BOOLEAN_TRUE;
					break;

					case 'l':
					argval   = va_arg(arglist, int64_t);
					intToStr = formatl((int64_t)argval, ITSbufSize, intToStrBuffer);
					break;

					default:
					localConsoleWriteChar('%'); // if this isn't %format, then the % is a regular sign and we should print it 
					dontRes = BOOLEAN_TRUE;		    // we didn't change the buffer, so we don't reset it.
					start = --end; 				    // we dont need to skip format char as it isn't one.
					--c;							// we need to print the char that was tested for format specification
					break;
				}

				localConsoleWriteString(intToStr);
				start = ++end; 						// skip format specifier char
				if(!dontRes) {
					memset(intToStr, 0x00, ITSbufSize - (intToStr - intToStrBuffer));
				}
				break;
		}
	}
	va_end(arglist);
}


*/

