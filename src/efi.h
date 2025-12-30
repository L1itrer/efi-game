#ifndef UEFI_H
#define UEFI_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef u8 char8;
typedef u16 char16;

typedef i8 bool8;
typedef i32 bool32;

#define EFI_TRUE 1
#define EFI_FALSE 0

typedef uintptr_t usize;
typedef ptrdiff_t isize;

typedef float f32;
typedef double f64;

#define EFI_SYSTEM_TABLE_SIGNATURE 0x5453595320494249
#define EFI_2_100_SYSTEM_TABLE_REVISION ((2<<16) | (100))
#define EFI_2_90_SYSTEM_TABLE_REVISION  ((2<<16) | (90))
#define EFI_2_80_SYSTEM_TABLE_REVISION  ((2<<16) | (80))
#define efi_2_70_system_table_revision  ((2<<16) | (70))
#define EFI_2_60_SYSTEM_TABLE_REVISION  ((2<<16) | (60))
#define EFI_2_50_SYSTEM_TABLE_REVISION  ((2<<16) | (50))
#define EFI_2_40_SYSTEM_TABLE_REVISION  ((2<<16) | (40))
#define EFI_2_31_SYSTEM_TABLE_REVISION  ((2<<16) | (31))
#define EFI_2_30_SYSTEM_TABLE_REVISION  ((2<<16) | (30))
#define EFI_2_20_SYSTEM_TABLE_REVISION  ((2<<16) | (20))
#define EFI_2_10_SYSTEM_TABLE_REVISION  ((2<<16) | (10))
#define EFI_2_00_SYSTEM_TABLE_REVISION  ((2<<16) | (00))
#define EFI_1_10_SYSTEM_TABLE_REVISION  ((1<<16) | (10))
#define EFI_1_02_SYSTEM_TABLE_REVISION  ((1<<16) | (02))
#define EFI_SPECIFICATION_VERSION       EFI_SYSTEM_TABLE_REVISION
#define EFI_SYSTEM_TABLE_REVISION       EFI_2_100_SYSTEM_TABLE_REVISION

#if defined(__x86_64__) || defined(_M_X64)
# define EFICALL __attribute__((ms_abi))
#else
# error "Only x86_64 supported for now!"
# define EFICALL
#endif

#define ERROR_CODES_WCHAR \
ERR_CODE(EFI_SUCCESS, L"Success") \
ERR_CODE(EFI_LOAD_ERROR, L"The image failed to load.") \
ERR_CODE(EFI_INVALID_PARAMETER, L"A parameter was incorrect.") \
ERR_CODE(EFI_UNSUPPORTED, L"The operation is not supported.") \
ERR_CODE(EFI_BAD_BUFFER_SIZE, L"The buffer was not the proper size for the request.") \
ERR_CODE(EFI_BUFFER_TOO_SMALL, L"The buffer is not large enough to hold the requested data. The required buffer size is returned in the appropriate parameter when this error occurs.") \
ERR_CODE(EFI_NOT_READY, L"There is no data pending upon return.") \
ERR_CODE(EFI_DEVICE_ERROR, L"The physical device reported an error while attempting the operation.") \
ERR_CODE(EFI_WRITE_PROTECTED, L"The device cannot be written to.") \
ERR_CODE(EFI_OUT_OF_RESOURCES, L"A resource has run out.") \
ERR_CODE(EFI_VOLUME_CORRUPTED, L"An inconstancy was detected on the file system causing the operating to fail.") \
ERR_CODE(EFI_VOLUME_FULL, L"There is no more space on the file system.") \
ERR_CODE(EFI_NO_MEDIA, L"The device does not contain any medium to perform the operation.") \
ERR_CODE(EFI_MEDIA_CHANGED, L"The medium in the device has changed since the last access.") \
ERR_CODE(EFI_NOT_FOUND, L"The item was not found.") \
ERR_CODE(EFI_ACCESS_DENIED, L"Access was denied.") \
ERR_CODE(EFI_NO_RESPONSE, L"The server was not found or did not respond to the request.") \
ERR_CODE(EFI_NO_MAPPING, L"A mapping to a device does not exist.") \
ERR_CODE(EFI_TIMEOUT, L"The timeout time expired.") \
ERR_CODE(EFI_NOT_STARTED, L"The protocol has not been started.") \
ERR_CODE(EFI_ALREADY_STARTED, L"The protocol has already been started.") \
ERR_CODE(EFI_ABORTED, L"The operation was aborted.") \
ERR_CODE(EFI_ICMP_ERROR, L"An ICMP error occurred during the network operation.") \
ERR_CODE(EFI_TFTP_ERROR, L"A TFTP error occurred during the network operation.") \
ERR_CODE(EFI_PROTOCOL_ERROR, L"A protocol error occurred during the network operation.") \
ERR_CODE(EFI_INCOMPATIBLE_VERSION, L"The function encountered an internal version that was incompatible with a version requested by the caller.") \
ERR_CODE(EFI_SECURITY_VIOLATION, L"The function was not performed due to a security violation.") \
ERR_CODE(EFI_CRC_ERROR, L"A CRC error was detected.") \
ERR_CODE(EFI_END_OF_MEDIA, L"Beginning or end of media was reached") \
ERR_CODE(EFI_END_OF_FILE, L"The end of the file was reached.") \
ERR_CODE(EFI_INVALID_LANGUAGE, L"The language specified was invalid.") \
ERR_CODE(EFI_COMPROMISED_DATA, L"The security status of the data is unknown or compromised and the data must be updated or replaced to restore a valid security status.") \
ERR_CODE(EFI_IP_ADDRESS_CONFLICT, L"There is an address conflict address allocation") \
ERR_CODE(EFI_HTTP_ERROR, L"A HTTP error occurred during the network operation.") \



#define ERROR_CODES \
ERR_CODE(EFI_SUCCESS, "Success") \
ERR_CODE(EFI_LOAD_ERROR, "The image failed to load.") \
ERR_CODE(EFI_INVALID_PARAMETER, "A parameter was incorrect.") \
ERR_CODE(EFI_UNSUPPORTED, "The operation is not supported.") \
ERR_CODE(EFI_BAD_BUFFER_SIZE, "The buffer was not the proper size for the request.") \
ERR_CODE(EFI_BUFFER_TOO_SMALL, "The buffer is not large enough to hold the requested data. The required buffer size is returned in the appropriate parameter when this error occurs.") \
ERR_CODE(EFI_NOT_READY, "There is no data pending upon return.") \
ERR_CODE(EFI_DEVICE_ERROR, "The physical device reported an error while attempting the operation.") \
ERR_CODE(EFI_WRITE_PROTECTED, "The device cannot be written to.") \
ERR_CODE(EFI_OUT_OF_RESOURCES, "A resource has run out.") \
ERR_CODE(EFI_VOLUME_CORRUPTED, "An inconstancy was detected on the file system causing the operating to fail.") \
ERR_CODE(EFI_VOLUME_FULL, "There is no more space on the file system.") \
ERR_CODE(EFI_NO_MEDIA, "The device does not contain any medium to perform the operation.") \
ERR_CODE(EFI_MEDIA_CHANGED, "The medium in the device has changed since the last access.") \
ERR_CODE(EFI_NOT_FOUND, "The item was not found.") \
ERR_CODE(EFI_ACCESS_DENIED, "Access was denied.") \
ERR_CODE(EFI_NO_RESPONSE, "The server was not found or did not respond to the request.") \
ERR_CODE(EFI_NO_MAPPING, "A mapping to a device does not exist.") \
ERR_CODE(EFI_TIMEOUT, "The timeout time expired.") \
ERR_CODE(EFI_NOT_STARTED, "The protocol has not been started.") \
ERR_CODE(EFI_ALREADY_STARTED, "The protocol has already been started.") \
ERR_CODE(EFI_ABORTED, "The operation was aborted.") \
ERR_CODE(EFI_ICMP_ERROR, "An ICMP error occurred during the network operation.") \
ERR_CODE(EFI_TFTP_ERROR, "A TFTP error occurred during the network operation.") \
ERR_CODE(EFI_PROTOCOL_ERROR, "A protocol error occurred during the network operation.") \
ERR_CODE(EFI_INCOMPATIBLE_VERSION, "The function encountered an internal version that was incompatible with a version requested by the caller.") \
ERR_CODE(EFI_SECURITY_VIOLATION, "The function was not performed due to a security violation.") \
ERR_CODE(EFI_CRC_ERROR, "A CRC error was detected.") \
ERR_CODE(EFI_END_OF_MEDIA, "Beginning or end of media was reached") \
ERR_CODE(EFI_END_OF_FILE, "The end of the file was reached.") \
ERR_CODE(EFI_INVALID_LANGUAGE, "The language specified was invalid.") \
ERR_CODE(EFI_COMPROMISED_DATA, "The security status of the data is unknown or compromised and the data must be updated or replaced to restore a valid security status.") \
ERR_CODE(EFI_IP_ADDRESS_CONFLICT, "There is an address conflict address allocation") \
ERR_CODE(EFI_HTTP_ERROR, "A HTTP error occurred during the network operation.") \

#define ERR_CODE(en, str) en,
typedef enum EfiErrorCodes {
  ERROR_CODES
}EfiErrorCodes;
#undef ERR_CODE



typedef void* EfiHandle;
typedef void* EfiEvent;
typedef uintptr_t EfiUsize;
typedef EfiUsize EfiStatus;


typedef u64 EfiPhysicalAddress;
typedef u64 EfiVirtualAddress;
typedef usize EfiTPL;

#define EFI_TPL_APPLICATION    4
#define EFI_TPL_CALLBACK       8
#define EFI_TPL_NOTIFY         16
#define EFI_TPL_HIGH_LEVEL     31

typedef struct EfiGuidStruct {
  u32 data1;
  u16 data2;
  u16 data3;
  u8 data4[8];
}EfiGuidStruct ;

typedef struct EfiGuid {
  union {
    u8 U8[16];
    u16 U16[8];
    u32 U32[4];
    u64 U64[2];
    EfiGuidStruct specified;
  };
} EfiGuid;

typedef struct EfiTableHeader{
  u64 signature;
  u32 revision;
  u32 headerSize;
  u32 crc32;
  u32 reserved;
}EfiTableHeader;

typedef struct EfiInputKey {
  u16 scanCode;
  char16 unicodeChar;
}EfiInputKey;

typedef struct EfiSimpleTextInputProtocol EfiSimpleTextInputProtocol;
typedef struct EfiSimpleTextInputProtocol {
  EfiStatus (EFICALL *reset) (
    struct EfiSimpleTextInputProtocol* _this,
    bool8 ExtendedVerification
  );
  EfiStatus (EFICALL *read_key_stroke) (
    EfiSimpleTextInputProtocol* _this,
    EfiInputKey* Key
  );
  EfiEvent waitForKey;
} EfiSimpleTextInputProtocol;

typedef struct EfiSimpleTextOutputMode {
  i32 maxMode;
  i32 mode;
  i32 attribute;
  i32 cursorColumn;
  i32 cursorRow;
  bool8 cursorVisible;
} EfiSimpleTextOutputMode;


typedef struct EfiSimpleTextOutputProtocol EfiSimpleTextOutputProtocol;
typedef struct EfiSimpleTextOutputProtocol {
  EfiStatus (EFICALL *efi_text_reset) (
    EfiSimpleTextOutputProtocol* _this,
    bool8 extendedVerification
  ); // reset the console device
  EfiStatus (EFICALL *output_string) (
    EfiSimpleTextOutputProtocol* _this,
    char16* str
  ); // display a string at current text location
  EfiStatus (EFICALL *test_string) (
    EfiSimpleTextOutputProtocol* _this,
    char16* str
  ); // test if this text can be written to console
  EfiStatus (EFICALL *query_mode) (
    EfiSimpleTextOutputProtocol* _this,
    usize modeNumber,
    usize* columns,
    usize* rows
  ); // query the supported text mode
  EfiStatus (EFICALL *set_mode) (
    EfiSimpleTextOutputProtocol* _this,
    usize modeNumber
  ); // sets the current mode
  EfiStatus (EFICALL *set_attribute) (
    EfiSimpleTextOutputProtocol* _this,
    usize attribute
  ); // sets the fg and bg color of the text
  EfiStatus (EFICALL *clear_screen) (
    EfiSimpleTextOutputProtocol* _this
  ); // self-explanatory
  EfiStatus (EFICALL *set_cursor_position) (
    EfiSimpleTextOutputProtocol* _this,
    usize column,
    usize row
  ); // self-explanatory
  EfiStatus (EFICALL *enable_cursor) (
    EfiSimpleTextOutputProtocol* _this,
    bool8 visible
  ); // toggles the visibility of the cursor
  EfiSimpleTextOutputMode* mode;
}EfiSimpleTextOutputProtocol;

typedef struct EfiTime {
  u16 year;
  u8 month;
  u8 day;
  u8 hour;
  u8 minute;
  u8 second;
  u8 padding1;
  u32 nanoseconds;
  i16 timeZone;
  u8 daylight;
  u8 padding2;
} EfiTime;


typedef struct EfiMemoryDescriptor {
  u32 type;
  EfiPhysicalAddress physical_start;
  EfiVirtualAddress virtual_start;
  u64 number_of_pages;
  u64 attribute;
} EfiMemoryDescriptor;

typedef struct EfiTimeCapabilites {
  u32 resulution;
  u32 accuracy;
  bool8 setsToZero;
} EfiTimeCapabilites;

typedef enum EfiResetType {
  EfiResetCold,
  EfiResetWarm,
  EfiResetShutdown,
  EfiResetPlatformSpecific
} EfiResetType;

typedef struct EfiCapsuleHeader {
  EfiGuid capsuleGuid;
  u32 headerSize;
  u32 flags;
  u32 capsuleImageSize;
} EfiCapsuleHeader;

typedef struct EfiRuntimeServices {
  EfiTableHeader hdr;
  EfiStatus (EFICALL *get_time) (
    EfiTime* time,
    EfiTimeCapabilites* capabilites // is optional
  );
  EfiStatus (EFICALL *set_time) (
    EfiTime* time
  );
  EfiStatus (EFICALL *get_wakeup_time) (
    bool8* enabled,
    bool8* pending,
    EfiTime* time
  );
  EfiStatus (EFICALL *set_wakeup_time) (
    bool8,
    EfiTime* time // optional
  );
  EfiStatus (EFICALL *set_virtual_address_map) (
    usize memoryMapSize,
    usize descriptorSize,
    u32 descriptorVersion,
    EfiMemoryDescriptor* virtualMap
  );
  EfiStatus (EFICALL *convert_pointer) (
    usize debugDisposition,
    void** address
  );
  EfiStatus (EFICALL *get_variable) (
    char16* variableName,
    EfiGuid* vendorGuid,
    u32* attributes, // optional
    usize* dataSize,
    void* data
  );
  EfiStatus (EFICALL *get_next_variable_name) (
    usize* variableNameSize,
    char16* variableName,
    EfiGuid* vendorGuid
  );
  EfiStatus (EFICALL *set_variable) (
    char16* variableName,
    EfiGuid* vendorGuid,
    u32 attributes,
    usize dataSize,
    void* data
  );
  EfiStatus (EFICALL *get_next_high_monotonic_count) (
    u32* highCount
  );
  EfiStatus (EFICALL *reset_system) (
    EfiResetType resetType,
    EfiStatus resetStatus,
    usize dataSize,
    void* resetData // can be NULL
  );
  EfiStatus (EFICALL *update_capsule) (
    EfiCapsuleHeader** capsuleHeaderArray,
    usize capsuleCount,
    EfiPhysicalAddress scatterGatherList
  );
  EfiStatus (EFICALL *query_capsule_capabilites) (
    EfiCapsuleHeader** capsuleHeaderArray,
    usize capsuleCount,
    u64* maximumCapsuleSize,
    EfiResetType* resetType
  );
  EfiStatus (EFICALL *query_variable_info) (
    u32 attributes,
    u64* maximumVariableStorageSize,
    u64* remainingVariableStorageSize,
    u64* maximumVariableSize
  );
} EfiRuntimeServices;

typedef enum EfiAllocateType{
   AllocateAnyPages,
   AllocateMaxAddress,
   AllocateAddress,
   MaxAllocateType
} EfiAllocateType;

typedef enum EfiMemoryType{
   EfiReservedMemoryType,
   EfiLoaderCode,
   EfiLoaderData,
   EfiBootServicesCode,
   EfiBootServicesData,
   EfiRuntimeServicesCode,
   EfiRuntimeServicesData,
   EfiConventionalMemory,
   EfiUnusableMemory,
   EfiACPIReclaimMemory,
   EfiACPIMemoryNVS,
   EfiMemoryMappedIO,
   EfiMemoryMappedIOPortSpace,
   EfiPalCode,
   EfiPersistentMemory,
   EfiUnacceptedMemoryType,
   EfiMaxMemoryType
} EfiMemoryType;

typedef void (EFICALL *EfiEventNotifyProc)(EfiEvent event, void* context);

typedef enum EfiTimerDelay{
   TimerCancel,
   TimerPeriodic,
   TimerRelative
} EfiTimerDelay;

typedef enum EfiInterfaceType{
   EFI_NATIVE_INTERFACE
} EfiInterfaceType;

typedef enum EfiLocateSearchType{
   AllHandles,
   ByRegisterNotify,
   ByProtocol
} EfiLocateSearchType;

typedef struct EfiDevicePathProtocol {
  u8 type;
  u8 subType;
  u8 length[2];
}EfiDevicePathProtocol;

typedef struct EfiOpenProtocolInformationEntry {
  EfiHandle agentHandle;
  EfiHandle controllerHandle;
  u32 attributes;
  u32 openCount;
} EfiOpenProtocolInformationEntry;

typedef struct EfiBootServices {
  EfiTableHeader hdr;
  EfiStatus (EFICALL *raise_tpl) (
    EfiTPL newTpl
  );
  EfiStatus (EFICALL *restore_tpl) (
    EfiTPL oldTpl
  );
  EfiStatus (EFICALL *allocate_pages) (
    EfiAllocateType type,
    EfiMemoryType memoryType,
    usize pages,
    EfiPhysicalAddress* memory
  );
  EfiStatus (EFICALL *free_pages) (
    EfiPhysicalAddress memory,
    usize pages
  );
  EfiStatus (EFICALL*get_memory_map) (
    usize* memoryMapSize,
    EfiMemoryDescriptor* memoryMap,
    usize* mapKey,
    usize* descriptorSize,
    u32* descriptorVersion
  );
  EfiStatus (EFICALL *allocate_pool) (
    EfiMemoryType poolType,
    usize size,
    void** buffer
  );
  EfiStatus (EFICALL *free_pool) (
    void* buffer
  );
  EfiStatus (EFICALL *create_event) (
    u32 type,
    EfiTPL notifyTpl,
    EfiEventNotifyProc notifyFunction,
    void* notifyContext,
    EfiEvent* event
  );
  EfiStatus (EFICALL *set_timer) (
    EfiEvent event,
    EfiTimerDelay type,
    u64 triggerTime
  );
  EfiStatus (EFICALL *wait_for_event) (
    usize numberOfEvents,
    EfiEvent* event,
    usize* index
  );
  EfiStatus (EFICALL *signal_event) (
    EfiEvent event
  );
  EfiStatus (EFICALL *close_event) (
    EfiEvent event
  );
  EfiStatus (EFICALL *check_event) (
    EfiEvent event
  );
  EfiStatus (EFICALL *install_protocol_interface) (
    EfiHandle* handle,
    EfiGuid* protocol,
    EfiInterfaceType interfaceType,
    void* interface
  );
  EfiStatus (EFICALL *reinstall_protocol_interface) (
    EfiHandle* handle,
    EfiGuid* protocol,
    void* oldInterface,
    void* newInterface
  );
  EfiStatus (EFICALL *uninstall_protocol_interface) (
    EfiHandle handle,
    EfiGuid* protocol,
    void* interface
  );
  EfiStatus (EFICALL *handle_protocol) (
    EfiHandle handle,
    EfiGuid* protocol,
    void** interface
  );
  void* _reserved;
  EfiStatus (EFICALL *register_protocol_notify) (
    EfiGuid* protocol,
    EfiEvent event,
    void** registration
  );
  EfiStatus (EFICALL *locate_handle) (
    EfiLocateSearchType searchType,
    EfiGuid* protocol,
    void* searchKey,
    usize* bufferSize,
    EfiHandle* buffer
  );
  EfiStatus (EFICALL *locate_device_path) (
    EfiGuid* protocol,
    EfiDevicePathProtocol** devicePath,
    EfiHandle* device
  );
  EfiStatus (EFICALL *install_configuration_table) (
    EfiGuid* guid,
    void* table
  );
  EfiStatus (EFICALL *image_load) (
    bool8 bootPolicy,
    EfiHandle parentImageHandle,
    EfiDevicePathProtocol* devicePath,
    void* sourceBuffer,
    usize sourceSize,
    EfiHandle* imageHandle
  );
  EfiStatus (EFICALL *image_start) (
    EfiHandle imageHandle,
    usize* exitDataSize,
    char16** exitData
  );
  EfiStatus (EFICALL *exit) (
    EfiHandle imageHandle,
    EfiStatus exitStatus,
    usize exitDataSize,
    char16* exitData
  );
  EfiStatus (EFICALL *image_unload) (
    EfiHandle imageHandle
  );
  EfiStatus (EFICALL* exit_boot_services) (
    EfiHandle imageHandle,
    usize mapKey
  );
  EfiStatus (EFICALL *get_next_monotonic_count) (
    u64* count
  );
  EfiStatus (EFICALL *stall) (
    usize microseconds
  );
  EfiStatus (EFICALL* set_watchdog_timer) (
    usize timeout,
    u64 watchdogCode,
    usize dataSize,
    char16* watchdogData
  );
  EfiStatus (EFICALL *connect_controller) (
    EfiHandle controllerHandle,
    EfiHandle* driverImageHandle,
    EfiDevicePathProtocol* remainingDevicePath,
    bool8 recursive
  );
  EfiStatus (EFICALL *disconnect_controller) (
    EfiHandle controllerHandle,
    EfiHandle driverImageHandle,
    EfiHandle childHandle
  );
  EfiStatus (EFICALL *open_protocol) (
    EfiHandle handle,
    EfiGuid* protocol,
    void** interface,
    EfiHandle agentHandle,
    EfiHandle controllerHandle,
    u32 attributes
  );
  EfiStatus (EFICALL *close_protocol) (
    EfiHandle handle,
    EfiGuid* protocol,
    EfiHandle agentHandle,
    EfiHandle controllerHandle
  );
  EfiStatus (EFICALL *open_protocol_information) (
    EfiHandle handle,
    EfiGuid* protocol,
    EfiOpenProtocolInformationEntry** entryBuffer,
    usize* entryCount
  );
  EfiStatus (EFICALL *protocols_per_handle) (
    EfiHandle handle,
    EfiGuid*** protocolBuffer,
    usize* protocolBufferCount
  );
  EfiStatus (EFICALL *locate_handle_buffer) (
    EfiLocateSearchType searchType,
    EfiGuid* protocol,
    void* searchKey,
    usize* noHandles,
    EfiHandle** buffer
  );
  EfiStatus (EFICALL *locate_protocol) (
    EfiGuid* protocol,
    void* registration,
    void** interface
  );
  EfiStatus (EFICALL *install_multiple_protocol_interfaces) (
    EfiHandle* handle,
    ...
  );
  EfiStatus (EFICALL *uninstall_multiple_protocol_interfaces) (
    EfiHandle* handle,
    ...
  );
  EfiStatus (EFICALL *calculate_crc32) (
    void* data,
    usize dataSize,
    u32* crc32
  );
  EfiStatus (EFICALL *copy_mem) (
    void* destination,
    void* source,
    usize length
  );
  EfiStatus (EFICALL *set_mem) (
    void* buffer,
    usize size,
    u8 value
  );
  EfiStatus (EFICALL *create_event_ex) (
    u32 type,
    EfiTPL notifyTpl,
    EfiEventNotifyProc notifyFunction,
    const void* notifyContext,
    const EfiGuid* eventGroup,
    EfiEvent* event
  );
} EfiBootServices;

typedef struct EfiConfigurationTable {
  EfiGuid vendorGuid;
  void* vendorTable;
} EfiConfigurationTable;

typedef struct EfiSystemTable {
  EfiTableHeader hdr;
  char16* firmwareVendor;
  u32 firmwareRevision;
  EfiHandle consoleInHandle;
  EfiSimpleTextInputProtocol* conIn;
  EfiHandle consoleOutHandle;
  EfiSimpleTextOutputProtocol* conOut;
  EfiHandle standardErrorHandle;
  EfiSimpleTextOutputProtocol* stdErr;
  EfiRuntimeServices* runtimeServices;
  EfiBootServices* bootServices;
  usize numberOfTableEntries;
  EfiConfigurationTable* configurationTable;
}EfiSystemTable;

// -------------- GOP -----------------

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
 {0x9042a9de,0x23dc,0x4a38,\
  {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

typedef enum EfiGraphicsPixelFormat {
  PixelRedGreenBlueReserved8BitPerColor,
  PixelBlueGreenRedReserved8BitPerColor,
  PixelBitMask,
  PixelBltOnly,
  PixelFormatMax
}EfiGraphicsPixelFormat;

typedef struct EfiPixelBitmask {
  u32 redMask;
  u32 greenMask;
  u32 blueMask;
  u32 reservedMask;
}EfiPixelBitmask;

typedef struct EfiGraphicsOutputModeInformation {
  u32 version;
  u32 hotizontalResolution;
  u32 verticalResolution;
  i32 pixelFormatEnum;
  EfiPixelBitmask pixelInformation;
  u32 pixelsPerScanLine;
}EfiGraphicsOutputModeInformation;

typedef struct EfiGraphicsOutputBltPixel {
  u8 blue, green, red, reserved;
}EfiGraphicsOutputBltPixel;

typedef enum GraphicsOutputBltOperation{
 EfiBltVideoFill,
 EfiBltVideoToBltBuffer,
 EfiBltBufferToVideo,
 EfiBltVideoToVideo,
 EfiGraphicsOutputBltOperationMax
}GraphicsOutputBltOperation;

typedef struct EfiGraphicsOutputProtocolMode {
  u32 maxMode, mode;
  EfiGraphicsOutputModeInformation* info;
  usize sizeOfInfo;
  EfiPhysicalAddress frameBufferBase;
  usize frameBufferSize;
}EfiGraphicsOutputProtocolMode ;

typedef struct EfiGraphicsOutputProtocol EfiGraphicsOutputProtocol ;

typedef struct EfiGraphicsOutputProtocol {
  EfiStatus (EFICALL *query_mode) (
    EfiGraphicsOutputProtocol* _this,
    u32 modeNumber,
    usize* sizeOfInfo,
    EfiGraphicsOutputModeInformation** info
  );
  EfiStatus (EFICALL *set_mode) (
    EfiGraphicsOutputProtocol* _this,
    u32 modeNumber
  );
  EfiStatus (EFICALL *blt) (
    EfiGraphicsOutputProtocol* _this,
    EfiGraphicsOutputBltPixel* bltBuffer,
    i32 bltOpreationEnum,
    usize sourceX,
    usize sourceY,
    usize destX,
    usize destY,
    usize width,
    usize height,
    usize delta
  );
  EfiGraphicsOutputProtocolMode* mode;
}EfiGraphicsOutputProtocol ;

#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
  {0x5B1B31A1,0x9562,0x11d2,\
    {0x8E,0x3F,0x00,0xA0,0xC9,0x69,0x72,0x3B}}

typedef struct EfiLoadedImageProtocol {
  u32 revision;
  EfiHandle parentHandle;
  EfiSystemTable* systemTable;
  EfiHandle deviceHandle;
  EfiDevicePathProtocol* filePath;
  void* reserved;
  u32 loadedOptionsSize;
  void* loadOptions;
  void* imageBase;
  u64 imageSize;
  EfiMemoryType imageCodeType;
  EfiMemoryType imageDataType;
  EfiStatus (EFICALL *unload) (
    EfiHandle imageHandle
  );
} EfiLoadedImageProtocol;

#define EFI_ERROR(status) (status != 0)

#endif
