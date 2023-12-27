TEXT_DIR  := $(CONTANTS_DIR)/Texts
FONT_DIR  := $(CONTANTS_DIR)/Fonts
SOUND_DIR := $(CONTANTS_DIR)/Sounds

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

# =========
# = Fonts =
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

# ==========
# = Sounds =
# ==========

CLEAN_DIRS += $(SOUND_DIR)
