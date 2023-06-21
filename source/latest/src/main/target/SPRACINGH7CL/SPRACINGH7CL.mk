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

ifneq ($(EXST),)
EXST = YES
EXST_ADJUST_VMA = 0x97CA0000
LIBRARY_SIZE   := 32
LD_SCRIPT       = $(SRC_DIR)/main/target/$(TARGET)/$(TARGET)_EXST.ld
endif

ifneq ($(EXST),YES)
LD_SCRIPT       = $(SRC_DIR)/main/target/$(TARGET)/$(TARGET)_RAM.ld
endif

MCU_OPTIONS = \
	STM32H730xx \
	STM32H7xx \
	USE_USB_HS \

BUILD_MODE = RUNTIME_LIBRARY