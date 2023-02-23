# SPRacing Pixel OSD

## What is it?

The SPRacing Pixel OSD system is a system that allows overlaying graphics, including dots; lines; shapes; text onto an
analog video signal (PAL or NTSC).  It also features signal generation code to generate the video signal sync pulses.

### Prototype hardware

<img src="https://raw.githubusercontent.com/spracing/spracingpixelosd/main/photos/IMG_20190718_222231.jpg" width="800">

Example OSD overlay, showing pixels and text:

<img src="https://raw.githubusercontent.com/spracing/spracingpixelosd/main/photos/IMG_20190519_212953.jpg" width="800">


### Random Development Videos

* SPRacingH7RF - 2020/05/16 - https://www.youtube.com/watch?v=yxXjFuN0uPc
* SPRacingH7CINE - 2020/03/04 - https://www.youtube.com/watch?v=1IUbb08_iKM

### Example of flight-controller PCBs

<img src="https://raw.githubusercontent.com/spracing/spracingpixelosd/main/photos/IMG_20230223_145659.jpg" width="1024">

Here you can see the typical PCB footprint, on the SPRacingH7CINE (Left), SPRacingH7RF (Center), SPRacingH7CL (Right) 


## Who made it?

Dominic Clifton created every part of the system, including design, architecture, schematics, prototypes and production
hardware.

## Features

* *Very* low-cost!
* Framebuffer based.
* Single or double buffered.
* 2 bits per pixel, black, white, grey, transparent.
* Adjustable resolution based on available framebuffer RAM and timer settings for pixel-clock.
* Very low CPU overhead, pixel stream is DMA based, CPU needs to respond promptly to video sync ISRs and transfer-complete ISRs.

## Hardware requirements

### MCU
* RAM for framebuffer.  Actual amount of RAM depends on how many lines of pixels you want to display and how big the pixels are.
* Hardware timers for Sync, Pixel, hardware timers need to be linked to the DAC and COMP and each other.
* Single DAC channel (2 DAC channels can be used for debugging purposes).
* Comparator.
* Clock speed fast enough to handle drawing operations, sync and pixel clocks.
* A GPIO port with 4 pins connected.  No other pins on the port can be configured to be DIGITAL OUT.  Pins on the same port CAN be
used in DIGITAL IN, Alternate Function, Etc. This is because the GPIO's ODR register is used.

The system works on many STMicro H7 CPUs and the L4 CPUs.  Others can be used.

The STM32L432KC was uses for prototyping as is very low cost and comes in extremely small 32-pin packages, has necessary
peripherals, an 80Mhz clock speed and enough RAM to support a good-resolution framebuffer.

STM32L432KC - https://www.st.com/en/microcontrollers-microprocessors/stm32l432kc.html

STM32H730VB - https://www.st.com/en/microcontrollers-microprocessors/stm32h730-value-line.html

For the H723/H730/H733/H743/H750 the following peripherals are used.
* DMA2 (2 streams, Stream 6 + 7).  1 stream for sync pulse generation, 1 stream for pixel generation.
* MDMA - for DMA based frame-buffer clearing.
* DAC1 - for generation of the voltage used by the comparator for video sync.
* COMP2 - for detecting sync pulses.
* ADC1 - for sampling video volages.
* TIM1 - Sync and video generation.
* TIM2 - ADC sampling.
* TIM15 - Pixel generation timer.
* GPIOE - Pixel generation IO.

### Additional components

* *Fast* Analog switch with negative voltage handling.
* *Fast* signal switching diode.
* Video filter + passives for sync-pulse noise rejection.
* ~12 passive components (resistors, capacitors).

## Products

### Commercially available

The SPRacingPixelOSD system is available in these products, this list will be updated as more are made.

| Manufacturer | Product      | Description                  | Website                                                |
|--------------|--------------|------------------------------|--------------------------------------------------------|
| SPRacing     | SPRacingH7RF | H730 based flight controller | http://seriouslypro.com/spracingh7rf                   |
| FlightOne    | LightningH7  | H730 based flight controller | https://flightone.com/flight_controllers.html          |

### Hobby/Maker/Hacker/Prototype projects

| Entity   | Project        | Description                       | Website |
|----------|----------------|-----------------------------------|---------|
| SPRacing | SPRacingH7CINE | H750 based flight controller      | N/A     |
| SPRacing | SPRacingH7CL   | H723/H733 based flight controller | N/A     |


## Why was it created?

For a long time the author has been wanting to push forward the technology used in flight-controller hardware and software.
FPV drones generally all use the same OSD chip, a MAX7452, which has long been discontinued by Maxim and is only available
from China-clone manufacturers under the AT7453/E. 

The MAX7456 chip has major limitations.
1) only supports characters.
2) is only available in a very large form factor.
3) is expensive, and contributes massively to the overall BOM cost.
4) has a 'large' PCB footprint for a 20x20mm flight controller.
5) limits software development which is stuck in the 1970's era of text based overlays where only text and crude text-based images
can be displayed (think Teletext/Ceefax).

To reduce BOM cost, and add features and reduce overall PCB footprint, the PixelOSD system can use the same processor
that runs the flight code.  An example of this can be found in the SPRacingH7RF which uses an STM32H730VBH6 MCU to run
flight code, Pixel OSD code and SPI ExpressLRS RF radio receiver code - all in a 20x20 mount PCB!

## When was it made?

The original prototype and code was created in April 2019.

## What license is it available under?

The schematics, documentation and code are available under the Apache License, content of the NOTICE file, repeated here
below, needs to be included wherever schematics, code and documentation are used or in any system that displays text via
the OSD system.  E.g. in standalone OSD systems, or when used by flight-control systems, e.g via way of a NOTICES menu
in a menu system, or a briefly displayed boot screen.

The NOTICE also needs to be included as a note on any schematics (e.g. PDFs, images) using schematics derived from this
project.  Please ensure that you add a text element in your EDA software containing the NOTICE.

If the above displaying of the NOTICE file isn't technically possible then please contact Dominic Clifton to discuss
potential solutions.

### NOTICE

```
SPRacingPixelOSD pixel-based on-screen-display system.
Copyright 2019-2023 Dominic Clifton.

This product includes software and hardware designs developed by Dominic Clifton.
* Dominic Clifton - http://dominicclifton.name
* Seriously Pro / SP Racing - http://seriouslypro.com

The Initial Developer of the SPRacingPixelOSD system is Dominic Clifton.
Copyright 2019 - 2023 Dominic Clifton. All Rights Reserved.
```

### IMPORTANT

As noted below, the code in the `prototype` folder contains code from other projects, which are not licensed under
the same terms as above and thus must NOT be used for production code as the licenses conflict with each other.
Many thanks to the respective authors, listed in the 'Acknowledgements and references' later in this document.

## Software

The software was originally written to be a stand-alone system that runs on an STM32L432KC and was developed using the 
NucleoL432KC development board.

https://www.st.com/en/evaluation-tools/nucleo-l432kc.html

The system was then later directly integrated into a popular flight control system, but was never released in this form.

However, for the curious, here's a branch with it:

https://github.com/spracing/betaflight/tree/bf-spracingh7cine-target-20200304-before-osdlib

Refer to the following directories:

https://github.com/spracing/betaflight/blob/bf-spracingh7cine-target-20200304-before-osdlib/src/main/target/SPRACINGH7CINE
https://github.com/spracing/betaflight/tree/bf-spracingh7cine-target-20200304-before-osdlib/src/main/drivers/spracingpixelosd
https://github.com/spracing/betaflight/tree/bf-spracingh7cine-target-20200304-before-osdlib/src/main/osd
https://github.com/spracing/betaflight/blob/bf-spracingh7cine-target-20200304-before-osdlib/src/main/io/displayport_spracing_pixel_osd.c

Later, the video sync and video generation code was moved into a library for a number of reasons:
1) re-use.
2) maintainability.
3) allows use with code compiled using a different compiler, as long as the ABI is common.
4) cannot be broken by changes to the flight-controller code.
5) allows the OSD bios code to be updated and released on a different schedule to the flight-controller code.

## Theory of operation.

### Video signal

A video signal is an analog signal.

It has two main parts.
1) Sync pulses to indicate start of field.
2) per field Horizontal line signals.

The sync pulses are output at the start of a frame, and are per-video system (e.g. PAL/NTSC).  PAL/NTSC can be detected
by counting the frame sync pulse lengths and the length of them.

The horizontal line signals comprises of a low voltage sync pulse, a black-level signal, a color bust, black
level again, then a per-line voltages, then black level again.

Some cameras output negative sync pulse voltages.
The sync pulse voltage can change over time, e.g. as a camera warms up or if the camera's average voltage level changes.

This system can handle both of the above.  However, most modern cameras output a 0-0.3 volt stable sync-voltage.

You can read more about the PAL/NTSC video signal specifications here:

http://martin.hinner.info/vga/pal.html

### Detecting sync

In order to detect sync, the system uses the following techniques.

1) Low-cost hardware Video filter to eliminate noise.  A FMS6141 was selected.

https://www.onsemi.com/products/custom-assp/audio-video-assp/video-conditioning/fms6141

2) A hardware RC low-pass filter comprised of a resistor and capacitor.

Formula: 1/2pi(RF)(CF) = 1/2pi(100)(570) = 2.79Mhz

100R and 560pF was used to get pretty close using standard values.  IMPORTANT: Use high-tolerance parts!

Refer to Renesas AN1269/AN1316 - "One Transistor Enables Clean HDTV and NTSC Video Sync Separation" 
See `application-notes` folder in this repository.

3) A fast comparator (usually internal to the MCU) that generates an ISR for both signal edge pulses.

4) A DAC to generate a reference voltage for the comparator.  No external pin is required when using a comparator in the
MCU as it is routed internally on the MCU, but the voltage can also be exposed on a pin, for debugging purposes.

5) A timer which is linked to the comparator edge transition signal, to record the sync pulse length in hardware via
a capture compare channel.

6) Gating of the comparator output signal for the horizontal field portion of the signal, so that voltages close to the 
comparator threshold voltage do not trigger the timer or additional ISRs.  This is technique is very important and is
achieved by using an additional channel on the comparator-transition-linked timer which is internally linked to the
comparator's blanking input.

7) Counting of short-vs-long sync pulses to detect the start of the frame and adjust the DAC output voltage.
That is the comparator voltage is either two high or two low if there are two few or two many incorrect-length pulses.

Video sync detection needs to be quick so that boot logos of flight control software can be displayed.

### Generating pixels

A second timer is started automatically by the sync-detection timer in hardware so that it always starts the correct
duration after the sync pulses.  It is critical that it's done in hardware so that pixels generated on the first
horizontal-line line-up with the pixels on subsequent lines.  The second timer cannot be started by software, thus
it is a hardware requirement that the pixel generation timer has both DMA update and is internally linked in the MCU to
the video sync timer.  On the L4 and H7 this means using TIM1 (sync) and TIM15 (pixel).

The timer is not started immediately, but needs to be started a short time after the sync pulse, thus another channel
on the sync timer is used as a delay timer, which when the delay expires triggers the pixel timer which runs until it
is stopped.

A DMA stream is configured, the source address is the linebuffer, the destination address is the ODR register of a GPIO
port.

The framebuffer contains the 2 bits per pixel image information.

Before a line of pixels can be output, a line-buffer needs to be created from the pixel-buffer. This is achieved by
mapping the bits that indicate the various pixel states to the corresponding bits that toggle GPIO pin output line leves
when the ODR is written to.

The pixel timer frequency is configured to so that one DMA update event is triggered for each pixel. When the DMA update
event is generated the DMA stream copies one element from the line buffer to the GPIO ODR register.

In order to reduce DMA bandwidth a only a single byte is transferred from the line buffer to the ODR register, but the
ODR register is a 16 bit register.  So either the high 8 bits of the ODR are used, or the low 8 bits are used.  That in
turn means that all the IO for the pixel generation circuit needs to be on the first 8 pins of the GPIO port or the 
last 8 pins of the GPIO port.  e.g.  0-7 or 8-15, not a mix of pins from 0-15.

The pixel timer is stopped by the DMA transfer-complete ISR.

When a sync pulse is generated, the software creates the line buffer from the frame buffer, this needs to be quick
and is done in the comparator interrupt handler.

### Video sync generation

Given that the timer for pixel generation is started by the timer linked to the comparator it's only possible to output
pixels when video sync is detected and present.

The system has the ability to generate a video signal with no pixels (i.e. just the field and line sync pulses).

The sync signal is generated by using the another channel on the same timer that is usually used for sync detection.
a PWM signal is used, the lengths of the pulses are transferred by DMA to the timer's registers, three registers are
used for this, ARR, REPETITION and CC1.  The DMA stream is configured in burst mode so that on a DMA update event occurs
three items are transferred to the timer registers.  Note that ARR, REPETITION and CC1 have addresses that form a
contigous space on both the DMA stream buffer and on the timer's peripheral memory space.  This is why only Channel 1 
can be used for sync generation, this in turn means that the timer's CC1 output pin is also the pin used in the 
schematic.

The lengths of the pulses for the PWM signal are pre-calculated by the software at compile-time and a two DMA buffers
are created, one for PAL and one for NTSC.

If a camera sync signal is present when also generating sync signals then the sync will be bad and the pixel
clock will be started at the wrong time resulting in display corruption.

Video sync output is only started if sync detection fails.

### Interrupts

Missing an interrupt is BAD.
Handling an interrupt late is BAD.

If ISRs are late or missing the following can occur:
1) Loss-of-sync.
2) Bad overlay output.
3) Black level held to long at end-of-frame which corrupts the sync pulse.

When integrating the OSD into flight controller software note that video ISR's need to be the highest priority ISRs in
the system.  For a flight controller this means that it's more important to handle the OSD than the GYRO, there's no
point handling a gyro EXTI if you can't see where you're going when flying FPV due to loss-of-sync!  One slightly late
handling of a gyro EXTI signal won't cause noticable flight behavior.

### Library 

The library is in the `library` folder of this repository.

The library uses two framebuffers, and this doubles the framebuffer memory requirement.

Two framebuffers are used so that the flight control software can draw into one framebuffer, when it has time, whilst
the video overlay is being displayed from the other framebuffer.

If the flight control software's scheduler is good enough, you can achieve 50(PAL)/60(NTSC) frames-per-second which 
looks amazing.  If the flight controller's scheduler doesn't devote much time to rendering, or the rendering code is
slow then the video overlay can be updated slower than 50/60 FPS but results in a sub-optimal user experience.

The library is compiled on a per-target basis, as the pin and hardware requirements differ per-target.

Currently, the build system for the library is designed to generate a binary which can be flashed to a target's external
flash.  The binary includes the hash for an EXST bootloader so that the EXST bootloader can verify the binary is not
corrupted.

The per-target pins, flash address and memory space is configured via the target's files. See the linker script `.ld`
and make `.mk` and header `.h` files in `library\src\main\<TARGET>\*`

For integration into flight control system, the flight controller linker scripts needs to reserve some RAM used by
the library.  An example of this can be found here:

https://github.com/spracing/betaflight/blob/565e48a1460bf01f4053733e78051ecdda55cb76/src/link/stm32_ram_h730_exst_spracingpixelosd.ld#L58-L82

The flight control system also needs to verify the presence of the library at the configured memory addresses. An
example of this is here:

https://github.com/spracing/betaflight/blob/565e48a1460bf01f4053733e78051ecdda55cb76/src/main/drivers/spracingpixelosd/spracing_pixel_osd_library.c#L39-L42

The flight controller needs to ensure that it reserves hardware resources that might otherwise be used by it, and
then call initialisation functions in the library.  An example of this is here:

https://github.com/spracing/betaflight/blob/565e48a1460bf01f4053733e78051ecdda55cb76/src/main/drivers/spracingpixelosd/spracing_pixel_osd.c#L341-L368

The flight control system also needs to ensure that interrupts for the OSD system are routed to the library.

And example of this can be found here:

https://github.com/spracing/betaflight/blob/565e48a1460bf01f4053733e78051ecdda55cb76/src/main/drivers/spracingpixelosd/spracing_pixel_osd.c#L101-L125

The flight control system then needs to draw to the frame buffer and handle the `onVSync` callback from the library.
The `onVSyncHandler` can be used so the flight controller knows which of the two frame buffers to draw into and which
is being used to display the video overlay.

Some example frame-buffer rendering code can be found here:

https://github.com/spracing/betaflight/blob/565e48a1460bf01f4053733e78051ecdda55cb76/src/main/drivers/spracingpixelosd/framebuffer.c

Some example high-level drawing routine implementation can be found here:

https://github.com/spracing/betaflight/blob/565e48a1460bf01f4053733e78051ecdda55cb76/src/main/drivers/spracingpixelosd/framebuffer_canvas.c

The library can be compiled so that:
1) it is stored in MCU internal flash (e.g. STM32H743).
2) it is stored on external flash and copied to RAM and run from RAM (e.g. STM32H750).
3) it is stored on external flash and run from external flash via memory mapped support (e.g. STM32H730 via OCTOSPI).
4) copied to RAM via a debugger and run from RAM.

All the above scenarios were performed during development of the library.

### Building

```
cd `library`
make TARGET=SPRACINGH7RF
```

### Artifacts

Artifacts appear in the `build` folder.

The following artifacts are generated:

1) ELF file.
2) ELF file patched for EXST bootloaders.
3) Binary, suitable for flashing to an MCU.

### Flashing

Example script for flashing via DFU can be found in `library\support\scripts\build_and_flash.sh`.

### Debugging

The library can be compiled with debug symbols and source-level debugged using GDB, just load the symbols from the 
`.elf` artifact.

e.g. For the SPRacingH7RF use this GDB commands:

```
add-symbol-file /library/build/SPRACINGH7RF_DEVELOPER_DEBUG.elf
```

### Prototype

The original prototype code, which was unmaintained after the initial development can be found in the `prototype` folder.
 
WARNING!  Do NOT use the code from the `prototype`folder other than for learning/education purposes.  i.e. sure, run
it if you like, but for anything commercial use the code in the `library` folder.

Furthermore, the code in the prototype contains code from other projects, which are not licensed under the same terms as
the library code and thus must not be used for production code as the licenses conflict with each other.

The code in the `library` folder, when compared to the `prototype` code has:
* improved video sync detection.
* improved video sync stability in noisy environments. (e.g. when used on a quad with more electrical noise).
* better video output.
* bug fixes.
* numerous additions.

## Schematics

There are two schematics, neither have been recently updated and this repository has been created at least
two years after they were created so there might be minor changes.

### Prototype Schematic

There is a schematic for the prototype using an STM32L432KC, available here, in PDF, PNG and DipTrace formats:

<img src="https://raw.githubusercontent.com/spracing/spracingpixelosd/main/schematics/spracingpixelosd/prototype/SPRacingPixelOSD-L432-Prototype-RevA.png" width="800">

See here: https://github.com/spracing/spracingpixelosd/tree/main/schematics/spracingpixelosd/prototype

### Reference schematic

There is a reference schematic for an STM32H7, available here, in PDF, PNG and DipTrace formats:

<img src="https://raw.githubusercontent.com/spracing/spracingpixelosd/main/schematics/spracingpixelosd/reference/SPRacingH7OSD-RevA.png" width="800">

See here: https://github.com/spracing/spracingpixelosd/tree/main/schematics/spracingpixelosd/reference


## Potential Improvements

1) The sync stability could be improved, by using the comparater-timer-triggered-delayed ADC conversion to periodically
sample the sync-voltage and dynamically adjust the thresholds and DAC voltage.  On production boards this wasn't found
to be necessary but some code that initialses the ADC was written for this purpose.  This is also why ADC1 is reserved
for video, so DO NOT use TIM2 and ADC1 in flight-controller code.
2) Speed-up line-buffer generation.
3) Use 4 more IO lines to generate different shades of grey, no extra DMA bandwidth required, but the framebuffer memory
requirement would double.  Would use more CPU time to generate line buffers, adds additional load to flight controller 
rendering code (e.g 4-8 bits per pixel instead of 2).


## Alternatives

At the time this document was written (2023/02/23) there are no known framebuffer-based open-source royalty free analog
OSD systems.

## Differences to other Pixel based OSD systems

* Vortex - The vortex OSD is great, but closed source and long discontinued, no published core or schematics.
* F1shpepper Tiny OSD - Isn't frame-buffer based. Doesn't integrate well into FC systems, no published schematics, not used in commercial projects.
* FrSkyOSD - More grey-scales, uses a resistor ladder, requires purchase from FrSky, high BOM cost, similar overall PCB footprint, no published code or schematics. Uses a UART for FC->OSD communication, no direct frame-buffer access.
* BrainFPV - FPGA based, no published code or schematics.
* SuperOSD - Uses two PIC processors, low resolution, old, unmaintained, archived.

## History/Timeline

Dominic Clifton has wanted to improve the state of OSD systems used in drones since way back when everyone was using the
Arduino MAX7456 based MininOSD system with MultiWII way back in 2014.

* 2010 - SuperOSD system by Thomas Oldbury created.
* 2013/2014 - MinimOSD system first used by Dominic with MultiWII.
* 2015 - Dominic designed and shipped the SPRacingF3 flight controller under the SPRacing brand.
* 2015 - Dominic started designing the SPRacingOSD add-on board for the SPRacingF3.  The idea was to create an OSD to
replace the MinimOSD and then later transitiion the SPRacingOSD to a pixel based OSD.  The latter never happened due to
the amount of work required on the flight controller and Cleanflight projects.
* 2016 - SPRacingOSD add-on board shipped.
* 2016 - Immersion RC Vortex 285 shipped with Pixel based OSD, but limited OSD/FC integration with Cleanflight.
* 2017 - Fishpepper posts details and code for his pixel-based, but not framebuffer based, tinyOSD.
* 2019-Q1 - INAV OSD repo created, schematic used an STM32F103.  Dominic contacted F1am and initially had some technical discussions, but no schematics or
running code were shared between either parties.  F1am was then offered a job by FrSky and communicaton ceased due to terms with
FrSky. F1am wanted a PixelOSD system that can be used by any flight controller, with a UART based protocol.
Dominic, still wanting a pixel OSD system then spend a week designing a schematic and a week writing protocol code and
got pixels displayed on the screen, see photos folder.  Note that it's no-coincidence the original prototype
SPRacingPixelOSD prototype and the final production both use STM32L43x MCUs due to technical discussions.  Likely some
of the use of STM processor hardware features was used by both projects in the same way, but there's no confirmation of this.
* 2019-Q2 - SPRacingPixelOSD ported to H7.
* 2019-Q3 - SPRacingPixelOSD code integrated into Betaflight.
* 2019-Q4 - FlightOne interested in PixelOSD system, Dominic designed the LightningH7 Flight Controller for FlightOne.  SPRacingPixelOSD code moved into a library, to be used by both Betaflight and FlightOne and Dominic integrated into both.
* 2020-Q1 - FlightOne shipped the LightningH7 FC with the SPRacingPixelOSD library.
* 2020-Q2 - FrSky shipped the FrSkyOSD.
* 2021-Q1 - Dominic designed the SPRacingH7RF FC with ELRS SPI, SPRacingPixelOSD, STM32H730 MCU and OctoSPI memory mapped flash.
* 2021-Q4 - SPRacing shipped the SPRacingH7RF.

flight controller, the idea was to write OSD code for an F3 using initially using a MAX7456 and then later to design
a pixel based system

## Acknowledgements and references.

Thomas Oldbury - SuperOSD system. https://code.google.com/archive/p/super-osd/
MWOSD - MultiWII OSD. http://www.mwosd.com/
F1shpepper - TinyOSD video timing calculation.  https://fishpepper.de/2019/03/11/tinyosd-tinyfinity-a-tiny-opensource-video-tx-with-full-graphic-osd/
F1am - Prototype MCU selection ideas and technical discussions - https://github.com/fiam / https://www.frsky-rc.com/product/frsky-osd/
Maxim - Datasheets and application notes.
Renasas - Datasheets and application notes.
ImmersionRC - A vortex OSD schematics was reverse engineered before this project was completed, included in schematics, some technical details were investigated but discarded. https://www.immersionrc.com/fpv-products/vortex-racing-quad/
FrSky - An FrSky OSD schematic was reverse engineered /after/ this project was completed, included in schematics.
SPRacing Customers - For being awesome and supporting this work though purchases and feedback.


## Errors and omissions

Likely there's some errors and omissions, as they are discovered they will be corrected.  Please raise pull-requests via
github to submit any corrections.