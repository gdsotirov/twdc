# Makefile for the Tumbleweed Developer's Contest Task
# Written by Georgi D. Sotirov <gdsotirov@dir.bg>
#

DEBUG = 1

CC = gcc
CXX = g++
ifeq ($(DEBUG), 1)
 CFLAGS = -Wall -ansi -pedantic -pipe -ggdb
else
 CFLAGS = -Wall -ansi -pedantic -pipe -O2
endif

CLIENT_NAME = client
SERVER_NAME = server

RM = rm
RMFLAGS = -f

.PHONY: all clean

all: $(CLIENT_NAME) $(SERVER_NAME)

$(CLIENT_NAME): client.c client.h globals.h protocol.h
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_NAME): server.c server.h globals.h protocol.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(RMFLAGS) $(CLIENT_NAME)
	$(RM) $(RMFLAGS) $(SERVER_NAME)

