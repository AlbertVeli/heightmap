# All Rights Reversed - No Rights Reserved

eXe = hm
OBJS = hmpng.o main.o map.o parseargs.o texpng.o

# Extra program to merge two elevation datafiles
merge_eXe = merge_topo_bathy_tif
merge_OBJS = merge_topo_bathy_tif.o map.o

# Uncomment 2 lines below for macports
#EXTRA_INCLUDE = -I /opt/local/include
#EXTRA_LIBS = -L /opt/local/lib

# Uncomment line below to enable debug build
#DEBUG = yes

ifdef DEBUG
DBGFLAGS = -g -O0
else
DBGFLAGS = -O2 -march=native
endif

WFLAGS = -W -Wall -Werror

CFLAGS = $(WFLAGS) $(EXTRA_INCLUDE) $(DBGFLAGS) -pipe
LIBS = $(EXTRA_LIBS) -lpng

merge_LIBS = $(EXTRA_LIBS) -ltiff

all: $(eXe) $(merge_eXe) download

# Default rule. Only build hm.
$(eXe): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# "make merge_topo_bathy_tif" builds the extra program
$(merge_eXe): $(merge_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(merge_OBJS) $(merge_LIBS)

# Optional. Run sparse to statically analyze source code.
#
# To checkout sparse, run:
#
# git clone git://git.kernel.org/pub/scm/devel/sparse/sparse.git
#
# Then you can statically analyze source code for errors with:
#
# make sparse
#
# If you get an error like:
#
# error: attribute '__leaf__': unknown attribute
#
# Try the fix below:
#
# cd sparse
# make clean
# patch -p1 < ../Add-leaf-to-ignored-attributes.patch
#
# then run make sparse again
SPARSE = sparse/sparse
SPARSE_WARNINGS := -Wcast-to-as -Wdefault-bitfield-sign -Wdo-while -Wparen-string -Wptr-subtraction-blows  -Wreturn-void -Wshadow -Wtypesign  -Wbitwise
$(SPARSE):
	make -C sparse

.PHONY: clean distclean sparse sparse-clean download

download:
	@echo "Downloading datafiles"
	@make -C datafiles

sparse: $(SPARSE)
	$(SPARSE) $(SPARSE_WARNINGS) $(CFLAGS) *.c

distclean: clean sparse-clean

sparse-clean:
	make -C sparse clean

clean:
	rm -f $(eXe) $(OBJS) $(merge_eXe) $(merge_OBJS) *~
