#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#endif

typedef uint64_t OodLZ_CompressFunc(
    int32_t codec, uint8_t *src_buf, size_t src_len, uint8_t *dst_buf, int32_t level,
    void *opts, size_t offs, size_t unused, void *scratch, size_t scratch_size);

enum OodleCompressionLevel
{
    NoCompression,
    SuperFast,
    VeryFast,
    Fast,
    Normal,
    Optimal1,
    Optimal2,
    Optimal3,
    Optimal4,
    Optimal5
};

enum OodleFormat
{
    LZH,
    LZHLW,
    LZNIB,
    NoFormat,
    LZB16,
    LZBLW,
    LZA,
    LZNA,
    Kraken,
    Mermaid,
    BitKnit,
    Selkie,
    Akkorokamui
};
 
OodLZ_CompressFunc *OodLZ_Compress;

#ifdef _WIN32
bool check_terminal(void)
{
    DWORD *buffer = malloc(sizeof(DWORD));

    if (!buffer)
        return false;

    DWORD count = GetConsoleProcessList(buffer, 1);
    free(buffer);

    if (count == 1) {
        return false;
    }
    else {
        return true;
    }
}

void press_any_key(void)
{
    if (check_terminal())
        return;

    printf("\nPress any key to exit...\n");
    getch();
}
#endif

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf("EternalTextureCompressor v1.0 by PowerBall253 :)\n\n");
        printf("Usage:\n");
        printf("EternalResourceExtractor [texture1] [texture2] [...]\n");
        printf("Alternatively, drag and drop files onto this executable.\n");
        return 0;
    }

    uint8_t magic[] = { 0x44, 0x49, 0x56, 0x49, 0x4E, 0x49, 0x54, 0x59 };

#ifdef _WIN32
    HMODULE oodle = LoadLibraryA("./oo2core_8_win64.dll");
    
    if (!oodle) {
        fprintf(stderr, "ERROR: Failed to load oo2core_8_win64.dll - is the dll in the same directory as this executable?\n");
        press_any_key();
        return 1;
    }

    OodLZ_Compress = (OodLZ_CompressFunc*)GetProcAddress(oodle, "OodleLZ_Compress");

    if (!OodLZ_Compress) {
        fprintf(stderr, "ERROR: Failed to load OodleLZ_Compress function from oo2core_8_win64.dll - corrupted dll?\n");
        press_any_key();
        return 1;
    }
#else
    void *oodle = dlopen("./liblinoodle.so", RTLD_LAZY);

    if (!oodle) {
        fprintf(stderr, "ERROR: Failed to load liblinoodle.so - is the dll in the same directory as this executable?\n");
        return 1;
    }

    OodLZ_Compress = dlsym(oodle, "OodleLZ_Compress");

    if (!OodLZ_Compress) {
        fprintf(stderr, "ERROR: Failed to load OodleLZ_Compress function from oo2core_8_win64.dll - is oo2core_8_win64.dll in the same directory as this executable?\n");
        return 1;
    }
#endif

    int successes = 0;

    for (int i = 1; i < argc; i++) {
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

        size_t enc_len = dec_len + 274 * ((dec_len + 0x3FFFF) / 0x40000);
        uint8_t *enc_bytes = malloc(enc_len * sizeof(uint8_t));

        enc_len = OodLZ_Compress(Kraken, dec_bytes, dec_len, enc_bytes, Normal, NULL, 0, 0, NULL, 0);

        if (enc_len <= 0) {
            fprintf(stderr, "ERROR: Failed to compress %s!\n", argv[i]);
            continue;
        }

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