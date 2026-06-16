#-------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error Please set DEVKITARM in your environment. export DEVKITARM=/path/to/devkitARM)
endif

include $(DEVKITARM)/base_tools

#-------------------------------------------------------------------------------

include config.mk

export BUILD := build
export SRC := src
export BINARY := $(BUILD)/linked.o
export GRAPHICS := graphics

export ARMIPS ?= armips
export GFX ?= gbagfx
export PREPROC ?= preproc
export LD := $(PREFIX)ld

export ASFLAGS := -mthumb
	
export INCLUDES := -I $(SRC) -I . -I include -D$(ROM_CODE)
export WARNINGFLAGS :=	-Wall -Wno-discarded-array-qualifiers \
	-Wno-int-conversion
export CFLAGS := -g -O2 $(WARNINGFLAGS) -mthumb -std=gnu17 $(INCLUDES) -mcpu=arm7tdmi \
	-march=armv4t -mno-thumb-interwork -fno-inline -fno-builtin -mlong-calls -DROM_$(ROM_CODE) \
	-fdiagnostics-color -mtune=arm7tdmi -finline -mabi=aapcs
# -mabi=apcs-gnu #EABI version 0
# -mabi=aapcs #EABI version 5
# -mthumb-interwork #el código admite llamadas ARM y Thumb, genera código más grande. usar sólo en EABI menor a 5. 

export DEPFLAGS = -MT $@ -MMD -MP -MF "$(@:%.o=%.d)"
export LDFLAGS := -T linker.ld -r $(ROM_CODE).ld 

#-------------------------------------------------------------------------------
	
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#Esta parte se encarga de convertir las imágenes
#automáticamente de acuerdo al tipo de gráfico.

#RESOURCES_BIN := $(GRAPHICS)/bin_to_lz
#obtenemos los nombres de los gráficos:
GRAPHICS_BIN:= $(call rwildcard,$(GRAPHICS)/bin_to_lz,*.bin)
GRAPHICS_PAL:= $(call rwildcard,$(GRAPHICS)/pal_to_gbapal,*.pal)
GRAPHICS_4BPP:= $(call rwildcard,$(GRAPHICS)/png_to_4bpp,*.png)
GRAPHICS_8BPP:= $(call rwildcard,$(GRAPHICS)/png_to_8bpp,*.png)
GRAPHICS_4BPP_LZ:= $(call rwildcard,$(GRAPHICS)/png_to_4bpp_lz,*.png)
GRAPHICS_8BPP_LZ:= $(call rwildcard,$(GRAPHICS)/png_to_8bpp_lz,*.png)

#creamos una copia de los nombres pero con nueva extensión
GBA_BIN_LZ := $(GRAPHICS_BIN:%=%.lz)

GBA_PAL := $(GRAPHICS_PAL:%.pal=%.gbapal)

GBA_4BPP := $(GRAPHICS_4BPP:%.png=%.4bpp)
GBA_4BPP_PAL := $(GRAPHICS_4BPP:%.png=%.pal)
GBA_4BPP_GBAPAL := $(GBA_4BPP_PAL:%.pal=%.gbapal)

GBA_8BPP := $(GRAPHICS_8BPP:%.png=%.8bpp)
GBA_8BPP_PAL := $(GRAPHICS_8BPP:%.png=%.pal)
GBA_8BPP_GBAPAL := $(GBA_8BPP_PAL:%.pal=%.gbapal)


GBA_4BPPLZ := $(GRAPHICS_4BPP_LZ:%.png=%.4bpp)
GBA_4BPP_LZ := $(GBA_4BPPLZ:%=%.lz)
GBA_4BPPPAL := $(GRAPHICS_4BPP_LZ:%.png=%.pal)
GBA_4BPP_GBAPALLZ := $(GBA_4BPPPAL:%.pal=%.gbapal)
GBA_4BPP_PAL_LZ := $(GBA_4BPP_GBAPALLZ:%=%.lz)

GBA_8BPPLZ := $(GRAPHICS_8BPP_LZ:%.png=%.8bpp)
GBA_8BPP_LZ := $(GBA_8BPPLZ:%=%.lz)
GBA_8BPPPAL := $(GRAPHICS_8BPP_LZ:%.png=%.pal)
GBA_8BPP_GBAPALLZ := $(GBA_8BPPPAL:%.pal=%.gbapal)
GBA_8BPP_PAL_LZ := $(GBA_8BPP_GBAPALLZ:%=%.lz)


# Sources
C_SRC := $(call rwildcard,$(SRC),*.c)
S_SRC := $(call rwildcard,$(SRC),*.s)

# Binaries
C_OBJ := $(C_SRC:%.c=$(BUILD)/%.o)
S_OBJ := $(S_SRC:%.s=$(BUILD)/%.o)

ALL_OBJ := $(C_OBJ) $(S_OBJ)

#-------------------------------------------------------------------------------

.PHONY: all rom clean graphics

all: clean graphics rom

rom: main$(ROM_CODE).asm $(BINARY)
	@echo "\nCreating ROM"
	$(ARMIPS) main$(ROM_CODE).asm -definelabel insertinto $(OFFSET) -sym offsets_$(ROM_CODE).txt

clean:
	rm -rf $(BINARY)
	rm -rf $(BUILD)/$(SRC)
	rm -rf $(GBA_BIN_LZ)
	rm -rf $(GBA_4BPP)
	rm -rf $(GBA_8BPP)
	rm -rf $(GBA_4BPP_LZ)
	rm -rf $(GBA_8BPP_LZ)
	rm -rf $(GBA_PAL)
	rm -rf $(GBA_4BPP_GBAPAL)
	rm -rf $(GBA_8BPP_GBAPAL)
	rm -rf $(GBA_4BPP_PAL_LZ)
	rm -rf $(GBA_8BPP_PAL_LZ)

$(BINARY): $(ALL_OBJ)
	@echo "\nLinking ELF binary $@"
	@$(LD) $(LDFLAGS) -o $@ $^

$(BUILD)/%.o: %.c
	@echo "\nCompiling $<"
	@mkdir -p $(@D)
	@$(CC) $(DEPFLAGS) $(CFLAGS) -E -c $< -o $(BUILD)/$*.i
	@$(PREPROC) $(BUILD)/$*.i charmap.txt | $(CC) $(CFLAGS) -x c -o $@ -c -

$(BUILD)/%.o: %.s
	@echo -e "Assembling $<"
	@mkdir -p $(@D)
	$(PREPROC) "$<" charmap.txt | @$(AS) $(ASFLAGS) -o $@

%.4bpp: %.png  ; $(GFX) $< $@
%.8bpp: %.png  ; $(GFX) $< $@
%.pal: %.png  ; $(GFX) $< $@
%.gbapal: %.pal ; $(GFX) $< $@
%.lz: % ; $(GFX) $< $@

bintolz: $(GBA_BIN_LZ)

pngto4bpp: $(GBA_4BPP) $(GBA_4BPPLZ)

pngto8bpp: $(GBA_8BPP) $(GBA_8BPPLZ)

gbaXbpptolz: $(GBA_4BPP_LZ) $(GBA_8BPP_LZ)
	rm -rf $(GBA_4BPPLZ)
	rm -rf $(GBA_8BPPLZ)

pngtopal: $(GBA_4BPP_PAL) $(GBA_8BPP_PAL) $(GBA_4BPPPAL) $(GBA_8BPPPAL)

paltogbapal: $(GBA_PAL) $(GBA_4BPP_GBAPAL) $(GBA_8BPP_GBAPAL) $(GBA_4BPP_GBAPALLZ) $(GBA_8BPP_GBAPALLZ)
	rm -rf $(GBA_4BPP_PAL)
	rm -rf $(GBA_8BPP_PAL)
	rm -rf $(GBA_4BPPPAL)
	rm -rf $(GBA_8BPPPAL)

gbapaltolz: $(GBA_4BPP_PAL_LZ) $(GBA_8BPP_PAL_LZ)
	rm -rf $(GBA_4BPP_GBAPALLZ)
	rm -rf $(GBA_8BPP_GBAPALLZ)


graphics: bintolz pngto4bpp pngto8bpp gbaXbpptolz pngtopal paltogbapal  gbapaltolz

firered:              	; @$(MAKE) ROM_CODE=BPRE
rojofuego:           	; @$(MAKE) ROM_CODE=BPRS
emerald:              	; @$(MAKE) ROM_CODE=BPEE
esmeralda:              ; @$(MAKE) ROM_CODE=BPES
