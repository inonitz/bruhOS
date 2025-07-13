#include <bootloader/err.h>
#include <bootloader/print.h>


__attribute__((noreturn)) void errexit(
    const char*   file_macro,
    unsigned long line_macro,
	EFI_STATUS    pstatus, 
	const CHAR16* format, 
	...
) {
	const CHAR16* errinf = (const CHAR16*)L"%a:%u [ADDITIONAL_INFO] ";
    const CHAR16* errstr = (const CHAR16*)L" | errexit() -> %s (Error Code %x)\n\r";
	va_list arglist;

    printf(errinf, file_macro, line_macro);

	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);

	printf(errstr, getErrorStr(pstatus), pstatus);
	while(TRUE) {}
}


void errnoexit(
    const char*   file_macro,
    unsigned long line_macro,
	EFI_STATUS    pstatus, 
	const CHAR16* format, 
	...
) {
	const CHAR16* errinf = (const CHAR16*)L"%a:%u [ADDITIONAL_INFO] ";
    const CHAR16* errstr = (const CHAR16*)L" | errnoexit() -> %s (Error Code %x)\n\r";
	va_list arglist;

    printf(errinf, file_macro, line_macro);

	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);

	printf(errstr, getErrorStr(pstatus), pstatus);
}


const CHAR16* getErrorStr(EFI_STATUS status)
{
	static const CHAR16* ErrStrings[32] = {
		(const CHAR16*)L"EFI_SUCCESS",
		(const CHAR16*)L"EFI_LOAD_ERROR",
		(const CHAR16*)L"EFI_INVALID_PARAMETER",
		(const CHAR16*)L"EFI_UNSUPPORTED",
		(const CHAR16*)L"EFI_BAD_BUFFER_SIZE",
		(const CHAR16*)L"EFI_BUFFER_TOO_SMALL",
		(const CHAR16*)L"EFI_NOT_READY",
		(const CHAR16*)L"EFI_DEVICE_ERROR",
		(const CHAR16*)L"EFI_WRITE_PROTECTED",
		(const CHAR16*)L"EFI_OUT_OF_RESOURCES",
		(const CHAR16*)L"EFI_VOLUME_CORRUPTED",
		(const CHAR16*)L"EFI_VOLUME_FULL",
		(const CHAR16*)L"EFI_NO_MEDIA",
		(const CHAR16*)L"EFI_MEDIA_CHANGED",
		(const CHAR16*)L"EFI_NOT_FOUND",
		(const CHAR16*)L"EFI_ACCESS_DENIED",
		(const CHAR16*)L"EFI_NO_RESPONSE",
		(const CHAR16*)L"EFI_NO_MAPPING",
		(const CHAR16*)L"EFI_TIMEOUT",
		(const CHAR16*)L"EFI_NOT_STARTED",
		(const CHAR16*)L"EFI_ALREADY_STARTED",
		(const CHAR16*)L"EFI_ABORTED",
		(const CHAR16*)L"EFI_ICMP_ERROR",
		(const CHAR16*)L"EFI_TFTP_ERROR",
		(const CHAR16*)L"EFI_PROTOCOL_ERROR",
		(const CHAR16*)L"EFI_INCOMPATIBLE_VERSION",
		(const CHAR16*)L"EFI_SECURITY_VIOLATION",
		(const CHAR16*)L"EFI_CRC_ERROR",
		(const CHAR16*)L"EFI_END_OF_MEDIA",
		(const CHAR16*)L"EFI_END_OF_FILE",
		(const CHAR16*)L"EFI_INVALID_LANGUAGE",
		(const CHAR16*)L"EFI_COMPROMISED_DATA"
	};
	return ErrStrings[status];
}