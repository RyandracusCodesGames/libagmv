CC = gcc
INCLUDES = -I"$(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../include)"
CFLAGS = -Wall -O2 $(INCLUDES)
LIBS = -L"$(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../libs)" -lagmv
LDFLAGS = $(LIBS)
OBJS = agmvcli.o
TARGET = agmvcli 

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	
clean:
	rm -f $(TARGET) $(OBJS) *~