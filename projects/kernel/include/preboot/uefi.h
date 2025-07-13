#ifndef __UEFI_RUNTIME_SERVICES_STRUCTURE__
#define __UEFI_RUNTIME_SERVICES_STRUCTURE__
#include <std/stdint.h>


#define efi_api __attribute__((ms_abi))
#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef __UINT8_AS_BOOL
#define __UINT8_AS_BOOL
    typedef uint8_t bool_t;
#endif

typedef uint64_t efi_status;


#ifndef EFI_MP_SERVICES_PROTOCOL_GUID
#define EFI_MP_SERVICES_PROTOCOL_GUID \
    { 0x3FDDA605, 0xA76E, 0x4F46, { 0xAD, 0x29, 0x12, 0xF4, 0x53, 0x1B, 0x3D, 0x08 } }
#endif

#ifndef EFI_TIME_IN_DAYLIGHT
#define EFI_TIME_IN_DAYLIGHT     0x02   // ^^^^
#endif

#ifndef EFI_UNSPECIFIED_TIMEZONE
#define EFI_UNSPECIFIED_TIMEZONE 0x07FF // Value definition for Efi_time.TimeZone
#endif

#ifndef EFI_GLOBAL_VARIABLE_GUID
#define EFI_GLOBAL_VARIABLE_GUID \
    { 0x8BE4DF61, 0x93CA, 0x11d2, { 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C } }
#endif

#ifndef EFI_VARIABLE_NON_VOLATILE
#define EFI_VARIABLE_NON_VOLATILE                          0x00000001 // Variable attributes
#endif

#ifndef EFI_VARIABLE_BOOTSERVICE_ACCESS
#define EFI_VARIABLE_BOOTSERVICE_ACCESS                    0x00000002 // ^^^^
#endif

#ifndef EFI_VARIABLE_RUNTIME_ACCESS
#define EFI_VARIABLE_RUNTIME_ACCESS                        0x00000004 // ^^^^
#endif

#ifndef EFI_VARIABLE_HARDWARE_ERROR_RECORD
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD                 0x00000008 // ^^^^
#endif

#ifndef EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS            0x00000010 // ^^^^
#endif

#ifndef EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020 // ^^^^
#endif

#ifndef EFI_VARIABLE_APPEND_WRITE
#define EFI_VARIABLE_APPEND_WRITE                          0x00000040 // ^^^^
#endif

#ifndef EFI_MAXIMUM_VARIABLE_SIZE
#define EFI_MAXIMUM_VARIABLE_SIZE                          1024u      // Variable size limitation
#endif


typedef enum {
    EfiColdReset,
    EfiWarmReset,
    EfiShutdownReset
} Efi_reset_type;


typedef struct __efi_global_universal_identifier {
    uint32_t  data1;
    uint16_t  data2;
    uint16_t  data3;
    uint8_t   data4[8]; 
} Efi_guid;


// Time Structure-Defintions
typedef struct {          
    uint16_t Year;       // 1998 - 20XX
    uint8_t  Month;      // 1 - 12
    uint8_t  Day;        // 1 - 31
    uint8_t  Hour;       // 0 - 23
    uint8_t  Minute;     // 0 - 59
    uint8_t  Second;     // 0 - 59
    uint8_t  Pad1;
    uint32_t Nanosecond; // 0 - 999,999,999
    int16_t  TimeZone;   // -1440 to 1440 or 2047
    uint8_t  Daylight;
    uint8_t  Pad2;
} Efi_time;


typedef struct {
        uint32_t Resolution; // 1e-6 parts per million
        uint32_t Accuracy;   // hertz
        uint8_t  SetsToZero; // Set clears sub-second time
} Efi_time_capabilities;


typedef struct __efi_table_header {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} Efi_table_header;




typedef efi_status (efi_api* Efi_get_time) (
    Efi_time*              Time,
    Efi_time_capabilities* Capabilities OPTIONAL
);


typedef efi_status (efi_api* Efi_set_time) (
    Efi_time* Time
);


typedef efi_status (efi_api* Efi_get_wakeup_time) (
    bool_t*   Enabled,
    bool_t*   Pending,
    Efi_time* Time
);


typedef efi_status (efi_api* Efi_set_wakeup_time) (
    bool_t    Enable,
    Efi_time* Time OPTIONAL
);



// Misc Struct Definitions.
typedef efi_status (efi_api* Efi_get_next_high_mono_count) (
    uint32_t* HighCount
);


typedef efi_status (efi_api* Efi_reset_system) (
    Efi_reset_type ResetType,
    efi_status     ResetStatus,
    uint64_t       DataSize,
    uint16_t*      ResetData OPTIONAL
);



// NVRAM Variable Struct Defintions
typedef efi_status (efi_api* Efi_get_variable) (
    uint16_t* VariableName,
    Efi_guid* VendorGuid,
    uint32_t* Attributes OPTIONAL,
    uint64_t* DataSize,
    void*     Data
);

typedef efi_status (efi_api* Efi_get_next_variable_name) (
    uint64_t* VariableNameSize,
    uint16_t* VariableName,
    Efi_guid* VendorGuid
);


typedef efi_status (efi_api* Efi_set_variable) (
    uint16_t * VariableName,
    Efi_guid*  VendorGuid,
    uint32_t   Attributes,
    uint64_t   DataSize,
    void*      Data
);


typedef struct  {
    Efi_table_header                Header;
    
    // time services
    Efi_get_time                    GetTime;
    Efi_set_time                    SetTime;
    Efi_get_wakeup_time             GetWakeupTime;
    Efi_set_wakeup_time             SetWakeupTime;

    // virtual memory services
    uint8_t                         padding0[2 * sizeof(void*)];
    
    // variable services
    Efi_get_variable                GetVariable;
    Efi_get_next_variable_name      GetNextVariableName;
    Efi_set_variable                SetVariable;
    
    // misc
    Efi_get_next_high_mono_count    GetNextHighMonotonicCount;
    Efi_reset_system                ResetSystem;
    
    // capsule services
    uint8_t                         padding1[3 * sizeof(void*)];
} Efi_runtime_services;


typedef struct _EFI_MP_SERVICES_PROTOCOL Efi_mp_services;


typedef efi_status (efi_api* Efi_mp_services_get_number_of_processors) (
    Efi_mp_services*           This,
    uint64_t*                  processorsCount,
    uint64_t*                  processorsEnabled
);


typedef efi_status (efi_api* Efi_mp_services_padding) (
    Efi_mp_services* This
);


struct _EFI_MP_SERVICES_PROTOCOL {
    Efi_mp_services_get_number_of_processors GetNumberOfProcessors;
    Efi_mp_services_padding                  unused[6];               
};


#endif /* __UEFI_RUNTIME_SERVICES_STRUCTURE__ */
