/* 
  build as make CC=<compiler> (e.g., make CC=gcc or make CC=clang)
*/
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

// Include SIMDe libraries
#include "simde/simde-arch.h"
#include "simde/simde-common.h"
#include "simde/simde-features.h"
#include "simde/x86/avx2.h"
#include "simde/x86/avx512.h"
#include "simde/x86/avx512/popcnt.h"

#include "popcount_kernels.h"

char format_string[] = "%-25s | %13.2f | %13.2f | %8.2fx\n";

/* ========================================================================= */
/*                          DATA GENERATION & STATS                          */
/* ========================================================================= */

void set_random_bits(uint64_t *data, size_t len, double percentage) {
  size_t total_bits = len * 64;
  size_t bits_to_set = (size_t)(total_bits * (percentage / 100.0));

  int invert = 0;
  if (percentage > 50.0) {
    invert = 1;
    bits_to_set = total_bits - bits_to_set;
  }

  memset(data, 0, len * sizeof(uint64_t));

  for (size_t i = 0; i < bits_to_set; i++) {
    while (1) {
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
    for (size_t i = 0; i < len; i++)
      data[i] = ~data[i];
  }
}

void print_stats(const char *name, double *times, int N, double scalar_mean) {
  double sum = 0;
  for (int i = 0; i < N; i++)
    sum += times[i];
  double mean = sum / N;

  double var_sum = 0;
  for (int i = 0; i < N; i++)
    var_sum += (times[i] - mean) * (times[i] - mean);
  double stddev = sqrt(var_sum / N);

  double ratio = scalar_mean / mean;
  printf(format_string, name, mean, stddev, ratio);
}

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("Usage: %s <length_in_words> <percentage_ones> <N_repetitions>\n",
           argv[0]);
    return 1;
  }

  size_t len = strtoull(argv[1], NULL, 10);
  double pct = atof(argv[2]);
  int N = atoi(argv[3]);

  srand(time(NULL));

  uint64_t *vec1 = aligned_alloc(64, len * sizeof(uint64_t));

  printf("Generating data...\n");
  set_random_bits(vec1, len, pct);

  printf("Verifying correctness of vectorized methods...\n");

  uint64_t expected_res = popcount_scalar(vec1, len);
  uint64_t scalar_guarded = popcount_scalar_guarded(vec1, len);
  uint64_t res_avx1_128_simde = popcount_avx1_128_simde(vec1, len);
  uint64_t res_avx2 = popcount_avx2(vec1, len);
  uint64_t res_avx512_notern = popcount_avx512(vec1, len, 0);
  uint64_t res_avx512_tern = popcount_avx512(vec1, len, 1);
  uint64_t res_avx512_vpopcnt = popcount_avx512_intrinsic(vec1, len);
  uint64_t res_libpopcnt = (uint64_t)popcount_libpopcnt_wrapper(vec1, len );
  uint64_t res_neon_simde = popcount_neon_simde(vec1, len);

  if (expected_res != res_avx1_128_simde) {
    printf("Error: AVX1 128-bit SIMDE result (%llu) does not match Scalar "
           "(%llu)\n",
           (unsigned long long)res_avx1_128_simde,
           (unsigned long long)expected_res);
    return 1;
  }
  if (expected_res != res_avx2) {
    printf("Error: AVX2 result (%llu) does not match Scalar (%llu)\n",
           (unsigned long long)res_avx2, (unsigned long long)expected_res);
    return 1;
  }
  if (expected_res != scalar_guarded) {
    printf("Error: Guarded scalar result (%llu) does not match Scalar (%llu)\n",
           (unsigned long long)scalar_guarded,
           (unsigned long long)expected_res);
    return 1;
  }
  if (expected_res != res_avx512_notern) {
    printf(
        "Error: AVX512 No Ternary result (%llu) does not match Scalar (%llu)\n",
        (unsigned long long)res_avx512_notern,
        (unsigned long long)expected_res);
    return 1;
  }
  if (expected_res != res_avx512_tern) {
    printf("Error: AVX512 Ternary result (%llu) does not match Scalar (%llu)\n",
           (unsigned long long)res_avx512_tern,
           (unsigned long long)expected_res);
    return 1;
  }
  if (expected_res != res_avx512_vpopcnt) {
    printf(
        "Error: AVX512 VPOPCNTDQ result (%llu) does not match Scalar (%llu)\n",
        (unsigned long long)res_avx512_vpopcnt,
        (unsigned long long)expected_res);
    return 1;
  }
  if (expected_res != res_neon_simde) {
    printf("Error: NEON SIMDE result (%llu) does not match Scalar (%llu)\n",
           (unsigned long long)res_neon_simde, (unsigned long long)expected_res);
    return 1;
  }
  if (expected_res != res_libpopcnt) {
    printf("Error: LIBPOPCNT result (%llu) does not match Scalar (%llu)\n",
           (unsigned long long)res_libpopcnt, (unsigned long long)expected_res);
    return 1;
  }
  printf("Verification passed. All methods yield the same result.\n\n");

  double times_scalar[N];
  double times_scalar_guarded[N];
  double times_avx1_128_simde[N];
  double times_avx2[N];
  double times_avx512_notern[N];
  double times_avx512_tern[N];
  double times_avx512_vpopcnt[N];
  double times_neon_simde[N];
  double times_libpopcnt[N];

  volatile uint64_t res;
  struct timespec start, end;

  printf("Running benchmarks...\n");

  for (int rep = 0; rep < N; rep++) {
    // Scalar
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = popcount_scalar(vec1, len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_scalar[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // Scalar guarded
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = popcount_scalar_guarded(vec1, len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_scalar_guarded[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // AVX1 128-bit SIMDE
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = popcount_avx1_128_simde(vec1, len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_avx1_128_simde[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // AVX2
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = popcount_avx2(vec1, len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_avx2[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // AVX512 No Ternary
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = popcount_avx512(vec1, len, 0);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_avx512_notern[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // AVX512 Ternary
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = popcount_avx512(vec1, len, 1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_avx512_tern[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // AVX512 VPOPCNTDQ
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = popcount_avx512_intrinsic(vec1, len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_avx512_vpopcnt[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // NEON SIMDE
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = popcount_neon_simde(vec1, len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_neon_simde[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // libpopcnt
    clock_gettime(CLOCK_MONOTONIC, &start);
    res = (uint64_t)popcount_libpopcnt_wrapper(vec1, len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    times_libpopcnt[rep] =
        (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
  }

  (void)res;
  FILE *fp = fopen("timings.csv", "w");
  if (fp) {
    fprintf(fp,
            "Repetition,ScalarGuarded_ns,Scalar_ns,AVX1_128_SIMDE_ns,AVX2_ns,"
            "AVX512_NoTern_ns,AVX512_Tern_ns,AVX512_Vpopcnt_ns,NEON_SIMDE_ns,Libpopcnt_ns\n");
    for (int i = 0; i < N; i++) {
      fprintf(fp, "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", i + 1,
              times_scalar_guarded[i], times_scalar[i], times_avx1_128_simde[i],
              times_avx2[i], times_avx512_notern[i], times_avx512_tern[i],
              times_avx512_vpopcnt[i], times_neon_simde[i], times_libpopcnt[i]);
    }
    fclose(fp);
  }

  double scalar_sum = 0;
  for (int i = 0; i < N; i++)
    scalar_sum += times_scalar_guarded[i];
  double scalar_mean = scalar_sum / N;

  printf("\n");
  printf(format_string, "Method", "Mean (ns)", "StdDev (ns)", "Speedup");
  printf("--------------------------+---------------+---------------+----------"
         "\n");
  print_stats("Scalar (Guarded)", times_scalar_guarded, N, scalar_mean);
  print_stats("Scalar (Compiler -O3)", times_scalar, N, scalar_mean);
  print_stats("AVX1 128-bit SIMDE", times_avx1_128_simde, N, scalar_mean);
  print_stats("AVX2", times_avx2, N, scalar_mean);
  print_stats("AVX-512 (No Tern)", times_avx512_notern, N, scalar_mean);
  print_stats("AVX-512 (Ternary)", times_avx512_tern, N, scalar_mean);
  print_stats("AVX-512 (VPOPCNTDQ)", times_avx512_vpopcnt, N, scalar_mean);
  print_stats("NEON SIMDE", times_neon_simde, N, scalar_mean);
  print_stats("LIBPOPCNT", times_libpopcnt, N, scalar_mean);
  free(vec1);
  return 0;
}
