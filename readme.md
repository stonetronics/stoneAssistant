stoneAssistant

This repository contains (pcb, mechanical) design files for the stoneAssistant project.
The stoneAssistant is meant to be a home-entertainment device built around a raspberry pi.

Planned features:
 - Raspberry Pi 3
 - LCD Touch Screen (official Raspberry Pi Touch Screen)
 - 15W Stereo sound
 - 12V Supply
 - 3 USB Ports
 - Speech Commands
   o microphone preamp
 - Pushbuttons for optional control (Home button, Play/Pause, etc)
 - ATtiny
   o RGB LED for nice appearance
   o Shutdown and Power Control
   o Controlable Fan
   o Serial Commands
 - 3D-printed enclosure
 
The device will be powered by a 230VAC/12VDC switching supply and another 12VDC/5VDC DC/DC converter that powers the Raspberry Pi, ATtiny and USB Ports.
For power control (5V on Raspberry Pi, Software controlable mains switch), fan control and the RGB LEDs an ATtiny2312 chip will be used. The Chip must implement a Serial command interface to talk to the Raspberry Pi and vice versa.
The Software (RingoWare) for touch screen control, Media, etc will be done by https://github.com/Ringo96

PCB files (Eagle 8)
 coming soon!
 
Mechanical files (FreeCAD 0.16)
 coming soon!
