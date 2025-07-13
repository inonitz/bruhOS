#ifndef __BOOTLOADER_C_DEFINITION_FILESYSTEM__
#define __BOOTLOADER_C_DEFINITION_FILESYSTEM__
#include <bootloader/api.h>


/* 
    * finds a file in the given directory
    * Directory - The directory to search
    * filename  - The name of the file to find
*/
EFI_API EFI_FILE_HANDLE findFile(
    IN EFI_FILE_HANDLE Directory,
    IN const CHAR16*   filename
);


/*
    * The function that performs the search inside findFile
    * The function returns the handle (if it is found) in out.
    * This function should be called inside the findFile function.
*/
/*
    * General algorithm
    * current_dir -> current directory being searched in
    * filename    -> the filename that is searched for
    * currfile    -> the opened file in current_dir
    * 
    * Open          : current_dir
    * get info about: currfile
    * if currfile is a directory: 
    *   look for filename in currfile (recursive call)
    * else
    *   compare names between currfile and filename
    *   if they match
    *       return currfile (we have found the handle for file with the name of "filename")
    *   else 
    *       keep searching in current_dir (until you get to the end of the directory)
*/
EFI_API void searchDirectoryRecursive(
    IN  EFI_FILE_HANDLE  current_dir, 
    IN  const CHAR16*    filename, 
    OUT EFI_FILE_HANDLE* out
);


/*
    Seeks 'byte_offset' bytes inside an already existing file handle,
    Tries to read 'byte_count' bytes,
        if success ->
            byteBuffer        will be set
            bytesActuallyRead will be set
        else
            byteBuffer        will not be set
            bytesActuallyRead will be 0
*/
EFI_API void readFile(
    IN EFI_FILE_HANDLE opened_file,
    IN uint64_t        byte_offset,
    IN uint64_t        byte_count,
    OUT uint8_t*       byteBuffer,
    OUT uint64_t*      bytesActuallyRead        
);


#endif /* __BOOTLOADER_C_DEFINITION_FILESYSTEM__ */
