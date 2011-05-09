/* -*- c-basic-offset: 2 -*- */
/* Copyright(C) 2010-2011 Brazil

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
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

#include <fcntl.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/resource.h>

#include <zmq.h>
#include <event.h>
#include <evhttp.h>
#include <msgpack.h>
#include <groonga.h>
#include <pthread.h>

/* groonga origin headers */
#include <str.h>

#include "util.h"

#define DEFAULT_PORT 8080
#define DEFAULT_MAX_THREADS 8

grn_rc grn_ctx_close(grn_ctx *ctx);

#define CONST_STR_LEN(x) x, x ? sizeof(x) - 1 : 0

#define LISTEN_BACKLOG 756
#define MIN_MAX_FDS 2048
#define LOG_SPLIT_LINES 1000000
#define MAX_THREADS 128 /* max 256 */

typedef enum {
  run_mode_none = 0,
  run_mode_daemon,
  run_mode_usage,
  run_mode_error
} run_mode;

#define RUN_MODE_MASK   0x007f


typedef struct {
  grn_ctx *ctx;
  grn_obj *db;
  void *zmq_sock;
  grn_obj cmd_buf;
  pthread_t thd;
  uint32_t thread_id;
  struct event_base *base;
  struct evhttp *httpd;
  struct event pulse;
  const char *log_base_path;
  FILE *log_file;
  uint32_t log_count;
} thd_data;

typedef struct {
  const char *db_path;
  const char *recv_endpoint;
  pthread_t thd;
  void *zmq_ctx;
} recv_thd_data;

#define CMD_BUF_SIZE 1024

static uint32_t default_max_threads = DEFAULT_MAX_THREADS;
static volatile sig_atomic_t loop = 1;
static grn_obj *db;

static int
suggest_result(struct evbuffer *res_buf, const char *types, const char *query,
               const char *target_name, int threshold, int limit,
               grn_obj *cmd_buf, grn_ctx *ctx)
{
  if (target_name && types && query) {
    GRN_BULK_REWIND(cmd_buf);
    GRN_TEXT_PUTS(ctx, cmd_buf, "/d/suggest?table=item_");
    grn_text_urlenc(ctx, cmd_buf, target_name, strlen(target_name));
    GRN_TEXT_PUTS(ctx, cmd_buf, "&column=kana&types=");
    grn_text_urlenc(ctx, cmd_buf, types, strlen(types));
    GRN_TEXT_PUTS(ctx, cmd_buf, "&query=");
    grn_text_urlenc(ctx, cmd_buf, query, strlen(query));
    GRN_TEXT_PUTS(ctx, cmd_buf, "&threshold=");
    grn_text_itoa(ctx, cmd_buf, threshold);
    GRN_TEXT_PUTS(ctx, cmd_buf, "&limit=");
    grn_text_itoa(ctx, cmd_buf, limit);
    {
      char *res;
      int flags;
      unsigned int res_len;

      grn_ctx_send(ctx, GRN_TEXT_VALUE(cmd_buf), GRN_TEXT_LEN(cmd_buf), 0);
      grn_ctx_recv(ctx, &res, &res_len, &flags);

      evbuffer_add(res_buf, res, res_len);
      return res_len;
    }
  } else {
    evbuffer_add(res_buf, "{}", 2);
    return 2;
  }
}

static int
log_send(struct evbuffer *res_buf, thd_data *thd, struct evkeyvalq *get_args)
{
  uint64_t millisec;
  int threshold, limit;
  const char *callback, *types, *query, *client_id, *target_name,
             *learn_target_name;

  parse_keyval(get_args, &query, &types, &client_id, &target_name,
               &learn_target_name, &callback, &millisec, &threshold, &limit);

  /* send data to learn client */
  if (thd->zmq_sock && millisec && client_id && query && learn_target_name) {
    char c;
    size_t l;
    msgpack_packer pk;
    msgpack_sbuffer sbuf;
    int cnt, submit_flag = 0;

    msgpack_sbuffer_init(&sbuf);
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    cnt = 4;
    if (types && !strcmp(types, "submit")) {
      cnt++;
      types = NULL;
      submit_flag = 1;
    }
    msgpack_pack_map(&pk, cnt);

    c = 'i';
    msgpack_pack_raw(&pk, 1);
    msgpack_pack_raw_body(&pk, &c, 1);
    l = strlen(client_id);
    msgpack_pack_raw(&pk, l);
    msgpack_pack_raw_body(&pk, client_id, l);

    c = 'q';
    msgpack_pack_raw(&pk, 1);
    msgpack_pack_raw_body(&pk, &c, 1);
    l = strlen(query);
    msgpack_pack_raw(&pk, l);
    msgpack_pack_raw_body(&pk, query, l);

    c = 's';
    msgpack_pack_raw(&pk, 1);
    msgpack_pack_raw_body(&pk, &c, 1);
    msgpack_pack_uint64(&pk, millisec);

    c = 'l';
    msgpack_pack_raw(&pk, 1);
    msgpack_pack_raw_body(&pk, &c, 1);
    l = strlen(learn_target_name);
    msgpack_pack_raw(&pk, l);
    msgpack_pack_raw_body(&pk, learn_target_name, l);

    if (submit_flag) {
      c = 't';
      msgpack_pack_raw(&pk, 1);
      msgpack_pack_raw_body(&pk, &c, 1);
      msgpack_pack_true(&pk);
    }
    {
      zmq_msg_t msg;
      if (!zmq_msg_init_size(&msg, sbuf.size)) {
        memcpy((void *)zmq_msg_data(&msg), sbuf.data, sbuf.size);
        if (zmq_send(thd->zmq_sock, &msg, 0)) {
          print_error("zmq_send() error");
        }
        zmq_msg_close(&msg);
      }
    }
    msgpack_sbuffer_destroy(&sbuf);
  }
  /* make result */
  {
    int content_length;
    if (callback) {
      content_length = strlen(callback);
      evbuffer_add(res_buf, callback, content_length);
      evbuffer_add(res_buf, "(", 1);
      content_length += suggest_result(res_buf, types, query, target_name,
        threshold, limit, &(thd->cmd_buf), thd->ctx) + 3;
      evbuffer_add(res_buf, ");", 2);
    } else {
      content_length = suggest_result(res_buf, types, query, target_name,
        threshold, limit, &(thd->cmd_buf), thd->ctx);
    }
    return content_length;
  }
}

static void
cleanup_httpd_thread(thd_data *thd) {
  if (thd->log_file) {
    fclose(thd->log_file);
  }
  if (thd->httpd) {
    evhttp_free(thd->httpd);
  }
  if (thd->zmq_sock) {
    zmq_close(thd->zmq_sock);
  }
  grn_obj_unlink(thd->ctx, &(thd->cmd_buf));
  if (thd->ctx) {
    grn_ctx_close(thd->ctx);
  }
  event_base_free(thd->base);
}

static void
generic_handler(struct evhttp_request *req, void *arg)
{
  struct evkeyvalq args;
  thd_data *thd = arg;

  if (!loop) {
    event_base_loopexit(thd->base, NULL);
    return;
  }
  if (!req->uri) { return; }

  {
    char *uri = evhttp_decode_uri(req->uri);
    evhttp_parse_query(uri, &args);
    free(uri);
  }

  {
    struct evbuffer *res_buf;
    if (!(res_buf = evbuffer_new())) {
      err(1, "failed to create response buffer");
    }

    evhttp_add_header(req->output_headers,
      "Content-Type", "text/javascript; charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "close");

    {
      int content_length = log_send(res_buf, thd, &args);
      if (content_length >= 0) {
        char num_buf[16];
        snprintf(num_buf, 16, "%d", content_length);
        evhttp_add_header(req->output_headers, "Content-Length", num_buf);
      }
    }
    evhttp_send_reply(req, HTTP_OK, "OK", res_buf);
    evbuffer_free(res_buf);
    /* logging */
    {
      if (thd->log_base_path) {
        if (!thd->log_file) {
          time_t n;
          struct tm *t_st;
          char p[PATH_MAX + 1];

          time(&n);
          t_st = localtime(&n);

          snprintf(p, PATH_MAX, "%s%04d%02d%02d%02d%02d%02d-%02d",
            thd->log_base_path,
            t_st->tm_year + 1900, t_st->tm_mon + 1, t_st->tm_mday,
            t_st->tm_hour, t_st->tm_min, t_st->tm_sec, thd->thread_id);

          if (!(thd->log_file = fopen(p, "a"))) {
            print_error("cannot open log_file %s.", p);
          } else {
            thd->log_count = 0;
          }
        }
        if (thd->log_file) {
          fprintf(thd->log_file, "%s\n", req->uri);
          if (++thd->log_count >= LOG_SPLIT_LINES) {
            fclose(thd->log_file);
            thd->log_file = NULL;
          }
        }
      }
    }
  }
  evhttp_clear_headers(&args);
}

static int
bind_socket(int port)
{
  int nfd;
  if ((nfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    print_error("cannot open socket for http.");
    return -1;
  } else {
    int r, one = 1;
    struct sockaddr_in addr;

    r = setsockopt(nfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if ((r = bind(nfd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
      print_error("cannot bind socket for http.");
      return r;
    }
    if ((r = listen(nfd, LISTEN_BACKLOG)) < 0) {
      print_error("cannot listen socket for http.");
      return r;
    }
    if ((r = fcntl(nfd, F_GETFL, 0)) < 0 || fcntl(nfd, F_SETFL, r | O_NONBLOCK) < 0 ) {
      print_error("cannot fcntl socket for http.");
      return -1;
    }
    return nfd;
  }
}

static void
signal_handler(int sig)
{
  loop = 0;
}

void
timeout_handler(int fd, short events, void *arg) {
  thd_data *thd = arg;
  if (!loop) {
    event_base_loopexit(thd->base, NULL);
  } else {
    struct timeval tv = {1, 0};
    evtimer_add(&(thd->pulse), &tv);
  }
}

static void *
dispatch(void *arg)
{
  event_base_dispatch((struct event_base *)arg);
  return NULL;
}

grn_rc grn_text_ulltoa(grn_ctx *ctx, grn_obj *buf, unsigned long long int i);

static void
msgpack2json(msgpack_object *o, grn_ctx *ctx, grn_obj *buf)
{
  switch (o->type) {
  case MSGPACK_OBJECT_POSITIVE_INTEGER:
    grn_text_ulltoa(ctx, buf, o->via.u64);
    break;
  case MSGPACK_OBJECT_RAW:
    grn_text_esc(ctx, buf, o->via.raw.ptr, o->via.raw.size);
    break;
  case MSGPACK_OBJECT_ARRAY:
    GRN_TEXT_PUTC(ctx, buf, '[');
    {
      int i;
      for (i = 0; i < o->via.array.size; i++) {
        msgpack2json(o->via.array.ptr, ctx, buf);
      }
    }
    GRN_TEXT_PUTC(ctx, buf, ']');
    break;
  case MSGPACK_OBJECT_DOUBLE:
    grn_text_ftoa(ctx, buf, o->via.dec);
    break;
  default:
    print_error("cannot handle this msgpack type.");
  }
}

static void
load_from_learner(msgpack_object *o, grn_ctx *ctx, grn_obj *cmd_buf)
{
  if (o->type == MSGPACK_OBJECT_MAP && o->via.map.size) {
    msgpack_object_kv *kv;
    kv = &(o->via.map.ptr[0]);
    if (kv->key.type == MSGPACK_OBJECT_RAW && kv->key.via.raw.size == 6 &&
        !memcmp(kv->key.via.raw.ptr, CONST_STR_LEN("target"))) {
      if (kv->val.type == MSGPACK_OBJECT_RAW) {
        int i;
        GRN_BULK_REWIND(cmd_buf);
        GRN_TEXT_PUTS(ctx, cmd_buf, "load --table ");
        GRN_TEXT_PUT(ctx, cmd_buf, kv->val.via.raw.ptr, kv->val.via.raw.size);
        grn_ctx_send(ctx, GRN_TEXT_VALUE(cmd_buf), GRN_TEXT_LEN(cmd_buf), GRN_CTX_MORE);
        grn_ctx_send(ctx, CONST_STR_LEN("["), GRN_CTX_MORE);
        if (kv->val.via.raw.size > 5) {
          if (!memcmp(kv->val.via.raw.ptr, CONST_STR_LEN("item_")) ||
              !memcmp(kv->val.via.raw.ptr, CONST_STR_LEN("pair_"))) {
            char delim = '{';
            GRN_BULK_REWIND(cmd_buf);
            for (i = 1; i < o->via.map.size; i++) {
              GRN_TEXT_PUTC(ctx, cmd_buf, delim);
              kv = &(o->via.map.ptr[i]);
              msgpack2json(&(kv->key), ctx, cmd_buf);
              GRN_TEXT_PUTC(ctx, cmd_buf, ':');
              msgpack2json(&(kv->val), ctx, cmd_buf);
              delim = ',';
            }
            GRN_TEXT_PUTC(ctx, cmd_buf, '}');
            /* printf("msg: %.*s\n", GRN_TEXT_LEN(cmd_buf), GRN_TEXT_VALUE(cmd_buf)); */
            grn_ctx_send(ctx, GRN_TEXT_VALUE(cmd_buf), GRN_TEXT_LEN(cmd_buf), GRN_CTX_MORE);
          }
        }
        grn_ctx_send(ctx, CONST_STR_LEN("]"), 0);
        {
          char *res;
          int flags;
          unsigned int res_len;
          grn_ctx_recv(ctx, &res, &res_len, &flags);
        }
      }
    }
  }
}

static void
recv_handler(grn_ctx *ctx, void *zmq_recv_sock, msgpack_zone *mempool, grn_obj *cmd_buf)
{
  zmq_msg_t msg;

  if (zmq_msg_init(&msg)) {
    print_error("cannot init zmq message.");
  } else {
    if (zmq_recv(zmq_recv_sock, &msg, 0)) {
      print_error("cannot recv zmq message.");
    } else {
      msgpack_object obj;
      msgpack_unpack_return ret;

      ret = msgpack_unpack(zmq_msg_data(&msg), zmq_msg_size(&msg), NULL, mempool, &obj);
      if (MSGPACK_UNPACK_SUCCESS == ret) {
        load_from_learner(&obj, ctx, cmd_buf);
      } else {
        print_error("invalid recv data.");
      }
      msgpack_zone_clear(mempool);
    }
    zmq_msg_close(&msg);
  }
}

static void *
recv_from_learner(void *arg)
{
  void *zmq_recv_sock;
  recv_thd_data *thd = arg;

  if ((zmq_recv_sock = zmq_socket(thd->zmq_ctx, ZMQ_SUB))) {
    if (!zmq_connect(zmq_recv_sock, thd->recv_endpoint)) {
      grn_ctx ctx;
      if (!grn_ctx_init(&ctx, 0)) {
        if ((!grn_ctx_use(&ctx, db))) {
          msgpack_zone *mempool;
          if ((mempool = msgpack_zone_new(MSGPACK_ZONE_CHUNK_SIZE))) {
            grn_obj cmd_buf;
            zmq_pollitem_t items[] = {
              { zmq_recv_sock, 0, ZMQ_POLLIN, 0}
            };
            GRN_TEXT_INIT(&cmd_buf, 0);
            zmq_setsockopt(zmq_recv_sock, ZMQ_SUBSCRIBE, "", 0);
            while (loop) {
              zmq_poll(items, 1, 10000);
              if (items[0].revents & ZMQ_POLLIN) {
                recv_handler(&ctx, zmq_recv_sock, mempool, &cmd_buf);
              }
            }
            grn_obj_unlink(&ctx, &cmd_buf);
            msgpack_zone_free(mempool);
          } else {
            print_error("cannot create msgpack zone.");
          }
          /* db_close */
        } else {
          print_error("error in grn_db_open() on recv thread.");
        }
        grn_ctx_fin(&ctx);
      } else {
        print_error("error in grn_ctx_init() on recv thread.");
      }
    } else {
      print_error("cannot create recv zmq_socket.");
    }
  } else {
    print_error("cannot connect zmq_socket.");
  }
  return NULL;
}

static int
serve_threads(int nthreads, int port, const char *db_path, void *zmq_ctx,
              const char *send_endpoint, const char *recv_endpoint,
              const char *log_base_path)
{
  int nfd;
  uint32_t i;
  thd_data thds[nthreads];

  if ((nfd = bind_socket(port)) < 0) {
    print_error("cannot bind socket. please check port number with netstat.");
    return -1;
  }

  for (i = 0; i < nthreads; i++) {
    memset(&thds[i], 0, sizeof(thds[i]));
    if (!(thds[i].base = event_init())) {
      print_error("error in event_init() on thread %d.", i);
    } else {
      if (!(thds[i].httpd = evhttp_new(thds[i].base))) {
        print_error("error in evhttp_new() on thread %d.", i);
      } else {
        int r;
        if ((r = evhttp_accept_socket(thds[i].httpd, nfd))) {
          print_error("error in evhttp_accept_socket() on thread %d.", i);
        } else {
          if (send_endpoint) {
            if (!(thds[i].zmq_sock = zmq_socket(zmq_ctx, ZMQ_PUB))) {
              print_error("cannot create zmq_socket.");
            } else if (zmq_connect(thds[i].zmq_sock, send_endpoint)) {
              print_error("cannot connect zmq_socket.");
              zmq_close(thds[i].zmq_sock);
              thds[i].zmq_sock = NULL;
            } else {
              uint64_t hwm = 1;
              zmq_setsockopt(thds[i].zmq_sock, ZMQ_HWM, &hwm, sizeof(uint64_t));
            }
          } else {
            thds[i].zmq_sock = NULL;
          }
          if (!(thds[i].ctx = grn_ctx_open(0))) {
            print_error("error in grn_ctx_open() on thread %d.", i);
          } else if (grn_ctx_use(thds[i].ctx, db)) {
            print_error("error in grn_db_open() on thread %d.", i);
          } else {
            GRN_TEXT_INIT(&(thds[i].cmd_buf), 0);
            thds[i].log_base_path = log_base_path;
            thds[i].thread_id = i;
            evhttp_set_gencb(thds[i].httpd, generic_handler, &thds[i]);
            evhttp_set_timeout(thds[i].httpd, 10);
            {
              struct timeval tv = {1, 0};
              evtimer_set(&(thds[i].pulse), timeout_handler, &thds[i]);
              evtimer_add(&(thds[i].pulse), &tv);
            }
            if ((r = pthread_create(&(thds[i].thd), NULL, dispatch, thds[i].base))) {
              print_error("error in pthread_create() on thread %d.", i);
            }
          }
        }
      }
    }
  }

  /* recv thread from learner */
  if (recv_endpoint) {
    recv_thd_data rthd;
    rthd.db_path = db_path;
    rthd.recv_endpoint = recv_endpoint;
    rthd.zmq_ctx = zmq_ctx;

    if (pthread_create(&(rthd.thd), NULL, recv_from_learner, &rthd)) {
      print_error("error in pthread_create() on thread %d.", i);
    }
    pthread_join(rthd.thd, NULL);
  } else {
    while (loop) { sleep(1000); }
  }

  /* join all httpd thread */
  for (i = 0; i < nthreads; i++) {
    if (thds[i].thd) {
      pthread_join(thds[i].thd, NULL);
    }
    cleanup_httpd_thread(&(thds[i]));
  }
  return 0;
}

static uint32_t
get_core_number(void)
{
#ifdef ACTUALLY_GET_CORE_NUMBER
#ifdef _SC_NPROCESSORS_CONF
  return sysconf(_SC_NPROCESSORS_CONF);
#else /* _SC_NPROCESSORS_CONF */
  int n_processors;
  size_t length = sizeof(n_processors);
  int mib[] = {CTL_HW, HW_NCPU};
  if (sysctl(mib, sizeof(mib) / sizeof(mib[0]),
             &n_processors, &length, NULL, 0) == 0 &&
      length == sizeof(n_processors) &&
      0 < n_processors) {
    return n_processors;
  } else {
    return 1;
  }
#endif /* _SC_NPROCESSORS_CONF */
#endif /* ACTUALLY_GET_CORE_NUMBER */
  return 0;
}

static void
usage(FILE *output)
{
  fprintf(output,
          "Usage: groonga-suggest-httpd [options...] db_path\n"
          "db_path:\n"
          "  specify groonga database path which is used for suggestion.\n"
          "options:\n"
          "  -p <port number>   : http server port number (default: %d)\n"
          "  -c <thread number> : server thread number (default: %d)\n"
          "  -s <send endpoint> : send endpoint (ex. tcp://example.com:1234)\n"
          "  -r <recv endpoint> : recv endpoint (ex. tcp://example.com:1235)\n"
          "  -l <path prefix>   : log path prefix\n"
          "  -d                 : daemonize\n",
          DEFAULT_PORT, default_max_threads);
}

int
main(int argc, char **argv)
{
  int port_no = DEFAULT_PORT, daemon = 0;
  const char *max_threads_string = NULL, *port_string = NULL;
  const char *address;
  const char *send_endpoint = NULL, *recv_endpoint = NULL, *log_base_path = NULL;
  int n_processed_args, flags;
  run_mode mode = run_mode_none;

  if (!(default_max_threads = get_core_number())) {
    default_max_threads = DEFAULT_MAX_THREADS;
  }

  /* parse options */
  {
    static grn_str_getopt_opt opts[] = {
      {'c', NULL, NULL, 0, getopt_op_none}, /* deprecated */
      {'t', "max-threads", NULL, 0, getopt_op_none},
      {'h', "help", NULL, run_mode_usage, getopt_op_update},
      {'p', "port", NULL, 0, getopt_op_none},
      {'a', "address", NULL, 0, getopt_op_none}, /* not supported yet */
      {'s', "send-endpoint", NULL, 0, getopt_op_none},
      {'r', "receive-endpoint", NULL, 0, getopt_op_none},
      {'l', "log-base-path", NULL, 0, getopt_op_none},
      {'d', "daemon", NULL, run_mode_daemon, getopt_op_update},
      {'\0', NULL, NULL, 0, 0}
    };
    opts[0].arg = &max_threads_string;
    opts[1].arg = &max_threads_string;
    opts[3].arg = &port_string;
    opts[4].arg = &address;
    opts[5].arg = &send_endpoint;
    opts[6].arg = &recv_endpoint;
    opts[7].arg = &log_base_path;

    n_processed_args = grn_str_getopt(argc, argv, opts, &flags);
  }

  /* main */
  mode = (flags & RUN_MODE_MASK);
  if (n_processed_args < 0 ||
      (argc - n_processed_args) != 1 ||
      mode == run_mode_error) {
    usage(stderr);
    return EXIT_FAILURE;
  } else if (mode == run_mode_usage) {
    usage(stdout);
    return EXIT_SUCCESS;
  } else {
    grn_ctx ctx;
    void *zmq_ctx;
    int max_threads;

    if (max_threads_string) {
      max_threads = atoi(max_threads_string);
      if (max_threads > MAX_THREADS) {
        print_error("too many threads. limit to %d.", MAX_THREADS);
        max_threads = MAX_THREADS;
      }
    } else {
      max_threads = default_max_threads;
    }

    if (port_string) {
      port_no = atoi(port_string);
    }

    /* check environment */
    {
      struct rlimit rlim;
      if (!getrlimit(RLIMIT_NOFILE, &rlim)) {
        if (rlim.rlim_max < MIN_MAX_FDS) {
          print_error("too small max fds. %d required.", MIN_MAX_FDS);
          return -1;
        }
        rlim.rlim_cur = rlim.rlim_cur;
        setrlimit(RLIMIT_NOFILE, &rlim);
      }
    }

    if (mode == run_mode_daemon) {
      daemonize();
    }

    grn_init();
    grn_ctx_init(&ctx, 0);
    if ((db = grn_db_open(&ctx, argv[n_processed_args]))) {
      if ((zmq_ctx = zmq_init(1))) {
        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);
        signal(SIGQUIT, signal_handler);

        serve_threads(max_threads, port_no, argv[n_processed_args], zmq_ctx,
                      send_endpoint, recv_endpoint, log_base_path);
        zmq_term(zmq_ctx);
      } else {
        print_error("cannot create zmq context.");
      }
      grn_obj_close(&ctx, db);
    } else {
      print_error("cannot open db.");
    }
    grn_ctx_fin(&ctx);
    grn_fin();
  }
  return 0;
}
