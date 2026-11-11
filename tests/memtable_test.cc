// Milestone 1 tests: hand-written cases + a randomized model check vs std::map.
#include "lsm/memtable.h"

#include <gtest/gtest.h>

#include <map>
#include <optional>
#include <random>
#include <string>

using lsm::GetResult;
using lsm::MemTable;

TEST(MemTable, EmptyGetIsNotFound) {
  MemTable m;
  std::string v;
  EXPECT_EQ(m.Get("a", &v), GetResult::kNotFound);
  EXPECT_TRUE(m.Entries().empty());
}

TEST(MemTable, PutThenGet) {
  MemTable m;
  m.Put("k", "v1");
  std::string v;
  ASSERT_EQ(m.Get("k", &v), GetResult::kFound);
  EXPECT_EQ(v, "v1");
}

TEST(MemTable, OverwriteKeepsLatest) {
  MemTable m;
  m.Put("k", "v1");
  m.Put("k", "v2");
  std::string v;
  ASSERT_EQ(m.Get("k", &v), GetResult::kFound);
  EXPECT_EQ(v, "v2");
  EXPECT_EQ(m.Entries().size(), 1u);
}

TEST(MemTable, DeleteLeavesTombstoneNotAbsence) {
  MemTable m;
  std::string v;
  m.Delete("never-existed");
  EXPECT_EQ(m.Get("never-existed", &v), GetResult::kDeleted);
  m.Put("k", "v");
  m.Delete("k");
  EXPECT_EQ(m.Get("k", &v), GetResult::kDeleted);
  m.Put("k", "back");
  ASSERT_EQ(m.Get("k", &v), GetResult::kFound);
  EXPECT_EQ(v, "back");
}

TEST(MemTable, EmptyKeyAndValueAndBinaryBytes) {
  MemTable m;
  m.Put("", "");
  m.Put(std::string("a\0b", 3), std::string("\0\xff", 2));
  std::string v;
  ASSERT_EQ(m.Get("", &v), GetResult::kFound);
  EXPECT_EQ(v, "");
  ASSERT_EQ(m.Get(std::string("a\0b", 3), &v), GetResult::kFound);
  EXPECT_EQ(v, std::string("\0\xff", 2));
}

TEST(MemTable, EntriesAreSortedBytewise) {
  MemTable m;
  for (auto k : {"banana", "apple", "cherry", "app", "b"}) m.Put(k, "x");
  auto e = m.Entries();
  ASSERT_EQ(e.size(), 5u);
  for (size_t i = 1; i < e.size(); ++i) EXPECT_LT(e[i - 1].key, e[i].key);
}

TEST(MemTable, MemoryUsageGrows) {
  MemTable m;
  size_t before = m.ApproximateMemoryUsage();
  for (int i = 0; i < 1000; ++i) m.Put("key" + std::to_string(i), std::string(100, 'x'));
  EXPECT_GE(m.ApproximateMemoryUsage(), before + 1000 * 100);
}

// Randomized model check: any divergence from std::map is a bug in your skiplist.
TEST(MemTable, RandomOpsMatchModel) {
  for (unsigned seed = 1; seed <= 20; ++seed) {
    std::mt19937 rng(seed);
    MemTable m;
    std::map<std::string, std::optional<std::string>> model;
    auto rkey = [&] { return "k" + std::to_string(rng() % 200); };
    for (int op = 0; op < 5000; ++op) {
      std::string k = rkey();
      switch (rng() % 4) {
        case 0:
        case 1: {
          std::string v = "v" + std::to_string(rng());
          m.Put(k, v);
          model[k] = v;
          break;
        }
        case 2:
          m.Delete(k);
          model[k] = std::nullopt;
          break;
        case 3: {
          std::string v;
          auto r = m.Get(k, &v);
          auto it = model.find(k);
          if (it == model.end()) {
            ASSERT_EQ(r, GetResult::kNotFound) << "seed " << seed << " op " << op;
          } else if (!it->second) {
            ASSERT_EQ(r, GetResult::kDeleted) << "seed " << seed << " op " << op;
          } else {
            ASSERT_EQ(r, GetResult::kFound) << "seed " << seed << " op " << op;
            ASSERT_EQ(v, *it->second);
          }
        }
      }
    }
    auto e = m.Entries();
    ASSERT_EQ(e.size(), model.size()) << "seed " << seed;
    size_t i = 0;
    for (auto& [k, v] : model) {
      ASSERT_EQ(e[i].key, k);
      ASSERT_EQ(e[i].value, v);
      ++i;
    }
  }
}
