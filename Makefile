# Makefile for the Tumbleweed Developer's Contest Task
# Written by Georgi D. Sotirov <gdsotirov@dir.bg>
# $Id: Makefile,v 1.6 2005/05/09 20:36:08 gsotirov Exp $
#

DEBUG = 1

CC = gcc
CXX = g++
ifeq ($(DEBUG), 1)
 CFLAGS = -Wall -std=c99 -pedantic -pipe -ggdb
else
 CFLAGS = -Wall -std=c99 -pedantic -pipe -O2
endif

CLIENT_NAME = client
SERVER_NAME = server

RM = rm
RMFLAGS = -f

.PHONY: all clean

COMMON_OBJS=protocol.o data.o

all: build

build: $(CLIENT_NAME) $(SERVER_NAME)

protocol.o: protocol.c protocol.h
	$(CC) $(CFLAGS) -o $@ -c $<

data.o: data.c data.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(CLIENT_NAME).o: client.c client.h globals.h protocol.h data.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(SERVER_NAME).o: server.c server.h globals.h protocol.h data.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(CLIENT_NAME): $(CLIENT_NAME).o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON_OBJS)

$(SERVER_NAME): $(SERVER_NAME).o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON_OBJS)

rebuild: clean build

clean:
	$(RM) $(RMFLAGS) *.o
	$(RM) $(RMFLAGS) $(CLIENT_NAME)
	$(RM) $(RMFLAGS) $(SERVER_NAME)

