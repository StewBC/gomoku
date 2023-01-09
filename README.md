# Gomoku
The game of Gomoku written for 8-bit computers using cc65  
  
0. UPDATES  
Jan 7, 2023 - Initial Version - Apple 2 and Windows Console (Powershell)  
  
1. INTRODUCTION  
  
From Wikipedia:  
Gomoku, also called Five in a Row, is an abstract strategy board game. It is traditionally played with Go pieces (black and white stones) on a Go board. It is played using a 15×15 board.  
  
Players alternate turns placing a stone of their color on an empty intersection. Black plays first. The winner is the first player to form an unbroken chain of five stones horizontally, vertically, or diagonally. Placing so that a line of more than five stones of the same color is created does not result in a win. These are called overlines.  
  
2. CURRENT STATUS  
  
The game plays a decent game of Gomoku on Hard AI.  The medium and easy modes can be very easy - good for younger kids.  If left at a menu or splash screen, the game will time-out and return to the splash screen from where it will start a demo mode.  The demo mode is just two AIs of random skill playing against one-another.

3. KEYS
  
The game does have help, but in general the keys are:  
```
 * WASD or Cursor keys to navigate menu's and the cursor on the board  
 * ENTER or SPACE to select/change a menu item, or place a piece  
 * U to Undo the last move (but not the winning move)  
 * R to Redo a move that was Un-done  
 * ESC to back up  
 ```  
  
4. TECHNICAL DETAILS  
  
The game is written to be easy to port.  The Windows Command port is an example of such a port.  All the game code is separate from the platform code.  In the src folder are port folders, such as apple2.  The apple2 specific version lives there.  The plat_* functions need to be implemented for any port.  The win-cmd port has one file, derived from the apple 2 version and is an example of how to make a port.  
  
    │   Makefile               - GNU Make style make file - see (6) below  
    │   Makefile-dsk.mk        - Uses apple commander to make an apple2 .dsk file  
    │  
    ├───apple2  
    │       template.dsk       - Blank ProDOS disk which becomes gomoku.dsk  
    │  
    ├───image                  - Source Art folder  
    │       Gomoku.bmp         - The splash screen image (280x192 140 expanded to 280)  
    │  
    └───src                    - Platform independent files live here  
        │   globals.h          - Types/structures and global variables visible to platform also  
        │   main.c             - The game code - logic AI etc  
        │   plat.h             - The platform specific function definitions     shared by independent code  
        │  
        ├───apple2             - Apple 2 port specific code  
        │       assembly.s     - Mainly drawing code in ASM for speed purposes  
        │       data.c         - Definition of tile and font data  
        │       data.h         - Definition of data to be used by plat-a2.c  
        │       decomp.s       - LZSA decompression Copyright (C) 2019 Emmanuel Marty  
        │       gomoku.cfg     - cc65 config file  
        │       Gomoku.lzh     - Gomoku splash screen LZSA compressed  
        │       plat-a2.c      - Apple 2 platform specific code (menu/draw/etc)  
        │  
        └───win-cmd            - Windows Console (Powershell) port  
                plat-win-cmd.c - Console platform specific code (menu/draw/etc)  
  
The windows version works in cmd.exe but works better in PowerShell because it uses the ASCII codes to do color (works in both) and clear screen ("```\x1b[2J\x1b[H```" works only in PowerShell, not in CMD.exe).  The windows version is not at all sophisticated, it's a proof of concept, and it also shows the "score board" the AI uses to think, and I used that to debug the undo code.  The windows console has to be user adjusted to fit the contents.  
  
I may make ports to more machines, and these will sit alongside apple2 and win-cmd.  I should not have to change any files in the src folder, only files in the src sub-folders for the port being made.  

Note to self:  To make the Gomoku.lzh I used the following on the 24BPP BMP of 280x192 in the image folder:  
```
    b2d.exe Gomoku.bmp gomuku hgr  
    lzsa -r -f2 GOMOKUC.BIN Gomoku.lzh  
```  
  
5. THE AI - HOW IT THINKS  
  
The AI is actually quite simple.  For any cell, I look at the neighbors and count lines of same-color neighbors in the 8 directions from a cell.  The length of the line leading from a cell indexes into this array to give a score:  
```0,1,3,7,23,0```  
If the line ends in a space, additional score is given by indexing into this array:  
```0,1,2,8,32```  
All of the scores are summed and that's the value of the tile.  As an example, where W represents white and B represents black, the following happens at X:  
```
    W  
     W  
     BX  
```
X will get 3 points for a length of 2 whites and will get 1 point for the black.  Since the black has a space on the other side, it will also get 1 point for the space, for a total value of 3+1+1=5.  
The only other situation worth explaining is when a line is broken up by one space, for example:  
```ZABBXCY```  
If A is played as Black, and C is also black, and X, Y and Z are spaces, the score at X will be C+B+B+A+Z, or length 4 (23) + space Z (at length 4, so 32) for a total of 23+32 = 55 (0x37 in the win-cmd display).  The space at Y does not score.  Perhaps it should?  
  
For skill level, the game randomly picks 3 moves from the highest, second highest and third highest value squares.  If the skill level is hard, the game will play the chosen highest scoring move.  On medium, the game will randomly choose the highest or second highest tile and on easy it will randomly choose between all three the selected tiles.  In easy, and medium sometimes, the game will thus make very bad choices.  On hard the game does well, but there may very well be exploits.  I did not try to find holes in the AI, I just engaged it as I would play another person, for fun.  
  
6. BUILDING THE GAME  
  
Using GNU make is the easiest way to build the game.  In the Makefile, near the top is this line:  
```TARGETS := apple2 win-cmd```  
To work on just one target, only list that target, or use this command to build just, for example, the apple2 target:  
```make TARGETS=apple2```  
To use AppleCommander to make a .dsk image for the Apple2, and to invoke AppleWin to debug, use:  
```make all dsk test or make TARGETS=apple2 all dsk test```  
To use another method of building a disk for the Apple 2, or to use a different debugger, modify the Makefile.  More about this Makefile can be found at: https://wiki.cc65.org/doku.php?id=cc65:project_setup  

7. CREDITS  
  
* Oliver Schmidt who wrote the Makefile and got me going on the Apple 2  
* Emmanuel Marty for the LZSA code, see file header for details
* Bill Buckels for Bmb2HDR which I used to make the splash screen HGR from my GIMP exported BMP  
* Everyone involved in the Apple II projects (AppleWin | AppleCommander)  
* Everyone involved in making the cc65 tools, it's very good  
  
8. CONTACT  
  
Feel free to contact me at swessels@email.com if you have thoughts or suggestions.  
  
Thank you  
Stefan Wessels  
7 January 2023 - Initial Revision  
