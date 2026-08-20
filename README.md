# Simple benchmark of various apporahces to compute the popcount using portable intrinsics (library SIMDe) or libpopcnt.
Compile with clang as: clang -O3 -march=native popcount_bench.c -o popcount_bench -lm

## Results

We will test guarded and non guarded scalar versions of the code, AVX2 Harley Searle based on AVX2, AVX512 (2 varieties, one involving the _mm512_ternarylogic_epi64 _ternary_ logic, another that does not use said logic), AVX-512 population counts and the implementation in the library libpopcnt. The library SIMDe (SIMD everywhere) is used to emulate all vectorized instructions, even in processors that do not use them.

# i9-7900x

This is an AVX512 platform, but lacks the VPOPCNTDQ 512 bit popcount instruction

Benchmarks for GCC, CLANG, ICX

```bash
ake -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       8627.07 |       2196.10 |     1.00x
Scalar (Compiler -O3)     |       8630.38 |       1985.24 |     1.00x
AVX1 128-bit SIMDE        |       3727.37 |       1224.76 |     2.31x
AVX2                      |       2127.89 |        761.90 |     4.05x
AVX-512 (No Tern)         |       1518.11 |        690.84 |     5.68x
AVX-512 (Ternary)         |       1253.98 |        647.97 |     6.88x
AVX-512 (VPOPCNTDQ)       |       2326.67 |       1046.61 |     3.71x
NEON SIMDE                |       3609.23 |       1174.15 |     2.39x
LIBPOPCNT                 |       2040.85 |        910.60 |     4.23x
```


```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |      28588.58 |       3048.90 |     1.00x
Scalar (Compiler -O3)     |       3511.25 |        958.60 |     8.14x
AVX1 128-bit SIMDE        |       3982.67 |       1074.71 |     7.18x
AVX2                      |       2362.90 |        665.11 |    12.10x
AVX-512 (No Tern)         |       1482.43 |        624.33 |    19.28x
AVX-512 (Ternary)         |        976.92 |        431.62 |    29.26x
AVX-512 (VPOPCNTDQ)       |       2143.89 |        706.20 |    13.33x
NEON SIMDE                |       4148.27 |        935.82 |     6.89x
LIBPOPCNT                 |       2266.74 |        866.08 |    12.61x
```


```bash
ake -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |     Mean (ns) |   StdDev (ns) |  Speedup
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       9332.12 |       1605.86 |     1.00x
Scalar (Compiler -O3)     |       3967.16 |       1035.81 |     2.35x
AVX1 128-bit SIMDE        |       4026.51 |       1028.59 |     2.32x
AVX2                      |       2407.10 |        964.75 |     3.88x
AVX-512 (No Tern)         |       1470.47 |        505.52 |     6.35x
AVX-512 (Ternary)         |       1007.02 |        525.54 |     9.27x
AVX-512 (VPOPCNTDQ)       |       2169.98 |        875.06 |     4.30x
NEON SIMDE                |       3930.46 |       1042.07 |     2.37x
LIBPOPCNT                 |       2263.82 |        744.64 |     4.12x
```

## i7-11700

This is an AVX512 platform with hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6203.07 |       1168.92 |     1.00x
Scalar (Compiler -O3)     |       1496.93 |        686.12 |     4.14x
AVX1 128-bit SIMDE        |       2848.97 |        603.54 |     2.18x
AVX2                      |       1592.88 |        435.87 |     3.89x
AVX-512 (No Tern)         |       1131.89 |        580.77 |     5.48x
AVX-512 (Ternary)         |        811.10 |        371.81 |     7.65x
AVX-512 (VPOPCNTDQ)       |        738.04 |        249.13 |     8.40x
NEON SIMDE                |       2899.56 |        795.05 |     2.14x
LIBPOPCNT                 |        568.19 |        315.61 |    10.92x
```


```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       5614.70 |       1142.58 |     1.00x
Scalar (Compiler -O3)     |        874.92 |         52.15 |     6.42x
AVX1 128-bit SIMDE        |       3041.19 |        343.77 |     1.85x
AVX2                      |       1608.01 |        388.76 |     3.49x
AVX-512 (No Tern)         |       1070.74 |        263.87 |     5.24x
AVX-512 (Ternary)         |        621.66 |        189.22 |     9.03x
AVX-512 (VPOPCNTDQ)       |        572.81 |         36.24 |     9.80x
NEON SIMDE                |       3048.08 |        323.18 |     1.84x
LIBPOPCNT                 |        561.13 |         24.24 |    10.01x
```


```bash
make -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6716.39 |        574.19 |     1.00x
Scalar (Compiler -O3)     |       1370.54 |        219.78 |     4.90x
AVX1 128-bit SIMDE        |       3037.45 |        417.81 |     2.21x
AVX2                      |       1496.25 |        351.39 |     4.49x
AVX-512 (No Tern)         |       1071.88 |        319.41 |     6.27x
AVX-512 (Ternary)         |        620.76 |        136.97 |    10.82x
AVX-512 (VPOPCNTDQ)       |        574.94 |         24.58 |    11.68x
NEON SIMDE                |       3055.23 |        320.84 |     2.20x
LIBPOPCNT                 |        609.95 |        224.37 |    11.01x
```


## i3-12100t

This is an AVX512 platform with hardware level VPOPCNTDQ.

Benchmarks for GCC, CLANG, ICX

```bash
make -s clean && make CC=gcc >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       4093.80 |        289.86 |     1.00x
Scalar (Compiler -O3)     |       4114.90 |        386.05 |     0.99x
AVX1 128-bit SIMDE        |       3871.53 |        127.50 |     1.06x
AVX2                      |       2166.36 |         70.51 |     1.89x
AVX-512 (No Tern)         |       2535.22 |         87.82 |     1.61x
AVX-512 (Ternary)         |       7733.18 |        255.72 |     0.53x
AVX-512 (VPOPCNTDQ)       |       3885.22 |        145.41 |     1.05x
NEON SIMDE                |       3887.83 |        118.66 |     1.05x
LIBPOPCNT                 |       2121.60 |         80.15 |     1.93x
```

```bash
make -s clean && make CC=clang >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6441.48 |        275.23 |     1.00x
Scalar (Compiler -O3)     |       2770.43 |        112.68 |     2.33x
AVX1 128-bit SIMDE        |       3994.71 |        163.75 |     1.61x
AVX2                      |       2286.26 |         99.37 |     2.82x
AVX-512 (No Tern)         |       2038.11 |        137.38 |     3.16x
AVX-512 (Ternary)         |       3808.27 |        189.54 |     1.69x
AVX-512 (VPOPCNTDQ)       |       2769.23 |        110.93 |     2.33x
NEON SIMDE                |       3999.26 |        159.82 |     1.61x
LIBPOPCNT                 |       2029.72 |         85.73 |     3.17x
```

```bash
make -s clean && make CC=icx >/dev/null 2>&1 && ./popcount_bench 16384 20 10000
Generating data...
Verifying correctness of vectorized methods...
Verification passed. All methods yield the same result.

Running benchmarks...

Method                    |          0.00 |          0.00 |     0.00x
--------------------------+---------------+---------------+----------
Scalar (Guarded)          |       6393.70 |        289.11 |     1.00x
Scalar (Compiler -O3)     |       2433.86 |        119.47 |     2.63x
AVX1 128-bit SIMDE        |       4000.80 |        195.51 |     1.60x
AVX2                      |       2320.37 |        110.26 |     2.76x
AVX-512 (No Tern)         |       1989.92 |        112.48 |     3.21x
AVX-512 (Ternary)         |       1900.11 |        100.74 |     3.36x
AVX-512 (VPOPCNTDQ)       |       2781.57 |        135.31 |     2.30x
NEON SIMDE                |       4006.30 |        174.14 |     1.60x
LIBPOPCNT                 |       2034.79 |        107.24 |     3.14x
```

## rockpro64 (Rockchip RK3399, 2 x A-72 and 2 x A 53)

This is a Neon platform with hardware level vcntq_u8.

Benchmarks for GCC, CLANG


## rock64 (Rockchip RK3328 4 x A53)

This is a Neon platform with hardware level vcntq_u8.

Benchmarks for GCC, CLANG