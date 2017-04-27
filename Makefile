#######################################################################
# Makefile for the dailyfile project.
#
# Copyright 2017  Christophe BLAESS (https://www.blaess.fr/christophe).
#
# License  GPLv3+:  This is free software:  you are free to  change and
# redistribute it.There is NO WARRANTY, to the extent permitted by law.
#
#######################################################################


EXE = dailyfile

OBJ = dailyfile.o

DIR = /usr/local/bin/

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


.PHONY: install
install: $(EXE)
	cp $(EXE) $(DIR)

.PHONY: uninstall
uninstall:
	rm -f $(DIR)$(EXE)
