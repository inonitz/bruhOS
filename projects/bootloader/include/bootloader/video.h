#ifndef __BOOTLOADER_C_DEFINITION_GRAPHICS_UTIL__
#define __BOOTLOADER_C_DEFINITION_GRAPHICS_UTIL__
#include <bootloader/api.h>

#define __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__
#include <preboot/framebuffer.h>
#undef __KERNEL_PREBOOT_RESOLVE_TYPE_CONFLICTS_WITH_EFI__


/*
    * Prints the available video modes to the console (screen)
    * Should be used to determine which mode to set, using setVideoMode()
*/
EFI_API void PrintAvailableVideoModes(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop /* graphics output buffer, from gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, ...) */
);


/* 
    * Sets The video mode in which the screen buffer will be.
    * each mode has a certain resolution -> The last mode has *usually* the biggest resolution.
*/
EFI_API void setVideoMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop,
    IN uint16_t                      mode,
    IN const CHAR16*                 optionalErrorMessage
);

/*
    Given a GOP Mode Info Structure, 
    this function returns a struct that contains all data which is relevant to a pixel in the framebuffers' memory. 
*/
pixelMetadata_t parseFramebufferPixelFormat(
    IN EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* outputInfo
);


/* 
    * Initializes the front buffer needed for the kernel to print/draw stuff
    * Finds a mode that has dimensions.<x, y> and sets to that mode.
    * By default if dimensions = { 0, 0 }, then the function will query for 1080p mode.  
    * if it doesn't find the mode, the framebuffer is set to the default mode (0, [res = 640x480])
*/
EFI_API void initialize_screenbuffer(
    IN  uint16_t        width,
    IN  uint16_t        height,
    OUT framebuffer_t*  init
);


/*
    Initializes the framebuffer to one of the following resolutions, given that they exist:
        640x480
        854x480
        1280x720
        1920x1080
        2560x1440
    Otherwise, the biggest resolution (that was found) will be set.
    In the case that either resolution is found from the list (& is also RGB),
    the framebuffer will be set to the minimum resolution.
*/
EFI_API void initialize_screenbuffer2(
    OUT framebuffer_t* init
);


#endif /* __BOOTLOADER_C_DEFINITION_GRAPHICS_UTIL__ */