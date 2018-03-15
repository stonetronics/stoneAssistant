stoneAssistant

This repository contains (pcb, firmware, mechanical) design files for the stoneAssistant project.
The stoneAssistant is meant to be a home-entertainment device built around a raspberry pi.

Planned features:
 - Raspberry Pi 3
 - LCD Touch Screen (official Raspberry Pi Touch Screen)
 - 15W Stereo sound
 - 12V Supply
 - 3 USB Ports
 - Speech Commands
   - microphone preamp
 - Pushbuttons for optional control (Home button, Play/Pause, etc)
 - ATtiny
   - RGB LED for nice appearance
   - Shutdown and Power Control
   - Controlable Fan
   - Serial Commands
 - 3D-printed enclosure
 
The device is powered by a 230VAC/12VDC switching supply and another 12VDC/5VDC DC/DC converter that powers the Raspberry Pi, ATtiny and USB Ports.
For power control (5V on Raspberry Pi, Software controlable mains switch), fan control and the RGB LEDs an ATtiny2312 chip is used. The chip implements a serial command interface on the uart to talk to the Raspberry Pi and vice versa.
The software (RingoWare) for touch screen control, media, etc is currently developed by https://github.com/Ringo96 and there will be a repo by him soon.
All the mechanical design files are drawn in FreeCAD 0.16. All the screw holes are designed to work with M3 screws. The PCBs sitting on top of the raspberry pi are meant to be held in place by standard M3 PCB spacers.


Files:

PCB files (Eagle 8)
- /PCBs/stoneAssistant_pwrMgmt.sch Power management and pwm schematic
- /PCBs/stoneAssistant_pwrMgmt.brd Power management and pwm board

Firmware files (Atmel Studio 7)
- /ATtinyFirmware/stoneAssistantController		Atmel Studio Solution for the ATtiny on the pwrMgmt board
 
Mechanical files (FreeCAD 0.16)
- /Mechanical/buttonPrototype    Prototype for Button with stopper
- /Mechanical/Frontplate         Frontplate that holds the raspberry pi display, buttons and speakers - prototype, did not fit in 3d printer
- /Mechanical/RPIMount           Mounting that holds the raspberry pi + PCBs on the raspberry pi display
- /Mechanical/Housing			 Model of the housing


Serial Command Interface:

The serial command interface is using the uart and is based on a simple protocol, described below:

1 packet:
[Startbyte(0xaa)][Length][payload 0][payload 1] ... payload[length - 1][Stopbyte(0x55)]

In the payload of this protocol, the commands are transmitted. The following commands are implemented:
- [R][x] set red led intensity to x (0-255) (rpi->attiny)
- [G][x] set green led intensity to x (0-255) (rpi->attiny)
- [B][x] set blue led intensity to x (0-255) (rpi->attiny)
- [F][x] set fan speed to x (0-255) (rpi->attiny)
- [C][0] shutdown in 15s (rpi->attiny)
- [C][1] shutdown request (attiny->rpi)
- [C][6] cut rpi power and turn it on again in 15s (rpi->attiny)
- [C][9] hard reset rpi immediately (rpi->attiny)
- [C][10] hard shutdown immediately (rpi->attiny)
