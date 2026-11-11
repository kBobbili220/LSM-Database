// libFuzzer target: interprets input bytes as a stream of ops, checks vs std::map.
// Build: cmake -B build-fuzz -DCMAKE_CXX_COMPILER=clang++ -DLSM_FUZZ=ON
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <optional>
#include <string>

#include "lsm/memtable.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  lsm::MemTable m;
  std::map<std::string, std::optional<std::string>> model;
  for (size_t i = 0; i + 3 <= size; i += 3) {
    std::string k(1, static_cast<char>(data[i + 1] % 16));
    switch (data[i] % 3) {
      case 0: {
        std::string v(1, static_cast<char>(data[i + 2]));
        m.Put(k, v);
        model[k] = v;
        break;
      }
      case 1:
        m.Delete(k);
        model[k] = std::nullopt;
        break;
      case 2: {
        std::string v;
        auto r = m.Get(k, &v);
        auto it = model.find(k);
        if (it == model.end()) { if (r != lsm::GetResult::kNotFound) abort(); }
        else if (!it->second) { if (r != lsm::GetResult::kDeleted) abort(); }
        else if (r != lsm::GetResult::kFound || v != *it->second) abort();
      }
    }
  }
  return 0;
}
