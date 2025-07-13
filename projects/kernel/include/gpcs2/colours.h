#ifndef __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_MACRO_COLOURS_DEFINITION__
#define __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_MACRO_COLOURS_DEFINITION__

/*
    Force a single implementation of all kernel console colours
    If someone defines these macros there will be issues
*/
#ifdef __KERNEL_CONSOLE_RGB
#	error "Macro __KERNEL_CONSOLE_BLACK Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_BGR
#	error "Macro __KERNEL_CONSOLE_WHITE Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_BLACK
#	error "Macro __KERNEL_CONSOLE_BLACK Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_WHITE
#	error "Macro __KERNEL_CONSOLE_WHITE Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_RED
#	error "Macro __KERNEL_CONSOLE_RED Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_BRIGHT_RED
#	error "Macro __KERNEL_CONSOLE_BRIGHT_RED Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_ORANGE
#	error "Macro __KERNEL_CONSOLE_ORANGE Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_LIME
#	error "Macro __KERNEL_CONSOLE_LIME Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_BLUE
#	error "Macro __KERNEL_CONSOLE_BLUE Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_YELLOW
#	error "Macro __KERNEL_CONSOLE_YELLOW Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_CYAN
#	error "Macro __KERNEL_CONSOLE_CYAN Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_MAGENTA
#	error "Macro __KERNEL_CONSOLE_MAGENTA Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_SILVER
#	error "Macro __KERNEL_CONSOLE_SILVER Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_GRAY
#	error "Macro __KERNEL_CONSOLE_GRAY Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_MAROON
#	error "Macro __KERNEL_CONSOLE_MAROON Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_OLIVE
#	error "Macro __KERNEL_CONSOLE_OLIVE Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_GREEN
#	error "Macro __KERNEL_CONSOLE_GREEN Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_NEON_GREEN
#	error "Macro __KERNEL_CONSOLE_NEON_GREEN Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_PURPLE
#	error "Macro __KERNEL_CONSOLE_PURPLE Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_TEAL
#	error "Macro __KERNEL_CONSOLE_TEAL Already Defined!"
#endif
#ifdef __KERNEL_CONSOLE_NAVY
#	error "Macro __KERNEL_CONSOLE_NAVY Already Defined!"
#endif


#define __KERNEL_CONSOLE_RGB(r, g, b) (uint32_t)(r << 24 + g << 16 + b << 8)
#define __KERNEL_CONSOLE_BGR(r, g, b) (uint32_t)(b << 24 + g << 16 + r << 8) 
//      Color      	     RES R  G  B   (little-endian BGR format)
#define __KERNEL_CONSOLE_BLACK      0x00000000	
#define __KERNEL_CONSOLE_WHITE	    0x00FFFFFF
#define __KERNEL_CONSOLE_RED		0x00FF0000
#define __KERNEL_CONSOLE_BRIGHT_RED 0x00ff1439
#define __KERNEL_CONSOLE_ORANGE     0x00FF6600
#define __KERNEL_CONSOLE_LIME	   	0x0000FF00
#define __KERNEL_CONSOLE_BLUE	   	0x000000FF
#define __KERNEL_CONSOLE_YELLOW	    0x00FFFF00
#define __KERNEL_CONSOLE_CYAN 	    0x0000FFFF
#define __KERNEL_CONSOLE_MAGENTA    0x00FF00FF
#define __KERNEL_CONSOLE_SILVER	    0x00C0C0C0
#define __KERNEL_CONSOLE_GRAY	   	0x00808080
#define __KERNEL_CONSOLE_MAROON	    0x00800000
#define __KERNEL_CONSOLE_OLIVE	    0x00808000
#define __KERNEL_CONSOLE_GREEN	    0x00008000
#define __KERNEL_CONSOLE_NEON_GREEN 0x0039ff14
#define __KERNEL_CONSOLE_PURPLE	    0x00800080
#define __KERNEL_CONSOLE_TEAL	    0x00008080
#define __KERNEL_CONSOLE_NAVY	    0x00000080


#endif /* __KERNEL_C_DEFINITION_CONSOLE_GRAPHICS_MACRO_COLOURS_DEFINITION__ */