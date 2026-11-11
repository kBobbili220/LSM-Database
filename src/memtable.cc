#include "lsm/memtable.h"

// MILESTONE 1: implement MemTable here (your own skiplist -- no std::map).
// Everything currently throws so the tests fail until you write it.

#include <stdexcept>

namespace lsm {

struct MemTable::Impl {};

MemTable::MemTable() : impl_(std::make_unique<Impl>()) {}
MemTable::~MemTable() = default;

void MemTable::Put(std::string_view, std::string_view) {
  throw std::logic_error("TODO: MemTable::Put");
}
void MemTable::Delete(std::string_view) {
  throw std::logic_error("TODO: MemTable::Delete");
}
GetResult MemTable::Get(std::string_view, std::string*) const {
  throw std::logic_error("TODO: MemTable::Get");
}
size_t MemTable::ApproximateMemoryUsage() const {
  throw std::logic_error("TODO: MemTable::ApproximateMemoryUsage");
}
std::vector<MemTable::Entry> MemTable::Entries() const {
  throw std::logic_error("TODO: MemTable::Entries");
}

}  // namespace lsm
