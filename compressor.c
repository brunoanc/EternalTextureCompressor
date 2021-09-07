#include <stdlib.h>
#include <stdbool.h>
#include "oodle.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <conio.h>
#endif

#ifdef _WIN32
// Check if the process is running through a terminal
bool check_terminal(void)
{
    DWORD buffer[1];
    return GetConsoleProcessList(buffer, 1) > 1;
}

// Display the 'Press any key to exit...' message
void press_any_key(void)
{
    if (check_terminal())
        return;

    printf("\nPress any key to exit...\n");
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
    const uint8_t magic[] = { 0x44, 0x49, 0x56, 0x49, 0x4E, 0x49, 0x54, 0x59 };

    int successes = 0;

    for (int i = 1; i < argc; i++) {
        // Read texture file into memory
        FILE *texture_file = fopen(argv[i], "rb");

        if (!texture_file) {
            fprintf(stderr, "ERROR: Failed to open %s for reading/writing!\n", argv[i]);
            continue;
        }

        fseek(texture_file, 0, SEEK_END);
        size_t dec_len = ftell(texture_file);
        fseek(texture_file, 0, SEEK_SET);

        uint8_t *dec_bytes = malloc(dec_len * sizeof(uint8_t));

        if (fread(dec_bytes, 1, dec_len * sizeof(uint8_t), texture_file) != dec_len) {
            fprintf(stderr, "ERROR: Failed to read bytes from %s!\n", argv[i]);
            continue;
        }

        fclose(texture_file);

        // Compress texture with oodle
        size_t enc_len = dec_len + 274 * ((dec_len + 0x3FFFF) / 0x40000);
        uint8_t *enc_bytes = malloc(enc_len * sizeof(uint8_t));

        enc_len = OodLZ_Compress(8, dec_bytes, dec_len, enc_bytes, 4, NULL, 0, 0, NULL, 0);

        if (enc_len <= 0) {
            fprintf(stderr, "ERROR: Failed to compress %s!\n", argv[i]);
            continue;
        }

        // Write new compressed file
        texture_file = fopen(argv[i], "wb");
    
        fseek(texture_file, 0, SEEK_SET);
        fwrite(magic, 1, sizeof(magic), texture_file);
        fwrite(&dec_len, sizeof(dec_len), 1, texture_file);
        fwrite(enc_bytes, 1, enc_len, texture_file);

        printf("Compressed %s.\n", argv[i]);
        successes++;
        fclose(texture_file);
    }

#ifdef _WIN32
    press_any_key();
#endif

    return successes == (argc - 1) ? 0 : 1;
}