#include "popcount_kernels.h"

#include <stddef.h>
#include <stdint.h>

// SIMDe vector intrinsics headers (ONLY needed here!)
#include "simde/simde-arch.h"
#include "simde/simde-common.h"
#include "simde/simde-features.h"
#include "simde/x86/avx2.h"
#include "simde/x86/avx512.h"
#include "simde/x86/avx512/popcnt.h"
#include "simde/x86/sse2.h"
#include <simde/arm/neon.h>

// Third-party library header (if included in kernels)
#include "libpopcnt.h"

/* ========================================================================= */
/*                           CARRY-SAVE ADDERS                               */
/* ========================================================================= */

// 1. AVX-512 with Ternary Logic
static inline void csa_avx512_ternary(simde__m512i *h, simde__m512i *l,
                                      simde__m512i a, simde__m512i b,
                                      simde__m512i c) {
  *l = simde_mm512_ternarylogic_epi64(a, b, c, 0x96);
  *h = simde_mm512_ternarylogic_epi64(a, b, c, 0xE8);
}

// 2. AVX-512 without Ternary Logic
static inline void csa_avx512_notern(simde__m512i *h, simde__m512i *l,
                                     simde__m512i a, simde__m512i b,
                                     simde__m512i c) {
  simde__m512i u = simde_mm512_xor_si512(a, b);
  *h = simde_mm512_or_si512(simde_mm512_and_si512(a, b),
                            simde_mm512_and_si512(u, c));
  *l = simde_mm512_xor_si512(u, c);
}

// 3. AVX2 logic
static inline void csa_avx2(simde__m256i *h, simde__m256i *l, simde__m256i a,
                            simde__m256i b, simde__m256i c) {
  simde__m256i u = simde_mm256_xor_si256(a, b);
  *h = simde_mm256_or_si256(simde_mm256_and_si256(a, b),
                            simde_mm256_and_si256(u, c));
  *l = simde_mm256_xor_si256(u, c);
}

// 4. AVX / SSE (128-bit)
static inline void csa_128_simde(simde__m128i *h, simde__m128i *l,
                                 simde__m128i a, simde__m128i b,
                                 simde__m128i c) {
  simde__m128i u = simde_mm_xor_si128(a, b);
  *h = simde_mm_or_si128(simde_mm_and_si128(a, b), simde_mm_and_si128(u, c));
  *l = simde_mm_xor_si128(u, c);
}

// 5. Neon (ARM Advanced SIMD)
static inline void csa_neon_simde(simde_uint64x2_t *h, simde_uint64x2_t *l,
                                  simde_uint64x2_t a, simde_uint64x2_t b,
                                  simde_uint64x2_t c) {
  // veorq = Vector Exclusive OR Quadword (128-bit XOR)
  simde_uint64x2_t u = simde_veorq_u64(a, b);

  // vandq = Vector AND Quadword, vorrq = Vector OR Quadword
  *h = simde_vorrq_u64(simde_vandq_u64(a, b), simde_vandq_u64(u, c));
  *l = simde_veorq_u64(u, c);
}

/* ========================================================================= */
/*                          VECTOR POPCOUNT HELPERS                          */
/* ========================================================================= */

static inline uint64_t popcount_512(simde__m512i v) {
  uint64_t val[8];
  simde_mm512_storeu_si512((simde__m512i *)val, v);
  uint64_t sum = 0;
  for (int i = 0; i < 8; i++)
    sum += __builtin_popcountll(val[i]);
  return sum;
}

static inline uint64_t popcount_256(simde__m256i v) {
  uint64_t val[4];
  simde_mm256_storeu_si256((simde__m256i *)val, v);
  uint64_t sum = 0;
  for (int i = 0; i < 4; i++)
    sum += __builtin_popcountll(val[i]);
  return sum;
}

static inline uint64_t sum_lanes_512(simde__m512i v) {
  uint64_t val[8];
  simde_mm512_storeu_si512((simde__m512i *)val, v);
  uint64_t sum = 0;
  for (int i = 0; i < 8; i++)
    sum += val[i];
  return sum;
}

static inline uint64_t popcount_128_simde(simde__m128i v) {
  uint64_t val[2];
  simde_mm_storeu_si128((simde__m128i *)val, v);
  return __builtin_popcountll(val[0]) + __builtin_popcountll(val[1]);
}

static inline uint64_t popcount_128_neon_simde(simde_uint64x2_t v) {
    uint64_t val[2];
    simde_vst1q_u64(val, v); // Store vector to memory
    return __builtin_popcountll(val[0]) + __builtin_popcountll(val[1]);
}
/* ========================================================================= */
/*                         HARLEY-SEAL IMPLEMENTATIONS                       */
/* ========================================================================= */

#define HARLEY_SEAL_TREE(CSA_FUNC, d)                                          \
  CSA_FUNC(&twosA, &ones, ones, d[0], d[1]);                                   \
  CSA_FUNC(&twosB, &ones, ones, d[2], d[3]);                                   \
  CSA_FUNC(&foursA, &twos, twos, twosA, twosB);                                \
  CSA_FUNC(&twosA, &ones, ones, d[4], d[5]);                                   \
  CSA_FUNC(&twosB, &ones, ones, d[6], d[7]);                                   \
  CSA_FUNC(&foursB, &twos, twos, twosA, twosB);                                \
  CSA_FUNC(&eightsA, &fours, fours, foursA, foursB);                           \
  CSA_FUNC(&twosA, &ones, ones, d[8], d[9]);                                   \
  CSA_FUNC(&twosB, &ones, ones, d[10], d[11]);                                 \
  CSA_FUNC(&foursA, &twos, twos, twosA, twosB);                                \
  CSA_FUNC(&twosA, &ones, ones, d[12], d[13]);                                 \
  CSA_FUNC(&twosB, &ones, ones, d[14], d[15]);                                 \
  CSA_FUNC(&foursB, &twos, twos, twosA, twosB);                                \
  CSA_FUNC(&eightsB, &fours, fours, foursA, foursB);                           \
  CSA_FUNC(&sixteens, &eights, eights, eightsA, eightsB);

uint64_t popcount_avx512(const uint64_t *data, size_t len, int use_ternary) {
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
    for (int j = 0; j < 16; j++)
      d[j] = simde_mm512_loadu_si512(&data[i + j * 8]);

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

  for (size_t i = limit; i < len; i++)
    total += __builtin_popcountll(data[i]);
  return total;
}

uint64_t popcount_avx2(const uint64_t *data, size_t len) {
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
    for (int j = 0; j < 16; j++)
      d[j] = simde_mm256_loadu_si256(&data[i + j * 4]);

    HARLEY_SEAL_TREE(csa_avx2, d);
    total_sixteens += popcount_256(sixteens);
  }

  uint64_t total = total_sixteens * 16;
  total += popcount_256(eights) * 8;
  total += popcount_256(fours) * 4;
  total += popcount_256(twos) * 2;
  total += popcount_256(ones) * 1;

  for (size_t i = limit; i < len; i++)
    total += __builtin_popcountll(data[i]);
  return total;
}

uint64_t popcount_avx1_128_simde(const uint64_t *data, size_t len) {
  simde__m128i ones = simde_mm_setzero_si128();
  simde__m128i twos = simde_mm_setzero_si128();
  simde__m128i fours = simde_mm_setzero_si128();
  simde__m128i eights = simde_mm_setzero_si128();
  simde__m128i sixteens = simde_mm_setzero_si128();
  simde__m128i twosA, twosB, foursA, foursB, eightsA, eightsB;

  uint64_t total_sixteens = 0;

  // Process 16 vectors of 128 bits (2 uint64_t words per vector = 32
  // words/iteration)
  size_t limit = len - (len % 32);

  for (size_t i = 0; i < limit; i += 32) {
    simde__m128i d[16];
    for (int j = 0; j < 16; j++) {
      // Load 128 bits (2 x uint64_t) via VEX-encoded vmovdqu
      d[j] = simde_mm_loadu_si128((const simde__m128i *)&data[i + j * 2]);
    }

    // Harley-Seal Carry-Save Adder Tree using 128-bit VEX integer ops
    HARLEY_SEAL_TREE(csa_128_simde, d);

    total_sixteens += popcount_128_simde(sixteens);
  }

  // Combine weight-accumulated layers
  uint64_t total = total_sixteens * 16;
  total += popcount_128_simde(eights) * 8;
  total += popcount_128_simde(fours) * 4;
  total += popcount_128_simde(twos) * 2;
  total += popcount_128_simde(ones) * 1;

  // Tail processing
  for (size_t i = limit; i < len; i++) {
    total += __builtin_popcountll(data[i]);
  }

  return total;
}

/* ========================================================================= */
/*                         DIRECT HARDWARE POPCOUNT                          */
/* ========================================================================= */

uint64_t popcount_avx512_intrinsic(const uint64_t *data, size_t len) {
  simde__m512i sum_vec = simde_mm512_setzero_si512();
  size_t limit = len - (len % 8);

  for (size_t i = 0; i < limit; i += 8) {
    simde__m512i v = simde_mm512_loadu_si512(&data[i]);
    simde__m512i p = simde_mm512_popcnt_epi64(v);
    sum_vec = simde_mm512_add_epi64(sum_vec, p);
  }

  uint64_t total = sum_lanes_512(sum_vec);
  for (size_t i = limit; i < len; i++)
    total += __builtin_popcountll(data[i]);

  return total;
}

uint64_t popcount_scalar(const uint64_t *data, size_t len) {
  uint64_t total = 0;
  for (size_t i = 0; i < len; i++)
    total += __builtin_popcountll(data[i]);
  return total;
}

/* ========================================================================= */
/*                              SCALAR FALLBACK                              */
/* ========================================================================= */

// Disable GCC auto-vectorization (ignoring Clang and Intel which define
// __GNUC__)
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) &&  \
    !defined(__INTEL_LLVM_COMPILER)
__attribute__((optimize("no-tree-vectorize")))
#endif
uint64_t popcount_scalar_guarded(const uint64_t *data, size_t len) {
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
    __asm__ volatile("" : "+g"(total));
  }
  return total;
}

/* ========================================================================= */
/*                              LIBPOPCNT WRAPPER                            */
/* ========================================================================= */

uint64_t popcount_libpopcnt_wrapper(const uint64_t *data, size_t len) {
  // libpopcnt expects the total size in bytes, not words.
  // The library handles its own CPUID dispatching internally.
  return popcnt(data, len * sizeof(uint64_t));
}

/* ========================================================================= */
/*                       128-BIT NEON HARLEY-SEAL ROUTINE                    */
/* ========================================================================= */

uint64_t popcount_neon_simde(const uint64_t *data, size_t len) {
    simde_uint64x2_t ones     = simde_vdupq_n_u64(0);
    simde_uint64x2_t twos     = simde_vdupq_n_u64(0);
    simde_uint64x2_t fours    = simde_vdupq_n_u64(0);
    simde_uint64x2_t eights   = simde_vdupq_n_u64(0);
    simde_uint64x2_t sixteens = simde_vdupq_n_u64(0);
    simde_uint64x2_t twosA, twosB, foursA, foursB, eightsA, eightsB;

    uint64_t total_sixteens = 0;
    
    // Process 16 vectors of 128 bits (2 uint64_t words per vector = 32 words/iter)
    size_t limit = len - (len % 32);

    for (size_t i = 0; i < limit; i += 32) {
        simde_uint64x2_t d[16];
        for (int j = 0; j < 16; j++) {
            // vld1q = Vector Load 1 Quadword
            d[j] = simde_vld1q_u64(&data[i + j * 2]);
        }
        
        // Re-use the exact same macro, passing the NEON CSA
        HARLEY_SEAL_TREE(csa_neon_simde, d);
        
        total_sixteens += popcount_128_neon_simde(sixteens);
    }

    uint64_t total = total_sixteens * 16;
    total += popcount_128_neon_simde(eights) * 8;
    total += popcount_128_neon_simde(fours)  * 4;
    total += popcount_128_neon_simde(twos)   * 2;
    total += popcount_128_neon_simde(ones)   * 1;

    for (size_t i = limit; i < len; i++) {
        total += __builtin_popcountll(data[i]);
    }
    
    return total;
}