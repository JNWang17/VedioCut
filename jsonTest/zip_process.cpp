//
// Created by 王金宁 on 2024/3/27.
//
#include <iostream>
#include <zip.h>

int main() {
    const char* zipFilename = "example.zip";

    // Open the ZIP archive
    int err;
    zip* archive = zip_open(zipFilename, 0, &err);
    if (!archive) {
        std::cerr << "Failed to open ZIP archive: " << zip_strerror(archive) << std::endl;
        return 1;
    }

    // Get the number of entries (files) in the ZIP archive
    int numEntries = zip_get_num_entries(archive, 0);
    if (numEntries < 0) {
        std::cerr << "Failed to get number of entries in ZIP archive: " << zip_strerror(archive) << std::endl;
        zip_close(archive);
        return 1;
    }

    // List the files in the ZIP archive
    for (int i = 0; i < numEntries; ++i) {
        const char* filename = zip_get_name(archive, i, 0);
        if (!filename) {
            std::cerr << "Failed to get name of entry " << i << " in ZIP archive: " << zip_strerror(archive) << std::endl;
            zip_close(archive);
            return 1;
        }
        std::cout << filename << std::endl;
    }

    // Close the ZIP archive
    zip_close(archive);

    return 0;
}