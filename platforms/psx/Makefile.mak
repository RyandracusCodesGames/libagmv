# ----------------------------
# PlayStation 1 Psy-Q MAKEFILE
# ----------------------------

LIBAGMV_INCLUDES = -Iinclude
OBJECTS = src/main.o \
		src/mem.o \
		src/file.o \
		src/decode.o \
		src/play.o
all:
	ccpsx -c -O3 $(LIBAGMV_INCLUDES) $(C_INCLUDE_PATH) src/main.c -o src/main.o
	ccpsx -c -O3 $(LIBAGMV_INCLUDES) $(C_INCLUDE_PATH) src/agmv_psx_mem_utils.c -o src/mem.o
	ccpsx -c -O3 $(LIBAGMV_INCLUDES) $(C_INCLUDE_PATH) src/agmv_psx_file_utils.c -o src/file.o
	ccpsx -c -O3 $(LIBAGMV_INCLUDES) $(C_INCLUDE_PATH) src/agmv_psx_decode.c -o src/decode.o
	ccpsx -c -O3 $(LIBAGMV_INCLUDES) $(C_INCLUDE_PATH) src/agmv_psx_playback.c -o src/play.o
	ccpsx -Xo$80010000 $(OBJECTS) -omain.cpe,main.sym,mem.map
	cpe2x /ca main.cpe
	
clean:
	del mem.map
	del main.sym
	del main.exe
	del main.cpe
	del src\main.o
	del src\mem.o
	del src\file.o
	del src\decode.o
	del src\play.o