# MINIX-NES Emulator 
An NES emulator codded intirely on C using device drivers developed in for LCOM@FEUP
<br><br/>
[![Static Badge](https://img.shields.io/badge/OS-Minix%203-blue)](https://minix3.org/)
<br><br/>

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [System Requirements](#system-requirements)
4. [Installation](#installation)
5. [Usage](#usage)
6. [Akacknowledgements](#acknowledgements)


## Introduction
Welcome to the NES Emulator! This project aims to provide a emulator for the Nintendo Entertainment System (NES) written from the ground up to be portable and easily connected to any device drivers, be it Linux, Windows or even Minix, allowing users to play classic NES games on modern hardware.

## Features
- Accurate emulation of NES CPU, PPU, cartidge loading, mapping and contoller connection.
- Support for the popular NES file formats (.nes) with iNES headers.
- Serial connection to real controller through a ESP8266 being used as a bridge.
- Various resolutions and scalling.
- Very fast loading and unloading of games.

## System Requirements
At this time this repo only runs on the Minix image developed for the LCOM subject at FEUP, you can download said image [Here](https://drive.google.com/file/d/1dM32zTzkTUEqNtTwsO-n__XO6r1Y5I-f/view?usp=sharing).

## Installation
From releases: Download the latest release follow the instructions in the release page.

From source:
<code>(move to shared folder with minix)
git clone git@github.com:itsnova204/LCOM.git
(move the roms that roms/roms.txt says into the roms/ folder)
if you wish to use serial port pipe the usb into virtualbox.
IN MINIX:
    cd labs/src/
    make
    lcom_run proj
<code>

## Usage

To run use `lcom_run proj`
You can also use these flags:
| Syntax | Description |
| --- | ----------- |
| --no-uart | Disables serial connection |
| --player-1-serial | Swaps controller 1 from keyboard to Serial |
| --vmode 0x115 | Enables VBE_MODE_DC_24 video mode while emulator running |

(Don`t forget to wrap the flags in " ")

## Controls
IN MENU:
Use mouse to select game.
Use Right and Left keyboard arrows to change game page.

IN GAME:
![NES KB CTRL](https://github.com/itsnova204/LCOM/blob/main/doc/image.png?raw=true)

## Acknowledgements
Big Thanks to:
Professor Nuno Cardoso: for never giving up on us and incentivising us to be do learn more.
OneLoneCoder, Javidx9: for his series on NES emulator development and his community for helping me understand how the NES works under the hood.
