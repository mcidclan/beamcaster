BINOUT = ./bin/
TARGET = $(BINOUT)beamcaster
PATHOBJS = ./bin/
OBJ = bmc.o pov.o main.o
OBJS = $(strip $(sort $(OBJ:%.o=$(PATHOBJS)%.o)))

PSPSDK = $(shell psp-config --pspsdk-path)

MEM_ALIGN = 4
CFLAGS = -Wno-narrowing -g0 -Wall \
         -funroll-loops \
         -I$(PSPSDK)/../include \
         -DM_PI=3.1415927f -DPSP \
         -fno-exceptions \
         -DMEM_ALIGN=$(MEM_ALIGN) \
         -falign-functions=$(MEM_ALIGN) \
         -fomit-frame-pointer \
         -Ofast \
         -ffunction-sections \
         -fdata-sections \
         -fno-unwind-tables \
         -fno-asynchronous-unwind-tables \
         -flto \
         -fmerge-all-constants
         
CXXFLAGS = $(CFLAGS) -fno-rtti -std=c++11
PSPBIN = $(PSPSDK)/../bin
LIBS += -lstdc++ -lpsppower -lpspgu

PSP_FW_VERSION=660
PSP_LARGE_MEMORY=1
PSP_EBOOT_SFO = $(BINOUT)PARAM.SFO
EXTRA_TARGETS = $(BINOUT)EBOOT.PBP
PSP_EBOOT = $(EXTRA_TARGETS)
PSP_EBOOT_ICON = 
PSP_EBOOT_TITLE = Beamcaster

all: $(OBJS)
$(PATHOBJS)main.o: ./main.cpp
	$(CC) -o $@ -c $< $(CXXFLAGS)
$(PATHOBJS)%.o: ../bmc/%.cpp
	$(CC) -o $@ -c $< $(CXXFLAGS)

include $(PSPSDK)/lib/build.mak
