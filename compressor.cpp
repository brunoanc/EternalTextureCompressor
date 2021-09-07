#include "oodle.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <conio.h>
#endif

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
        printf("EternalTextureCompressor v1.0 by PowerBall253 :)\n\n");
        printf("Usage:\n");
        printf("EternalResourceExtractor [texture1] [texture2] [...]\n");
        printf("Alternatively, drag and drop files onto this executable.\n");
        return 0;
    }

    // DIVINITY magic
    constexpr uint8_t magic[] = { 0x44, 0x49, 0x56, 0x49, 0x4E, 0x49, 0x54, 0x59 };

    int successes = 0;

    for (int i = 1; i < argc; i++) {
        // Read texture file into memory
        FILE *textureFile = fopen(argv[i], "rb");

        if (!textureFile) {
            std::cerr << "ERROR: Failed to open " << argv[i] << " for reading/writing!\n" << std::endl;
            continue;
        }

        fseek(textureFile, 0, SEEK_END);
        size_t decLen = ftell(textureFile);
        fseek(textureFile, 0, SEEK_SET);

        uint8_t *decBytes = new(std::nothrow) uint8_t[decLen];

        if (fread(decBytes, 1, decLen * sizeof(uint8_t), textureFile) != decLen) {
            std::cerr << "ERROR: Failed to read bytes from " << argv[i] << "!" << std::endl;
            continue;
        }

        fclose(textureFile);

        // Compress texture with oodle
        size_t encLen = decLen + 274 * ((decLen + 0x3FFFF) / 0x40000);
        uint8_t *encBytes = new(std::nothrow) uint8_t[encLen];

        encLen = OodleLZ_Compress(8, decBytes, decLen, encBytes, 4);

        if (encLen <= 0) {
            std::cerr << "ERROR: Failed to compress " << argv[i] << "!" << std::endl;
            continue;
        }

        // Write new compressed file
        textureFile = fopen(argv[i], "wb");
    
        fseek(textureFile, 0, SEEK_SET);
        fwrite(magic, 1, sizeof(magic), textureFile);
        fwrite(&decLen, sizeof(decLen), 1, textureFile);
        fwrite(encBytes, 1, encLen, textureFile);

        std::cout << "Compressed " << argv[i] << "." << std::endl;
        successes++;
        fclose(textureFile);
    }

#ifdef _WIN32
    pressAnyKey();
#endif

    return successes == (argc - 1) ? 0 : 1;
}