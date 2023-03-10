##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [2.29.2] date: [Tue Jan 15 17:34:48 CET 2019] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET ?= SPRacingGraphicOSD

# Compile-time options
OPTIONS   ?=

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
OPT_DEFAULT = -O1
OPT_SIZE = -Os
OPT_SPEED = -Ofast

CC_OPT_LTO = -flto 
LD_OPT_LTO = -flto -fuse-linker-plugin $(OPT_SPEED)

LD_OPT = $(LD_OPT_LTO)
CC_OPT_BASE = -fno-fast-math
endif
#######################################
# paths
#######################################

SRC_DIR = src
LIB_DIR = lib


# source path
SOURCES_DIR =  \
$(SRC_DIR)/Application/User \
$(LIB_DIR)/main/STM32L4xx_HAL_Driver \
$(SRC_DIR)/Application \
$(LIB_DIR)/main/BSP \
$(LIB_DIR)/main/CMSIS \
$(LIB_DIR)/main

# firmware library path
PERIFLIB_PATH = 

# Build path
BUILD_ROOT = build
BUILD_DIR = $(BUILD_ROOT)/$(TARGET)

TARGET_ELF = $(BUILD_ROOT)/$(TARGET).elf
TARGET_HEX = $(BUILD_ROOT)/$(TARGET).hex
TARGET_BIN = $(BUILD_ROOT)/$(TARGET).bin

######################################
# source
######################################

LINKER_SCRIPTS = \
	$(shell find $(SRC_DIR)/ -type f -name '*.ld')

# C sources
MAIN_ALL_SOURCES = \
	$(shell find $(SRC_DIR)/ -type f -name '*.c')

MAIN_SOURCE_EXCLUDES = \
	$(filter $(SRC_DIR)/main/target/%,$(MAIN_ALL_SOURCES))

MAIN_FILTERED_SOURCES = $(filter-out $(MAIN_SOURCE_EXCLUDES),$(MAIN_ALL_SOURCES))

MAIN_SOURCES = \
	$(MAIN_FILTERED_SOURCES) \
	$(wildcard $(SRC_DIR)/main/target/$(TARGET)/*.c) \

HAL_DIR = $(LIB_DIR)/main/STM32L4xx_HAL_Driver

HAL_SOURCES = \
    $(HAL_DIR)/Src/stm32l4xx_hal_adc.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_adc_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_comp.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_cortex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_dac.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_dac_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_opamp.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_opamp_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_i2c.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_i2c_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_spi.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_spi_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_tim.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_tim_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_uart.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_uart_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_rcc.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_rcc_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_flash.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_flash_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_flash_ramfunc.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_gpio.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_dma.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_dma_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_pcd.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_pcd_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_pwr.c \
    $(HAL_DIR)/Src/stm32l4xx_hal_pwr_ex.c \
    $(HAL_DIR)/Src/stm32l4xx_hal.c \
    $(HAL_DIR)/Src/stm32l4xx_ll_usb.c \

ALL_HAL_SOURCES = \
	$(shell find $(HAL_DIR)/ -type f -name '*.c')

HAL_EXCLUDES = \
	$(filter %_template.c,$(ALL_HAL_SOURCES))

FILTERED_HAL_SOURCES = $(filter-out $(HAL_EXCLUDES),$(ALL_HAL_SOURCES))

PRINTF_DIR = $(LIB_DIR)/main/printf

PRINTF_SOURCES = \
    $(PRINTF_DIR)/printf.c \
    
PRINTF_OPTIONS = PRINTF_INCLUDE_CONFIG_H

USBLIB_DIR = $(LIB_DIR)/main/STM32_USB_Device_Library

USBLIB_SOURCES = \
	$(USBLIB_DIR)/Core/Src/usbd_core.c \
	$(USBLIB_DIR)/Core/Src/usbd_ctlreq.c \
	$(USBLIB_DIR)/Core/Src/usbd_ioreq.c \
	$(USBLIB_DIR)/Class/CDC/Src/usbd_cdc.c \

C_SOURCES =  \
    $(MAIN_SOURCES) \
    $(HAL_SOURCES) \
    $(USBLIB_SOURCES) \
    $(PRINTF_SOURCES) \

# ASM sources
ASM_SOURCES =  \
    $(SRC_DIR)/main/startup_stm32l432xx.s \


# Source with optimization requirements
SPEED_OPTIMISED_SOURCES = \
#    $(SRC_DIR)/main/stm32l4xx_it.c \

SIZE_OPTIMISED_SOURCES = \
#	$(HAL_SOURCES) \

ALWAYS_SIZE_OPTIMISED_SOURCES = \
#	$(HAL_SOURCES) \

ALWAYS_SPEED_OPTIMISED_SOURCES = \
    $(SRC_DIR)/main/video_interrupt.c \

NOT_OPTIMISED_SRC = \
#   $(SRC_DIR)/main/not_omptimized.c \


XO_SOURCES = \
#	$(shell find $(SRC_DIR)/main/xo/ -type f -name '*.c') \
#	$(HAL_SOURCES) \
#	$(SRC_DIR)/main/main.c \

######################################
# firmware library
######################################
PERIFLIB_SOURCES = \ 

#######################################
# binaries
#######################################

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
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
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 
# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L432xx \
-DSTM32L4xx \
-DTARGET_$(TARGET) \
$(addprefix -D,$(PRINTF_OPTIONS)) \
$(addprefix -D,$(OPTIONS)) \


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-I$(SRC_DIR)/main \
-I$(SRC_DIR)/main/drivers \
-I$(SRC_DIR)/main/drivers/usb \
-I$(SRC_DIR)/main/target/$(TARGET) \
-I$(HAL_DIR)/Inc \
-I$(HAL_DIR)/Inc/Legacy \
-I$(LIB_DIR)/main/CMSIS/Device/ST/STM32L4xx/Include \
-I$(LIB_DIR)/main/CMSIS/Include \
-I$(USBLIB_DIR)/Core/Inc \
-I$(USBLIB_DIR)/Class/CDC/Inc \
-I$(PRINTF_DIR) \


CC_OPT_SPEED = $(CC_OPT_BASE) $(OPT_SPEED)
CC_OPT_SIZE = $(CC_OPT_BASE) $(OPT_SIZE)
CC_OPT_DEFAULT = $(CC_OPT_BASE) $(OPT_DEFAULT)
CC_OPT_DEBUG = $(CC_OPT_BASE) $(OPT_DEBUG)
CC_OPT_NONE = 

DEBUG_FLAGS = -g -gdwarf-2
#DEBUG_FLAGS = -g -ggdb3

DEBUG_DEFS = \
-DDEBUG \

ifeq ($(DEBUG), 1)
C_DEFS := $(C_DEFS) $(DEBUG_DEFS)
else
C_DEFS := $(C_DEFS) -DNDEBUG
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
	-std=gnu99 \
	
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
# link script
LDSCRIPT = $(SRC_DIR)/main/STM32L432KCUx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR = 
LDFLAGS = \
    $(DEBUG_FLAGS) \
    $(MCU) \
    -specs=nano.specs \
    -specs=nosys.specs \
    -T$(LDSCRIPT) \
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

DEPS = \
	$(addsuffix .d, $(addprefix $(BUILD_DIR)/,$(basename $(C_SOURCES)))) \
	$(addsuffix .d, $(addprefix $(BUILD_DIR)/,$(basename $(ASM_SOURCES)))) \

ifeq ($(DEBUG),1)
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	@mkdir -p $(dir $@)
	$(if $(findstring $<,$(NOT_OPTIMISED_SRC)), \
		@echo "CC (none$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" && \
		$(CC) $(CC_OPT_NONE) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
	, \
		$(if $(findstring $<,$(SPEED_OPTIMISED_SOURCES) $(ALWAYS_SPEED_OPTIMISED_SOURCES)), \
			@echo "CC (speed$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" "$(STDOUT)" && \
			$(CC) $(CC_OPT_SPEED) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
		, \
			$(if $(findstring $<,$(ALWAYS_SIZE_OPTIMISED_SOURCES)), \
				@echo "CC (size$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" "$(STDOUT)" && \
				$(CC) $(CC_OPT_SIZE) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
			, \
				@echo "CC (debug$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" && \
				$(CC) $(CC_OPT_DEBUG) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
			)\
		)\
	)
else
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(if $(findstring $<,$(NOT_OPTIMISED_SRC)), \
		@echo "CC (none$(if $(findstring $<,$(XO_SOURCES)),/XO,)) $<" "$(STDOUT)" && \
		$(CC) $(CC_OPT_NONE) -c $(CFLAGS) $(if $(findstring $<,$(XO_SOURCES)),$(XO_CFLAGS),$(NOXO_CFLAGS)) $(DEP_FLAGS) $(GENERATED_ASSEMBER_FLAGS) $< -o $@\
	, \
		$(if $(findstring $<,$(SPEED_OPTIMISED_SOURCES) $(ALWAYS_SPEED_OPTIMISED_SOURCES)), \
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

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "AS (default) $<"
	@$(AS) -c $(ASFLAGS) $(DEP_FLAGS) $< -o $@

$(TARGET_ELF): $(OBJECTS) Makefile $(LINKER_SCRIPTS)
	@echo LD $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo RE $@
	@$(RE) -S $@
	@echo SZ $@
	@$(SZ) $@

$(TARGET_HEX): $(TARGET_ELF) | $(BUILD_DIR)
	@echo HEX $<
	@$(HEX) $< $@
	
$(TARGET_BIN): $(TARGET_ELF) | $(BUILD_DIR)
	@echo BIN $<
	@$(BIN) $< $@
	
$(BUILD_DIR):
	mkdir $@
#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
	-rm -f $(TARGET_ELF) $(TARGET_HEX) $(TARGET_BIN)
  
#######################################
# dependencies
#######################################
-include $(DEPS)

# *** EOF ***
