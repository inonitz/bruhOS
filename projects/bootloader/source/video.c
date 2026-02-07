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
    printf((const CHAR16*)L"Printing %u Available Video Modes... Current Mode is: %u\r\n", modecount, gop->Mode->Mode);
    

    printf((const CHAR16*)L"Info: Mode, Resolution (Horizontal, PixelsPerScanLine, Vertical), PixelFormat, (RED_MASK, BLUE_MASK, GREEN_MASK, ALPHA_MASK)(If Fmt=PixelBitMask) \r\n");
	for (uint32_t i = 0; i < modecount; ++i) {
		status = gop->QueryMode(gop, i, &infSize, &gop_mode);
		CHECKERR(status)
        
		printf((const CHAR16*)L"    %d - (%u, %u, %u)      (%u) %s   (%x, %x, %x, %x)\r\n",
			i,
			gop_mode->HorizontalResolution,
            gop_mode->PixelsPerScanLine,
			gop_mode->VerticalResolution,
            gop_mode->PixelFormat, pixel_formats[gop_mode->PixelFormat],
            gop_mode->PixelInformation.RedMask,
            gop_mode->PixelInformation.GreenMask,
            gop_mode->PixelInformation.BlueMask,
            gop_mode->PixelInformation.ReservedMask
		);
	}


    return;
}


void setVideoMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL* gop,
    IN uint16_t                      mode,
    IN const CHAR16*                 optionalErrorMessage
) {
    EFI_STATUS status = gop->SetMode(gop, (uint32_t)mode);
    CHECKERRFMT(status, optionalErrorMessage);
    return;
}


// UINT16 FindHighestSetBit(UINTN Value) {
//     UINTN HighestBit = 0;
//     while (Value >>= 1) {
//         ++HighestBit;
//     }
//     return HighestBit;
// }

// UINTN GetPixelBitmaskElementSize(
//     EFI_PIXEL_BITMASK* PixelBitMask
// ) {
//     /* Each Bit N That is set in every mask[R, G, B, A],
//         represents each channels' Bits in the 32bit Integer.
//         More Specifically, if RedMask=0x4F, Then only the following bits Should be counted, For the Red Channel:
//             bits 0->4:  On
//             bits 5->6:  Ignored
//             bits 7->7:  On
//             bits 8->32: Ignored
//     */
//     UINTN   highestBitSet = 0;
//     UINT32  tmpBitSet     = 0;
//     UINT32* pixelMask    = &PixelBitMask->RedMask;
//     for(uint8_t i = 0; i < 4; ++i) {
//         tmpBitSet = FindHighestSetBit(pixelMask[i]);
//         highestBitSet = highestBitSet > tmpBitSet ? highestBitSet : tmpBitSet;
//     }
//     return highestBitSet;
// }


static pixelChannelFormat_t parseBitmaskMetadata(uint32_t mask) 
{
    pixelChannelFormat_t out = (pixelChannelFormat_t){0, 0};
    uint32_t oldMask = mask;


    if (mask == 0) {
        return out;
    }


    // Find the position of the first set bit (LSB)
    while (!(oldMask & 1)) {
        oldMask >>= 1;
        ++out.bitShift;
    }

    // Count the number of set bits
    oldMask = mask;
    while (oldMask & 1) {
        oldMask >>= 1;
        ++out.bitWidth;
    }


    return out;
}


/* 
    Partially taken from: 
    https://uefi.org/specs/UEFI/2.9_A/12_Protocols_Console_Support.html#efi-graphics-output-protocol
*/
pixelMetadata_t parseFramebufferPixelFormat(
    IN EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* outputInfo
) {
    UINT32 PixelElementSizeBytes = 0;
    pixelMetadata_t fillOut = (pixelMetadata_t){
        .m_sizeBytes      = 0, 
        .m_enumFormat     = (uint8_t)outputInfo->PixelFormat,
        .m_channelFormats = { { 0, 0 }, { 0, 0 }, { 0, 0 } }
    };


    switch (outputInfo->PixelFormat) {
        case PixelBlueGreenRedReserved8BitPerColor:
        fillOut.m_sizeBytes = sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
        fillOut.m_channelFormats[0] = (pixelChannelFormat_t){ .bitWidth = 8, .bitShift = 0  };
        fillOut.m_channelFormats[1] = (pixelChannelFormat_t){ .bitWidth = 8, .bitShift = 8  };
        fillOut.m_channelFormats[2] = (pixelChannelFormat_t){ .bitWidth = 8, .bitShift = 16 };
        break;

        case PixelRedGreenBlueReserved8BitPerColor:
        fillOut.m_sizeBytes = sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
        fillOut.m_channelFormats[0] = (pixelChannelFormat_t){ .bitWidth = 8, .bitShift = 16 };
        fillOut.m_channelFormats[1] = (pixelChannelFormat_t){ .bitWidth = 8, .bitShift = 8  };
        fillOut.m_channelFormats[2] = (pixelChannelFormat_t){ .bitWidth = 8, .bitShift = 0  };
        break;

        case PixelBitMask:
        PixelElementSizeBytes = 0b00
                    | outputInfo->PixelInformation.RedMask
                    | outputInfo->PixelInformation.GreenMask
                    | outputInfo->PixelInformation.BlueMask
                    | outputInfo->PixelInformation.ReservedMask;
        PixelElementSizeBytes = (PixelElementSizeBytes < 0xFFFF  ) ? 2 : PixelElementSizeBytes;
        PixelElementSizeBytes = (PixelElementSizeBytes < 0xFFFFFF) ? 3 : PixelElementSizeBytes;
        PixelElementSizeBytes = 4; /* The only option left. We still need to record the EFI_PIXEL_BITMASK (down below) */
    
        fillOut.m_sizeBytes         = PixelElementSizeBytes;
        fillOut.m_channelFormats[0] = parseBitmaskMetadata(outputInfo->PixelInformation.RedMask);
        fillOut.m_channelFormats[1] = parseBitmaskMetadata(outputInfo->PixelInformation.GreenMask);
        fillOut.m_channelFormats[2] = parseBitmaskMetadata(outputInfo->PixelInformation.BlueMask);
        break;

        case PixelBltOnly:
        case PixelFormatMax:
        default:
        break;
    }


    return fillOut;
}


void initialize_screenbuffer2(
    OUT framebuffer_t*  init
) {
    typedef struct __generic_gop_resolution {
        uint32_t width, height;
    } monitorRes;


    EFI_STATUS                            status         = EFI_SUCCESS;
    EFI_GRAPHICS_OUTPUT_PROTOCOL*         gop            = (EFI_GRAPHICS_OUTPUT_PROTOCOL*)NULL;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* gop_mode       = (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION*)NULL;
    uint64_t                              infSize        = 0;
    uint32_t                              modecount      = 0;
    monitorRes defaultModes[5] = {
        { 640u,  480u },
        { 854u,  480u },
        { 1280u, 720u  },
        { 1920u, 1080u },
        { 2560u, 1440u }
    };
    monitorRes maxResolution = {0, 0};
    uint8_t foundModes[5] = {
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF
    };
    uint8_t maxResolutionIndex = 0xFF;


    printf((const CHAR16*)L"\nInitializing Graphics Framebuffer & Data... \r\n");


    // locate the gop protocol
    status = uefi_call_wrapper(gBS->LocateProtocol, 3, 
        &gEfiGraphicsOutputProtocolGuid, 
        NULL, 
        (void**)&gop
    );
    CHECKERRFMT(status, L"    Couldn't get an active screen protocol (GRAPHICS_OUTPUT_PROTOCOL)\r\n");
    ondebug(printfAvailableVideoModes(gop))


    // get the current mode GOP is set to
	status = gop->QueryMode(gop, gop->Mode->Mode, &infSize, &gop_mode);
    CHECKERRFMT(status, L"   couldn't query current video mode in order to initialize screen framebuffer\n\r");


	modecount = gop->Mode->MaxMode;	
    for (uint32_t i = 0; i < modecount; ++i) 
    {
		status = gop->QueryMode(gop, i, &infSize, &gop_mode);
		CHECKERR(status)

        if(gop_mode->PixelFormat == PixelBltOnly || gop_mode->PixelFormat == PixelFormatMax) {
            continue;
        }


        uint8_t foundModeLocal = 0xFF;
        for(uint8_t mode = 0; mode < __carraysize(foundModes); ++mode) {
            foundModeLocal = 
                (defaultModes[mode].width == gop_mode->HorizontalResolution) &&
                (defaultModes[mode].height == gop_mode->VerticalResolution);            

            foundModes[mode] = foundModeLocal ? i : foundModes[mode];
        }
        foundModeLocal = (maxResolution.width * maxResolution.height) < (gop_mode->HorizontalResolution * gop_mode->VerticalResolution);
        maxResolution.width  = foundModeLocal ? gop_mode->HorizontalResolution : maxResolution.width;
        maxResolution.height = foundModeLocal ? gop_mode->VerticalResolution : maxResolution.width;
        maxResolutionIndex = foundModeLocal ? i : maxResolutionIndex;
	}

    ondebug(
        for(uint8_t i = 0; i < __carraysize(foundModes); ++i) {
            if(foundModes[i] == 0xFF) {
                continue;
            }
            debug_printb((const CHAR16*)L"Found Mode #%u With Resolution (%u, %u)\r\n", foundModes[i], defaultModes[i].width, defaultModes[i].height);
        }
    )
    ondebug(setVideoMode(gop, 0, L"   DEBUG: Couldn't reset video mode\r\n"));


    /* Set the first video mode that was found using defaultModes */
    uint8_t setVideoModeFlag = BOOLEAN_FALSE;
    for(uint8_t i = 0; i < __carraysize(foundModes) && !setVideoModeFlag; ++i) {
        if(foundModes[i] != 0xFF) {
            status = gop->SetMode(gop, foundModes[i]);
            CHECKERRFMT(status, L"   Couldn't change to appropriate video mode %u\r\n", foundModes[i]);
            setVideoModeFlag = BOOLEAN_TRUE;
        }
    }
    if(!setVideoModeFlag) {
        if(maxResolutionIndex != 0xFF) {
            status = gop->SetMode(gop, maxResolutionIndex);
            CHECKERRFMT(status, L"   Couldn't change to appropriate video mode %u\r\n", maxResolutionIndex);
        }
        /* If maxResolutionIndex is indeed 0xFF, then keep the default that is already set... */
        status = (
            gop->Mode->Info->PixelFormat == PixelBltOnly || 
            gop->Mode->Info->PixelFormat == PixelFormatMax
        ) ? -1 : 0;
        CHECKERRFMT(status, (const CHAR16*)L"    Could Not Find a Video Mode to get a framebuffer\r\n    Default Video Mode doesn't support software Rendering...\r\n")
        setVideoModeFlag = BOOLEAN_TRUE;
    }


    *init = (framebuffer_t){
        .m_baseAddress  = (void*)((uintptr_t)gop->Mode->FrameBufferBase),
        .m_width        = (uint16_t)gop->Mode->Info->HorizontalResolution,
        .m_height       = (uint16_t)gop->Mode->Info->VerticalResolution,
        .m_pixelsPerScanLine      = gop->Mode->Info->PixelsPerScanLine,
        .m_pixelInfo    = parseFramebufferPixelFormat(gop->Mode->Info)
    };  
    CHECKERRFMT(infSize > UINT8_MAX ? -1 : 0, L"For Some reason The Pixel Format enumerator is bigger than 255 Bytes - This Should never error!\r\n");

    debug_printb(L"Framebuffer Data\r\n\
        Physical Address   : %p (%u, %u)\r\n\
        Pixels Per Scanline: %u\r\n\
        Single Pixel Size  : %u Bytes\r\n",
        init->m_baseAddress,
        init->m_width, init->m_height,
        init->m_pixelsPerScanLine,
        init->m_pixelInfo.m_sizeBytes
    );
    return;
}