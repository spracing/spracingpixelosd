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


/* Note: No entry point, this is for a non-relocatable library. */
/* Note: No stack/heap configuration. */

_spracing_pixel_osd_framebuffer_size = (360 * 288 * 2 / 8) * 2; /* (H * V * Bit-per-pixel / Bits-per-byte) * 2 frame buffers */
_spracing_pixel_osd_pixelbuffer_size = (360 + 1) * 2;           /* (H + 1) * 2 pixel buffers */
_library_d2_ram_size = MAX(1K, (_spracing_pixel_osd_pixelbuffer_size) + 32); /* extra bytes required for alignment */ 

/* Define output sections */
SECTIONS
{
  .framebuffer_dma_ram (NOLOAD) :
  {
    . = ALIGN(32);
    PROVIDE(__library_d2_ram_start = .);
    PROVIDE(__frame_buffer_start = .);
    KEEP(*(.framebuffer_ram))
    PROVIDE(__frame_buffer_end = .);
  } >FRAMEBUFFER_RAM

  .library_d2_ram (NOLOAD) :
  {
    . = ALIGN(32);
    PROVIDE(__pixel_buffer_start = .);
    KEEP(*(.pixelbuffer_ram))
    PROVIDE(__pixel_buffer_end = .);
    KEEP(*(.library_d2_ram))
    
    PROVIDE(__library_d2_ram_end = .);
  } >LIB_D2_RAM

  .library_d1_ram (NOLOAD) :
  {
    . = ALIGN(32);
    PROVIDE(__library_d1_ram_start = .);
    KEEP(*(.library_d1_ram))
    
    PROVIDE(__library_d1_ram_end = .);
  } >LIB_D1_RAM

  .library_api :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__library_descriptor_start = .);
    KEEP(*(.library_descriptor)); 
    . = ALIGN(4);
    PROVIDE_HIDDEN (__library_vtable_start = .);
    KEEP(*(.library_vtable)); 
    . = ALIGN(4);
  } >LIB_CODE
  
  .library_code :
  {
    . = ALIGN(4);
    __library_code_start = .;
    *(.text)
    *(.text*)
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))
    
    . = ALIGN(4);
    __library_code_end = .;
  } >LIB_CODE
  
  .ARM.extab   : {
    *(.ARM.extab* .gnu.linkonce.armextab.*) 
  } >LIB_CODE
  
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >LIB_CODE

  __library_initalized_data = LOADADDR(.library_data);

  .library_data : 
  {
    . = ALIGN(4);
    __library_data_start = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    __library_data_end = .;
  } >LIB_DTCMRAM AT> LIB_CODE

  __library_d1_initalized_data = LOADADDR(.library_d1_data);

  .library_d1_data : 
  {
    . = ALIGN(4);
    __library_d1_data_start = .;
    *(.library_d1_data)
    *(.library_d1_data*)
    . = ALIGN(4);
    __library_d1_data_end = .;
  } >LIB_D1_RAM AT> LIB_CODE

  __library_d2_initalized_data = LOADADDR(.library_d2_data);

  .library_d2_data : 
  {
    . = ALIGN(4);
    __library_d2_data_start = .;
    *(.library_d2_data)
    *(.library_d2_data*)
    . = ALIGN(4);
    __library_d2_data_end = .;
  } >LIB_D2_RAM AT> LIB_CODE

  . = ALIGN(4);
    
  .library_bss (NOLOAD):
  {
    __library_bss = .;
    *(.bss)
    *(SORT_BY_ALIGNMENT(.bss*))
    *(COMMON)

    . = ALIGN(4);
    __library_bss_end = .;
  } >LIB_DTCMRAM
  
  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}


