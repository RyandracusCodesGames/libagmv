CC = gcc
INCLUDES = -I"C:/Users/ryand/OneDrive/Desktop/AGIDL/AGIDL_Proj/include" -I"C:/MinGW/include" -I"C:/Users/ryand/OneDrive/Desktop/AGMV/include" -I"$(CURDIR)/include"
CFLAGS = -Wall -O2 $(INCLUDES)
LDFLAGS = -L"C:/Users/ryand/OneDrive/Desktop/AGIDL/AGIDL_Proj/lib" -L"C:/Users/ryand/OneDrive/Desktop/AGMV/libs" -L"C:/MinGW/lib" -static-libgcc -lagidl -lagmv -lgdi32 -lwinmm -lcomdlg32 -lshlwapi
OBJS = agmvp.o src/screen.o agmvp_res.res
DEPS = include/screen.h
TARGET = agmvp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	
agmvp_res.res: agmvp_res.rc 
	windres -i agmvp_res.rc --input-format=rc -o agmvp_res.res -O coff 
	
clean:
	rm -f $(OBJS) $(TARGET) *~