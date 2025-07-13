#ifndef __KERNEL_C_DEFINITION_STD_NONSTANDARD_ERROR_HEADER__
#define __KERNEL_C_DEFINITION_STD_NONSTANDARD_ERROR_HEADER__
#include <std/io.h>
#include <std/enable_bool.h>
#include <std/bool.h>


#ifdef printLocation
#	error "Macro printLocation Already Defined!"
#endif
#ifdef debug_print
#	error "Macro debug_print Already Defined!"
#endif
#ifdef KERNEL_ERROR
#	error "Macro KERNEL_ERROR Already Defined!"
#endif
#ifdef KERNEL_ERROR_IF
#	error "Macro KERNEL_ERROR_IF Already Defined!"
#endif


#ifdef _DEBUG
	#define DEBUG(x) { x }
#else
	#define DEBUG(x) do {} while(0);
#endif


#define printLocation() printk("file: %s\nline: %s", __FILE__, __LINE__)
#define debug_printk(str, ...) DEBUG(printk(str, __VA_ARGS__);)

#define KERNEL_ERROR(err_type, is_critical_section, msg) \
	kernelErrorReport(__FILE__, __func__, __LINE__, err_type, is_critical_section, msg); \


#define KERNEL_ERROR_IF(condition, err_type, is_critical_section, msg) \
	if(boolean(condition)) { \
		kernelErrorReport(__FILE__, __func__, __LINE__, err_type, is_critical_section, msg); \
	} \


void kernelErrorReport(
	const char_t*  filename, 
	const char_t*  funcname, 
	const uint64_t line, 
		  uint8_t  errorcode, 
		  bool_t   code_section_critical,
	const char_t*  ErrorMessage
);


void system_status(bool_t status, const char_t* msg);


#endif /* __KERNEL_C_DEFINITION_STD_NONSTANDARD_ERROR_HEADER__ */
