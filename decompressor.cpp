#include <iostream>
#include <cstring>
#include "ooz.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <conio.h>
#endif

#define SAFE_SPACE 64

#ifdef _WIN32
// Check if the process is running through a terminal
bool checkTerminal(void)
{
    DWORD buffer[1];
    return GetConsoleProcessList(buffer, 1) > 1;
}

// Display the 'Press any key to exit...' message
void pressAnyKey(void)
{
    if (checkTerminal())
        return;

    std::cout << "\nPress any key to exit..." << std::endl;
    _getch();
}
#endif

// Main function
int main(int argc, char **argv)
{
    // Display help
    if (argc == 1) {
        printf("EternalTextureDecompressor v3.0 by PowerBall253 :)\n\n");
        printf("Usage:\n");
        printf("EternalTextureDecompressor [compressed1] [compressed2] [...]\n");
        printf("Alternatively, drag and drop files onto this executable.\n");
        return 0;
    }

    // DIVINITY magic
    constexpr uint8_t magic[] = { 0x44, 0x49, 0x56, 0x49, 0x4E, 0x49, 0x54, 0x59 };

    int successes = 0;

    for (int i = 1; i < argc; i++) {
        // Read compressed file into memory
        FILE *textureFile = fopen(argv[i], "rb");

        if (!textureFile) {
            std::cerr << "ERROR: Failed to open " << argv[i] << " for reading/writing!\n" << std::endl;
            continue;
        }

        fseek(textureFile, 0, SEEK_END);
        size_t encLen = ftell(textureFile) - 16;
        fseek(textureFile, 0, SEEK_SET);

        // Check for magic
        uint8_t file_magic[8];

        if (fread(file_magic, 1, 8, textureFile) != 8) {
            std::cerr << "ERROR: Failed to read bytes from " << argv[i] << "!" << std::endl;
            continue;
        }

        if (std::memcmp(file_magic, magic, 8) != 0) {
            std::cerr << "ERROR: " << argv[i] << " is not a compressed texture!" << std::endl;
            continue;
        }

        // Read decompressed size
        size_t decLen;

        if (fread(&decLen, 8, 1, textureFile) != 1) {
            std::cerr << "ERROR: Failed to read bytes from " << argv[i] << "!" << std::endl;
            continue;
        }

        uint8_t *encBytes = new(std::nothrow) uint8_t[encLen];

        if (fread(encBytes, 1, encLen, textureFile) != encLen) {
            std::cerr << "ERROR: Failed to read bytes from " << argv[i] << "!" << std::endl;
            continue;
        }

        fclose(textureFile);

        // Decompress texture with kraken
        uint8_t *decBytes = new(std::nothrow) uint8_t[decLen + SAFE_SPACE];
        int result = Kraken_Decompress(encBytes, encLen, decBytes, decLen);

        if (result <= 0) {
            std::cerr << "ERROR: Failed to decompress " << argv[i] << "!" << std::endl;
            continue;
        }

        // Write new compressed file
        textureFile = fopen(argv[i], "wb");
    
        fseek(textureFile, 0, SEEK_SET);
        fwrite(decBytes, 1, result, textureFile);

        std::cout << "Decompressed " << argv[i] << "." << std::endl;
        successes++;
        fclose(textureFile);
    }

#ifdef _WIN32
    pressAnyKey();
#endif

    return successes == (argc - 1) ? 0 : 1;
}
