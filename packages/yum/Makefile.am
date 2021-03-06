SERVER_PATH = $(RSYNC_PATH)
DISTRIBUTIONS = fedora centos
ARCHITECTURES = i386 x86_64
CHROOT_BASE = /var/lib/chroot
BASE_URL_PREFIX = http://packages.groonga.org
HAVE_DEVELOPMENT_BRANCH = no
USE_RPMFORGE = no
USE_ATRPMS = no

release: download build sign update upload

ensure-rsync-path:
	@if test -z "$(RSYNC_PATH)"; then				\
	  echo "--with-rsync-path configure option must be specified.";	\
	  false;							\
	fi

sign:
	./sign-rpm.sh '$(DISTRIBUTIONS)'

update:
	./update-repository.sh $(PACKAGE) '$(DISTRIBUTIONS)'

upload: ensure-rsync-path
	for distribution in $(DISTRIBUTIONS); do		\
	  rsync -avz --delete --exclude .gitignore		\
	    $${distribution}/ $(SERVER_PATH)/$${distribution};	\
	done

download: ensure-rsync-path
	for distribution in $(DISTRIBUTIONS); do		\
	  (rm -rf $${distribution} &&				\
	   mkdir -p $${distribution} &&				\
	   rsync -avz --delete					\
	     $(SERVER_PATH)/$${distribution}/ $${distribution});\
	done

build: build-in-chroot build-repository-rpm

build-in-chroot: source
	./build-in-chroot.sh			\
	  $(PACKAGE)				\
	  $(VERSION)				\
	  $(top_builddir)/$(PACKAGE)		\
	  ../rpm				\
	  $(CHROOT_BASE)			\
	  '$(ARCHITECTURES)'			\
	  '$(DISTRIBUTIONS)'			\
	  '$(HAVE_DEVELOPMENT_BRANCH)'		\
	  '$(USE_RPMFORGE)'			\
	  '$(USE_ATRPMS)'

build-repository-rpm: RPM-GPG-KEY-$(PACKAGE)
	./build-repository-rpm.sh		\
	  $(PACKAGE)				\
	  '$(PACKAGE_TITLE)'			\
	  $(BASE_URL_PREFIX)			\
	  '$(DISTRIBUTIONS)'			\
	  $(HAVE_DEVELOPMENT_BRANCH)

RPM-GPG-KEY-$(PACKAGE):
	./gpg-public-key.sh > $@

source: ../$(PACKAGE)-$(VERSION).tar.gz

../$(PACKAGE)-$(VERSION).tar.gz: $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	ln -s $(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz ../

$(abs_top_builddir)/$(PACKAGE)-$(VERSION).tar.gz:
	cd $(abs_top_builddir) && make dist
