
INCLUDE_FILES = message.h
BUILD_FILES = server.c client.c
TARGET_FILES = server client

CC = gcc -Wall -O0

all: $(BUILD_FILES) 
	$(CC) server.c -o server -lpthread
	$(CC) client.c -o client -lpthread

clean: 
		rm -f $(TARGET_FILES)