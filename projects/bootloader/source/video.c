#include <bootloader/video.h>
#include <bootloader/debug_print.h>
#include <bootloader/err.h>
#include <bootloader/pause.h>

#include <bootloader/lib.h>


void printfAvailableVideoModes(IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop)
{
    static const CHAR16* pixel_formats[5] = {
        (const CHAR16*)L"RedGreenBlueReserved8BitPerColor",
        (const CHAR16*)L"BlueGreenRedReserved8BitPerColor",
        (const CHAR16*)L"BitMask",
        (const CHAR16*)L"BltOnly",
        (const CHAR16*)L"FormatMax"
    };
	EFI_STATUS							  status = EFI_SUCCESS;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode;
    uint64_t                              infSize;
    uint32_t                              modecount;


	status = gop->QueryMode(gop, gop->Mode->Mode, &infSize, &gop_mode);
	CHECKERR(status)
	modecount = gop->Mode->MaxMode;
    printf((const CHAR16*)L"printfing %u Available Video Modes... Current Mode is: %u\r\n", modecount, gop->Mode->Mode);
    

	for (uint32_t i = 0; i < modecount; ++i) {
		status = gop->QueryMode(gop, i, &infSize, &gop_mode);
		CHECKERR(status)
        
		printf((const CHAR16*)L"    Mode %d - <%u, %u> pxfmt: %s\r\n",
			i,
			gop_mode->HorizontalResolution,
			gop_mode->VerticalResolution,
            pixel_formats[gop_mode->PixelFormat]
		);
	}


    return;
}


void setVideoMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop,
    IN uint16_t                      mode
) {
    gop->SetMode(gop, (uint32_t)mode);
}


void initialize_screenbuffer(
    IN  uint16_t        width,
    IN  uint16_t        height,
    OUT framebuffer_t*  init
) {
    EFI_STATUS                            status     = EFI_SUCCESS;
    EFI_GRAPHICS_OUTPUT_PROTOCOL*         gop        = (EFI_GRAPHICS_OUTPUT_PROTOCOL*)NULL;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode   = (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION*)NULL;
    uint64_t                              infSize    = 0;
    uint32_t                              modecount  = 0;
    uint32_t                              i          = 0;
    uint8_t                               found_mode = FALSE;


    printf((const CHAR16*)L"\nInitializing Graphics Framebuffer... \r\n");

    // locate the gop protocol
    status = uefi_call_wrapper(gBS->LocateProtocol, 3, 
        &gEfiGraphicsOutputProtocolGuid, 
        NULL, 
        (void**)&gop
    );
    CHECKERRFMT(status, L"    Couldn't get an active screen protocol (GRAPHICS_OUTPUT_PROTOCOL)\r\n");
    // ondebug(printfAvailableVideoModes(gop))


    // get the current mode GOP is set to
	status = gop->QueryMode(gop, gop->Mode->Mode, &infSize, &gop_mode);
    CHECKERRFMT(status, L"   couldn't query current video mode in order to initialize screen framebuffer\n\r");
    

    width  = (width  == 0) ? 1920u : width;
    height = (height == 0) ? 1080u : height;
    if(gop_mode->HorizontalResolution == width && gop_mode->VerticalResolution == height) {
        debug_printb(L"    Current GRAPHICS_OUTPUT_PROTOCOL configuration is already set to %ux%u.\r\n", width, height);
        found_mode = TRUE;
        i          = gop->Mode->Mode;
    }   
    else {
        debug_printb(L"    Framebuffer isn't set to %ux%u, searching for possible configuration...\r\n", width, height);
        pause(125000);
        modecount = gop->Mode->MaxMode;
        for (; i < modecount && !found_mode; ++i) 
        {
            status = gop->QueryMode(gop, i, &infSize, &gop_mode);
            CHECKERR(status)
            found_mode = 
                (gop_mode->HorizontalResolution == width) 
                && 
                (gop_mode->VerticalResolution == height);
        }
        printf((const CHAR16*)L"    Framebuffer initialized Successfully to %ux%u resolution.\r\n", init->dims.x, init->dims.y);
    }
    
    
    ondebug({
        status = gop->SetMode(gop, 0); // reset the video mode.
        CHECKERRFMT(status, L"   DEBUG: Couldn't reset video mode\r\n");
    })


    // if we found the video-mode, set to that. else, set to mode 0 (default).
    status = gop->SetMode(gop, found_mode * i);
    CHECKERRFMT(status, L"   Couldn't change to appropriate video mode %u\r\n", found_mode * i);


    init->start  = (void*)((uintptr_t)gop->Mode->FrameBufferBase);
    init->dims.x = (uint16_t)gop->Mode->Info->HorizontalResolution;
    init->dims.y = (uint16_t)gop->Mode->Info->VerticalResolution;
    debug_printb(L"    Framebuffer Physical Address: %p\r\n", init->start);
    return;
}