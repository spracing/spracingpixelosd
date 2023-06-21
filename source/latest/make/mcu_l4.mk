CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

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
	$(shell find $(HAL_DIR) -type f -name '*.c')

HAL_EXCLUDES = \
	$(filter %_template.c,$(ALL_HAL_SOURCES))

FILTERED_HAL_SOURCES = $(filter-out $(HAL_EXCLUDES),$(ALL_HAL_SOURCES))

CMSIS_DEVICE = STM32L4xx

MCU_SOURCES = \
	$(SRC_DIR)/main/common/system/system_stm32l4xx.c \

ifeq ($(BUILD_MODE),STANDALONE)
STANDALONE_BUILD_SOURCES := $(STANDALONE_BUILD_SOURCES) \
	$(SRC_DIR)/main/standalone/startup_stm32l4xx.c \
	$(SRC_DIR)/main/standalone/stm32l4xx_it.c \

ASM_SOURCES := $(ASM_SOURCES) \
	$(SRC_DIR)/main/standalone/startup_stm32l432xx.s \

endif
