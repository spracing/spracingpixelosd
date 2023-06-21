ifeq ($(MCU_OPTIONS),)
$(error No MCU options specified)
endif

ifneq (,$(findstring STM32L4xx,$(MCU_OPTIONS)))
    include make/mcu_l4.mk
endif

ifneq (,$(findstring STM32H7xx,$(MCU_OPTIONS)))
    include make/mcu_h7.mk
endif
