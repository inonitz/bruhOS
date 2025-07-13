#ifndef __KERNEL_C_DEFINITION_STD_NONSTANDRAD_ERROR_MACRO_HEADER__
#define __KERNEL_C_DEFINITION_STD_NONSTANDRAD_ERROR_MACRO_HEADER__



#ifdef KERNEL_SUCCESS
#   pragma WARN("Macro 'KERNEL_SUCCESS' already defined somewhere else!")
#endif
#ifdef KERNEL_FAILURE
#   pragma WARN("Macro 'KERNEL_FAILURE' already defined somewhere else!")
#endif
#ifdef KERNEL_FAILURE_SEVERE
#   pragma WARN("Macro 'KERNEL_FAILURE_SEVERE' already defined somewhere else!")
#endif
#ifdef KERNEL_FAILURE_CRITICAL
#   pragma WARN("Macro 'KERNEL_FAILURE_CRITICAL' already defined somewhere else!")
#endif
#ifdef KERNEL_MISSING_DATA
#   pragma WARN("Macro 'KERNEL_MISSING_DATA' already defined somewhere else!")
#endif
#ifdef KERNEL_BAD_DATA
#   pragma WARN("Macro 'KERNEL_BAD_DATA' already defined somewhere else!")
#endif
#ifdef KERNEL_INVALID_INPUT
#   pragma WARN("Macro 'KERNEL_INVALID_INPUT' already defined somewhere else!")
#endif
#ifdef KERNEL_FILE_NOT_FOUND
#   pragma WARN("Macro 'KERNEL_FILE_NOT_FOUND' already defined somewhere else!")
#endif
#ifdef KERNEL_BAD_INPUT
#   pragma WARN("Macro 'KERNEL_BAD_INPUT' already defined somewhere else!")
#endif
#ifdef KERNEL_BUFFER_TOO_SMALL
#   pragma WARN("Macro 'KERNEL_BUFFER_TOO_SMALL' already defined somewhere else!")
#endif
#ifdef KERNEL_NOT_IMPLEMENTED
#   pragma WARN("Macro 'KERNEL_NOT_IMPLEMENTED' already defined somewhere else!")
#endif
#ifdef KERNEL_INVALID_ERROR
#   pragma WARN("Macro 'KERNEL_INVALID_ERROR' already defined somewhere else!")
#endif


// useful error types
#define KERNEL_SUCCESS			0u
#define KERNEL_FAILURE			1u
#define KERNEL_FAILURE_SEVERE   2u
#define KERNEL_FAILURE_CRITICAL 3u

#define KERNEL_MISSING_DATA     4u
#define KERNEL_BAD_DATA         5u
#define KERNEL_INVALID_INPUT    6u
#define KERNEL_FILE_NOT_FOUND   8u
#define KERNEL_BAD_INPUT        9u
#define KERNEL_BUFFER_TOO_SMALL 10u

#define KERNEL_NOT_IMPLEMENTED  18u
#define KERNEL_INVALID_ERROR    19u


#endif /* __KERNEL_C_DEFINITION_STD_NONSTANDRAD_ERROR_MACRO_HEADER__ */