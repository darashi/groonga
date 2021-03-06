/* -*- c-basic-offset: 2 -*- */
/* Copyright(C) 2010- Brazil

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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "util.h"

#define DEFAULT_FREQUENCY_THRESHOLD 100
#define DEFAULT_CONDITIONAL_PROBABILITY_THRESHOLD 0.2

int
print_error(const char *format, ...)
{
  int r;
  va_list l;

  va_start(l, format);
  vfprintf(stderr, format, l);
  r = fprintf(stderr, "\n");
  fflush(stderr);
  va_end(l);

  return r;
}

int
daemonize(void)
{
  pid_t pid;

  switch (fork()) {
  case 0:
    break;
  case -1:
    print_error("fork failed.");
    return -1;
  default:
    wait(NULL);
    _exit(0);
  }
  switch ((pid = fork())) {
  case 0:
    break;
  case -1:
    perror("fork");
    return -1;
  default:
    fprintf(stderr, "%d\n", pid);
    _exit(0);
  }
  {
    int null_fd = open("/dev/null", O_RDWR, 0);
    if (null_fd != -1) {
      dup2(null_fd, 0);
      dup2(null_fd, 1);
      dup2(null_fd, 2);
      if (null_fd > 2) { close(null_fd); }
    }
  }
  return 1;
}

static uint64_t
atouint64_t(const char *s)
{
  uint64_t r;
  for (r = 0; *s; s++) {
    r *= 10;
    r += (*s - '0');
  }
  return r;
}

void
parse_keyval(struct evkeyvalq *get_args,
             const char **query, const char **types,
             const char **client_id, const char **target_name,
             const char **learn_target_name,
             const char **callback,
             uint64_t *millisec,
             int *frequency_threshold,
             double *conditional_probability_threshold,
             int *limit)
{
  struct evkeyval *get;

  if (query) { *query = NULL; }
  if (types) { *types = NULL; }
  if (client_id) { *client_id = NULL; }
  if (target_name) { *target_name = NULL; }
  if (learn_target_name) { *learn_target_name = NULL; }
  if (callback) { *callback = NULL; }
  if (millisec) { *millisec = 0; }
  if (frequency_threshold) {
    *frequency_threshold = DEFAULT_FREQUENCY_THRESHOLD;
  }
  if (conditional_probability_threshold) {
    *conditional_probability_threshold = DEFAULT_CONDITIONAL_PROBABILITY_THRESHOLD;
  }
  if (limit) { *limit = -1; }

  TAILQ_FOREACH(get, get_args, next) {
    switch(get->key[0]) {
    case 'q':
      if (query) {
        *query = get->value;
      }
      break;
    case 't':
      /* TODO: check types */
      if (types) {
        *types = get->value;
      }
      break;
    case 'i':
      if (client_id) {
        *client_id = get->value;
      }
      break;
    case 'c':
      if (!strcmp(get->key, "callback") && callback) {
        *callback = get->value;
      }
      break;
    case 's':
      if (millisec) {
        *millisec = atouint64_t(get->value);
      }
      break;
    case 'n':
      /* TODO: check target_name */
      if (target_name) {
        *target_name = get->value;
      }
      break;
    case 'l':
      if (learn_target_name) {
        *learn_target_name = get->value;
      }
      break;
    case 'h':
      if (frequency_threshold) {
        *frequency_threshold = atoi(get->value);
      }
      break;
    case 'p':
      if (conditional_probability_threshold) {
        *conditional_probability_threshold = strtod(get->value, NULL);
      }
      break;
    case 'm':
      if (limit) {
        *limit = atoi(get->value);
      }
      break;
    default:
      break;
    }
  }
}
