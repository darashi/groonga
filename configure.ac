AC_PREREQ(2.59)
AC_INIT([groonga],[1.2.9],[groonga@razil.jp])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(tar-pax)

# for Autoconf 2.60 earlier.
if test -z "${datarootdir}"; then
   datarootdir="\${prefix}/share"
   AC_SUBST(datarootdir)
fi

AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(HOST_CPU, ["$host_cpu"], [host CPU])
AC_DEFINE_UNQUOTED(HOST_OS, ["$host_os"], [host OS])

AC_MSG_CHECKING([for native Win32])
case "$host_os" in
  mingw*)
    os_win32=yes
    ;;
  *)
    os_win32=no
    ;;
esac
AC_MSG_RESULT([$os_win32])

AC_MSG_CHECKING([for some Win32 platform])
case "$host_os" in
  mingw*|cygwin*)
    platform_win32=yes
    ;;
  *)
    platform_win32=no
    ;;
esac
AC_MSG_RESULT([$platform_win32])

AM_CONDITIONAL(OS_WIN32, test "$os_win32" = "yes")
AM_CONDITIONAL(PLATFORM_WIN32, test "$platform_win32" = "yes")

AC_MSG_CHECKING([for NetBSD.])
case "$host_os" in
  netbsd*)
    netbsd=yes
    ;;
  *)
    netbsd=no
    ;;
esac
AC_MSG_RESULT([$netbsd])

AC_MSG_CHECKING([for Solaris.])
case "$host_os" in
  solaris*)
    solaris=yes
    ;;
  *)
    solaris=no
    ;;
esac
AC_MSG_RESULT([$solaris])

AC_C_BIGENDIAN
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
m4_ifdef([PKG_PROG_PKG_CONFIG],
         [PKG_PROG_PKG_CONFIG([0.19])
          m4_pattern_allow(PKG_CONFIG_LIBDIR)])

AC_DEFUN([CHECK_CFLAG], [
  AC_MSG_CHECKING([if gcc supports $1])
  old_CFLAGS=$CFLAGS
  flag=`echo '$1' | sed -e 's,^-Wno-,-W,'`
  CFLAGS="$CFLAGS $flag -Werror"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [check_cflag=yes],
    [check_cflag=no])
  CFLAGS="$old_CFLAGS"
  if test "x$check_cflag" = "xyes"; then
    CFLAGS="$CFLAGS $1"
  fi
  AC_MSG_RESULT([$check_cflag])
])

AC_DEFUN([CHECK_CXXFLAG], [
  AC_MSG_CHECKING([if g++ supports $1])
  old_CXXFLAGS=$CXXFLAGS
  flag=`echo '$1' | sed -e 's,^-Wno-,-W,'`
  CXXFLAGS="$CXXFLAGS $flag -Werror"
  AC_LANG_PUSH([C++])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [check_cxxflag=yes],
    [check_cxxflag=no])
  AC_LANG_POP([C++])
  CXXFLAGS="$old_CXXFLAGS"
  if test "x$check_cxxflag" = "xyes"; then
    CXXFLAGS="$CXXFLAGS $1"
  fi
  AC_MSG_RESULT([$check_cxxflag])
])

AC_DEFUN([REMOVE_CXXFLAG], [
  AC_MSG_CHECKING([whether g++ option $1 is needed to be removed])
  if echo "$CXXFLAGS" | grep -q -- "$1"; then
    CXXFLAGS=`echo "$CXXFLAGS" | sed -e 's,$1,,'`
    remove_cxxflag=yes
  else
    remove_cxxflag=no
  fi
  AC_MSG_RESULT([$remove_cxxflag])
])

if test "$GCC" = "yes"; then
  CHECK_CFLAG([-Wall])
  CHECK_CFLAG([-Wextra])
  if test "x$check_cflag" = "xno"; then
    CHECK_CFLAG([-W])
  fi
  CHECK_CFLAG([-Wno-unused-but-set-variable]) # FIXME: enable it.
  CHECK_CFLAG([-Wno-unused-parameter])
  CHECK_CFLAG([-Wno-sign-compare])
  CHECK_CFLAG([-Wno-pointer-sign])
  CHECK_CFLAG([-Wno-missing-field-initializers])

  CHECK_CFLAG([-Wformat=2])
  CHECK_CFLAG([-Wstrict-aliasing=2])
  CHECK_CFLAG([-Wdisabled-optimization])
  CHECK_CFLAG([-Wfloat-equal])
  CHECK_CFLAG([-Wpointer-arith])
  CHECK_CFLAG([-Wdeclaration-after-statement])
  CHECK_CFLAG([-Wbad-function-cast])
  CHECK_CFLAG([-Wcast-align])
  CHECK_CFLAG([-Wredundant-decls])
#  CHECK_CFLAG([-Wunsafe-loop-optimizations])
#  CHECK_CFLAG([-Wunreachable-code])
#  CHECK_CFLAG([-Wswitch-enum])
#  CHECK_CFLAG([-Wshadow])
#  CHECK_CFLAG([-Wconversion])
#  CHECK_CFLAG([-Wwrite-strings])
#  CHECK_CFLAG([-Winline])

  CHECK_CXXFLAG([-fexceptions])
  CHECK_CXXFLAG([-fimplicit-templates])
fi

AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
m4_ifdef([LT_OUTPUT], [LT_OUTPUT])

LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
LT_VERSION_INFO="\$(LT_CURRENT):\$(LT_REVISION):\$(LT_AGE)"
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
AC_SUBST(LT_VERSION_INFO)

GRN_DLL_FILENAME="libgroonga-\$(LT_CURRENT).dll"
AC_SUBST(GRN_DLL_FILENAME)

if test "$srcdir/version.sh"; then
  source "$srcdir/version.sh"
  AC_SUBST(GROONGA_VERSION)
fi

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/suggest/Makefile
  lib/Makefile
  lib/dat/Makefile
  include/Makefile
  include/groonga/Makefile
  plugins/Makefile
  plugins/tokenizers/Makefile
  plugins/suggest/Makefile
  examples/Makefile
  examples/dictionary/Makefile
  examples/dictionary/edict/Makefile
  examples/dictionary/eijiro/Makefile
  examples/dictionary/gene95/Makefile
  examples/dictionary/jmdict/Makefile
  packages/Makefile
  packages/apt/Makefile
  packages/rpm/Makefile
  packages/rpm/centos/Makefile
  packages/rpm/fedora/Makefile
  packages/yum/Makefile
  packages/source/Makefile
  packages/windows/Makefile
  packages/windows/patches/Makefile
  packages/windows/language-files/Makefile
  packages/windows/setup-x64.nsi
  data/Makefile
  data/html/Makefile
  data/munin/Makefile
  data/init.d/Makefile
  data/init.d/redhat/Makefile
  data/init.d/redhat/sysconfig/Makefile
  tools/Makefile
  doc/Makefile
  doc/locale/Makefile
  doc/locale/en/Makefile
  doc/locale/en/LC_MESSAGES/Makefile
  doc/locale/ja/Makefile
  doc/locale/ja/LC_MESSAGES/Makefile
  test/Makefile
  test/unit/Makefile
  test/unit/lib/Makefile
  test/unit/lib/ruby/Makefile
  test/unit/fixtures/Makefile
  test/unit/fixtures/inverted-index/Makefile
  test/unit/fixtures/stress/Makefile
  test/unit/fixtures/performance/Makefile
  test/unit/fixtures/plugins/Makefile
  test/unit/fixtures/geo/Makefile
  test/unit/fixtures/story/Makefile
  test/unit/fixtures/story/taiyaki/Makefile
  test/unit/fixtures/story/rurema/Makefile
  test/unit/util/Makefile
  test/unit/core/Makefile
  test/unit/core/dat/Makefile
  test/unit/memcached/Makefile
  test/unit/http/Makefile
  test/unit/gqtp/Makefile
  test/unit/story/Makefile
  test/unit/grntest/Makefile
  test/benchmark/Makefile
  test/benchmark/fixtures/Makefile
  test/benchmark/fixtures/geo-select/Makefile
  test/benchmark/lib/Makefile
])

if test "$GCC" = "yes"; then
  AC_DEFINE(_GNU_SOURCE, [1], [Define to 1 if you use GCC.])
fi

if test "$netbsd" = "yes"; then
  AC_DEFINE(_NETBSD_SOURCE, [1], [Define to 1 if you are on NetBSD.])
fi

if test "$solaris" = "yes"; then
  AC_DEFINE(_XPG4_2, [1],
            [Define to 1 for msghdr.msg_control if you are on Solaris.])
  AC_DEFINE(__EXTENSIONS__, [1],
            [Define to 1 for singal.h with _XPG4_2 if you are on Solaris.])
fi

# AIO
AC_ARG_ENABLE(aio,
  [AS_HELP_STRING([--enable-aio],
                  [use aio/dio based cache. [default=no]])],
  ,
  [enable_aio="no"])
if test "x$enable_aio" != "xno"; then
  AC_DEFINE(_XOPEN_SOURCE, [600], [Define to 600 for use AIO.])
  AC_CHECK_LIB(rt, nanosleep, [], [AC_MSG_ERROR("No librt found")])
  AC_DEFINE(USE_AIO, [1], [use AIO/DIO])
fi

AC_CHECK_LIB(m, log, [], [AC_MSG_ERROR("No libm found")])
AC_MSG_CHECKING([for fpclassify])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM(
    [#include <math.h>],
    [if (fpclassify(0.0)) {return 0;}]
   )],
  [
    AC_DEFINE(HAVE_FPCLASSIFY, [1], [use fpclassify])
    AC_MSG_RESULT(yes)
  ],
  [
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
	[#define _ISOC99_SOURCE
         #include <math.h>],
	[if (fpclassify(0.0)) {return 0;}]
       )],
      [
        AC_DEFINE(_ISOC99_SOURCE, [1], [Define to 1 for fpclassify])
	AC_DEFINE(HAVE_FPCLASSIFY, [1], [use fpclassify with _ISOC99_SOURCE])
	AC_MSG_RESULT(yes)
      ],
      [
        AC_MSG_RESULT(no)
      ])
  ])

AC_CHECK_HEADERS(sys/mman.h sys/time.h sys/timeb.h sys/param.h sys/types.h sys/resource.h)
AC_CHECK_HEADERS(netdb.h sys/wait.h sys/socket.h netinet/in.h netinet/tcp.h)
AC_CHECK_HEADERS(ucontext.h signal.h errno.h execinfo.h sys/sysctl.h)
AC_CHECK_HEADERS(time.h)
AC_CHECK_FUNCS(localtime_r gmtime_r)
BACKTRACE_LIBS=
AC_CHECK_FUNCS(backtrace,
 [],
 [AC_CHECK_LIB([execinfo], [backtrace],
   [BACKTRACE_LIBS="-lexecinfo"
    AC_DEFINE(HAVE_BACKTRACE,
      [1],
      [Define to 1 if you have the `backtrace' function.])
   ])
 ])
AC_SUBST(BACKTRACE_LIBS)
AC_CHECK_LIB(rt, clock_gettime, [
  RT_LIBS="-lrt"
  AC_DEFINE(HAVE_CLOCK_GETTIME, [1], [use clock_gettime])
], [])
AC_SYS_LARGEFILE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_SIZEOF(off_t)

# log path
AC_ARG_WITH(log_path,
  [AS_HELP_STRING([--with-log-path=PATH],
    [specify groonga log path.])],
  grn_log_path="$withval",
  grn_log_path="\$(localstatedir)/log/\$(PACKAGE_NAME)/\$(PACKAGE_NAME).log")
AC_SUBST(grn_log_path)

# default encoding
AC_ARG_WITH(default_encoding,
  [AS_HELP_STRING([--with-default-encoding=ENCODING],
    [specify groonga default encoding(euc_jp/sjis/utf8/latin1/koi8r/none)])],
  GRN_DEFAULT_ENCODING="$withval",
  GRN_DEFAULT_ENCODING="utf8")
AC_DEFINE_UNQUOTED(GRN_DEFAULT_ENCODING, "$GRN_DEFAULT_ENCODING", "specified default encoding")

# default match escalation threshold
AC_ARG_WITH(match_escalation_threshold,
  [AS_HELP_STRING([--with-match-escalation-threshold=NUMBER],
    [specify groonga default match escalation threshold])],
  GRN_DEFAULT_MATCH_ESCALATION_THRESHOLD="$withval",
  GRN_DEFAULT_MATCH_ESCALATION_THRESHOLD="0")
AC_DEFINE_UNQUOTED(GRN_DEFAULT_MATCH_ESCALATION_THRESHOLD, $GRN_DEFAULT_MATCH_ESCALATION_THRESHOLD, "specified match escalation threshold")

if test "$os_win32" != "yes"; then
  AC_CHECK_HEADERS(pthread.h)
  AC_CHECK_LIB(pthread, pthread_mutex_init,
               [PTHREAD_LIBS="-lpthread"],
               [AC_MSG_ERROR("No libpthread found")])
fi
AC_CHECK_LIB(nsl, gethostbyname, [NSL_LIBS="-lnsl"])
AC_CHECK_LIB(socket, socket, [SOCKET_LIBS="-lsocket"])
AC_CHECK_LIB(dl, dlopen, [DL_LIBS="-ldl"])

# nfkc
AC_ARG_ENABLE(nfkc,
  [AS_HELP_STRING([--enable-nfkc],
    [use nfkc based utf8 normalization. [default=yes]])],,
  [enable_nfkc="yes"])
if test "x$enable_nfkc" != "xyes"; then
  AC_DEFINE(NO_NFKC, [1], [compile without nfkc.c])
fi

# coverage
m4_ifdef([AC_CHECK_COVERAGE], [AC_CHECK_COVERAGE])
GENHTML_OPTIONS="--title 'groonga Code Coverage'"

# microyield
AC_MSG_CHECKING([whether enable uyield])
AC_ARG_ENABLE(uyield,
  [AS_HELP_STRING([--enable-uyield],
    [build for detecting race conditions. [default=no]])],
  ,
  [enable_uyield="no"])
AC_MSG_RESULT($enable_uyield)

## malloc
force_enable_dynamic_malloc_change="no"

# exact-alloc-count
AC_MSG_CHECKING([whether enable exact-alloc-count])
AC_ARG_ENABLE(exact-alloc-count,
  [AS_HELP_STRING([--enable-exact-alloc-count],
    [atomic counting for memory alloc count. [default=yes]])],,
  [enable_exact_alloc_count="yes"])
if test "x$enable_exact_alloc_count" != "xno"; then
  AC_DEFINE(USE_EXACT_ALLOC_COUNT, [1], [alloc_count with atomic])
fi
AC_MSG_RESULT($enable_exact_alloc_count)

# failmalloc
AC_MSG_CHECKING([whether enable fmalloc])
AC_ARG_ENABLE(fmalloc,
  [AS_HELP_STRING([--enable-fmalloc],
    [make memory allocation failed in specified condition for debug. [default=no]])],
  ,
  [enable_fmalloc="no"])
if test "x$enable_fmalloc" != "xno"; then
  force_enable_dynamic_malloc_change="yes"
  AC_DEFINE(USE_FAIL_MALLOC, [1], [use fmalloc])
fi
AC_MSG_RESULT($enable_fmalloc)

# abort
AC_MSG_CHECKING([whether enable abort])
AC_ARG_ENABLE(abort,
  [AS_HELP_STRING([--enable-abort],
    [enable query abortion. [default=no]])],
  ,
  [enable_abort="no"])
if test "x$enable_abort" != "xno"; then
  force_enable_dynamic_malloc_change="yes"
  AC_DEFINE(USE_QUERY_ABORT, [1], [use abort])
fi
AC_MSG_RESULT($enable_abort)

# dynamic malloc change
AC_MSG_CHECKING([whether allow dynamic memory allocation change])
AC_ARG_ENABLE(dynamic-malloc-change,
  [AS_HELP_STRING([--enable-dynamic-malloc-change],
    [allow dynamic memory allocation change for testing. [default=no]])],
  ,
  [enable_dynamic_malloc_change="no"])
if test "x$enable_dynamic_malloc_change" != "xyes" -a \
     "x$force_enable_dynamic_malloc_change" = "xyes"; then
  enable_dynamic_malloc_change="yes"
  AC_MSG_RESULT([$enable_dynamic_malloc_change (force)])
else
  AC_MSG_RESULT([$enable_dynamic_malloc_change])
fi

if test "x$enable_dynamic_malloc_change" = "xyes"; then
  AC_DEFINE(USE_DYNAMIC_MALLOC_CHANGE, [1],
            [Define to 1 if you enable dynamic malloc change])
fi

# memory debug
AC_MSG_CHECKING([whether debug memory management])
AC_ARG_ENABLE(memory-debug,
  [AS_HELP_STRING([--enable-memory-debug],
    [debug memory management. [default=no]])],
  ,
  [enable_memory_debug="no"])
AC_MSG_RESULT([$enable_memory_debug])

if test "x$enable_memory_debug" = "xyes"; then
  AC_DEFINE(ENABLE_MEMORY_DEBUG, [1],
            [Define to 1 if you enable debuging memory management])
fi

# epoll/kqueue/poll/select check
AC_CHECK_HEADER(sys/epoll.h, [
  AC_CHECK_FUNC(epoll_create, [
    AC_TRY_RUN([
#include <sys/epoll.h>
int main(int argc, char **argv) { return (epoll_create(16) < 0); }
    ],
    [
      have_epoll="yes"
      AC_DEFINE(USE_EPOLL, [1], [use epoll])
    ]
    )
  ])
])

if test "x$have_epoll" != "xyes"; then
  AC_CHECK_HEADER(sys/event.h, [
    AC_CHECK_FUNC(kevent, [
      have_kqueue="yes"
      AC_DEFINE(USE_KQUEUE, [1], [use kqueue])
    ])
  ])
  if test "x$have_kqueue" != "xyes"; then
    AC_CHECK_HEADER(sys/poll.h, [
      AC_CHECK_FUNC(poll, [
        have_poll="yes"
        AC_DEFINE(USE_POLL, [1], [use poll])
      ])
    ])
    if test "x$have_poll" != "xyes"; then
      if test "$os_win32" = "yes"; then
	AC_CHECK_HEADER(winsock2.h, [have_select="yes"])
      else
        AC_CHECK_FUNC(select, [
          have_select="yes"
          AC_CHECK_HEADERS(sys/select.h)
        ])
      fi
      if test "x$have_select" = "xyes"; then
        AC_DEFINE(USE_SELECT, [1], [use select])
      else
        AC_MSG_ERROR([epoll/kqueue/poll/select is missing.])
      fi
    fi
  fi
fi

# check MSG_MORE defined
AC_MSG_CHECKING([whether MSG_MORE defined])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
  return MSG_MORE;
}
    ])],
    [
      AC_MSG_RESULT(yes)
      AC_DEFINE(USE_MSG_MORE, [1], [use MSG_MORE])
    ],
    [
      AC_MSG_RESULT(no)
    ])

# check MSG_NOSIGNAL defined
AC_MSG_CHECKING([whether MSG_NOSIGNAL defined])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
  return MSG_NOSIGNAL;
}
    ])],
    [
      AC_MSG_RESULT(yes)
      AC_DEFINE(USE_MSG_NOSIGNAL, [1], [use MSG_NOSIGNAL])
    ],
    [
      AC_MSG_RESULT(no)
    ])

# MinGW
if test "$os_win32" = "yes"; then
  WINDOWS_LDFLAGS="-mwindows"
  WINDOWS_LIBS="-ladvapi32 -lws2_32"
else
  WINDOWS_LDFLAGS=
  WINDOWS_LIBS=
fi
AC_SUBST(WINDOWS_LDFLAGS)
AC_SUBST(WINDOWS_LIBS)

# groonga binary path
GROONGA="\$(abs_top_builddir)/src/groonga"
AC_SUBST(GROONGA)

# grntest binary path
GRNTEST="\$(abs_top_builddir)/src/grntest"
AC_SUBST(GRNTEST)

# check Cutter with GLib support if available
REQUIRED_MINIMUM_CUTTER_VERSION=1.1.6
REQUIRED_MINIMUM_CPPCUTTER_VERSION=1.2.0
m4_ifdef([AC_CHECK_GCUTTER],
         [AC_CHECK_GCUTTER(>= $REQUIRED_MINIMUM_CUTTER_VERSION)],
         [cutter_use_cutter="no"])
m4_ifdef([AC_CHECK_CPPCUTTER],
         [AC_CHECK_CPPCUTTER(>= $REQUIRED_MINIMUM_CPPCUTTER_VERSION)],
         [cutter_use_cppcutter="no"])

AM_CONDITIONAL([WITH_CUTTER], [test "$cutter_use_cutter" = "yes"])
AM_CONDITIONAL([WITH_CPPCUTTER], [test "$cutter_use_cppcutter" = "yes"])
if test "$cutter_use_cutter" = "yes"; then
  AC_DEFINE(WITH_CUTTER, 1, [Define to 1 if you use Cutter])

  ac_cv_have_libmemcached=no
  AC_CHECK_HEADER(libmemcached/memcached.h,
                  [AC_CHECK_LIB(memcached, memcached_increment_with_initial,
                                [ac_cv_have_libmemcached=yes])])
  AM_CONDITIONAL([WITH_LIBMEMCACHED],
                 [test "$ac_cv_have_libmemcached" = "yes"])
  if test "$ac_cv_have_libmemcached" = "yes"; then
    AC_DEFINE(WITH_LIBMEMCACHED, 1, [Define to 1 if you use libmemcached])
  fi
else
  AM_CONDITIONAL([WITH_LIBMEMCACHED], false)
fi

# check for benchmark
AC_ARG_ENABLE(benchmark,
  [AS_HELP_STRING([--disable-benchmark],
    [don't build benchmark programs.])],,
  [enable_benchmark="yes"])
if test "x$enable_benchmark" = "xno"; then
  benchmark_available="no"
else
  REQUIRED_GLIB_VERSION=2.8.0
  m4_ifdef([AM_PATH_GLIB_2_0],
           [AM_PATH_GLIB_2_0($REQUIRED_GLIB_VERSION,
                             [benchmark_available="yes"],
                             [benchmark_available="no"],
                             [gobject gthread])],
           [benchmark_available="no"])
  AC_MSG_CHECKING(for benchmark availablity)
  AC_MSG_RESULT($ac_benchmark_available)
fi
if test "$benchmark_available" = "yes"; then
  AC_DEFINE(WITH_BENCHMARK, 1, [Define to 1 if benchamrk is available])
fi
AM_CONDITIONAL([WITH_BENCHMARK], [test "$benchmark_available" = "yes"])

# check Ruby for HTTP test
ac_cv_ruby_available="no"
AC_ARG_WITH([ruby],
            AS_HELP_STRING([--with-ruby=PATH],
                           [Ruby interpreter path (default: auto-detect)]),
            [RUBY="$withval"])

if test "$RUBY" = "no"; then
  : # ignore
elif test "$RUBY" = ""; then
  AC_PATH_PROG(RUBY, ruby, none)
  if test "$RUBY" != "none"; then
    ac_cv_ruby_available="yes"
  fi
else
  AC_CHECK_FILE([$RUBY],
                [ac_cv_ruby_available="yes"],
                [AC_MSG_WARN([$RUBY is not found. Disable HTTP test.])])
fi
AC_SUBST(RUBY)
AM_CONDITIONAL([WITH_RUBY], [test "$ac_cv_ruby_available" = "yes"])

AM_CONDITIONAL([WITH_UNIT_TEST],
               [test "$cutter_use_cutter" = "yes" -o \
                     "$ac_cv_ruby_available" = "yes"])

# libedit
AC_ARG_ENABLE(libedit,
  [AS_HELP_STRING([--disable-libedit],
    [use libedit for console. [default=auto-detect]])],
  [enable_libedit="$enableval"],
  [enable_libedit="auto"])
if test "x$enable_libedit" != "xno"; then
  m4_ifdef([PKG_CHECK_MODULES], [
    PKG_CHECK_MODULES([LIBEDIT], [libedit >= 3.0],
      [LDFLAGS_SAVE="$LDFLAGS"
       LDFLAGS="$LIBEDIT_LIBS $LDFLAGS"
       AC_CHECK_LIB(edit, el_wline,
         [libedit_available=yes],
         [libedit_available=no])
       LDFLAGS="$LDFLAGS_SAVE"],
      [libedit_available=no])
    ],
    [libedit_available=no])
  if test "x$libedit_available" = "xyes"; then
    AC_DEFINE(HAVE_LIBEDIT, [1], [Use libedit with multibyte support.])
  else
    if test "x$enable_editline" = "xyes"; then
      AC_MSG_ERROR("No libedit found")
    fi
  fi
fi

# zlib
AC_ARG_WITH(zlib,
  [AS_HELP_STRING([--with-zlib],
    [use zlib for data compression. [default=no]])],
  [with_zlib="$withval"],
  [with_zlib="no"])
if test "x$with_zlib" = "xno"; then
  AC_DEFINE(NO_ZLIB, [1], [without zlib])
else
  AC_CHECK_LIB(z, compress, [ZLIB_LIBS="-lz"], [AC_MSG_ERROR("No libz found")])
fi

# lzo
AC_ARG_WITH(lzo,
  [AS_HELP_STRING([--with-lzo],
    [use lzo for data compression. [default=no]])],
  [with_lzo="$withval"],
  [with_lzo="no"])
if test "x$with_lzo" = "xno"; then
  AC_DEFINE(NO_LZO, [1], [without lzo])
else
  AC_CHECK_LIB(lzo2, lzo1_compress, [LZO_LIBS="-llzo2"], [AC_MSG_ERROR("No liblzo2 found")])
fi

# mecab
# NOTE: MUST be checked last
AC_ARG_WITH(mecab,
  [AS_HELP_STRING([--with-mecab],
    [use mecab for morphological analysis. [default=yes]])],
  [with_mecab="$withval"],
  [with_mecab="yes"])
AC_MSG_CHECKING([whether enable MeCab])
AC_MSG_RESULT($with_mecab)
if test "x$with_mecab" = "xyes"; then
  # mecab-config
  AC_ARG_WITH(mecab-config,
    [AS_HELP_STRING([--with-mecab-config=PATH],
      [set mecab-config location. [default=auto-detect]])],
    [if test "$cross_compiling" = "yes"; then
       MECAB_CONFIG="$withval"
     else
       AC_CHECK_FILE("$withval", MECAB_CONFIG="$withval", MECAB_CONFIG=no)
     fi],
    [AC_PATH_PROG(MECAB_CONFIG, mecab-config, no)])
  if test "x$MECAB_CONFIG" = "xno"; then
    with_mecab="no"
  else
    MECAB_CPPFLAGS="-I`$MECAB_CONFIG --inc-dir`"
    MECAB_LDFLAGS="-L`$MECAB_CONFIG --libs-only-L`"
    _SAVE_LIBS="$LIBS"
    _SAVE_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS $MECAB_LDFLAGS"
    AC_SEARCH_LIBS(mecab_new,
                   mecab,
                   [MECAB_LIBS="-lmecab $PTHREAD_LIBS"],
                   [AC_MSG_ERROR("No libmecab found")],
                   $PTHREAD_LIBS)
    LDFLAGS="$_SAVE_LDFLAGS"
    LIBS="$_SAVE_LIBS"
    _SAVE_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $MECAB_CPPFLAGS"
    AC_CHECK_HEADER(mecab.h, , [AC_MSG_ERROR("No mecab.h found")])
    AC_CHECK_TYPE([mecab_dictionary_info_t],
                  [AC_DEFINE([HAVE_MECAB_DICTIONARY_INFO_T],
                             [1],
                             [Define to 1 if MeCab has the type `mecab_dictionary_info_t'.])],
                  [],
                  [[#include <mecab.h>]])
    CPPFLAGS="$_SAVE_CPPFLAGS"
    AC_SUBST(MECAB_CPPFLAGS)
    AC_SUBST(MECAB_LDFLAGS)
    AC_SUBST(MECAB_LIBS)
  fi
fi
if test "x$with_mecab" = "xyes"; then
  AC_DEFINE(WITH_MECAB, [1], [use MeCab])
fi
AM_CONDITIONAL(WITH_MECAB, test "x$with_mecab" = "xyes")

# futex check
AC_ARG_ENABLE(futex,
  [AS_HELP_STRING([--enable-futex],
    [use futex. [default=no]])],
  ,
  [enable_futex="no"])
if test "x$enable_futex" != "xno"; then
  AC_CHECK_HEADERS(linux/futex.h sys/syscall.h, [
    AC_DEFINE(USE_FUTEX, [1], [use futex])
  ], [
    AC_MSG_ERROR("linux/futex.h or sys/syscall.h not found")
  ])
fi
AC_MSG_CHECKING([whether enable futex])
AC_MSG_RESULT($enable_futex)

# ZeroMQ
AC_ARG_ENABLE(zeromq,
  [AS_HELP_STRING([--disable-zeromq],
    [Disable ZeroMQ used for suggestion. [default=auto-detect]])],
  [enable_zeromq="$enableval"],
  [enable_zeromq="auto"])
if test "x$enable_zeromq" != "xno"; then
  m4_ifdef([PKG_CHECK_MODULES], [
    PKG_CHECK_MODULES([LIBZMQ],
                      [libzmq],
                      [zeromq_available=yes],
                      [zeromq_available=no])
    ],
    [zeromq_available=no])
  if test "x$zeromq_available" = "xyes"; then
    AC_DEFINE(HAVE_ZEROMQ, [1], [Define to 1 if ZeroMQ is available.])
  else
    if test "x$enable_zeromq" = "xyes"; then
      AC_MSG_ERROR("No ZeroMQ found")
    fi
  fi
fi

# libevent
AC_ARG_WITH(libevent,
  [AS_HELP_STRING([--without-libevent],
    [Disable libevent used for suggestion. [default=/usr]])],
  [with_libevent="$withval"],
  [with_libevent="/usr"])
if test "x$with_libevent" != "xno"; then
  _SAVE_CFLAGS="$CFLAGS"
  _SAVE_LDFLAGS="$LDFLAGS"
  CFLAGS="$CFLAGS -I$with_libevent/include"
  LDFLAGS="$LDFLAGS -L$with_libevent/lib"
  AC_CHECK_LIB(event, event_init,
               [libevent_available=yes],
               [libevent_available=no])
  CFLAGS="$_SAVE_CFLAGS"
  LDFLAGS="$_SAVE_LDFLAGS"
  if test "x$libevent_available" = "xyes"; then
    AC_DEFINE(HAVE_LIBEVENT, [1], [Define to 1 if libevent is available.])
    LIBEVENT_CFLAGS="-I$with_libevent/include"
    LIBEVENT_LIBS="-L$with_libevent/lib -levent"
  else
    if test "x$with_libevent" = "xyes"; then
      AC_MSG_ERROR("No libevent found")
    fi
  fi
fi
AC_SUBST(LIBEVENT_CFLAGS)
AC_SUBST(LIBEVENT_LIBS)

# MessagePack
AC_ARG_WITH(message-pack,
  [AS_HELP_STRING([--without-message-pack],
    [Disable MessagePack used for suggestion. [default=/usr]])],
  [with_message_pack="$withval"],
  [with_message_pack="/usr"])
if test "x$with_message_pack" != "xno"; then
  _SAVE_CFLAGS="$CFLAGS"
  _SAVE_LDFLAGS="$LDFLAGS"
  CFLAGS="$CFLAGS -I$with_message_pack/include"
  LDFLAGS="$LDFLAGS -L$with_message_pack/lib"
  AC_CHECK_LIB(msgpack, msgpack_version,
               [message_pack_available=yes],
               [message_pack_available=no])
  CFLAGS="$_SAVE_CFLAGS"
  LDFLAGS="$_SAVE_LDFLAGS"
  if test "x$message_pack_available" = "xyes"; then
    AC_DEFINE(HAVE_MESSAGE_PACK, [1], [Define to 1 if MessagePack is available.])
    MESSAGE_PACK_CFLAGS="-I$with_message_pack/include"
    MESSAGE_PACK_LIBS="-L$with_message_pack/lib -lmsgpack"
  else
    if test "x$with_message_pack" = "xyes"; then
      AC_MSG_ERROR("No MessagePack found")
    fi
  fi
fi
AC_SUBST(MESSAGE_PACK_CFLAGS)
AC_SUBST(MESSAGE_PACK_LIBS)

AM_CONDITIONAL([ENABLE_SUGGEST_LEARNER],
               [test "$zeromq_available" = "yes" -a \
                     "$libevent_available" = "yes" -a \
                     "$message_pack_available" = "yes"])

# Document
AC_MSG_CHECKING([whether enable document])
AC_ARG_ENABLE(document,
  [AS_HELP_STRING([--enable-document],
    [enable document generation by Sphinx. [default=auto]])],
  [enable_document="$enableval"],
  [enable_document="auto"])
AC_MSG_RESULT($enable_document)

document_available=no
if test x"$enable_document" != x"no"; then
  if test -f "$srcdir/doc/build-stamp"; then
    document_available=yes
  else
    AC_PATH_PROG(HG, hg, hg-not-found)
    if test x"$HG" = x"hg-not-found"; then
      if test x"$enable_document" = x"yes"; then
        AC_MSG_ERROR("No hg found")
      fi
    else
      document_available=yes
    fi
    AC_SUBST(HG)
  fi
fi

AM_CONDITIONAL([ENABLE_DOCUMENT],
               [test "${document_available}" = "yes"])
AC_MSG_CHECKING([whether document available])
AC_MSG_RESULT($document_available)

# Munin plugins
AC_MSG_CHECKING([whether install munin plugins])
AC_ARG_WITH(munin-plugins,
  [AS_HELP_STRING([--with-munin-plugins],
    [install Munin plugins. [default=no]])],
  [install_munin_plugins="$withval"],
  [install_munin_plugins="no"])
AC_MSG_RESULT($install_munin_plugins)

AM_CONDITIONAL([INSTALL_MUNIN_PLUGINS],
               [test "${install_munin_plugins}" = "yes"])

# platform
AC_MSG_CHECKING([whether package platform])
AC_ARG_WITH(package-platform,
  [AS_HELP_STRING([--with-package-platform=PLATFORM],
    [install package platform related files. [default=no]
     (supported package platforms: redhat)])],
  [package_platform="$withval"],
  [package_platform="no"])
AC_MSG_RESULT($package_platform)

AM_CONDITIONAL([REDHAT_PLATFORM],
               [test "${package_platform}" = "redhat"])

# plugins check
relative_pluginsdir="\$(PACKAGE)/plugins"
AC_SUBST(relative_pluginsdir)

pluginsdir="\${libdir}/\$(relative_pluginsdir)"
AC_SUBST(pluginsdir)

tokenizer_pluginsdir="\${pluginsdir}/tokenizers"
AC_SUBST(tokenizer_pluginsdir)

suggest_pluginsdir="\${pluginsdir}/suggest"
AC_SUBST(suggest_pluginsdir)

AC_MSG_CHECKING(for the suffix of plugin shared libraries)
shrext_cmds=$(./libtool --config | grep '^shrext_cmds=')
eval $shrext_cmds
module=yes eval suffix="$shrext_cmds"
AC_MSG_RESULT($suffix)
if test -z "$suffix"; then
  AC_MSG_ERROR([can't detect plugin suffix])
fi
AC_DEFINE_UNQUOTED(GRN_PLUGIN_SUFFIX, ["$suffix"], "plugin suffix")

# for examples
examplesdir="\$(pkgdatadir)/examples"
AC_SUBST(examplesdir)

examples_dictionarydir="\$(examplesdir)/dictionary"
AC_SUBST(examples_dictionarydir)

# flags for compile groonga
GRN_CFLAGS=""
AC_SUBST(GRN_CFLAGS)
GRN_DEFS=""
GRN_DEFS="$GRN_DEFS -DGRN_DLL_FILENAME=L\\\"\"\$(GRN_DLL_FILENAME)\"\\\""
GRN_DEFS="$GRN_DEFS -DGRN_PLUGINS_DIR=\\\"\"\$(pluginsdir)\"\\\""
GRN_DEFS="$GRN_DEFS -DGRN_RELATIVE_PLUGINS_DIR=\\\"\"lib/\$(relative_pluginsdir)\"\\\""
GRN_DEFS="$GRN_DEFS -DGRN_LOG_PATH=\\\"\"\$(grn_log_path)\"\\\""
GRN_DEFS="$GRN_DEFS -DGRN_DEFAULT_DOCUMENT_ROOT=\\\"\"\$(pkgdatadir)/html/admin\"\\\""
GRN_DEFS="$GRN_DEFS -DGRN_DEFAULT_RELATIVE_DOCUMENT_ROOT=\\\"\"share/\$(PACKAGE)/html/admin\"\\\""
AC_SUBST(GRN_DEFS)
CFLAGS="$CFLAGS $OPT_CFLAGS "
LIBS="$LIBS $ZLIB_LIBS $LZO_LIBS $BACKTRACE_LIBS $RT_LIBS $PTHREAD_LIBS"
LIBS="$LIBS $NSL_LIBS $SOCKET_LIBS $DL_LIBS $WINDOWS_LIBS"
AC_DEFINE_UNQUOTED(CONFIGURE_OPTIONS, "$ac_configure_args", "specified configure options")

# For groonga.org
AC_ARG_WITH(groonga-github-com-path,
            [AS_HELP_STRING([--with-groonga-github-com-path=PATH],
            [specify groonga.github.com path to update groonga.org.])],
            [GROONGA_GITHUB_COM_PATH="$withval"],
            [GROONGA_GITHUB_COM_PATH=""])
AC_SUBST(GROONGA_GITHUB_COM_PATH)

# For Debian package release
AC_ARG_WITH(rsync-path,
            [AS_HELP_STRING([--with-rsync-path=PATH],
            [specify rsync path to upload groonga packages.])],
            [RSYNC_PATH="$withval"],
            [RSYNC_PATH=""])
AC_SUBST(RSYNC_PATH)

pkgsysconfdir="\$(sysconfdir)/$PACKAGE_NAME"
AC_SUBST(pkgsysconfdir)

GRN_CONFIG_PATH="`
  test \"$prefix\" = NONE && prefix=
  eval echo ${sysconfdir}/groonga/groonga.conf
`"
AC_DEFINE_UNQUOTED(GRN_CONFIG_PATH, ["$GRN_CONFIG_PATH"],
  [Default command line option configuration file.])

AC_OUTPUT([
  packages/rpm/centos/groonga.spec
  packages/rpm/fedora/groonga.spec
  groonga.pc
  ])

echo "$PACKAGE_NAME $PACKAGE_VERSION configuration:"
echo "-----------------------"
echo "  Compiler:              ${CC}"
echo "  CFLAGS:                ${CFLAGS}"
echo "  CXXFLAGS:              ${CXXFLAGS}"
echo "  Libraries:             ${LIBS}"
echo "  Documentation:         ${document_available}"
echo "  Munin plugins:         ${install_munin_plugins}"
echo "  Package platform:      ${package_platform}"
echo
echo "Paths:"
echo "  Install path prefix:   ${prefix}"
echo

echo "Tokenizers:"
echo "  MeCab:                 $with_mecab"
if test "x$with_mecab" = "xyes"; then
  echo "    CPPFLAGS:            $MECAB_CPPFLAGS"
  echo "    LDFLAGS:             $MECAB_LDFLAGS"
  echo "    LIBS:                $MECAB_LIBS"
fi
echo

echo "Libraries:"
echo "  ZeroMQ:                $zeromq_available"
if test "x$zeromq_available" = "xyes"; then
  echo "    CFLAGS:              ${LIBZMQ_CFLAGS}"
  echo "    LIBS:                ${LIBZMQ_LIBS}"
fi
echo "  libevent:              $libevent_available"
if test "x$libevent_available" = "xyes"; then
  echo "    CFLAGS:              ${LIBEVENT_CFLAGS}"
  echo "    LIBS:                ${LIBEVENT_LIBS}"
fi
echo "  MessagePack:           $message_pack_available"
if test "x$message_pack_available" = "xyes"; then
  echo "    CFLAGS:              ${MESSAGE_PACK_CFLAGS}"
  echo "    LIBS:                ${MESSAGE_PACK_LIBS}"
fi
echo

echo "Tools:"
# echo "  Sphinx:                ${enable_sphinx}"
echo "  hg:                    ${HG}"
echo "  Ruby:                  ${RUBY}"
echo "  Cutter:                ${CUTTER}"
echo

echo "Now type 'make' to build $PACKAGE_NAME $PACKAGE_VERSION!"
