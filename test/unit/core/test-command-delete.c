/* -*- c-basic-offset: 2; coding: utf-8 -*- */
/*
  Copyright (C) 2010-2011  Kouhei Sutou <kou@clear-code.com>

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

#include "../lib/grn-assertions.h"

#include <str.h>

void test_by_id(void);
void test_by_key(void);
void test_referenced_record(void);
void test_uint64(void);
void test_last_token(void);
void test_no_key_twice(void);
void test_no_key_by_id(void);
void test_corrupt_jagged_array(void);

static gchar *tmp_directory;

static grn_ctx *context;
static grn_obj *database;

void
cut_startup(void)
{
  tmp_directory = g_build_filename(grn_test_get_tmp_dir(),
                                   "command-delete",
                                   NULL);
}

void
cut_shutdown(void)
{
  g_free(tmp_directory);
}

static void
remove_tmp_directory(void)
{
  cut_remove_path(tmp_directory, NULL);
}

static void
setup_data(void)
{
  assert_send_command("table_create Users TABLE_HASH_KEY ShortText");
  assert_send_command("table_create Bookmarks TABLE_HASH_KEY ShortText");
  assert_send_command("table_create Bigram TABLE_PAT_KEY ShortText "
                      "--default_tokenizer TokenBigram");

  assert_send_command("column_create Bookmarks user COLUMN_SCALAR Users");
  assert_send_command("column_create Users bookmarks COLUMN_INDEX "
                      "Bookmarks user");
  assert_send_command("column_create Bigram user COLUMN_INDEX|WITH_POSITION "
                      "Users _key");

  assert_send_command("load --table Users --columns '_key'\n"
                      "[\n"
                      "  [\"mori\"],\n"
                      "  [\"tapo\"]\n"
                      "]");
  assert_send_command("load --table Bookmarks --columns '_key, user'\n"
                      "[\n"
                      "  [\"http://groonga.org/\", \"yu\"],\n"
                      "  [\"http://cutter.sourceforge.net/\", \"tasukuchan\"]\n"
                      "]");
}

void
cut_setup(void)
{
  const gchar *database_path;

  remove_tmp_directory();
  g_mkdir_with_parents(tmp_directory, 0700);

  context = g_new0(grn_ctx, 1);
  grn_ctx_init(context, 0);

  database_path = cut_build_path(tmp_directory, "database.groonga", NULL);
  database = grn_db_create(context, database_path, NULL);

  setup_data();
}

void
cut_teardown(void)
{
  if (context) {
    grn_obj_close(context, database);
    grn_ctx_fin(context);
    g_free(context);
  }

  remove_tmp_directory();
}

void
test_by_id(void)
{
  assert_send_command("delete Users --id 1");
  cut_assert_equal_string("[[[3],"
                            "[[\"_key\",\"ShortText\"]],"
                            "[\"tapo\"],"
                            "[\"yu\"],"
                            "[\"tasukuchan\"]]]",
                          send_command("select Users "
                                       "--output_columns _key"));
}

void
test_by_delete(void)
{
  assert_send_command("delete Users tapo");
  cut_assert_equal_string("[[[3],"
                            "[[\"_key\",\"ShortText\"]],"
                            "[\"mori\"],"
                            "[\"yu\"],"
                            "[\"tasukuchan\"]]]",
                          send_command("select Users "
                                       "--output_columns _key"));
}

void
test_referenced_record(void)
{
  assert_send_command_error(GRN_OPERATION_NOT_PERMITTED,
                            "undeletable record (Users:4) "
                            "has value (bookmarks:1)",
                            "delete Users tasukuchan");
  cut_assert_equal_string("[[[2],"
                            "[[\"_key\",\"ShortText\"]],"
                            "[\"tapo\"],"
                            "[\"tasukuchan\"]]]",
                          send_command("select Users "
                                       "--output_columns _key "
                                       "--match_columns \"_key\" "
                                       "--query \"ta\""));
}

void
test_uint64(void)
{
  assert_send_command("table_create Students TABLE_HASH_KEY UInt64");
  assert_send_command("load --table Students --columns '_key'\n"
                      "[\n"
                      "  [29],\n"
                      "  [2929]\n"
                      "]");
  assert_send_command("delete Students 2929");
  cut_assert_equal_string("[[[1],"
                            "[[\"_key\",\"UInt64\"]],"
                            "[29]]]",
                          send_command("select Students "
                                       "--output_columns _key"));
}

void
test_last_token(void)
{
  cut_assert_equal_string("[[[2],"
                            "[[\"_key\",\"ShortText\"]],"
                            "[\"mori\"],"
                            "[\"tapo\"]]]",
                          send_command("select Users "
                                       "--match_columns _key "
                                       "--query o "
                                       "--output_columns _key"));
  assert_send_command("delete Users tapo");
  cut_assert_equal_string("[[[1],"
                            "[[\"_key\",\"ShortText\"]],"
                            "[\"mori\"]]]",
                          send_command("select Users "
                                       "--match_columns _key "
                                       "--query o "
                                       "--output_columns _key"));
}

void
test_no_key_twice(void)
{
  assert_send_command("table_create Sites TABLE_NO_KEY");
  assert_send_command("column_create Sites title COLUMN_SCALAR ShortText");
  assert_send_command("delete Sites --id 999");
  assert_send_command("delete Sites --id 999");
  cut_assert_equal_string("3",
                          send_command("load --table Sites\n"
                                       "[\n"
                                       "{\"title\": \"groonga\"},\n"
                                       "{\"title\": \"Ruby\"},\n"
                                       "{\"title\": \"Cutter\"}\n"
                                       "]"));
  cut_assert_equal_string("[[[3],"
                           "[[\"_id\",\"UInt32\"],"
                            "[\"title\",\"ShortText\"]],"
                           "[1,\"groonga\"],"
                           "[2,\"Ruby\"],"
                           "[3,\"Cutter\"]]]",
                          send_command("select Sites"));
}

void
test_no_key_by_id(void)
{
  assert_send_command("table_create Sites TABLE_NO_KEY");
  assert_send_command("column_create Sites title COLUMN_SCALAR ShortText");
  cut_assert_equal_string("3",
                          send_command("load --table Sites\n"
                                       "[\n"
                                       "{\"title\": \"groonga\"},\n"
                                       "{\"title\": \"Ruby\"},\n"
                                       "{\"title\": \"Cutter\"}\n"
                                       "]"));
  assert_send_command("delete Sites --id 2");
  cut_assert_equal_string("[[[2],"
                           "[[\"_id\",\"UInt32\"],"
                            "[\"title\",\"ShortText\"]],"
                           "[1,\"groonga\"],"
                           "[3,\"Cutter\"]]]",
                          send_command("select Sites"));
}

void
test_corrupt_jagged_array(void)
{
  const gchar *text_65bytes =
    "65bytes text "
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
  const gchar *text_129bytes =
    "129bytes text "
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

  assert_send_command("table_create Sites TABLE_NO_KEY");
  assert_send_command("column_create Sites description COLUMN_SCALAR ShortText");
  cut_assert_equal_string(
    "1",
    send_command(cut_take_printf("load --table Sites\n"
                                 "[[\"description\"],\n"
                                 "[\"%s\"]\n"
                                 "]",
                                 text_129bytes)));
  assert_send_command("delete Sites --id 1");

  cut_assert_equal_string(
    "3",
    send_command(cut_take_printf("load --table Sites\n"
                                 "[[\"description\"],\n"
                                 "[\"%s\"],\n"
                                 "[\"%s\"],\n"
                                 "[\"%s\"]"
                                 "]",
                                 text_65bytes,
                                 text_65bytes,
                                 text_129bytes)));
  cut_assert_equal_string(
    cut_take_printf("[[[3],"
                    "[[\"_id\",\"UInt32\"],"
                     "[\"description\",\"ShortText\"]],"
                     "[2,\"%s\"],"
                     "[3,\"%s\"],"
                     "[4,\"%s\"]]]",
                    text_65bytes,
                    text_65bytes,
                    text_129bytes),
                    send_command("select Sites"));
}
