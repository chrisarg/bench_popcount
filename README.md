# Simple benchmark of various apporahces to compute the popcount using portable intrinsics (library SIMDe) or libpopcnt.
Compile with clang as: clang -O3 -march=native popcount_bench.c -o popcount_bench -lm

## Results

We will test guarded and non guarded scalar versions of the code, AVX2 Harley Searle based on AVX2, AVX512 (2 varieties, one involving the _mm512_ternarylogic_epi64 _ternary_ logic, another that does not use said logic), AVX-512 population counts and the implementation in the library libpopcnt. The library SIMDe (SIMD everywhere) is used to emulate all vectorized instructions, even in processors that do not use them.

# i9-7900x

This is an AVX512 platform, but lacks the VPOPCNTDQ 512 bit popcount instruction

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       8545.68 |       1859.09 |     1.00x
Scalar (Compiler -O3)     |       8553.24 |       1946.32 |     1.00x
AVX1 128-bit SIMDE        |       3683.50 |       1214.34 |     2.32x
AVX2                      |       2122.15 |        789.24 |     4.03x
AVX-512 (No Tern)         |       1521.02 |        646.60 |     5.62x
AVX-512 (Ternary)         |       1258.75 |        440.42 |     6.79x
AVX-512 (VPOPCNTDQ)       |       2263.98 |        778.94 |     3.77x
NEON SIMDE                |       3566.76 |       1006.52 |     2.40x
LIBPOPCNT                 |       2019.83 |        887.43 |     4.23x
```


```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      28123.12 |       2248.07 |     1.00x
Scalar (Compiler -O3)     |       3562.12 |        878.70 |     7.90x
AVX1 128-bit SIMDE        |       3902.14 |        659.02 |     7.21x
AVX2                      |       2376.35 |        754.80 |    11.83x
AVX-512 (No Tern)         |       1461.99 |        463.66 |    19.24x
AVX-512 (Ternary)         |        994.91 |        330.36 |    28.27x
AVX-512 (VPOPCNTDQ)       |       2124.63 |        525.78 |    13.24x
NEON SIMDE                |       4093.10 |        759.29 |     6.87x
LIBPOPCNT                 |       2235.95 |        566.45 |    12.58x
```


```bash
make -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       9099.37 |       1202.20 |     1.00x
Scalar (Compiler -O3)     |       3844.24 |        780.35 |     2.37x
AVX1 128-bit SIMDE        |       3951.64 |        693.88 |     2.30x
AVX2                      |       2326.32 |        638.84 |     3.91x
AVX-512 (No Tern)         |       1466.73 |        556.95 |     6.20x
AVX-512 (Ternary)         |        975.29 |        443.10 |     9.33x
AVX-512 (VPOPCNTDQ)       |       2111.42 |        458.01 |     4.31x
NEON SIMDE                |       3865.99 |        722.72 |     2.35x
LIBPOPCNT                 |       2219.59 |        521.34 |     4.10x
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


## i3-12100t

This is an AVX512 platform with hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG, ICX


## rockpro64 (Rockchip RK3399, 2 x A-72 and 2 x A 53)

This is an AVX512 platform with hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG, ICX