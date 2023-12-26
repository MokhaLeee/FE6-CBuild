MAKEFLAGS += --no-print-directory
PYTHON3 := python3
EXE :=

GAME := fe6

MAIN    := $(PWD)/main.event
FE6_CHX := $(PWD)/$(GAME)-chax.gba
FE6_GBA := $(PWD)/$(GAME).gba

PRE_BUILD    :=
CLEAN_DIRS   :=
CLEAN_TARGET :=
CLEAN_BUILD  :=

# =========
# = Tools =
# =========

ifeq ($(strip $(DEVKITPRO)),)
  $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitpro)
endif

ifeq ($(strip $(DEVKITARM)),)
  $(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

TOOLCHAIN ?= $(DEVKITARM)
ifneq (,$(TOOLCHAIN))
  export PATH := $(TOOLCHAIN)/bin:$(PATH)
endif

ifeq ($(OS),Windows_NT)
  EXE := .exe
else
  EXE :=
endif

ifeq ($(shell python3 -c 'import sys; print(int(sys.version_info[0] > 2))'),1)
  PYTHON3 := python3
else
  PYTHON3 := python
endif

PREFIX  ?= arm-none-eabi-
CC      := $(PREFIX)gcc
AS      := $(PREFIX)as
OBJCOPY := $(PREFIX)objcopy

TOOL_DIR := $(PWD)/tools

EA_DIR            := $(TOOL_DIR)/EventAssembler
EA                := $(EA_DIR)/ColorzCore$(EXE)
PARSEFILE         := $(EA_DIR)/Tools/ParseFile$(EXE)
PORTRAITFORMATTER := $(EA_DIR)/Tools/PortraitFormatter$(EXE)
PNG2DMP           := $(EA_DIR)/Tools/Png2Dmp$(EXE)
COMPRESS          := $(EA_DIR)/Tools/compress$(EXE)
LYN               := $(EA_DIR)/Tools/lyn$(EXE) -longcalls
EA_DEP            := $(EA_DIR)/ea-dep$(EXE)

C2EA              := $(PYTHON3) $(TOOL_DIR)/FE-PyTools/NMM2CSV/c2ea.py
TMX2EA            := $(PYTHON3) $(TOOL_DIR)/scripts/tmx2ea-mokha.py
GRIT              := $(DEVKITPRO)/tools/bin/grit$(EXE)

PORTRAIT_PROCESS  := $(PYTHON3) $(TOOL_DIR)/scripts/portrait-process-mokha.py

CACHE_DIR := $(PWD)/.cache_dir
$(shell mkdir -p $(CACHE_DIR) > /dev/null)
CLEAN_TARGET += $(CACHE_DIR) $(shell find -name __pycache__)

# ========
# = Main =
# ========

all:
	@$(MAKE) pre_build
	@$(MAKE) chax

LIB_DIR  := $(TOOL_DIR)/FE6-CLib
FE6_REF  := $(LIB_DIR)/reference/$(GAME).ref.o
FE6_SYM  := $(LIB_DIR)/reference/$(GAME).sym

WIZARDRY_DIR := $(PWD)/wizardry
GAMEDATA_DIR := $(PWD)/data

CLEAN_DIRS += $(WIZARDRY_DIR) $(CONTANTS_DIR) $(GAMEDATA_DIR)

CONTANTS_DIR := $(PWD)/contants
TEXT_DIR     := $(CONTANTS_DIR)/Texts
FONT_DIR 	 := $(CONTANTS_DIR)/Fonts

include contants/contants.mk

# ========
# = CHAX =
# ========

chax: $(FE6_CHX)

$(FE6_CHX): $(MAIN) $(FE6_GBA) $(FE6_SYM) $(shell $(EA_DEP) $(MAIN) -I $(EA_DIR) --add-missings)
	@echo "[GEN]	$@"
	@cp -f $(FE6_GBA) $(FE6_CHX)
	@$(EA) A FE6 -werr -input:$(MAIN) -output:$(FE6_CHX) --nocash-sym || rm -f $(FE6_CHX)
	@cat $(FE6_SYM) >> $(FE6_CHX:.gba=.sym)

CLEAN_TARGET += $(FE6_CHX)  $(FE6_CHX:.gba=.sym)

# ============
# = Wizardry =
# ============

INC_DIRS := $(LIB_DIR)/include include
INC_FLAG := $(foreach dir, $(INC_DIRS), -I $(dir))

ARCH    := -mcpu=arm7tdmi -mthumb -mthumb-interwork
CFLAGS  := $(ARCH) $(INC_FLAG) -Wall -Wextra -Werror=implicit-fallthrough=0 -Wno-unused-parameter -O2 -mtune=arm7tdmi -mlong-calls
ASFLAGS := $(ARCH) $(INC_FLAG)

CDEPFLAGS = -MMD -MT "$*.o" -MT "$*.asm" -MF "$(CACHE_DIR)/$(notdir $*).d" -MP
SDEPFLAGS = --MD "$(CACHE_DIR)/$(notdir $*).d"

REF_DIR := include/refs/
EXT_REF := $(shell find $(REF_DIR) -name *.s)
LYN_REF := $(EXT_REF:.s=.o) $(FE6_REF)

%.lyn.event: %.o $(LYN_REF)
	@echo "[LYN]	$@"
	@$(LYN) $< $(LYN_REF) > $@

%.dmp: %.o
	@echo "[GEN]	$@"
	@$(OBJCOPY) -S $< -O binary $@

%.o: %.s
	@echo "[AS ]	$@"
	@$(AS) $(ASFLAGS) $(SDEPFLAGS) -I $(dir $<) $< -o $@

%.o: %.c
	@echo "[CC ]	$@"
	@$(CC) $(CFLAGS) $(CDEPFLAGS) -g -c $< -o $@

%.asm: %.c
	@echo "[CC ]	$@"
	@$(CC) $(CFLAGS) $(CDEPFLAGS) -S $< -o $@ -fverbose-asm

.PRECIOUS: %.o;

-include $(wildcard $(CACHE_DIR)/*.d)

CFILES := $(shell find $(CLEAN_DIRS) -type f -name '*.c')
CLEAN_TARGET += $(CFILES:.c=.o) $(CFILES:.c=.asm) $(CFILES:.c=.dmp) $(CFILES:.c=.lyn.event)

SFILES := $(shell find $(CLEAN_DIRS) -type f -name '*.s')
CLEAN_TARGET += $(SFILES:.s=.o) $(SFILES:.s=.dmp) $(SFILES:.s=.lyn.event)

# ============
# = Spritans =
# ============

%.4bpp: %.png
	@echo "[GEN]	$@"
	@$(PNG2DMP) $< -o $@

%.gbapal: %.png
	@echo "[GEN]	$@"
	@$(PNG2DMP) $< -po $@ --palette-only

%.lz: %
	@echo "[LZ ]	$@"
	@$(COMPRESS) $< $@

PNG_FILES := $(shell find $(CLEAN_DIRS) -type f -name '*.png')
TSA_FILES := $(shell find $(CLEAN_DIRS) -type f -name '*.tsa')

CLEAN_TARGET += $(PNG_FILES:.png=.gbapal) $(PNG_FILES:.png=.4bpp) $(PNG_FILES:.png=.4bpp.lz)
CLEAN_TARGET += $(TSA_FILES:.tsa=.tsa.lz)

%.img.bin %.map.bin %.pal.bin: %.png
	@echo "[GEN]	$@"
	@$(GRIT) $< -gB 4 -gzl -m -mLf -mR4 -mzl -pn 16 -ftb -fh! -o $@

CLEAN_TARGET += $(PNG_FILES:.png=.img.bin) $(PNG_FILES:.png=.map.bin) $(PNG_FILES:.png=.pal.bin)

# =========
# = Glyph =
# =========

GLYPH_INSTALLER := $(FONT_DIR)/GlyphInstaller.event
GLYPH_DEPS := $(FONT_DIR)/FontList.txt

font: $(GLYPH_INSTALLER)

$(GLYPH_INSTALLER): $(GLYPH_DEPS)
	@$(MAKE) -C $(FONT_DIR)

%_font.img.bin: %_font.png
	@echo "[GEN]	$@"
	@$(GRIT) $< -gB2 -p! -tw16 -th16 -ftb -fh! -o $@

PRE_BUILD += font
CLEAN_BUILD += $(FONT_DIR)

# =====================
# = Hard-coded string =
# =====================

HARD_COD_DIR := $(PWD)/wizardry/Utils/TextDecoding/HardcodedString

STRING_LOCATIONS := $(HARD_COD_DIR)/string-locations.txt
STRINGS_REPOINT := $(HARD_COD_DIR)/data/fe6-strings-repoint.event
STRINGS_SOURCES := $(HARD_COD_DIR)/data/fe6-strings.s

hard_cod: $(STRINGS_REPOINT)

$(STRINGS_REPOINT): $(STRING_LOCATIONS)
	@$(MAKE) -C $(HARD_COD_DIR)
	

PRE_BUILD += hard_cod
CLEAN_BUILD += $(HARD_COD_DIR)

# =========
# = Texts =
# =========

TEXT_MAIN   := $(TEXT_DIR)/Source/TextMain.txt
TEXT_SOURCE := $(shell find $(TEXT_DIR) -type f -name '*.txt')

export TEXT_DEF := $(TEXT_DIR)/TextDefinitions.h

text: $(TEXT_DEF)

$(TEXT_DEF): $(TEXT_MAIN) $(TEXT_SOURCE)
	@$(MAKE) -C $(TEXT_DIR)

%.fetxt.dmp: %.fetxt
	@$(MAKE) -f $(TEXT_DIR)/makefile $@

PRE_BUILD += text
CLEAN_BUILD += $(TEXT_DIR)

# ==============
# = PRE-BUILD ==
# ==============

pre_build: $(PRE_BUILD)

# ==============
# = MAKE CLEAN =
# ==============

clean_targets:
	@rm -rf $(CLEAN_TARGET)

clean:
	@$(MAKE) clean_targets
	@for i in $(CLEAN_BUILD); do $(MAKE) -C $$i clean || { exit 1;} done
	@echo "All cleaned .."
