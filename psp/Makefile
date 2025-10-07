BINOUT = ./bin/
TARGET = $(BINOUT)beamcaster
PATHOBJS = ./bin/
OBJ = bmc.o pov.o main.o
OBJS = $(strip $(sort $(OBJ:%.o=$(PATHOBJS)%.o)))

PSPSDK = $(shell psp-config --pspsdk-path)

MEM_ALIGN = 4
CFLAGS = -fsingle-precision-constant \
				 -Wno-narrowing -g0 -Wall \
         -funroll-loops \
         -I$(PSPSDK)/../include \
         -DM_PI=3.1415927f -DPSP \
         -fno-exceptions \
         -fvisibility=hidden \
         -DMEM_ALIGN=$(MEM_ALIGN) \
         -falign-functions=$(MEM_ALIGN) \
         -falign-loops=$(MEM_ALIGN) \
         -falign-jumps=$(MEM_ALIGN) \
         -falign-labels=$(MEM_ALIGN) \
         -fomit-frame-pointer \
         -Ofast -DBEAM_CONFIG_PSP \
         -fno-signed-zeros \
         -fno-unwind-tables \
         -fno-asynchronous-unwind-tables \
         -flto=auto \
         -fmerge-all-constants
         
CXXFLAGS = $(CFLAGS) -fno-rtti -std=c++11
PSPBIN = $(PSPSDK)/../bin
LIBS += -lstdc++ -lpsppower -lpspgu

LDFLAGS = -Wl,--strip-all -Wl,--as-needed

PSP_FW_VERSION=660
PSP_LARGE_MEMORY=1
PSP_EBOOT_SFO = $(BINOUT)PARAM.SFO
EXTRA_TARGETS = $(BINOUT)EBOOT.PBP
PSP_EBOOT = $(EXTRA_TARGETS)
PSP_EBOOT_ICON = 
PSP_EBOOT_TITLE = Beamcaster

include $(PSPSDK)/lib/build.mak

all: $(OBJS)
$(PATHOBJS)main.o: ./main.cpp
	$(CC) -o $@ -c $< $(CXXFLAGS)
$(PATHOBJS)%.o: ../bmc/%.cpp
	$(CC) -o $@ -c $< $(CXXFLAGS)
