BINOUT = ./bin/
TARGET = $(BINOUT)beamcaster
PATHOBJS = ./bin/
OBJ = bmc.o pov.o main.o
OBJS = $(strip $(sort $(OBJ:%.o=$(PATHOBJS)%.o)))

CC = g++
MEM_ALIGN = 4

CFLAGS = -Wno-narrowing -g0 -Wall \
         -funroll-loops \
         -DM_PI=3.1415927f \
         -DUSE_RAND_16BITS -fno-exceptions \
         -DMEM_ALIGN=$(MEM_ALIGN) \
         -falign-functions=$(MEM_ALIGN) \
         -fomit-frame-pointer \
         -Ofast -DBEAM_CONFIG_GENERIC \
         -ffunction-sections \
         -fdata-sections \
         -fno-unwind-tables \
         -fno-asynchronous-unwind-tables \
         -flto \
         -fmerge-all-constants \
         -march=native -Wl,--gc-sections

CXXFLAGS = $(CFLAGS) -fno-rtti -std=c++11

INCLUDES =

LDFLAGS = -Wl,--gc-sections

LIBS = -lglfw -lGLEW -lGL -lGLU -lm -lpthread

all: prepare $(TARGET)

prepare:
	@mkdir -p $(BINOUT)
	@mkdir -p $(PATHOBJS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

$(PATHOBJS)main.o: ./main.cpp
	$(CC) $(INCLUDES) -o $@ -c $< $(CXXFLAGS)

$(PATHOBJS)%.o: ../bmc/%.cpp
	$(CC) $(INCLUDES) -o $@ -c $< $(CXXFLAGS)

clean:
	rm -f $(BINOUT)beamcaster $(PATHOBJS)*.o

rebuild: clean all

.PHONY: all clean rebuild prepare

