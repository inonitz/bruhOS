/*++

Copyright (c) 1998  Intel Corporation

Module Name:

    lib.h

Abstract:

    EFI library header files



Revision History

--*/


#ifndef __GNU_EFI_DIDNT_INCLUDE_A_FUCKING_HEADER_GUARD__
#define __GNU_EFI_DIDNT_INCLUDE_A_FUCKING_HEADER_GUARD__
#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif


#include "efi.h"
#include "efilib.h"
#include "efirtlib.h"

//
// Include non architectural protocols
//
#include "protocol/efivar.h"
#include "protocol/legacyboot.h"
#include "protocol/intload.h"
#include "protocol/vgaclass.h"
#include "protocol/eficonsplit.h"
#include "protocol/adapterdebug.h"
#include "protocol/intload.h"

#include "efigpt.h"
#include "libsmbios.h"

//
// Prototypes
//

VOID
InitializeGuid (
    VOID
    );


BOOLEAN
LibMatchDevicePaths (
    IN  EFI_DEVICE_PATH *Multi,
    IN  EFI_DEVICE_PATH *Single
    );

EFI_DEVICE_PATH *
LibDuplicateDevicePathInstance (
    IN EFI_DEVICE_PATH  *DevPath
    );


//
// Globals
//
extern BOOLEAN                          LibInitialized;
extern BOOLEAN                          LibFwInstance;
extern EFI_HANDLE			            LibImageHandle;
extern SIMPLE_TEXT_OUTPUT_INTERFACE*    LibRuntimeDebugOut;
extern EFI_UNICODE_COLLATION_INTERFACE* UnicodeInterface;
extern EFI_UNICODE_COLLATION_INTERFACE  LibStubUnicodeInterface;
extern EFI_RAISE_TPL                    LibRuntimeRaiseTPL;
extern EFI_RESTORE_TPL                  LibRuntimeRestoreTPL;



#endif /* __GNU_EFI_DIDNT_INCLUDE_A_FUCKING_HEADER_GUARD__ */