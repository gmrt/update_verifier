#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "libverifier/verifier.h"

int main(int argc, char *argv[]) {
    const char *file_path;
    const char *keys_path;
    unsigned char *update_file;
    struct stat s;
    int fd;
    int ret;

    if (argc != 3) {
        printf("Usage: %s ZIP_PATH KEYS_PATH\n", argv[0]);
        return VERIFY_FAILURE;
    }

    file_path = argv[1];
    keys_path = argv[2];

    fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Failed to open file\n");
        close(fd);
        return VERIFY_FAILURE;
    }

    if (fstat(fd, &s) == -1) {
        fprintf(stderr, "Failed to get file size\n");
        close(fd);
        return VERIFY_FAILURE;
    }

    std::vector<Certificate> loadedKeys;
    if (!load_keys(keys_path, loadedKeys)) {
        fprintf(stderr, "Failed to load keys\n");
        close(fd);
        return VERIFY_FAILURE;
    }

    update_file = (unsigned char *) mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
    ret = verify_file(update_file, s.st_size, loadedKeys);
    munmap(update_file, s.st_size);
    close(fd);

    return ret;
}
