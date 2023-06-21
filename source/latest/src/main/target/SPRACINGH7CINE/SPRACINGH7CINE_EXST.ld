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

_firmware_d1_ram_size = 64K; /* 512KB - 448KB = 64KB */

/* see .exst section below */
_exst_hash_size = 64;

/* Specify the memory areas */
MEMORY
{
DTCMRAM_REGION (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
LIB_DTCMRAM    (xrw) : ORIGIN = ORIGIN(DTCMRAM_REGION) + LENGTH(DTCMRAM_REGION) - 4K, LENGTH = 4K

RAM_D1_REGION  (rwx) : ORIGIN = 0x24000000, LENGTH = 512K
LIB_D1_RAM     (rwx) : ORIGIN = ORIGIN(RAM_D1_REGION) + _firmware_d1_ram_size - 2K, LENGTH = 2K

RAM_D2_REGION  (xrw) : ORIGIN = 0x30000000, LENGTH = 256K
FRAMEBUFFER_RAM (rw) : ORIGIN = ORIGIN(RAM_D2_REGION), LENGTH = _spracing_pixel_osd_framebuffer_size
LIB_D2_RAM      (rw) : ORIGIN = ORIGIN(FRAMEBUFFER_RAM) + LENGTH(FRAMEBUFFER_RAM), LENGTH = _library_d2_ram_size 
RAM_D2         (xrw) : ORIGIN = ORIGIN(LIB_D2_RAM) + LENGTH(LIB_D2_RAM), LENGTH = LENGTH(RAM_D2_REGION) - LENGTH(FRAMEBUFFER_RAM) - LENGTH(LIB_D2_RAM)

RAM_D3_REGION  (xrw) : ORIGIN = 0x38000000, LENGTH = 64K

ITCMRAM_REGION (xrw) : ORIGIN = 0x00000000, LENGTH = 64K
LIB_CODE        (rx) : ORIGIN = ORIGIN(ITCMRAM_REGION) + LENGTH(ITCMRAM_REGION) - 32K, LENGTH = 32K - _exst_hash_size
EXST_HASH       (rx) : ORIGIN = ORIGIN(LIB_CODE) + LENGTH(LIB_CODE), LENGTH = _exst_hash_size

FLASH_REGION    (rx) : ORIGIN = 0x08000000, LENGTH = 128K
}

INCLUDE "src/main/stm32h7xx_common.ld"

SECTIONS
{
  /* Create space for a hash.  Currently an MD5 has is used, which is 16   */
  /* bytes long. however the last 64 bytes are RESERVED for hash related   */
  .exst_hash :
  {
    /* 64 bytes is the size of an MD5 hashing block size.                  */
    . = ORIGIN(EXST_HASH);

    /* 4 byte header */
    BYTE(0x00); /* block format */
    BYTE(0x00); /* Checksum method, 0x00 = MD5 hash */
    BYTE(0x00); /* Reserved */
    BYTE(0x00); /* Reserved */
    
    /* Fill the last 60 bytes with data, including an empty hash aligned   */
    /* to the last 16 bytes.                                               */
    FILL(0x00000000); /* Reserved */
    
    . = ORIGIN(EXST_HASH) + LENGTH(EXST_HASH) - 16;
    __md5_hash_address__ = .;
    LONG(0x00000000);
    LONG(0x00000000);
    LONG(0x00000000);
    LONG(0x00000000);
    . = ORIGIN(EXST_HASH) + LENGTH(EXST_HASH);
  } >EXST_HASH
}
