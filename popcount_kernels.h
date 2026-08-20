#pragma once

#include <stddef.h>
#include <stdint.h>

// Function Prototypes
uint64_t popcount_scalar(const uint64_t *data, size_t len);
uint64_t popcount_scalar_guarded(const uint64_t *data, size_t len);
uint64_t popcount_avx1_128_simde(const uint64_t *data, size_t len);
uint64_t popcount_avx2(const uint64_t *data, size_t len);
uint64_t popcount_avx512(const uint64_t *data, size_t len, int use_ternary);
uint64_t popcount_avx512_intrinsic(const uint64_t *data, size_t len);
uint64_t popcount_libpopcnt_wrapper(const uint64_t *data, size_t len);
uint64_t popcount_neon_simde(const uint64_t *data, size_t len);