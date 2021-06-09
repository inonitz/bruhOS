#pragma once
#ifndef __DEBUG__
#define __DEBUG__
#include "io.h"



// #define _DEBUG
#ifdef _DEBUG
	#define DEBUG(x) { x }
#else
	#define DEBUG(x) do {} while(0);
#endif


#define printLocation() printk("file: %s\nline: %s", __FILE__, __LINE__)


#define KERNEL_ERROR(err_type, is_critical_section, msg) \
	kernelErrorReport(__FILE__, __func__, __LINE__, err_type, is_critical_section, msg); \


#define KERNEL_ERROR_IF(condition, err_type, is_critical_section, msg) \
	if(boolean(condition)) { \
		kernelErrorReport(__FILE__, __func__, __LINE__, err_type, is_critical_section, msg); \
	} \


extern void noret pauseloop();
extern void noret idleloop();
extern void noret busyloop();
extern void noret abshalt();
	   void noret hang();

	   
void kernelErrorReport(
	const char_t*  filename, 
	const char_t*  funcname, 
	const uint64_t line, 
		  uint8_t  err, 
		  bool_t   code_section_critical,
	const char_t*  ErrorMessage
);


void system_status(bool_t status, char_t* msg);

#endif