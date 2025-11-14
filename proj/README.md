# Project

This project is a C-based game for MINIX 3, developed for the Computer Laboratory course at FEUP.  
The primary objective was to apply the knowledge acquired in the lab assignments to develop an application that integrates several I/O devices (Timer, Keyboard, Mouse, Graphics Card, and RTC), handling their interrupts to create a fully functional, event-driven application.

## About the Game

This project is a C implementation of [Claustro](https://boardgamegeek.com/boardgame/391334/claustro), a 2-player, 5x5 abstract strategy board game. The goal is for a player to be the first to move one of their four pieces to their designated "goal" square.  
The game logic includes orthogonal movement, diagonal captures, and repositioning of pieces, all handled according to the game's official rules.

## Features

This project integrates multiple hardware devices to create a complete user experience:

* `Full Graphics Rendering`: The entire game, including menus, board, and pieces, is rendered in VBE graphics mode (800x600, 24-bit color)
 
* `Double Buffering`: Utilizes a secondary buffer to draw all game frames, which are then copied to video memory at a consistent 30 FPS. This prevents screen tearing and flickering

* `Dual Control Scheme`: The game seamlessly handles input from both the Keyboard (W/A/S/D + ENTER) and the Mouse (Cursor + Left-Click). Players can even alternate between them during a single move (e.g., select a piece with the keyboard and its destination with the mouse)

* `Dynamic Dark Mode`: The game automatically reads the system time from the Real-Time Clock (RTC). It switches to a "dark mode" theme (changing the board and piece colors) if the game is played between 20:00 and 08:00

* `Event-Driven State Machine`: The game logic is built as a state machine (handling states like `NEW_GAME`, `CHOOSE_PAWN`, `MOVEMENT`, `CAPTURE`, and `GAME_OVER`) . This machine is driven entirely by hardware interrupts from the peripherals

* `Move Highlighting`: When a player hovers the cursor over a valid piece, the game highlights all possible moves (both orthogonal movements and diagonal captures)

## Technical Implementation: I/O Device Integration

The core of the project is a central "driver-receive" loop that processes interrupts from all I/O devices to update the game state and render frames.

* `Graphics Card (Video)`: Used in VBE graphics mode to draw all visual elements. All rendering is done using custom-made XPM sprites

* `Timer (Timer 0)`: Configured to generate interrupts at 30 Hz (by dividing the default 60 Hz). The interrupt handler (`timer_action`) triggers the **double buffering** copy, ensuring a stable 30 FPS frame rate

* `Keyboard (KBD)`: Uses an interrupt handler (`kbd_ih`) to read scancodes upon key press/release. These events are processed in `kbd_action` to control menu navigation and piece movement (W, A, S, D, ENTER)

* `Mouse`: Uses an interrupt handler (`mouse_ih`) to read data packets in **stream mode**. The `mouse_action` function processes these packets to update the cursor's (x, y) position and detect left-clicks for selection

* `Real-Time Clock (RTC)`: Configured to send **update interrupts**. The handler (`rtc_action`) reads the current hour, minute, and second registers to check if the `dark_mode` flag should be enabled

## Demo