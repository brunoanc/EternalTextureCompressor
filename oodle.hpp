#ifndef OODLE_H
#define OODLE_H

#include <iostream>

#ifdef _WIN32
#define LINKAGE __stdcall
#else
#define LINKAGE
#endif

extern "C"
{
    typedef struct __attribute__((__packed__))
    {
        uint32_t unused_was_verbosity;
        int32_t min_match_len;
        int32_t seek_chunk_reset;
        int32_t seek_chunk_len;
        int profile;
        int32_t dictionary_size;
        int32_t space_speed_tradeoff_bytes;
        int32_t unused_was_max_huffmans_per_chunk;
        int32_t send_quantum_crcs;
        int32_t max_local_dictionary_size;
        int32_t make_long_range_matcher;
        int32_t match_table_size_log2;
        int jobify;
        void *jobify_user_ptr;
        int32_t far_match_min_len;
        int32_t far_match_offset_log2;
        uint32_t reserved[4];
    } OodleLZ_CompressOptions;

    intptr_t LINKAGE OodleLZ_Compress(int compressor, const void *raw_buf, intptr_t raw_len, void *comp_buf, int level,
        const OodleLZ_CompressOptions *p_options = nullptr, const void *dictionary_base = nullptr, const void *lrm = nullptr,
        void *scratch_mem = nullptr, intptr_t scratch_size = 0);
}

#endif
