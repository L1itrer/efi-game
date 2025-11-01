#include <uefi.h>


efi_status_t efi_main(efi_handle_t imageHandle, efi_system_table_t* st);

int main(int argc, char **argv)
{
    efi_loaded_image_protocol_t *loaded_image;
    efi_status_t status;
    // Define the GUID variable (cannot pass macro directly)
    efi_guid_t LoadedImageProtocolGUID = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    // Retrieve the Loaded Image Protocol using HandleProtocol
    status = BS->HandleProtocol(IM, &LoadedImageProtocolGUID, (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        printf("HandleProtocol failed: 0x%lx\n", status);
        return status;
    }

    // Print the actual base address of the loaded image
    printf("Image loaded at: 0x%lx\n", (uint64_t)loaded_image->ImageBase);

    // Write image base and marker for GDB
    volatile uint64_t *marker_ptr = (uint64_t *)0x10000;
    volatile uint64_t *image_base_ptr = (uint64_t *)0x10008;
    *image_base_ptr = (uint64_t)loaded_image->ImageBase;  // Store ImageBase
    *marker_ptr = 0xDEADBEEF;   // Set marker

    efi_main(IM, ST);
    return EFI_SUCCESS;
}
