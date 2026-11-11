# lsmdb

A from-scratch LSM-tree key-value storage engine in C++20, built to learn.

```bash
cmake -B build && cmake --build build -j
ctest --test-dir build --output-on-failure
```

Other configurations:

```bash
cmake -B build-rel -DCMAKE_BUILD_TYPE=Release          # benchmarks
cmake -B build-tsan -DLSM_TSAN=ON                      # ThreadSanitizer
cmake -B build-fuzz -DCMAKE_CXX_COMPILER=clang++ -DLSM_FUZZ=ON && cmake --build build-fuzz
./build-fuzz/fuzz_memtable -max_total_time=600
```

See `docs/ROADMAP.md` for milestones and `NOTES.md` for the why-log.
