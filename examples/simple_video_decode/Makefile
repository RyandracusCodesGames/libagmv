CC = gcc
AGMV_INCLUDE_DIR = -I"$(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../include)"
AGMV_LIB_DIR = -L"$(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../libs)"
CFLAGS = -Wall -O2 $(AGMV_INCLUDE_DIR)
LDFLAGS = $(AGMV_LIB_DIR) -lagmv
OBJS = simple_video_decode.o
TARGET = main

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	
clean:
	rm -f $(OBJS) $(TARGET) *~