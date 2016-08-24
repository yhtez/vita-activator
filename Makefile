rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

TITLE_ID = ACTI00000
TARGET = vita-activator
NAME = Vita Activator

SRC_C :=$(call rwildcard, src/, *.c)

OBJ_DIRS := $(addprefix out/, $(dir $(SRC_C:src/%.c=%.o)))
OBJS := $(addprefix out/, $(SRC_C:src/%.c=%.o))

LIBS := -lSceKernel_stub -lSceAppUtil_stub -lSceCommonDialog_stub -lSceIme_stub -lSceCtrl_stub -lSceVshBridge_stub -lSceSysmodule_stub -lSceNet_stub -lSceNetCtl_stub -lSceHttp_stub -lSceSsl_stub -lSceDisplay_stub -lSceReg_stub

CC := arm-vita-eabi-gcc
STRIP := arm-vita-eabi-strip

CONF ?= RELEASE

CFLAGS += -Wl,-q -Wall -O3 -D $(CONF)
ASFLAGS += $(CFLAGS)

.PHONY: all vpk eboot velf clean
all: vpk eboot velf
vpk: build/$(TARGET).vpk
eboot: build/eboot.bin
velf: build/$(TARGET).velf

build/%.vpk: vpk/eboot.bin vpk/sce_sys/param.sfo
	mkdir -p build
	cd vpk; zip -r -q ../$@ ./*; cd ..

build/eboot.bin: vpk/eboot.bin
	mkdir -p build
	cp vpk/eboot.bin build
	
build/%.velf: out/%.velf
	mkdir -p build
	cp out/$(@:build/%=%) build

vpk/sce_sys/param.sfo:
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(NAME)" $@
	
vpk/eboot.bin: out/$(TARGET).velf
	vita-make-fself $< $@
	
%.velf: %.elf
	vita-elf-create $< $@
	
%.elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(OBJ_DIRS):
	mkdir -p $@

out/%.o : src/%.c | $(OBJ_DIRS)
	$(CC) -c -o $@ $(CFLAGS) $<

clean:
	@rm -rf out/*.velf out/*.elf $(OBJS) release/* vpk/eboot.bin vpk/sce_sys/param.sfo