BINOUT = ./bin/
TARGET = $(BINOUT)beamcaster
PATHOBJS = ./bin/
OBJ = bmc.o pov.o main.o
OBJS = $(strip $(sort $(OBJ:%.o=$(PATHOBJS)%.o)))

CC = g++
WINDRES = windres
MEM_ALIGN = 4

CFLAGS = -Wno-narrowing -g0 -Wall \
         -funroll-loops \
         -DM_PI=3.1415927f \
         -DUSE_RAND_16BITS -fno-exceptions \
         -DMEM_ALIGN=$(MEM_ALIGN) \
         -falign-functions=$(MEM_ALIGN) \
         -fomit-frame-pointer \
         -O3 -DBEAM_CONFIG_GENERIC \
         -ffunction-sections \
         -fdata-sections \
         -fno-unwind-tables \
         -fno-asynchronous-unwind-tables \
         -flto \
         -fmerge-all-constants \
         -march=native -Wl,--gc-sections

CXXFLAGS = $(CFLAGS) -fno-rtti -std=c++11

INCLUDES = -I/mingw64/include -I.

LDFLAGS = -Wl,--gc-sections

LIBS = -L/mingw64/lib -lglfw3 -lglew32 -lopengl32 -lglu32 -lgdi32  -static -lstdc++ -lm

all: prepare $(TARGET).exe

prepare:
	@mkdir -p $(BINOUT)
	@mkdir -p $(PATHOBJS)

$(TARGET).exe: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

$(PATHOBJS)main.o: ./main.cpp
	$(CC) $(INCLUDES) -o $@ -c $< $(CXXFLAGS)

$(PATHOBJS)%.o: ../bmc/%.cpp
	$(CC) $(INCLUDES) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -f $(BINOUT)*.exe $(PATHOBJS)*.o

rebuild: clean all

.PHONY: all clean rebuild prepare
