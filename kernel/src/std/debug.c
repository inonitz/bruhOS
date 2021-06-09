#include "debug.h"
#include "../gpcs2/console/user_ifce.h"



noret void hang() {
	idleloop();
}


void kernelErrorReport(
	const char_t*  filename, 
	const char_t*  funcname, 
	const uint64_t line, 
		  uint8_t  err, 
		  bool_t   code_section_critical,
	const char_t*  ErrorMessage
) {
		const char_t* ErrStrings[20] = {
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
	
	consoleSetBackgroundColor(RED);
	consoleClearScreen();


	if(code_section_critical) {
		printk("CRITICAL CODE SECTION FAILED IN FILE %s AT FUNCTION %s : LINE %u (ERROR CODE %u, %s)\nERROR MESSAGE: %s\nHALTING EXECUTION.\n", 
			filename, 
			funcname,
			line, 
			err,
			ErrStrings[err],
			ErrorMessage
		);
		abshalt();
	}
	
	printk("Kernel FAILED in FILE %s AT FUNCTION %s LINE %u (Error Code %u, %s). Error Message: %s.\n", 
		filename,
		funcname, 
		line, 
		err, 
		ErrStrings[err],
		ErrorMessage
	);
	return;
}


void system_status(bool_t status, char_t* message)
{
    uint64_t statusColor = status * BRIGHT_RED +  !status * NEON_GREEN;
    char_t* msg[2] = {
        "GOOD",
        "FAIL"
    };
    statusColor <<= 32;
	puts("["); printkcol(statusColor, msg[boolean(status)]); puts("] ");
	if(message != nullptr) {
		printk(message);
	}
	return;
}