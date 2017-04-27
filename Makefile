
EXE = dailyfile

OBJ = dailyfile.o

CROSS_COMPILE ?=
CC             = gcc
CFLAGS         = -Wall -W -DPROGRAM_VERSION="$(shell git describe 2>/dev/null | tr -d '\r\n')"
LDFLAGS        = 

.PHONY: all

all: $(EXE)

$(EXE): $(OBJ)
	$(CROSS_COMPILE)$(CC) -o $(EXE) $(OBJ) $(LDFLAGS)

%.o:%.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c  $<

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ) *~

