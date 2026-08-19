// build as clang -O3 -march=native popcount_bench.c -o popcount_bench -lm
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Include SIMDe libraries
#include "simde/x86/avx512.h"
#include "simde/x86/avx2.h"
#include "simde/simde-arch.h"
#include "simde/simde-common.h"
#include "simde/simde-features.h"
#include "simde/x86/avx512/popcnt.h"

#include "libpopcnt.h"

char format_string[] = "%-25s | %13.2f | %13.2f | %8.2fx\n";
/* ========================================================================= */
/*                           CARRY-SAVE ADDERS                               */
/* ========================================================================= */

// 1. AVX-512 with Ternary Logic
static inline void csa_avx512_ternary(simde__m512i* h, simde__m512i* l, simde__m512i a, simde__m512i b, simde__m512i c) {
    *l = simde_mm512_ternarylogic_epi64(a, b, c, 0x96);
    *h = simde_mm512_ternarylogic_epi64(a, b, c, 0xE8);
}

// 2. AVX-512 without Ternary Logic
static inline void csa_avx512_notern(simde__m512i* h, simde__m512i* l, simde__m512i a, simde__m512i b, simde__m512i c) {
    simde__m512i u = simde_mm512_xor_si512(a, b);
    *h = simde_mm512_or_si512(simde_mm512_and_si512(a, b), simde_mm512_and_si512(u, c));
    *l = simde_mm512_xor_si512(u, c);
}

// 3. AVX2 logic
static inline void csa_avx2(simde__m256i* h, simde__m256i* l, simde__m256i a, simde__m256i b, simde__m256i c) {
    simde__m256i u = simde_mm256_xor_si256(a, b);
    *h = simde_mm256_or_si256(simde_mm256_and_si256(a, b), simde_mm256_and_si256(u, c));
    *l = simde_mm256_xor_si256(u, c);
}

/* ========================================================================= */
/*                          VECTOR POPCOUNT HELPERS                          */
/* ========================================================================= */

static inline uint64_t popcount_512(simde__m512i v) {
    uint64_t val[8];
    simde_mm512_storeu_si512((simde__m512i*)val, v);
    uint64_t sum = 0;
    for(int i = 0; i < 8; i++) sum += __builtin_popcountll(val[i]);
    return sum;
}

static inline uint64_t popcount_256(simde__m256i v) {
    uint64_t val[4];
    simde_mm256_storeu_si256((simde__m256i*)val, v);
    uint64_t sum = 0;
    for(int i = 0; i < 4; i++) sum += __builtin_popcountll(val[i]);
    return sum;
}

static inline uint64_t sum_lanes_512(simde__m512i v) {
    uint64_t val[8];
    simde_mm512_storeu_si512((simde__m512i*)val, v);
    uint64_t sum = 0;
    for(int i = 0; i < 8; i++) sum += val[i];
    return sum;
}


/* ========================================================================= */
/*                         HARLEY-SEAL IMPLEMENTATIONS                       */
/* ========================================================================= */

#define HARLEY_SEAL_TREE(CSA_FUNC, d) \
    CSA_FUNC(&twosA, &ones, ones, d[0], d[1]); \
    CSA_FUNC(&twosB, &ones, ones, d[2], d[3]); \
    CSA_FUNC(&foursA, &twos, twos, twosA, twosB); \
    CSA_FUNC(&twosA, &ones, ones, d[4], d[5]); \
    CSA_FUNC(&twosB, &ones, ones, d[6], d[7]); \
    CSA_FUNC(&foursB, &twos, twos, twosA, twosB); \
    CSA_FUNC(&eightsA, &fours, fours, foursA, foursB); \
    CSA_FUNC(&twosA, &ones, ones, d[8], d[9]); \
    CSA_FUNC(&twosB, &ones, ones, d[10], d[11]); \
    CSA_FUNC(&foursA, &twos, twos, twosA, twosB); \
    CSA_FUNC(&twosA, &ones, ones, d[12], d[13]); \
    CSA_FUNC(&twosB, &ones, ones, d[14], d[15]); \
    CSA_FUNC(&foursB, &twos, twos, twosA, twosB); \
    CSA_FUNC(&eightsB, &fours, fours, foursA, foursB); \
    CSA_FUNC(&sixteens, &eights, eights, eightsA, eightsB);

uint64_t popcount_avx512(const uint64_t* data, size_t len, int use_ternary) {
    simde__m512i ones = simde_mm512_setzero_si512();
    simde__m512i twos = simde_mm512_setzero_si512();
    simde__m512i fours = simde_mm512_setzero_si512();
    simde__m512i eights = simde_mm512_setzero_si512();
    simde__m512i sixteens = simde_mm512_setzero_si512();
    simde__m512i twosA, twosB, foursA, foursB, eightsA, eightsB;

    uint64_t total_sixteens = 0;
    size_t limit = len - (len % 128);

    for (size_t i = 0; i < limit; i += 128) {
        simde__m512i d[16];
        for(int j=0; j<16; j++) d[j] = simde_mm512_loadu_si512(&data[i + j*8]);

        if (use_ternary) {
            HARLEY_SEAL_TREE(csa_avx512_ternary, d);
        } else {
            HARLEY_SEAL_TREE(csa_avx512_notern, d);
        }
        total_sixteens += popcount_512(sixteens);
    }

    uint64_t total = total_sixteens * 16;
    total += popcount_512(eights) * 8;
    total += popcount_512(fours) * 4;
    total += popcount_512(twos) * 2;
    total += popcount_512(ones) * 1;

    for (size_t i = limit; i < len; i++) total += __builtin_popcountll(data[i]);
    return total;
}

uint64_t popcount_avx2(const uint64_t* data, size_t len) {
    simde__m256i ones = simde_mm256_setzero_si256();
    simde__m256i twos = simde_mm256_setzero_si256();
    simde__m256i fours = simde_mm256_setzero_si256();
    simde__m256i eights = simde_mm256_setzero_si256();
    simde__m256i sixteens = simde_mm256_setzero_si256();
    simde__m256i twosA, twosB, foursA, foursB, eightsA, eightsB;

    uint64_t total_sixteens = 0;
    size_t limit = len - (len % 64); 

    for (size_t i = 0; i < limit; i += 64) {
        simde__m256i d[16];
        for(int j=0; j<16; j++) d[j] = simde_mm256_loadu_si256(&data[i + j*4]);
        
        HARLEY_SEAL_TREE(csa_avx2, d);
        total_sixteens += popcount_256(sixteens);
    }

    uint64_t total = total_sixteens * 16;
    total += popcount_256(eights) * 8;
    total += popcount_256(fours) * 4;
    total += popcount_256(twos) * 2;
    total += popcount_256(ones) * 1;

    for (size_t i = limit; i < len; i++) total += __builtin_popcountll(data[i]);
    return total;
}

/* ========================================================================= */
/*                         DIRECT HARDWARE POPCOUNT                          */
/* ========================================================================= */

uint64_t popcount_avx512_intrinsic(const uint64_t* data, size_t len) {
    simde__m512i sum_vec = simde_mm512_setzero_si512();
    size_t limit = len - (len % 8);

    for (size_t i = 0; i < limit; i += 8) {
        simde__m512i v = simde_mm512_loadu_si512(&data[i]);
        simde__m512i p = simde_mm512_popcnt_epi64(v);
        sum_vec = simde_mm512_add_epi64(sum_vec, p);
    }

    uint64_t total = sum_lanes_512(sum_vec);
    for (size_t i = limit; i < len; i++) total += __builtin_popcountll(data[i]);
    
    return total;
}

uint64_t popcount_scalar(const uint64_t* data, size_t len) {
    uint64_t total = 0;
    for (size_t i = 0; i < len; i++) total += __builtin_popcountll(data[i]);
    return total;
}

/* ========================================================================= */
/*                              SCALAR FALLBACK                              */
/* ========================================================================= */

// Disable GCC auto-vectorization (ignoring Clang and Intel which define __GNUC__)
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
__attribute__((optimize("no-tree-vectorize")))
#endif
uint64_t popcount_scalar_guarded(const uint64_t* data, size_t len) {
    uint64_t total = 0;
    
    // Disable auto-vectorization for Clang
    #if defined(__clang__) && !defined(__INTEL_LLVM_COMPILER)
    #pragma clang loop vectorize(disable) interleave(disable)
    #endif

    // Disable auto-vectorization for Intel Compilers (Classic ICC and Modern ICX)
    #if defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)
    #pragma novector
    #endif

    for (size_t i = 0; i < len; i++) {
        total += __builtin_popcountll(data[i]);
        
        // Universal bulletproof compiler barrier:
        // Works across GCC, Clang, and ICX to break loop-carried dependencies 
        // that auto-vectorizers rely on, without generating any machine code.
        __asm__ volatile ("" : "+g" (total));
    }
    return total;
}

/* ========================================================================= */
/*                          DATA GENERATION & STATS                          */
/* ========================================================================= */

void set_random_bits(uint64_t* data, size_t len, double percentage) {
    size_t total_bits = len * 64;
    size_t bits_to_set = (size_t)(total_bits * (percentage / 100.0));

    int invert = 0;
    if (percentage > 50.0) {
        invert = 1;
        bits_to_set = total_bits - bits_to_set;
    }

    memset(data, 0, len * sizeof(uint64_t));

    for(size_t i = 0; i < bits_to_set; i++) {
        while(1) {
            uint64_t idx = ((uint64_t)(rand() & 0xFF) << 56) |
                           ((uint64_t)(rand() & 0xFF) << 48) |
                           ((uint64_t)(rand() & 0xFF) << 40) |
                           ((uint64_t)(rand() & 0xFF) << 32) |
                           ((uint64_t)(rand() & 0xFFFF) << 16) |
                           ((uint64_t)rand() & 0xFFFF);
            idx %= total_bits;
            size_t word_idx = idx / 64;
            size_t bit_idx = idx % 64;
            if ((data[word_idx] & (1ULL << bit_idx)) == 0) {
                data[word_idx] |= (1ULL << bit_idx);
                break;
            }
        }
    }

    if (invert) {
        for(size_t i = 0; i < len; i++) data[i] = ~data[i];
    }
}

void print_stats(const char* name, double* times, int N, double scalar_mean) {
    double sum = 0;
    for(int i = 0; i < N; i++) sum += times[i];
    double mean = sum / N;

    double var_sum = 0;
    for(int i = 0; i < N; i++) var_sum += (times[i] - mean) * (times[i] - mean);
    double stddev = sqrt(var_sum / N);

    double ratio = scalar_mean / mean;
    printf(format_string, name, mean, stddev, ratio);
}


int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Usage: %s <length_in_words> <percentage_ones> <N_repetitions>\n", argv[0]);
        return 1;
    }

    size_t len = strtoull(argv[1], NULL, 10);
    double pct = atof(argv[2]);
    int N = atoi(argv[3]);

    srand(time(NULL));

    uint64_t* vec1 = aligned_alloc(64, len * sizeof(uint64_t));

    printf("Generating data...\n");
    set_random_bits(vec1, len, pct);
    
    printf("Verifying correctness of vectorized methods...\n");
    
    uint64_t expected_res = popcount_scalar(vec1, len);
    uint64_t scalar_guarded = popcount_scalar_guarded(vec1,len);
    uint64_t res_avx2 = popcount_avx2(vec1, len);
    uint64_t res_avx512_notern = popcount_avx512(vec1, len, 0);
    uint64_t res_avx512_tern = popcount_avx512(vec1, len, 1);
    uint64_t res_avx512_vpopcnt = popcount_avx512_intrinsic(vec1, len);
    uint64_t res_libpopcnt = (uint64_t)popcnt(vec1,len* sizeof(uint64_t));

    if (expected_res != res_avx2) {
        printf("Error: AVX2 result (%llu) does not match Scalar (%llu)\n", (unsigned long long)res_avx2, (unsigned long long)expected_res);
        return 1;
    }
    if (expected_res != scalar_guarded) {
        printf("Error: Guarded scalar result (%llu) does not match Scalar (%llu)\n", (unsigned long long)scalar_guarded, (unsigned long long)expected_res);
        return 1;
    }
    if (expected_res != res_avx512_notern) {
        printf("Error: AVX512 No Ternary result (%llu) does not match Scalar (%llu)\n", (unsigned long long)res_avx512_notern, (unsigned long long)expected_res);
        return 1;
    }
    if (expected_res != res_avx512_tern) {
        printf("Error: AVX512 Ternary result (%llu) does not match Scalar (%llu)\n", (unsigned long long)res_avx512_tern, (unsigned long long)expected_res);
        return 1;
    }
    if (expected_res != res_avx512_vpopcnt) {
        printf("Error: AVX512 VPOPCNTDQ result (%llu) does not match Scalar (%llu)\n", (unsigned long long)res_avx512_vpopcnt, (unsigned long long)expected_res);
        return 1;
    }
    if (expected_res != res_libpopcnt) {
        printf("Error: LIBPOPCNT result (%llu) does not match Scalar (%llu)\n", (unsigned long long)res_libpopcnt, (unsigned long long)expected_res);
        return 1;
    }
    printf("Verification passed. All methods yield the same result.\n\n");
    
    double times_scalar[N];
    double times_scalar_guarded[N];
    double times_avx2[N];
    double times_avx512_notern[N];
    double times_avx512_tern[N];
    double times_avx512_vpopcnt[N];
    double times_libpopcnt[N];

    volatile uint64_t res;
    struct timespec start, end;

    printf("Running benchmarks...\n");

    for (int rep = 0; rep < N; rep++) {
        // Scalar
        clock_gettime(CLOCK_MONOTONIC, &start);
        res = popcount_scalar(vec1, len);
        clock_gettime(CLOCK_MONOTONIC, &end);
        times_scalar[rep] = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        
        // Scalar guarded
        clock_gettime(CLOCK_MONOTONIC, &start);
        res = popcount_scalar_guarded(vec1, len) ;
        clock_gettime(CLOCK_MONOTONIC, &end);
        times_scalar_guarded[rep] = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

        // AVX2
        clock_gettime(CLOCK_MONOTONIC, &start);
        res = popcount_avx2(vec1, len) ;
        clock_gettime(CLOCK_MONOTONIC, &end);
        times_avx2[rep] = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

        // AVX512 No Ternary
        clock_gettime(CLOCK_MONOTONIC, &start);
        res = popcount_avx512(vec1, len, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);
        times_avx512_notern[rep] = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

        // AVX512 Ternary
        clock_gettime(CLOCK_MONOTONIC, &start);
        res = popcount_avx512(vec1, len, 1);
        clock_gettime(CLOCK_MONOTONIC, &end);
        times_avx512_tern[rep] = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

        // AVX512 VPOPCNTDQ
        clock_gettime(CLOCK_MONOTONIC, &start);
        res = popcount_avx512_intrinsic(vec1, len) ;
        clock_gettime(CLOCK_MONOTONIC, &end);
        times_avx512_vpopcnt[rep] = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        
        // libpopcnt
        clock_gettime(CLOCK_MONOTONIC, &start);
        res = (uint64_t)popcnt(vec1, len* sizeof(uint64_t)) ;
        clock_gettime(CLOCK_MONOTONIC, &end);
        times_libpopcnt[rep] = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    }

    FILE* fp = fopen("timings.csv", "w");
    if (fp) {
        fprintf(fp, "Repetition,ScalarGuarded_ns,Scalar_ns,AVX2_ns,AVX512_NoTern_ns,AVX512_Tern_ns,AVX512_Vpopcnt_ns\n");
        for (int i = 0; i < N; i++) {
            fprintf(fp, "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", i+1,times_scalar_guarded[i], times_scalar[i], times_avx2[i], times_avx512_notern[i], times_avx512_tern[i], times_avx512_vpopcnt[i],times_libpopcnt[i]);
        }
        fclose(fp);
    }

    double scalar_sum = 0;
    for(int i = 0; i < N; i++) scalar_sum += times_scalar_guarded[i];
    double scalar_mean = scalar_sum / N;

    printf("\n");
    printf(format_string, "Method", "Mean (ns)", "StdDev (ns)", "Speedup");
    printf("--------------------------+---------------+---------------+----------\n");
    print_stats("Scalar (Guarded)", times_scalar_guarded, N, scalar_mean);
    print_stats("Scalar (Compiler -O3)", times_scalar, N, scalar_mean);
    print_stats("AVX2", times_avx2, N, scalar_mean);
    print_stats("AVX-512 (No Tern)", times_avx512_notern, N, scalar_mean);
    print_stats("AVX-512 (Ternary)", times_avx512_tern, N, scalar_mean);
    print_stats("AVX-512 (VPOPCNTDQ)", times_avx512_vpopcnt, N, scalar_mean);
    print_stats("LIBPOPCNT", times_libpopcnt, N, scalar_mean);
    free(vec1);
    return 0;
}
