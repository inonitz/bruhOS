#pragma once
#include <efi.h>
#include <efilib.h>
#include "../../shared/def.h"



// #define _DEBUG 1
#ifdef _DEBUG
#define ondebug(__do) { __do; }
#else
#define ondebug(__do)
#endif

#define CHECKERR(prog_status, ...) \
	if (prog_status) 			   \
	{              				   \
		__VA_ARGS__;			   \
		ExitMainFunc(prog_status); \
	}							   \
	
#define BOOL(val) (!!val)


const CHAR16* ErrStrings[33] = {
	L"EFI_SUCCESS",
	L"EFI_LOAD_ERROR",
	L"EFI_INVALID_PARAMETER",
	L"EFI_UNSUPPORTED",
	L"EFI_BAD_BUFFER_SIZE",
	L"EFI_BUFFER_TOO_SMALL",
	L"EFI_NOT_READY",
	L"EFI_DEVICE_ERROR",
	L"EFI_WRITE_PROTECTED",
	L"EFI_OUT_OF_RESOURCES",
	L"EFI_VOLUME_CORRUPTED",
	L"EFI_VOLUME_FULL",
	L"EFI_NO_MEDIA",
	L"EFI_MEDIA_CHANGED",
	L"EFI_NOT_FOUND",
	L"EFI_ACCESS_DENIED",
	L"EFI_NO_RESPONSE",
	L"EFI_NO_MAPPING",
	L"EFI_TIMEOUT",
	L"EFI_NOT_STARTED",
	L"EFI_ALREADY_STARTED",
	L"EFI_ABORTED",
	L"EFI_ICMP_ERROR",
	L"EFI_TFTP_ERROR",
	L"EFI_PROTOCOL_ERROR",
	L"EFI_INCOMPATIBLE_VERSION",
	L"EFI_SECURITY_VIOLATION",
	L"EFI_CRC_ERROR",
	L"EFI_END_OF_MEDIA",
	L"EFI_END_OF_FILE",
	L"EFI_INVALID_LANGUAGE",
	L"EFI_COMPROMISED_DATA"
};

inline const CHAR16* getErrStr(EFI_STATUS s)
{
	return ErrStrings[s];	
}

__attribute__((noreturn, always_inline)) inline void ExitMainFunc(EFI_STATUS pstatus)
{
	Print(L"\nThe Bootloader didn't work properly... %s (Error Code %u)\n", getErrStr(pstatus), pstatus);
	while(1) {}
}


uint64_t memcmp(unsigned char* first, unsigned char* second, uint64_t amount)
{
	uint64_t i = 0;
	for(; i < amount && (first[i] == second[i]); ++i);
	return amount == i;
}


inline void __attribute__((noreturn, always_inline)) pauseloop()
{
	while(1) { __asm__ volatile("pause"); }
}

inline void __attribute__((noreturn, always_inline)) halt()
{
	__asm__ volatile("hlt");
	pauseloop();
}