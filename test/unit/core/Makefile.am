SUBDIRS =					\
	dat

if WITH_CUTTER
noinst_LTLIBRARIES =				\
	test-context.la				\
	test-hash.la				\
	test-hash-sort.la			\
	test-hash-cursor.la			\
	test-inverted-index.la			\
	test-patricia-trie.la			\
	test-patricia-trie-cursor.la		\
	test-patricia-trie-search.la		\
	test-performance.la			\
	test-stress.la				\
	test-public-context.la			\
	test-query.la				\
	test-table.la				\
	test-table-patricia-trie-cursor.la	\
	test-table-sort-geo.la			\
	test-column.la				\
	test-database.la			\
	test-database-create.la			\
	test-table-cursor.la			\
	test-table-select.la			\
	test-table-select-normalize.la		\
	test-table-hook.la			\
	test-expr.la				\
	test-expr-script.la			\
	test-expr-query.la			\
	test-expr-parse.la			\
	test-text.la				\
	test-cast-basic.la			\
	test-cast-table.la			\
	test-encoding.la			\
	test-view.la				\
	test-view-operations.la			\
	test-plugin.la				\
	test-function.la			\
	test-function-edit-distance.la		\
	test-store-ja.la			\
	test-log.la				\
	test-table-sort-key-from-str.la		\
	test-inspect.la				\
	test-command-load.la			\
	test-command-table-create.la		\
	test-command-table-list.la		\
	test-command-column-create.la		\
	test-command-column-list.la		\
	test-command-select.la			\
	test-command-select-sort.la		\
	test-command-select-prefix-search.la	\
	test-command-select-filter-invalid.la	\
	test-command-select-query.la		\
	test-command-select-filter.la		\
	test-command-select-query-expansion.la	\
	test-command-define-selector.la		\
	test-command-cache-limit.la		\
	test-command-delete.la			\
	test-command-dump.la			\
	test-command-truncate.la		\
	test-geo.la				\
	test-geo-in-rectangle.la		\
	test-geo-in-rectangle-border.la		\
	test-accessor.la			\
	test-object.la				\
	test-rename.la
endif

INCLUDES =			\
	-I$(top_srcdir)/include	\
	-I$(top_srcdir)/lib	\
	$(GROONGA_INCLUDEDIR)

AM_CFLAGS =						\
	$(GCUTTER_CFLAGS)				\
	-DGROONGA=\"$(abs_top_builddir)/src/groonga\"

AM_LDFLAGS = -module -rpath $(libdir) -avoid-version

LIBS =								\
	$(top_builddir)/lib/libgroonga.la			\
	$(GCUTTER_LIBS)						\
	$(top_builddir)/test/unit/lib/libgrn-test-utils.la	\
	$(top_builddir)/test/unit/lib/libgrn-test-hash-utils.la

noinst_HEADERS =		\
	test-patricia-trie.h	\
	test-hash.h

test_context_la_SOURCES			= test-context.c
test_hash_la_SOURCES			= test-hash.c
test_hash_sort_la_SOURCES		= test-hash-sort.c
test_hash_cursor_la_SOURCES		= test-hash-cursor.c
test_inverted_index_la_SOURCES		= test-inverted-index.c
test_patricia_trie_la_SOURCES		= test-patricia-trie.c
test_patricia_trie_cursor_la_SOURCES	= test-patricia-trie-cursor.c
test_patricia_trie_search_la_SOURCES	= test-patricia-trie-search.c
test_performance_la_SOURCES		= test-performance.c
test_stress_la_SOURCES			= test-stress.c
test_public_context_la_SOURCES		= test-public-context.c
test_query_la_SOURCES			= test-query.c
test_table_la_SOURCES			= test-table.c
test_table_patricia_trie_cursor_la_SOURCES	= test-table-patricia-trie-cursor.c
test_table_sort_geo_la_SOURCES		= test-table-sort-geo.c
test_column_la_SOURCES			= test-column.c
test_database_la_SOURCES		= test-database.c
test_database_create_la_SOURCES		= test-database-create.c
test_table_cursor_la_SOURCES		= test-table-cursor.c
test_table_select_la_SOURCES		= test-table-select.c
test_table_select_normalize_la_SOURCES	= test-table-select-normalize.c
test_table_hook_la_SOURCES		= test-table-hook.c
test_expr_la_SOURCES			= test-expr.c
test_expr_script_la_SOURCES		= test-expr-script.c
test_expr_query_la_SOURCES		= test-expr-query.c
test_expr_parse_la_SOURCES		= test-expr-parse.c
test_text_la_SOURCES			= test-text.c
test_cast_basic_la_SOURCES		= test-cast-basic.c
test_cast_table_la_SOURCES		= test-cast-table.c
test_encoding_la_SOURCES		= test-encoding.c
test_view_la_SOURCES			= test-view.c
test_view_operations_la_SOURCES		= test-view-operations.c
test_plugin_la_SOURCES			= test-plugin.c
test_function_la_SOURCES		= test-function.c
test_function_edit_distance_la_SOURCES	= test-function-edit-distance.c
test_store_ja_la_SOURCES		= test-store-ja.c
test_log_la_SOURCES			= test-log.c
test_table_sort_key_from_str_la_SOURCES	= test-table-sort-key-from-str.c
test_inspect_la_SOURCES			= test-inspect.c
test_command_load_la_SOURCES		= test-command-load.c
test_command_table_create_la_SOURCES	= test-command-table-create.c
test_command_table_list_la_SOURCES	= test-command-table-list.c
test_command_column_create_la_SOURCES	= test-command-column-create.c
test_command_column_list_la_SOURCES	= test-command-column-list.c
test_command_select_la_SOURCES		= test-command-select.c
test_command_select_sort_la_SOURCES	= test-command-select-sort.c
test_command_select_prefix_search_la_SOURCES	= test-command-select-prefix-search.c
test_command_select_filter_invalid_la_SOURCES	= test-command-select-filter-invalid.c
test_command_select_query_la_SOURCES	= test-command-select-query.c
test_command_select_filter_la_SOURCES	= test-command-select-filter.c
test_command_select_query_expansion_la_SOURCES	= test-command-select-query-expansion.c
test_command_define_selector_la_SOURCES	= test-command-define-selector.c
test_command_cache_limit_la_SOURCES	= test-command-cache-limit.c
test_command_delete_la_SOURCES		= test-command-delete.c
test_command_dump_la_SOURCES		= test-command-dump.c
test_command_truncate_la_SOURCES	= test-command-truncate.c
test_geo_la_SOURCES			= test-geo.c
test_geo_in_rectangle_la_SOURCES	= test-geo-in-rectangle.c
test_geo_in_rectangle_border_la_SOURCES	= test-geo-in-rectangle-border.c
test_accessor_la_SOURCES		= test-accessor.c
test_object_la_SOURCES			= test-object.c
test_rename_la_SOURCES			= test-rename.c
