# All Rights Reversed - No Rights Reserved

eXe = hm
OBJS = main.o map.o hmpng.o texpng.o

# Uncomment 2 lines below for macports
EXTRA_INCLUDE = -I /opt/local/include
EXTRA_LIBS = -L /opt/local/lib

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

$(eXe): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(eXe) $(OBJS) *~
