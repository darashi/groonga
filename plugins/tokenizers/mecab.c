/* -*- c-basic-offset: 2 -*- */
/* Copyright(C) 2009-2011 Brazil

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

#include <ctx.h>
#include <db.h>

#include <str.h>
#include <token.h>

#include <groonga.h>
#include <groonga/plugin.h>

#include <mecab.h>

#include <string.h>
#include <ctype.h>

static mecab_t *sole_mecab;
static grn_critical_section sole_mecab_lock;

typedef struct {
  grn_str *nstr;
  mecab_t *mecab;
  char *buf;
  char *next;
  char *end;
  grn_encoding encoding;
  grn_obj curr_;
  grn_obj stat_;
} grn_mecab_tokenizer;

/*
  This function is called for a full text search query or a document to be
  indexed. This means that both short/long strings are given.
  The return value of this function is ignored. When an error occurs in this
  function, `ctx->rc' is overwritten with an error code (not GRN_SUCCESS).
 */
static grn_obj *
mecab_init(grn_ctx *ctx, int nargs, grn_obj **args, grn_user_data *user_data)
{
  grn_obj *str;
  int nflags = 0;
  char *buf, *s, *p;
  char mecab_err[256];
  grn_obj *table = args[0];
  grn_obj_flags table_flags;
  grn_mecab_tokenizer *token;
  unsigned int bufsize, maxtrial = 10, len;
  if (!(str = grn_ctx_pop(ctx))) {
    ERR(GRN_INVALID_ARGUMENT, "missing argument");
    return NULL;
  }
  mecab_err[sizeof(mecab_err) - 1] = '\0';
  if (!sole_mecab) {
    static char *argv[] = {"", "-Owakati"};
    CRITICAL_SECTION_ENTER(sole_mecab_lock);
    if (!sole_mecab) {
      sole_mecab = mecab_new(2, argv);
      if (!sole_mecab) {
        strncpy(mecab_err, mecab_strerror(NULL), sizeof(mecab_err) - 1);
      }
    }
    CRITICAL_SECTION_LEAVE(sole_mecab_lock);
  }
  if (!sole_mecab) {
    ERR(GRN_TOKENIZER_ERROR,
        "mecab_new failed on grn_mecab_init: %s", mecab_err);
    return NULL;
  }
  if (!(token = GRN_MALLOC(sizeof(grn_mecab_tokenizer)))) { return NULL; }
  token->mecab = sole_mecab;
  grn_table_get_info(ctx, table, &table_flags, &token->encoding, NULL);
  nflags |= (table_flags & GRN_OBJ_KEY_NORMALIZE);
  if (!(token->nstr = grn_str_open_(ctx, GRN_TEXT_VALUE(str), GRN_TEXT_LEN(str),
                                    nflags, token->encoding))) {
    GRN_FREE(token);
    ERR(GRN_TOKENIZER_ERROR, "grn_str_open failed at grn_token_open");
    return NULL;
  }
  len = token->nstr->norm_blen;
  for (bufsize = len * 2 + 1; maxtrial; bufsize *= 2, maxtrial--) {
    if (!(buf = GRN_MALLOC(bufsize + 1))) {
      GRN_LOG(ctx, GRN_LOG_ALERT, "buffer allocation on mecab_init failed !");
      grn_str_close(ctx, token->nstr);
      GRN_FREE(token);
      return NULL;
    }
    CRITICAL_SECTION_ENTER(sole_mecab_lock);
    s = mecab_sparse_tostr3(token->mecab, token->nstr->norm, len, buf, bufsize);
    if (!s) {
      strncpy(mecab_err, mecab_strerror(token->mecab), sizeof(mecab_err) - 1);
    }
    CRITICAL_SECTION_LEAVE(sole_mecab_lock);
    if (s) { break; }
    GRN_FREE(buf);
    if (strstr(mecab_err, "output buffer overflow") == NULL) { break; }
  }
  if (!s) {
    ERR(GRN_TOKENIZER_ERROR, "mecab_sparse_tostr failed len=%d bufsize=%d err=%s",
        len, bufsize, mecab_err);
    grn_str_close(ctx, token->nstr);
    GRN_FREE(token);
    return NULL;
  }
  /* A certain version of mecab returns trailing lf or spaces. */
  for (p = buf + strlen(buf) - 1;
       buf <= p && isspace(*(unsigned char *)p);
       p--) { *p = '\0'; }
  /* grn_log("sparsed='%s'", s); */
  user_data->ptr = token;
  token->buf = buf;
  token->next = buf;
  token->end = buf + strlen(buf);
  GRN_TEXT_INIT(&token->curr_, GRN_OBJ_DO_SHALLOW_COPY);
  GRN_UINT32_INIT(&token->stat_, 0);
  return NULL;
}

/*
  This function returns tokens one by one.
 */
static grn_obj *
mecab_next(grn_ctx *ctx, int nargs, grn_obj **args, grn_user_data *user_data)
{
  size_t cl;
  /* grn_obj *table = args[0]; */
  grn_mecab_tokenizer *token = user_data->ptr;
  char *p = token->next, *r;
  char *e = token->end;
  for (r = p; r < e; r += cl) {
    if (!(cl = grn_charlen_(ctx, r, e, token->encoding))) {
      token->next = e;
      break;
    }
    if (grn_isspace(r, token->encoding)) {
      char *q = r;
      while ((cl = grn_isspace(q, token->encoding))) { q += cl; }
      token->next = q;
      break;
    }
  }
  GRN_TEXT_SET_REF(&token->curr_, p, r - p);
  GRN_UINT32_SET(ctx, &token->stat_, r == e ? GRN_TOKEN_LAST : 0);
  grn_ctx_push(ctx, &token->curr_);
  grn_ctx_push(ctx, &token->stat_);
  return NULL;
}

/*
  This function finalizes a tokenization.
 */
static grn_obj *
mecab_fin(grn_ctx *ctx, int nargs, grn_obj **args, grn_user_data *user_data)
{
  grn_mecab_tokenizer *token = user_data->ptr;
  grn_str_close(ctx, token->nstr);
  GRN_FREE(token->buf);
  GRN_FREE(token);
  return NULL;
}

static void
check_mecab_dictionary_encoding(grn_ctx *ctx)
{
#ifdef HAVE_MECAB_DICTIONARY_INFO_T
  mecab_t *mecab;

  mecab = mecab_new(0, NULL);
  if (mecab) {
    grn_encoding encoding;
    const mecab_dictionary_info_t *dictionary;
    int have_same_encoding_dictionary = 0;

    encoding = GRN_CTX_GET_ENCODING(ctx);
    dictionary = mecab_dictionary_info(mecab);
    for (; dictionary; dictionary = dictionary->next) {
      switch (encoding) {
      case GRN_ENC_EUC_JP:
        if (strcasecmp(dictionary->charset, "euc-jp") == 0) {
          have_same_encoding_dictionary = 1;
        }
        break;
      case GRN_ENC_UTF8:
        if (strcasecmp(dictionary->charset, "utf-8") == 0 ||
            strcasecmp(dictionary->charset, "utf8") == 0) {
          have_same_encoding_dictionary = 1;
        }
        break;
      case GRN_ENC_SJIS:
        if (strcasecmp(dictionary->charset, "shift_jis") == 0 ||
            strcasecmp(dictionary->charset, "shift-jis") == 0 ||
            strcasecmp(dictionary->charset, "sjis") == 0) {
          have_same_encoding_dictionary = 1;
        }
        break;
      default:
        break;
      }
    }
    mecab_destroy(mecab);

    if (!have_same_encoding_dictionary) {
      ERR(GRN_TOKENIZER_ERROR,
          "MeCab has no dictionary that uses the context encoding: <%s>",
          grn_enctostr(encoding));
    }
  } else {
    ERR(GRN_TOKENIZER_ERROR,
        "mecab_new failed in check_mecab_dictionary_encoding");
  }
#endif
}

/*
  This function initializes a plugin. This function fails if there is no
  dictionary that uses the context encoding of groonga.
 */
grn_rc
GRN_PLUGIN_INIT(grn_ctx *ctx)
{
  sole_mecab = NULL;
  CRITICAL_SECTION_INIT(sole_mecab_lock);

  check_mecab_dictionary_encoding(ctx);
  return ctx->rc;
}

/*
  This function registers a plugin to a database.
 */
grn_rc
GRN_PLUGIN_REGISTER(grn_ctx *ctx)
{
  grn_obj *obj;
  grn_expr_var vars[] = {
    {NULL, 0},
    {NULL, 0},
    {NULL, 0}
  };
  GRN_TEXT_INIT(&vars[0].value, 0);
  GRN_TEXT_INIT(&vars[1].value, 0);
  GRN_UINT32_INIT(&vars[2].value, 0);

  /*
    grn_proc_create() registers a plugin to a database which is associated
    with `ctx'. A returned object must not be finalized here.
   */
  obj = grn_proc_create(ctx, "TokenMecab", 10, GRN_PROC_TOKENIZER,
                        mecab_init, mecab_next, mecab_fin, 3, vars);
  if (!obj || ((grn_db_obj *)obj)->id != GRN_DB_MECAB) { return GRN_FILE_CORRUPT; }

  return GRN_SUCCESS;
}

/*
  This function finalizes a plugin.
 */
grn_rc
GRN_PLUGIN_FIN(grn_ctx *ctx)
{
  if (sole_mecab) {
    mecab_destroy(sole_mecab);
    sole_mecab = NULL;
  }
  CRITICAL_SECTION_FIN(sole_mecab_lock);

  return GRN_SUCCESS;
}
