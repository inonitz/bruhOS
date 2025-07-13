#include <std/error.h>
#include <std/macro.h>
#include <std/halt.h>
#include <gpcs2/colours.h>
#include <gpcs2/console/user_ifce.h>





void kernelErrorReport(
	const char_t*  filename, 
	const char_t*  funcname, 
	const uint64_t line, 
		  uint8_t  errorcode, 
		  bool_t   code_section_critical,
	const char_t*  ErrorMessage
) {
	static const char_t* ErrStrings[20] = {
		"SUCCESS",
		"FAILURE",
		"FAILURE_SEVERE",
		"FAILURE_CRITICAL",
		"MISSING_DATA",
		"BAD_DATA",
		"INVALID_INPUT",
		"",
		"FILE_NOT_FOUND",
		"BAD_INPUT",
		"BUFFER_TOO_SMALL",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"NOT_IMPLEMENTED",
		"INVALID_ERROR"
	};
	
	consoleSetBackgroundColor(__KERNEL_CONSOLE_RED);
	consoleClearScreen();


	if(boolean(code_section_critical)) {
		printk("CRITICAL CODE SECTION FAILED IN FILE %s AT FUNCTION %s : LINE %u (ERROR CODE %u, %s)\nERROR MESSAGE: %s\nHALTING EXECUTION.\n", 
			filename, 
			funcname,
			line, 
			errorcode,
			ErrStrings[errorcode],
			ErrorMessage
		);
		abshalt();
	}
	
	printk("Kernel FAILED in FILE %s AT FUNCTION %s LINE %u (Error Code %u, %s). Error Message: %s.\n", 
		filename,
		funcname, 
		line, 
		errorcode, 
		ErrStrings[errorcode],
		ErrorMessage
	);
	return;
}


void system_status(bool_t status, const char_t* message)
{
    uint64_t statusColor = (status * __KERNEL_CONSOLE_BRIGHT_RED) + (!boolean(status) * __KERNEL_CONSOLE_NEON_GREEN);
    static const char_t* msg[2] = {
        "GOOD",
        "FAIL"
    };
    statusColor <<= 32;
	puts("["); printkcol(statusColor, msg[boolean(status)]); puts("] ");
	if(unlikely(message != NULLPTR)) {
		printk(message);
	}
	return;
}
