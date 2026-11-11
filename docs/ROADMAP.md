# Roadmap

Status: `[ ]` todo, `[~]` in progress, `[x]` done.

- [ ] **1. Memtable**: own skiplist, put/get/delete with tombstones.  
      Done when: `tests/memtable_test.cc` passes under ASan/UBSan, fuzz target runs clean for 10 min.
- [ ] **2. WAL**: append-only log, CRC32 checksums, `fsync`, replay.  
      Done when: kill -9 mid-write loop, restart, no acknowledged write lost, torn tail record is ignored.
- [ ] **3. SSTable**: sorted data blocks, index block, footer; writer + reader.  
      Done when: round-trip 1M keys, `Get` via index binary search.
- [ ] **4. Bloom filters** per SSTable.  
      Done when: missing-key lookups skip disk; false-positive rate measured.
- [ ] **5. Flush + read path**: memtable -> SSTable; merged reads newest-first.  
      Done when: correct reads with overwrites/deletes across 5+ tables.
- [ ] **6. Compaction**: size-tiered, then leveled; tombstone GC.  
      Done when: disk usage stays bounded under long random-write workload.
- [ ] **7. MANIFEST + versioning**: atomic live-file-set updates.  
      Done when: crash during compaction recovers to consistent state.
- [ ] **8. Benchmark + tune**: db_bench-style tool, `perf`/flamegraph, compare vs LevelDB/RocksDB.  
      Done when: graph + honest write-up of why you're slower.

## Reading (do the reading *before* the milestone)
- M1: *Database Internals* ch. 1-3; LevelDB `skiplist.h` (after writing yours)
- M2: *Database Internals* WAL section; LevelDB `log_format.md`
- M3: LevelDB `table_format.md`; *Database Internals* ch. 4-7 (SSTables)
- M4: Bloom filter paper / any good write-up; LevelDB `filter_block`
- M6: O'Neil et al. LSM-tree paper; LevelDB `impl.md` (compaction)
- M7: LevelDB `version_set` + MANIFEST docs
