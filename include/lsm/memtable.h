#pragma once
// MILESTONE 1: the in-memory ordered table. YOU design and write this.
//
// The test suite (tests/memtable_test.cc) fixes the *interface* below so that it
// can check your implementation against std::map. The internals (skiplist, node
// layout, memory management, ordering of tombstones) are entirely up to you.
//
// Semantics the tests expect:
//  - Put(k, v) overwrites any previous value for k.
//  - Delete(k) records a tombstone (it is NOT the same as "never inserted").
//  - Get(k) returns:  kFound + value | kDeleted (tombstone) | kNotFound
//  - Iteration yields entries in ascending key order, tombstones included.

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace lsm {

enum class GetResult { kFound, kDeleted, kNotFound };

class MemTable {
 public:
  MemTable();
  ~MemTable();
  MemTable(const MemTable&) = delete;
  MemTable& operator=(const MemTable&) = delete;

  void Put(std::string_view key, std::string_view value);
  void Delete(std::string_view key);
  GetResult Get(std::string_view key, std::string* value_out) const;

  // Approximate bytes used; the DB will flush when this crosses a threshold.
  size_t ApproximateMemoryUsage() const;

  struct Entry {
    std::string key;
    std::optional<std::string> value;  // nullopt == tombstone
  };
  // Snapshot of all entries in ascending key order. (Replace with a real
  // iterator later; this keeps milestone 1 testable.)
  std::vector<Entry> Entries() const;

 private:
  // TODO(you): skiplist head, arena, rng, ...
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace lsm
