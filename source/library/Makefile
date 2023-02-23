##############################################################################
#
# Copyright 2019-2023 Dominic Clifton
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

######################################
# Authors:
# - Dominic Clifton - Initial version.
######################################

######################################
# target
######################################
TARGET ?=

# Compile-time options
OPTIONS   ?=

# Test build, YES=Internal testing only, NO=PRODUCTION
DEVELOPER_BUILD ?= NO

# Compile for EXST bootloader
EXST ?= yes

######################################
# building variables
######################################
# debug build?
# 1 = Build with debug information and minimal optimization
# empty = Builds with optimizations
DEBUG ?=

# optimization
ifeq ($(DEBUG),1)
	OPT_BASE = 
	LD_OPT =
	CC_OPT_BASE = -fno-fast-math
	OPT_DEBUG = #-Og 
	OPT_SIZE = -Os
else
	OPT_DEBUG = -Og
	OPT_DEFAULT = -O2
	OPT_SIZE = -Os
	OPT_SPEED = -Ofast

	CC_OPT_LTO = -flto
	 
	LD_OPT_LTO = -flto -fuse-linker-plugin $(OPT_SPEED)
	
	LD_OPT = $(LD_OPT_LTO)
	
	#CC_OPT_BASE = -fno-fast-math
	CC_OPT_BASE = -ffast-math
endif
#######################################
# paths
#######################################

SRC_DIR = src
LIB_DIR = lib

# Build path
BUILD_ROOT = build
BUILD_DIR_BASE = $(BUILD_ROOT)/$(TARGET)
TARGET_BASE = $(BUILD_ROOT)/$(TARGET)

ARTIFACT_PREFIX = PRODUCTION

ifeq ($(DEVELOPER_BUILD),YES)
	TARGET_ARTIFACT_BASE = $(TARGET_BASE)_DEVELOPER
	ARTIFACT_PREFIX= DEVELOPER
endif

ifeq ($(DEBUG),1)
	TARGET_ARTIFACT_BASE := $(TARGET_BASE)_$(ARTIFACT_PREFIX)_DEBUG
	BUILD_DIR := $(BUILD_DIR_BASE)/$(ARTIFACT_PREFIX)_DEBUG
else
	TARGET_ARTIFACT_BASE := $(TARGET_BASE)_$(ARTIFACT_PREFIX)_RELEASE
	BUILD_DIR := $(BUILD_DIR_BASE)/$(ARTIFACT_PREFIX)_RELEASE
endif

TARGET_ELF = $(TARGET_ARTIFACT_BASE).elf
TARGET_HEX = $(TARGET_ARTIFACT_BASE).hex
TARGET_BIN = $(TARGET_ARTIFACT_BASE).bin

-include $(SRC_DIR)/main/target/$(TARGET)/$(TARGET).mk

######################################
# source
######################################

LINKER_SCRIPTS = \
	$(shell find $(SRC_DIR) -type f -name '*.ld')

# C sources
MAIN_ALL_SOURCES = \
	$(shell find $(SRC_DIR) -type f -name '*.c')

MAIN_SOURCE_EXCLUDES = \
	$(filter $(SRC_DIR)/main/target/%,$(MAIN_ALL_SOURCES))

MAIN_FILTERED_SOURCES = $(filter-out $(MAIN_SOURCE_EXCLUDES),$(MAIN_ALL_SOURCES))

MAIN_SOURCES = \
	$(MAIN_FILTERED_SOURCES) \
	$(wildcard $(SRC_DIR)/main/target/$(TARGET)/*.c) \

MAIN_ALL_CXX_SOURCES = \
	$(shell find $(SRC_DIR) -type f -name '*.cpp')

MAIN_CXX_SOURCE_EXCLUDES = \
	$(filter $(SRC_DIR)/main/target/%,$(MAIN_ALL_CXX_SOURCES))

MAIN_FILTERED_CXX_SOURCES = $(filter-out $(MAIN_CXX_SOURCE_EXCLUDES),$(MAIN_ALL_CXX_SOURCES))

MAIN_CXX_SOURCES = \
	$(MAIN_FILTERED_CXX_SOURCES) \
	$(wildcard $(SRC_DIR)/main/target/$(TARGET)/*.cpp) \

HAL_DIR = $(LIB_DIR)/main/STM32H7xx_HAL_Driver

HAL_SOURCES = \
	$(HAL_DIR)/Src/stm32h7xx_hal.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_adc.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_adc_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_cec.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_comp.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_cordic.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_cortex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_crc.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_crc_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_cryp.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_cryp_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dac.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dac_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dcmi.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dfsdm.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dfsdm_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dma.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dma_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dma2d.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dsi.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_dts.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_eth.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_eth_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_exti.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_fdcan.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_flash.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_flash_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_fmac.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_gfxmmu.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_gpio.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_hash.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_hash_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_hcd.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_hrtim.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_hsem.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_i2c.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_i2c_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_i2s.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_i2s_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_irda.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_iwdg.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_jpeg.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_lptim.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_ltdc.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_ltdc_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_mdios.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_mdma.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_mmc.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_mmc_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_nand.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_nor.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_opamp.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_opamp_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_ospi.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_otfdec.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_pcd.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_pcd_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_pssi.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_pwr.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_pwr_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_qspi.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_ramecc.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_rcc.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_rcc_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_rng.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_rng_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_rtc.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_rtc_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_sai.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_sai_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_sd.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_sd_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_sdram.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_smartcard.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_smartcard_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_smbus.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_spdifrx.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_spi.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_spi_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_sram.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_swpmi.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_tim.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_tim_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_uart.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_uart_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_usart.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_usart_ex.c \
	$(HAL_DIR)/Src/stm32h7xx_hal_wwdg.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_adc.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_bdma.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_comp.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_cordic.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_crc.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_crs.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_dac.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_delayblock.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_dma.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_dma2d.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_exti.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_fmac.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_fmc.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_gpio.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_hrtim.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_i2c.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_lptim.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_lpuart.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_mdma.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_opamp.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_pwr.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_rcc.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_rng.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_rtc.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_sdmmc.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_spi.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_swpmi.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_tim.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_usart.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_usb.c \
	$(HAL_DIR)/Src/stm32h7xx_ll_utils.c \

ALL_HAL_SOURCES = \
	$(shell find $(HAL_DIR) -type f -name '*.c')

HAL_EXCLUDES = \
	$(filter %_template.c,$(ALL_HAL_SOURCES))

FILTERED_HAL_SOURCES = $(filter-out $(HAL_EXCLUDES),$(ALL_HAL_SOURCES))

C_SOURCES =  \
	$(MAIN_SOURCES) \
	$(HAL_SOURCES) \

CXX_SOURCES = \
	$(MAIN_CXX_SOURCES) \

# ASM sources
ASM_SOURCES =  \

# Source with optimization requirements
SPEED_OPTIMISED_SOURCES = \
	$(SRC_DIR)/main/videoframe.c \
	$(SRC_DIR)/main/pixelbuffer.c \

SIZE_OPTIMISED_SOURCES = \

ALWAYS_SIZE_OPTIMISED_SOURCES = \

ifeq ($(DEBUG),1)
ALWAYS_SIZE_OPTIMISED_SOURCES += \
	$(HAL_SOURCES) \

endif

NOT_OPTIMISED_SRC = \

XO_SOURCES = \

#######################################
# binaries
#######################################

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CXX = $(PREFIX)g++
CP = $(PREFIX)objcopy
DUMP = $(PREFIX)objdump
AR = $(PREFIX)ar
SZ = $(PREFIX)size
RE = $(PREFIX)readelf
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

GCC_REQUIRED_VERSION = 9.3.1
GCC_VERSION = $(shell $(CC) -dumpversion)
ifneq ($(GCC_VERSION), $(GCC_REQUIRED_VERSION))
    $(error $(CC) versiom mismatch.  Expected '$(GCC_REQUIRED_VERSION)', found: '$(GCC_VERSION)')
else ifeq ($(GCC_VERSION),)
    $(error $(CC) not found)
endif

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m7

# fpu
FPU = -mfpu=fpv5-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

ifeq ($(MCU_OPTIONS),)
$(warning No MCU options specified, using H750 as defaults)
MCU_OPTIONS = \
	STM32H750xx \
	STM32H7xx \
	USE_USB_FS \

endif

# macros for gcc
# AS defines
AS_DEFS = 
# C defines
C_DEFS =  \
	-DUSE_HAL_DRIVER \
	-DUSE_FULL_LL_DRIVER \
	$(addprefix -D,$(MCU_OPTIONS)) \
	-D"HSE_VALUE=8000000" \
	-D"HSE_STARTUP_TIMEOUT=250" \
	-DTARGET_$(TARGET) \
	$(addprefix -D,$(PRINTF_OPTIONS)) \
	$(addprefix -D,$(MD5_OPTIONS)) \
	$(addprefix -D,$(WOLFSSL_OPTIONS)) \
	$(addprefix -D,$(OPTIONS)) \

ifeq ($(DEVELOPER_BUILD),YES)
	C_DEFS := $(C_DEFS) -DDEVELOPER_BUILD
endif

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
	-I$(SRC_DIR)/main \
	-I$(SRC_DIR)/main/target/$(TARGET) \
	-I$(HAL_DIR)/Inc \
	-I$(HAL_DIR)/Inc/Legacy \
	-I$(LIB_DIR)/main/CMSIS/Device/ST/STM32H7xx/Include \
	-I$(LIB_DIR)/main/CMSIS/Include \

CC_OPT_SPEED = $(CC_OPT_BASE) $(OPT_SPEED)
CC_OPT_SIZE = $(CC_OPT_BASE) $(OPT_SIZE)
CC_OPT_DEFAULT = $(CC_OPT_BASE) $(OPT_DEFAULT)
CC_OPT_DEBUG = $(CC_OPT_BASE) $(OPT_DEBUG)
CC_OPT_NONE = 

DEBUG_FLAGS = -g -gdwarf-5 -ggdb3
#DEBUG_FLAGS = -g -ggdb3

DEBUG_DEFS = \
-DDEBUG \

ifeq ($(DEBUG), 1)
	C_DEFS := $(C_DEFS) $(DEBUG_DEFS)
else
	C_DEFS := $(C_DEFS) -DNDEBUG
endif

ifeq ($(EXST),yes)
	C_DEFS := $(C_DEFS) -DUSE_EXST
endif


# compile gcc flags
ASFLAGS = \
	$(DEBUG_FLAGS) \
	$(MCU) \
	$(AS_DEFS) \
	$(AS_INCLUDES) \
	-Wall \
	-fdata-sections \
	-ffunction-sections \
	-x assembler-with-cpp \
	-MMD -MP \
	
CFLAGS = \
	$(DEBUG_FLAGS) \
	$(MCU) \
	$(C_DEFS) \
	$(C_INCLUDES) \
	-Wall \
	-fdata-sections \
	-ffunction-sections \
	-fsignaling-nans \
	-fno-unsafe-math-optimizations \
	-save-temps=obj \
	-std=gnu99 \

CXXFLAGS = \
	$(DEBUG_FLAGS) \
	$(MCU) \
	$(C_DEFS) \
	$(C_INCLUDES) \
	-Wall \
	-fdata-sections \
	-ffunction-sections \
	-fno-exceptions \
	-fsignaling-nans \
	-fno-unsafe-math-optimizations \
	-std=c++11 \
	
XO_CFLAGS = \
	-mpure-code \

NOXO_CFLAGS = \
	$(CC_OPT_LTO) \

# Generate dependency information
DEP_FLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT $@

GENERATED_ASSEMBER_FLAGS = -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(<:.c=.lst)

#######################################
# LDFLAGS
#######################################
DEFAULT_LD_SCRIPT = $(SRC_DIR)/main/target/$(TARGET)/$(TARGET).ld

ifeq ($(LD_SCRIPT),)
LD_SCRIPT = $(DEFAULT_LD_SCRIPT)
endif

# libraries
LIBS = -lc -lm -lnosys -lstdc++
LIBDIR = 
LDFLAGS = \
	$(DEBUG_FLAGS) \
	$(MCU) \
	-specs=nano.specs \
	-specs=nosys.specs \
	-T$(LD_SCRIPT) \
	$(LIBDIR) \
	$(LIBS) \
	-static \
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage \
	$(LD_OPT) \

# default action: build all
all: $(TARGET_ELF) $(TARGET_HEX) $(TARGET_BIN)


#######################################
# build the application
#######################################
# ASM startup code must be listed first due to an issue with LTO, see https://bugs.launchpad.net/gcc-arm-embedded/+bug/1747966

# list of ASM program objects
OBJECTS = $(addsuffix .o, $(addprefix $(BUILD_DIR)/,$(basename $(ASM_SOURCES)))) 
#$(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# list of C program objects
OBJECTS += $(addsuffix .o, $(addprefix $(BUILD_DIR)/,$(basename $(C_SOURCES))))
vpath %.c $(sort $(dir $(C_SOURCES)))


# list of CPP program objects
OBJECTS += $(addsuffix .o, $(addprefix $(BUILD_DIR)/,$(basename $(CXX_SOURCES))))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))

DEPS = \
	$(addsuffix .d, $(addprefix $(BUILD_DIR)/,$(basename $(C_SOURCES)))) \
	$(addsuffix .d, $(addprefix $(BUILD_DIR)/,$(basename $(CXX_SOURCES)))) \
	$(addsuffix .d, $(addprefix $(BUILD_DIR)/,$(basename $(ASM_SOURCES)))) \

CLEAN_ARTIFACTS := $(TARGET_BIN)
CLEAN_ARTIFACTS += $(TARGET_HEX)
CLEAN_ARTIFACTS += $(TARGET_ELF)


ifeq ($(DEBUG),1)
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@mkdir -p $(dir $@)
	$(if $(findstring $<,$(NOT_OPTIMISED_SRC)), \
		@echo "CC (none$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" && \
		$(CC) $(CC_OPT_NONE) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
	, \
		$(if $(findstring $<,$(ALWAYS_SIZE_OPTIMISED_SOURCES)), \
			@echo "CC (size$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" "$(STDOUT)" && \
			$(CC) $(CC_OPT_SIZE) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
		, \
			@echo "CC (debug$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" && \
			$(CC) $(CC_OPT_DEBUG) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
		)\
	)
else
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(if $(findstring $<,$(NOT_OPTIMISED_SRC)), \
		@echo "CC (none$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" "$(STDOUT)" && \
		$(CC) $(CC_OPT_NONE) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
	, \
		$(if $(findstring $<,$(SPEED_OPTIMISED_SOURCES)), \
			@echo "CC (speed$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" "$(STDOUT)" && \
			$(CC) $(CC_OPT_SPEED) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
		, \
			$(if $(findstring $<,$(SIZE_OPTIMISED_SOURCES) $(ALWAYS_SIZE_OPTIMISED_SOURCES)), \
				@echo "CC (size$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" "$(STDOUT)" && \
				$(CC) $(CC_OPT_SIZE) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
			, \
				@echo "CC (default$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" "$(STDOUT)" && \
				$(CC) $(CC_OPT_DEFAULT) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
			)\
		)\
	)
endif

ifeq ($(DEBUG),1)
$(BUILD_DIR)/%.o: %.cpp Makefile
	@mkdir -p $(dir $@)
	@echo "CXX (debug$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<"
	@$(CXX) $(CC_OPT_DEBUG) -c $(CXXFLAGS) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@
else
$(BUILD_DIR)/%.o: %.cpp Makefile
	@mkdir -p $(dir $@)
	@echo "CXX (default$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<"
	@$(CXX) $(CC_OPT_DEFAULT) -c $(CXXFLAGS) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@
endif

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "AS (default) $<"
	@$(AS) -c $(ASFLAGS) $(DEP_FLAGS) $< -o $@

$(TARGET_ELF): $(OBJECTS) Makefile $(LINKER_SCRIPTS)
	@echo LD $(LD_SCRIPT) $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo RE $@
	@$(RE) -S $@
	@echo SZ $@
	@$(SZ) $@

ifeq ($(EXST),no)
#Non-EXST goals
$(TARGET_HEX): $(TARGET_ELF) | $(BUILD_DIR)
	@echo HEX $<
	@$(HEX) $< $@
	
$(TARGET_BIN): $(TARGET_ELF) | $(BUILD_DIR)
	@echo BIN $<
	@$(BIN) $< $@
else
#EXST goals

TARGET_ELF = $(TARGET_ARTIFACT_BASE).elf
TARGET_EXST_ELF = $(TARGET_ARTIFACT_BASE)_EXST.elf
TARGET_UNPATCHED_BIN = $(TARGET_ARTIFACT_BASE)_UNPATCHED.bin
TARGET_EXST_HASH_SECTION_FILE = $(TARGET_ARTIFACT_BASE)_exst_hash_section.bin

CLEAN_ARTIFACTS += $(TARGET_UNPATCHED_BIN) $(TARGET_EXST_HASH_SECTION_FILE) $(TARGET_EXST_ELF)

$(TARGET_UNPATCHED_BIN): $(TARGET_ELF)
	@echo "Creating BIN (without checksum) $(TARGET_UNPATCHED_BIN)" "$(STDOUT)"
	$(CP) -O binary $< $@

$(TARGET_BIN): $(TARGET_UNPATCHED_BIN)
	@echo "Creating EXST $(TARGET_BIN)" "$(STDOUT)"
# Linker script should allow .bin generation from a .elf which results in a file that is the same length as the LIBRARY_SIZE.
# These 'dd' commands will pad a short binary to length LIBRARY_SIZE.
	dd if=/dev/zero ibs=1k count=$(LIBRARY_SIZE) of=$(TARGET_BIN)
	dd if=$(TARGET_UNPATCHED_BIN) of=$(TARGET_BIN) conv=notrunc

	@echo "Generating MD5 hash of binary" "$(STDOUT)"
	openssl dgst -md5 $(TARGET_BIN) > $(TARGET_UNPATCHED_BIN).md5 
	
	@echo "Patching MD5 hash into binary" "$(STDOUT)"
	cat $(TARGET_UNPATCHED_BIN).md5 | awk '{printf("%08x: %s",(1024*$(LIBRARY_SIZE))-16,$$2);}' | xxd -r - $(TARGET_BIN)
	echo $(LIBRARY_SIZE) | awk '{printf("-s 0x%08x -l 16 -c 16 %s",(1024*$$1)-16,"$(TARGET_BIN)");}' | xargs xxd

# Note: From the objcopy manual "If you do not specify outfile, objcopy creates a temporary file and destructively renames the result with the name of infile"
# Due to this a temporary file must be created and removed, even though we're only extracting data from the input file.
# If this temporary file is NOT used the $(TARGET_ELF) is modified, and running make a second time will result in
# a) regeneration of $(TARGET_BIN), and
# b) the results of $(TARGET_BIN) will not be as expected.
	@echo "Extracting HASH section from unpatched EXST elf $(TARGET_ELF)" "$(STDOUT)"
	$(CP) $(TARGET_ELF) $(TARGET_EXST_ELF).tmp --dump-section .exst_hash=$(TARGET_EXST_HASH_SECTION_FILE)
	rm $(TARGET_EXST_ELF).tmp
	
	@echo "Patching MD5 hash into HASH section" "$(STDOUT)"
	cat $(TARGET_UNPATCHED_BIN).md5 | awk '{printf("%08x: %s",64-16,$$2);}' | xxd -r - $(TARGET_EXST_HASH_SECTION_FILE)
	
	@echo "Patching updated HASH section into $(TARGET_EXST_ELF)" "$(STDOUT)"
	$(CP) $(TARGET_ELF) $(TARGET_EXST_ELF) --update-section .exst_hash=$(TARGET_EXST_HASH_SECTION_FILE)

$(TARGET_HEX): $(TARGET_BIN)
	$(if $(EXST_ADJUST_VMA),,$(error "EXST_ADJUST_VMA not specified"))

	@echo "Creating EXST HEX from patched EXST BIN $(TARGET_BIN), VMA Adjust $(EXST_ADJUST_VMA)" "$(STDOUT)"
	$(CP) -I binary -O ihex --adjust-vma=$(EXST_ADJUST_VMA) $(TARGET_BIN) $@
endif

$(BUILD_DIR):
	mkdir -p $@
#######################################
# clean up
#######################################
clean:
	@echo "Cleaning $(TARGET)"
	rm -fR $(BUILD_DIR_BASE)
	rm -f $(TARGET_BASE)*.*
	rm -f $(CLEAN_ARTIFACTS)
  
#######################################
# dependencies
#######################################
-include $(DEPS)

# *** EOF ***
