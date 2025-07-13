#ifndef __BOOTLOADER_C_DEFINITION_GET_ERROR__
#define __BOOTLOADER_C_DEFINITION_GET_ERROR__
#include <bootloader/api.h>


__attribute__((noreturn)) EFI_API void errexit(
    const char*   file_macro,
    unsigned long line_macro,
	EFI_STATUS    pstatus, 
	const CHAR16* format, 
	...
);


EFI_API void errnoexit(
    const char*   file_macro,
    unsigned long line_macro,
	EFI_STATUS    pstatus, 
	const CHAR16* format, 
	...
);


EFI_API const CHAR16* getErrorStr(EFI_STATUS status);

#ifdef CHECKERR_NOEXIT
#   error "Macro CHECKERR_NOEXIT Should not be defined!"
#endif
#ifdef CHECKERRFMT_NOEXIT
#   error "Macro CHECKERRFMT_NOEXIT Should not be defined!"
#endif
#ifdef CHECKERR
#   error "Macro CHECKERR Should not be defined!"
#endif
#ifdef CHECKERRFMT
#   error "Macro CHECKERR Should not be defined!"
#endif
#ifdef CHECKERR_DO
#   error "Macro CHECKERR_DO Should not be defined!"
#endif

#ifdef _DEBUG
#   define ondebug(__do) { __do; }
#else
#   define ondebug(__do)
#endif


#define CHECKERR_NOEXIT(prog_status) \
	if (EFI_ERROR((prog_status))) \
		errnoexit(__FILE__, __LINE__, (prog_status), (const CHAR16*)L""); \

#define CHECKERRFMT_NOEXIT(prog_status, format, ...) \
	if (EFI_ERROR((prog_status))) \
		errnoexit(__FILE__, __LINE__, (prog_status), (const CHAR16*)format, ##__VA_ARGS__); \


#define CHECKERR(prog_status) \
	if (EFI_ERROR((prog_status))) errexit(__FILE__, __LINE__, (prog_status), (const CHAR16*)L""); \


#define CHECKERRFMT(prog_status, format, ...) \
	if (EFI_ERROR((prog_status))) \
	{ \
		errexit(__FILE__, __LINE__, (prog_status), (const CHAR16*)format, ##__VA_ARGS__); \
	} \


#define CHECKERR_DO(prog_status, ...) \
	if (EFI_ERROR((prog_status))) \
	{              				   	   \
		__VA_ARGS__;			   	   \
		errexit(__FILE__, __LINE__, (prog_status), L""); \
	}


#endif /* __BOOTLOADER_C_DEFINITION_GET_ERROR__ */