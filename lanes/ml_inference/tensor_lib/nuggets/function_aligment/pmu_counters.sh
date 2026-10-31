#!/usr/bin/env bash
set -euo pipefail

  printf "Measuring 32-byte loop alignment (after) against the unaligned build (before)\n"

for variant in before after; do
      sudo perf stat -r 3 \
          -e cycles:u,instructions:u \
          -e de_no_dispatch_per_slot.no_ops_from_frontend:u \
          -e ic_tag_hit_miss.instruction_cache_miss:u \
          taskset -c 0 "./build/loop-alignment/$variant" \
          --benchmark_filter='^TensorAdd/256x256$' \
          --benchmark_min_time=100000x
  done

