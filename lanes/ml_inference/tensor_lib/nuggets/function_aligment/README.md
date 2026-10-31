# Check loop alignment

Check GCC's default loop-alignment setting for the current machine:

```sh
g++ -Q -O3 --help=optimizers | rg 'falign-loops'
```

Run from `lanes/ml_inference/tensor_lib` after the Release build in the
[main README](../../README.md). These commands use Linux, GCC, and ripgrep.

Docs: https://gcc.gnu.org/onlinedocs/gcc-15.1.0/gcc/Optimize-Options.html#index-falign-loops

## 1. Build before and after

Reuse the Google Benchmark dependency from the existing build:

```sh
bash nuggets/function_aligment/compile.sh
```

Only the second command in the script adds `-falign-loops=32`.

## 2. Look at the addresses

Show the Add, Sub, and Mul function addresses and sizes at 256×256 and
1024×1024 (both columns are hexadecimal).
Ignore entries marked `.cold`.

```sh
nm -S -C build/loop-alignment/before build/loop-alignment/after \
    | rg '(^.*:$|BM_Tensor(Add|Sub|Mul)<(256|1024)ul)'
```

Show the arithmetic loops. Each backward `jne` names its operation and tensor
size, for example `BM_TensorSub<1024ul, 1024ul>`:

```sh
objdump -d -C build/loop-alignment/before | rg -B 3 -A 7 '(addps|subps|mulps)'
objdump -d -C build/loop-alignment/after  | rg -B 3 -A 7 '(addps|subps|mulps)'
```

The backward `jne` jumps to the loop's first instruction. The loop ends just
after that `jne`; in this build, `jne` occupies two bytes.

## 3. Compare

Measured from both executables built with GCC 13.3 on a Ryzen 7 7700.
Addresses may differ in your build.

| Function | Before: address / size | After: address / size |
| --- | --- | --- |
| Add 256×256 | `0xb9f0` / 870 bytes | `0xba20` / 894 bytes |
| Sub 256×256 | `0xcb20` / 870 bytes | `0xcba0` / 894 bytes |
| Mul 256×256 | `0xc7b0` / 870 bytes | `0xc820` / 894 bytes |
| Add 1024×1024 | `0xd9a0` / 870 bytes | `0xda60` / 894 bytes |
| Sub 1024×1024 | `0xbd60` / 870 bytes | `0xbda0` / 894 bytes |
| Mul 1024×1024 | `0xc0d0` / 870 bytes | `0xc120` / 894 bytes |

Each inner arithmetic loop is **28 bytes**. Below, `[start, end)` includes the
backward `jne` and excludes the instruction immediately after it.

| Loop | Before: [start, end) | After: [start, end) | Offset within 64-byte line: before → after | Lines spanned: before → after |
| --- | --- | --- | ---: | --- |
| Add 256×256 | `[0xbcf8, 0xbd14)` | `[0xbd40, 0xbd5c)` | 56 → 0 | **2 → 1** |
| Sub 256×256 | `[0xce28, 0xce44)` | `[0xcec0, 0xcedc)` | 40 → 0 | **2 → 1** |
| Mul 256×256 | `[0xcab8, 0xcad4)` | `[0xcb40, 0xcb5c)` | 56 → 0 | **2 → 1** |
| Add 1024×1024 | `[0xdca8, 0xdcc4)` | `[0xdd80, 0xdd9c)` | 40 → 0 | **2 → 1** |
| Sub 1024×1024 | `[0xc068, 0xc084)` | `[0xc0c0, 0xc0dc)` | 40 → 0 | **2 → 1** |
| Mul 1024×1024 | `[0xc3d8, 0xc3f4)` | `[0xc440, 0xc45c)` | 24 → 0 | 1 → 1 |

Calculate the offset as `start % 64`, then add the 28-byte loop size:

- **256×256 Add and Mul cross a line:** `56 + 28 = 84` before alignment.
- **256×256 Sub and 1024×1024 Add/Sub cross a line:** `40 + 28 = 68` before alignment.
- **1024×1024 Mul already fits:** `24 + 28 = 52`.
- **After 32-byte alignment, all six observed loops fit:** `0 + 28 = 28`.

The `-falign-loops=32` option requests 32-byte loop boundaries; it does not
guarantee a 64-byte cache-line boundary. In this build, the compiler placed all
six observed loops at a cache-line boundary. Increasing the tensor size changes
how many times the loop runs; its code remains 28 bytes.

**These measurements prove whether a loop fits within one cache line.** All
listed functions are also smaller than the CPU's 32 KiB L1 instruction cache.
Addresses alone do not prove cache residency or a cache miss. The PMU script
measures 256×256 Add only; it does not provide separate counters for Sub or the
1024×1024 cases.

## 4. Measure PMU counters

From `lanes/ml_inference/tensor_lib`, run:

```sh
bash nuggets/function_aligment/pmu_counters.sh
```

The script measures `before` and `after` on CPU 0, repeating each run three
times. Each run adds two 256×256 tensors in place 100,000 times: **6,553,600,000
elements**. It uses `sudo` to access hardware counters; enter your password in
your terminal if prompted.

### Recorded PMU results

These results were collected on the Ryzen 7 7700 with GCC 13.3. Values are
per-run averages over three runs of the 256×256 Add case; parentheses show the
reported variation. `:u` counts user-space execution, including startup and
initialization.

| Measurement | Before | After (`-falign-loops=32`) |
| --- | ---: | ---: |
| CPU cycles | 3,364,607,625 (±2.34%) | 1,990,015,795 (±0.02%) |
| Retired instructions | 11,472,070,571 (±0.00%) | 11,472,270,265 (±0.00%) |
| Empty dispatch slots because the front end supplied too few operations | 10,216,636,418 (±3.39%) | 15,786,113 (±12.10%) |
| Instruction-cache misses | 66,976 (±2.74%) | 65,091 (±1.86%) |
| Elapsed time | 0.679901871 s (±8.93%) | 0.375852962 s (±0.04%) |

To make those counts easier to compare:

| Calculated metric | Before | After |
| --- | ---: | ---: |
| Instructions per cycle | 3.41 | 5.76 |
| Empty dispatch slots / available slots | 50.61% | 0.132% |
| Instruction-cache misses per million instructions | 5.84 | 5.67 |
| Cycles per tensor element | 0.513 | 0.304 |

Zen 4 has six dispatch slots per cycle, so the empty-slot percentage is
`empty_slots / (6 × cycles) × 100`. This is a percentage of slots, not execution
time. Misses per million instructions is not the cache miss rate; that would
require the number of cache accesses.

**Alignment used 40.85% fewer cycles for essentially the same instruction work.**
The empty-slot fraction fell from about half to almost zero, strongly supporting
an instruction-delivery bottleneck in the original build. Instruction-cache
misses remained low and changed only slightly, so these results do not support
fewer cache misses as the explanation. They do not isolate the specific fetch or
decode mechanism affected by alignment.

Elapsed time also improved, but the before run had more timing variation
(±8.93%); CPU cycles provide the more stable comparison in these measurements.
