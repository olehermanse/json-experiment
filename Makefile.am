# This Makefile provides 2 example targets, they both print argv, but
# one uses a libutils data structure to do it. Both binaries are
# built using size-optimizing flags and options for stripping dead code.
# On my system the binaries are 9KB (without libutils) and 11KB (with libutils).

# In my testing, trying to include individual files from libutils
# is actually worse than linking to libutils.la and letting the compiler/linker
# optimize for size and strip dead code.

# Run make inside libutils as well:
SUBDIRS = libntech

# Look for macros inside m4 directory:
ACLOCAL_AMFLAGS = -I m4 --install

# "Trick" to ensure m4 directory exists in source tarballs:
EXTRA_DIST = m4/NOTES

bin_PROGRAMS = \
	json_manip

# An example target using sequnce data structure from libutils:
json_manip_SOURCES = \
	src/json_manip.c

# Statically link to libutils:
json_manip_LDADD = libntech/libutils/.libs/libutils.la -lpcre
# Tell the c preprocessor (CPP) where to find headers:
json_manip_CPPFLAGS = -I libntech/libutils/ -I libntech/libcompat/
# Optimize for binary size and strip dead code:
json_manip_CFLAGS = -Os -fdata-sections -ffunction-sections
json_manip_LDFLAGS = $(STRIP_LDFLAGS) # Platform specific
