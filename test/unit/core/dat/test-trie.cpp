/* -*- c-basic-offset: 2; coding: utf-8 -*- */
/*
  Copyright (C) 2011  Brazil

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <gcutter.h>
#include <glib/gstdio.h>
#include <cppcutter.h>

#include <grn-assertions.h>
#include <dat/trie.hpp>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <set>
#include <string>
#include <vector>

namespace
{
  void create_keys(std::vector<std::string> *keys, std::size_t num_keys,
                   std::size_t min_length, std::size_t max_length)
  {
    std::string key;
    std::set<std::string> keyset;
    while (keyset.size() < num_keys) {
      key.resize(min_length + (std::rand() % (max_length - min_length + 1)));
      for (std::size_t j = 0; j < key.size(); ++j) {
        key[j] = '0' + (std::rand() % 10);
      }
      keyset.insert(key);
    }
    std::vector<std::string>(keyset.begin(), keyset.end()).swap(*keys);
    std::random_shuffle(keys->begin(), keys->end());
  }
}

namespace test_dat_trie
{
  const gchar *base_dir = NULL;

  void cut_setup(void)
  {
    std::srand(static_cast<unsigned int>(std::time(NULL)));
    base_dir = grn_test_get_tmp_dir();
    cut_remove_path(base_dir, NULL);
    g_mkdir_with_parents(base_dir, 0755);
  }

  void cut_teardown(void)
  {
    if (base_dir) {
      cut_remove_path(base_dir, NULL);
    }
  }

  void test_empty_trie(void)
  {
    grn::dat::Trie trie;
    trie.create();

    cppcut_assert_equal(grn::dat::DEFAULT_FILE_SIZE, trie.file_size());
    cppcut_assert_equal(grn::dat::UInt64(4236), trie.virtual_size());
    cppcut_assert_equal(grn::dat::UInt32(0), trie.total_key_length());
    cppcut_assert_equal(grn::dat::UInt32(0), trie.num_keys());
    cppcut_assert_equal(grn::dat::UInt32(1), trie.min_key_id());
    cppcut_assert_equal(grn::dat::UInt32(1), trie.next_key_id());
    cppcut_assert_equal(grn::dat::UInt32(0), trie.max_key_id());
    cppcut_assert_equal(grn::dat::UInt32(0), trie.num_keys());
    cppcut_assert_equal(grn::dat::UInt32(17893), trie.max_num_keys());
    cppcut_assert_equal(grn::dat::UInt32(512), trie.num_nodes());
    cppcut_assert_equal(grn::dat::UInt32(511), trie.num_phantoms());
    cppcut_assert_equal(grn::dat::UInt32(0), trie.num_zombies());
    cppcut_assert_equal(grn::dat::UInt32(71680), trie.max_num_nodes());
    cppcut_assert_equal(grn::dat::UInt32(1), trie.num_blocks());
    cppcut_assert_equal(grn::dat::UInt32(140), trie.max_num_blocks());
    cppcut_assert_equal(grn::dat::UInt32(0), trie.next_key_pos());
    cppcut_assert_equal(grn::dat::UInt32(100439), trie.key_buf_size());
  }

  void test_trie_on_memory(void)
  {
    std::vector<std::string> keys;
    create_keys(&keys, 1000, 1, 16);

    grn::dat::Trie trie;
    trie.create();

    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(true, trie.insert(keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(true, trie.search(keys[i].c_str(), keys[i].length()));
    }
  }

  void test_trie_on_file(void)
  {
    char trie_path[PATH_MAX];
    std::strcpy(trie_path, base_dir);
    std::strcat(trie_path, "test_trie_on_file.dat");

    std::vector<std::string> keys;
    create_keys(&keys, 1000, 1, 16);

    grn::dat::Trie trie;
    trie.create(trie_path);

    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(true, trie.insert(keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(true, trie.search(keys[i].c_str(), keys[i].length()));
    }

    grn::dat::Trie new_trie;
    new_trie.open(trie_path);

    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(false, new_trie.insert(keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(true, new_trie.search(keys[i].c_str(), keys[i].length()));
    }
  }

  void test_insert(void)
  {
    std::vector<std::string> keys;
    create_keys(&keys, 1000, 1, 16);

    grn::dat::Trie trie;
    trie.create();

    std::size_t total_key_length = 0;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      grn::dat::UInt32 key_pos;
      cppcut_assert_equal(true,
                          trie.insert(keys[i].c_str(), keys[i].length(), &key_pos));

      const grn::dat::Key &key = trie.get_key(key_pos);
      cppcut_assert_equal(true, key.is_valid());
      cppcut_assert_equal(static_cast<grn::dat::UInt32>(i + 1), key.id());
      cppcut_assert_equal(static_cast<grn::dat::UInt32>(keys[i].length()), key.length());
      cppcut_assert_equal(0, std::memcmp(key.ptr(), keys[i].c_str(), key.length()));

      grn::dat::UInt32 key_pos_again;
      cppcut_assert_equal(false,
                          trie.insert(keys[i].c_str(), keys[i].length(), &key_pos_again));
      cppcut_assert_equal(key_pos, key_pos_again);

      total_key_length += keys[i].length();
      cppcut_assert_equal(total_key_length, static_cast<std::size_t>(trie.total_key_length()));
    }
  }

  void test_ith_key(void)
  {
    std::vector<std::string> keys;
    create_keys(&keys, 1000, 1, 16);

    grn::dat::Trie trie;
    trie.create();

    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(false, trie.ith_key(i + 1).is_valid());

      grn::dat::UInt32 key_pos;
      cppcut_assert_equal(true, trie.insert(keys[i].c_str(), keys[i].length(), &key_pos));

      const grn::dat::Key &key_by_pos = trie.get_key(key_pos);
      const grn::dat::Key &key_by_id = trie.ith_key(i + 1);
      cppcut_assert_equal(&key_by_id, &key_by_pos);
    }
  }

  void test_search(void)
  {
    std::vector<std::string> keys;
    create_keys(&keys, 1000, 1, 16);

    grn::dat::Trie trie;
    trie.create();

    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(false, trie.search(keys[i].c_str(), keys[i].length()));

      grn::dat::UInt32 key_pos_inserted;
      cppcut_assert_equal(true,
                          trie.insert(keys[i].c_str(), keys[i].length(), &key_pos_inserted));

      grn::dat::UInt32 key_pos_found;
      cppcut_assert_equal(true,
                          trie.search(keys[i].c_str(), keys[i].length(), &key_pos_found));
      cppcut_assert_equal(key_pos_inserted, key_pos_found);
    }
  }

  void test_remove(void)
  {
    std::vector<std::string> keys;
    create_keys(&keys, 1000, 1, 16);

    grn::dat::Trie trie;
    trie.create();

    std::size_t total_key_length = 0;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(true, trie.insert(keys[i].c_str(), keys[i].length()));
      total_key_length += keys[i].length();
    }
    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(static_cast<grn::dat::UInt32>(keys.size() - i),
                          trie.num_keys());
      cppcut_assert_equal(true, trie.remove(i + 1));
      cppcut_assert_equal(false, trie.ith_key(i + 1).is_valid());
      cppcut_assert_equal(false, trie.search(keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(false, trie.remove(i + 1));

      total_key_length -= keys[i].length();
      cppcut_assert_equal(total_key_length, static_cast<std::size_t>(trie.total_key_length()));
    }

    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(true, trie.insert(keys[i].c_str(), keys[i].length()));
    }
    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(true, trie.remove(keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(false, trie.ith_key(keys.size() - i).is_valid());
      cppcut_assert_equal(false, trie.search(keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(false, trie.remove(keys[i].c_str(), keys[i].length()));
    }
  }

  void test_update(void)
  {
    std::vector<std::string> keys;
    create_keys(&keys, 1000 * 2, 1, 16);

    grn::dat::Trie trie;
    trie.create();

    std::size_t total_key_length = 0;
    for (std::size_t i = 0; i < (keys.size() / 2); ++i) {
      cppcut_assert_equal(true, trie.insert(keys[i].c_str(), keys[i].length()));
      total_key_length += keys[i].length();
    }
    for (std::size_t i = (keys.size() / 2); i < keys.size(); ++i) {
      const grn::dat::UInt32 key_id = i + 1 - (keys.size() / 2);
      const std::string &src_key = keys[i - (keys.size() / 2)];
      cppcut_assert_equal(true, trie.update(key_id, keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(true, trie.ith_key(key_id).is_valid());
      cppcut_assert_equal(true, trie.search(keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(false, trie.search(src_key.c_str(), src_key.length()));

      total_key_length += keys[i].length() - src_key.length();
      cppcut_assert_equal(total_key_length, static_cast<std::size_t>(trie.total_key_length()));
    }
    for (std::size_t i = 0; i < (keys.size() / 2); ++i) {
      const std::string &src_key = keys[i + (keys.size() / 2)];
      cppcut_assert_equal(true, trie.update(src_key.c_str(), src_key.length(),
                                            keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(true, trie.ith_key(i + 1).is_valid());
      cppcut_assert_equal(true, trie.search(keys[i].c_str(), keys[i].length()));
      cppcut_assert_equal(false, trie.search(src_key.c_str(), src_key.length()));

      total_key_length += keys[i].length() - src_key.length();
      cppcut_assert_equal(total_key_length, static_cast<std::size_t>(trie.total_key_length()));
    }
  }

  void test_create(void)
  {
    std::vector<std::string> keys;
    create_keys(&keys, 1000, 1, 16);

    grn::dat::Trie src_trie;
    src_trie.create();

    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(true, src_trie.insert(keys[i].c_str(), keys[i].length()));
    }

    grn::dat::Trie dest_trie;
    dest_trie.create(src_trie);

    for (std::size_t i = 0; i < keys.size(); ++i) {
      cppcut_assert_equal(true, dest_trie.search(keys[i].c_str(), keys[i].length()));
    }

    cppcut_assert_equal(src_trie.file_size(), dest_trie.file_size());
    cppcut_assert_equal(src_trie.total_key_length(), dest_trie.total_key_length());
    cppcut_assert_equal(src_trie.min_key_id(), dest_trie.min_key_id());
    cppcut_assert_equal(src_trie.next_key_id(), dest_trie.next_key_id());
    cppcut_assert_equal(src_trie.max_key_id(), dest_trie.max_key_id());
    cppcut_assert_equal(src_trie.num_keys(), dest_trie.num_keys());
    cppcut_assert_equal(src_trie.next_key_pos(), dest_trie.next_key_pos());

    cut_assert(dest_trie.num_nodes() < src_trie.num_nodes());
    cppcut_assert_equal(grn::dat::UInt32(0), dest_trie.num_zombies());
    cut_assert(dest_trie.num_blocks() < src_trie.num_nodes());
  }
}