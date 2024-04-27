# cpu
CPU = -mcpu=cortex-m7

# fpu
FPU = -mfpu=fpv5-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

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

ASM_SOURCES = \

CMSIS_DEVICE = STM32H7xx

MCU_SOURCES = \
	$(SRC_DIR)/main/common/system/system_stm32h7xx.c \

ifeq ($(BUILD_MODE),STANDALONE)
STANDALONE_BUILD_SOURCES := $(STANDALONE_BUILD_SOURCES) \
	$(SRC_DIR)/main/standalone/startup_stm32h7xx.c \
	$(SRC_DIR)/main/standalone/stm32h7xx_it.c \

ASM_SOURCES := $(ASM_SOURCES) \
	$(SRC_DIR)/main/standalone/startup_stm32h7xx.s \

endif
