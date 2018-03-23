#include <iostream>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

#include "libverifier/verifier.h"

#ifdef _WIN32
#include "Windows.h"
#endif

int main(int argc, char *argv[]) {
    const char *filePath;
    const char *keysPath;
    unsigned char *updateFile;
    size_t fileSize;
    int ret;

    if (argc != 3) {
        std::cout << "Usage " << argv[0] << " ZIP_PATH KEYS_PATH" << std::endl;
        return VERIFY_FAILURE;
    }

    filePath = argv[1];
    keysPath = argv[2];

    std::vector<Certificate> loadedKeys;
    if (!load_keys(keysPath, loadedKeys)) {
        std::cerr << "Failed to load keys" << std::endl;
        return VERIFY_FAILURE;
    }

#ifndef _WIN32
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open file" << std::endl;
        close(fd);
        return VERIFY_FAILURE;
    }

    struct stat s;
    if (fstat(fd, &s) == -1) {
        std::cerr << "Could not get file size" << std::endl;
        close(fd);
        return VERIFY_FAILURE;
    }
    fileSize = s.st_size;

    updateFile = (unsigned char *) mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (updateFile == NULL) {
        std::cerr << "Failed to memory map update file" << std::endl;
        close(fd);
        return VERIFY_FAILURE;
    }
#else
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, 0,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file" << std::endl;
        CloseHandle(hFile);
        return VERIFY_FAILURE;
    }
    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMap == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create file mapping" << std::endl;
        CloseHandle(hFile);
        return VERIFY_FAILURE;
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(hFile, &size)) {
        std::cerr << "Failed to get file size" << std::endl;
        CloseHandle(hMap);
        CloseHandle(hFile);
        return VERIFY_FAILURE;
    }
    fileSize = size.QuadPart;
    updateFile = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, fileSize);
    if (updateFile == NULL) {
        std::cerr << "Failed to memory map update file" << std::endl;
        CloseHandle(hMap);
        CloseHandle(hFile);
        return VERIFY_FAILURE;
    }
#endif

    ret = verify_file(updateFile, fileSize, loadedKeys);

#ifndef _WIN32
    munmap(updateFile, fileSize);
    close(fd);
#else
    UnmapViewOfFile(updateFile);
    CloseHandle(hMap);
    CloseHandle(hFile);
#endif

    return ret;
}
