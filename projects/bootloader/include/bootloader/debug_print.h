#ifndef __BOOTLOADER_C_DEFINITION_PRINTF_DEBUG_MACRO__
#define __BOOTLOADER_C_DEFINITION_PRINTF_DEBUG_MACRO__
#include <bootloader/print.h>


#if !defined(debug_printb)
#   if defined(_DEBUG)
#       define debug_printb(format, ...) { \
            printf((const CHAR16*)L"[debug] "); \
            printf((const CHAR16*)(format), ##__VA_ARGS__); \
        }
#   else
#       define debug_printb(format, ...) {}
#   endif
#else
#   error "debug_printb already defined somewhere else!"   
#endif


#endif /* __BOOTLOADER_C_DEFINITION_PRINTF_DEBUG_MACRO__ */