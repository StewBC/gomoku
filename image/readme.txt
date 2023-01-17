To Create the C64 version of the bitmap, I used these commands:
Saved the image (160x200) as indexed into a gif file
magick.exe mogrify -format RGB .\Gomoku160.gif
python data2asm.py -i .\Gomoku160.rgb -n splash -w 160 -p .\pal.txt -b 3
I used the first image (splash_flush_data) and turned it into the image that can be seen in main.c
Compiled and ran main.c to give me bitmap.bin
lzsa -r -f2 bitmap.bin Gomoku.lzh
copy .\Gomoku.lzh ..\src\c64\Gomoku.lzh
