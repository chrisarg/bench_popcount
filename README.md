# Simple benchmark of various apporahces to compute the popcount using portable intrinsics (library SIMDe) or libpopcnt.
Compile with clang as: clang -O3 -march=native popcount_bench.c -o popcount_bench -lm

## Results

We will test guarded and non guarded scalar versions of the code, AVX2 Harley Searle based on AVX2, AVX512 (2 varieties, one involving the _mm512_ternarylogic_epi64 _ternary_ logic, another that does not use said logic), AVX-512 population counts and the implementation in the library libpopcnt. The library SIMDe (SIMD everywhere) is used to emulate all vectorized instructions, even in processors that do not use them.

# i9-7900x

This is an AVX512 platform, but lacks the VPOPCNTDQ 512 bit popcount instruction

Benchmarks for GCC, CLANG, ICX

```bash
gcc -O3 -march=native popcount_bench.c -o popcount_bench -lm && ./popcount_bench 16384 30 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6958.30 |       2654.11 |     1.00x
Scalar (Compiler -O3)     |       6975.29 |       2425.00 |     1.00x
AVX2                      |       2240.93 |       1120.05 |     3.11x
AVX-512 (No Tern)         |       1590.64 |        969.78 |     4.37x
AVX-512 (Ternary)         |       1313.67 |        912.24 |     5.30x
AVX-512 (VPOPCNTDQ)       |       2417.79 |       1148.61 |     2.88x
LIBPOPCNT                 |       2118.59 |       1020.78 |     3.28x
```


```bash
clang -O3 -march=native popcount_bench.c -o popcount_bench -lm && ./popcount_bench 16384 30 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      29360.14 |       4452.07 |     1.00x
Scalar (Compiler -O3)     |       3759.48 |       1355.21 |     7.81x
AVX2                      |       2511.68 |       1182.72 |    11.69x
AVX-512 (No Tern)         |       1532.55 |        707.58 |    19.16x
AVX-512 (Ternary)         |       1078.76 |        800.40 |    27.22x
AVX-512 (VPOPCNTDQ)       |       2249.14 |       1060.73 |    13.05x
LIBPOPCNT                 |       2353.94 |        992.73 |    12.47x
```


```bash
icx -O3 -march=native popcount_bench.c -o popcount_bench -lm && ./popcount_bench 16384 30 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       9963.76 |       3509.41 |     1.00x
Scalar (Compiler -O3)     |       2446.11 |       1218.52 |     4.07x
AVX2                      |       2531.47 |       1082.99 |     3.94x
AVX-512 (No Tern)         |       1563.76 |       1205.51 |     6.37x
AVX-512 (Ternary)         |       1079.28 |        702.24 |     9.23x
AVX-512 (VPOPCNTDQ)       |       2294.69 |       1205.33 |     4.34x
LIBPOPCNT                 |       2413.61 |       1158.65 |     4.13x
```

## i7-11700

This is an AVX512 platform with hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG, ICX

```bash
gcc -O3 -march=native popcount_bench.c -o popcount_bench -lm && ./popcount_bench 16384 30 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6345.57 |       1577.85 |     1.00x
Scalar (Compiler -O3)     |       1469.37 |        690.01 |     4.32x
AVX2                      |       1670.52 |        598.87 |     3.80x
AVX-512 (No Tern)         |       1160.08 |        527.16 |     5.47x
AVX-512 (Ternary)         |        847.44 |        512.17 |     7.49x
AVX-512 (VPOPCNTDQ)       |        777.88 |        517.32 |     8.16x
LIBPOPCNT                 |        593.82 |        520.90 |    10.69x
```


```bash
clang -O3 -march=native popcount_bench.c -o popcount_bench -lm && ./popcount_bench 16384 30 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6215.90 |       1928.99 |     1.00x
Scalar (Compiler -O3)     |        940.07 |        530.76 |     6.61x
AVX2                      |       1764.52 |        922.85 |     3.52x
AVX-512 (No Tern)         |       1148.62 |        752.82 |     5.41x
AVX-512 (Ternary)         |        667.90 |        435.47 |     9.31x
AVX-512 (VPOPCNTDQ)       |        622.69 |        538.95 |     9.98x
LIBPOPCNT                 |        602.49 |        392.81 |    10.32x
```


```bash
icx -O3 -march=native popcount_bench.c -o popcount_bench -lm && ./popcount_bench 16384 30 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       7147.58 |       1735.08 |     1.00x
Scalar (Compiler -O3)     |        791.11 |        506.33 |     9.03x
AVX2                      |       1662.49 |        876.45 |     4.30x
AVX-512 (No Tern)         |       1164.73 |        728.80 |     6.14x
AVX-512 (Ternary)         |        679.47 |        531.82 |    10.52x
AVX-512 (VPOPCNTDQ)       |        620.83 |        406.38 |    11.51x
LIBPOPCNT                 |        665.22 |        578.70 |    10.74x
```