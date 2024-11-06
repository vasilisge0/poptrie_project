#
# Copyright (c) 2016,2018 Hirochika Asai.
# All rights reserved.
#
# Authors:
#      Hirochika Asai  <asai@jar.jp>
#

EXTRA_DIST = tests/linx-rib.20141217.0000-p46.txt
bin_PROGRAMS = radix_test_basic
radix_test_basic_SOURCES = tests/basic.c radix.c radix.h

CLEANFILES = *~

test: all
	@echo "Testing all..."
	$(top_builddir)/radix_test_basic
