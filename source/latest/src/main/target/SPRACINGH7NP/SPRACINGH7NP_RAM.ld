/*
 * Copyright 2019-2023 Dominic Clifton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Specify the memory areas */
MEMORY
{
DTCMRAM_REGION (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
LIB_DTCMRAM    (xrw) : ORIGIN = ORIGIN(DTCMRAM_REGION) + LENGTH(DTCMRAM_REGION) - 4K, LENGTH = 4K

RAM_D1_REGION  (rwx) : ORIGIN = 0x24000000, LENGTH = 512K
LIB_D1_RAM     (rwx) : ORIGIN = ORIGIN(RAM_D1_REGION) + LENGTH(RAM_D1_REGION) - 2K, LENGTH = 2K

RAM_D2_REGION  (xrw) : ORIGIN = 0x30000000, LENGTH = 256K
FRAMEBUFFER_RAM (rw) : ORIGIN = ORIGIN(RAM_D2_REGION), LENGTH = _spracing_pixel_osd_framebuffer_size
LIB_D2_RAM      (rw) : ORIGIN = ORIGIN(FRAMEBUFFER_RAM) + LENGTH(FRAMEBUFFER_RAM), LENGTH = _library_d2_ram_size 
RAM_D2         (xrw) : ORIGIN = ORIGIN(LIB_D2_RAM) + LENGTH(LIB_D2_RAM), LENGTH = LENGTH(RAM_D2_REGION) - LENGTH(FRAMEBUFFER_RAM) - LENGTH(LIB_D2_RAM)

RAM_D3_REGION  (xrw) : ORIGIN = 0x38000000, LENGTH = 64K

ITCMRAM_REGION (xrw) : ORIGIN = 0x00000000, LENGTH = 64K
LIB_CODE        (rx) : ORIGIN = ORIGIN(ITCMRAM_REGION) + LENGTH(ITCMRAM_REGION) - 32K, LENGTH = 32K

FLASH_REGION    (rx) : ORIGIN = 0x08000000, LENGTH = 128K
}

INCLUDE "src/main/stm32h7xx_common.ld"

SECTIONS
{
  /* Due to issues with BMP debugger not transferring all the data this debug marker is currently required */
  /* See https://github.com/blacksphere/blackmagic/issues/447 */
  /* The issue still occurs, a new test-case is required. */

  .debug_marker :
  {
    . = ALIGN(16);
    LONG(0xDEB9DEB9)
    LONG(0xDEB9DEB9)
    LONG(0xDEB9DEB9)
    LONG(0xDEB9DEB9)
    . = ALIGN(16);
  } >LIB_CODE
}
