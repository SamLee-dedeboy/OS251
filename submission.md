## Build steps
Different versions of our demo are located at:
- `cartridge_empty/`: Baseline Cartridge provided for other groups to start with.
- `tetris/`: Tetris game developed on our own OS.
- `group_0/`: Tetris game developed on Group0's OS. 
- `group_2/`: Tetris game developed on Group2's OS. 

All directory include a Makefile to build the game. Simply execute `make clean` and `make` to build. 

## Header Structure
The api calls provided by our OS include:
- `api.h`: Defines the system call identifier macros. All of our system calls are implemented with `systemcall(FUNC_ID)`. A dispatcher in the kernel will read the `FUNC_ID` and execute the corresponding system call. By doing this, we don't need to manually load the function identifier to `a5` in `crt0.s` as demonstrated in previous discussion examples. We further provide a wrapper for each system call so that developers can use them as normal api calls. For example:
```C
uint32_t getStatus()
{
    return systemcall(CONTROLLER_STATUS, 0, 0, 0, 0, 0);
}

// then in cartridge:
int main() 
{
   // ...
   controller_status = getStatus();
   // ...
}
```

- `systemcall.h`: declares all the system related system calls
- `graphic.h`: declares all the graphic related system calls
- `thread.h`: declares all the multi-thread related system calls

## Graphics APIs
Graphics utility functions are listed in `graphic.h`

Note: functions marked with `*` requires the arguments to be merged by `ab = merge_arg(a, b)` before assigning to the function in order to reduce the number of parameters. `merge_arg(a,b)` is a simple utility macro provided to merge two arguments:
```C 
// graphic.h
#define merge_arg(a, b) ((a<<16) | (b))
```
### Video controls
 - `int setVideoMode(int mode)`
    - Changes the mode of video controller. 0 = TEXT_MODE; 1 = GRAPHICS_MODE.
    - On success will return 1, upon failure will return -1.

 - `void setRefreshRate(uint8_t rate)`
    - Changes the refresh rate of the screen.
    - ⚠️A bug appears when setting the refresh value to other than default 0, cannot move sprites

### Backgrounds
 - `int setBackgroundPalette(uint8_t palette_num, uint32_t entry_num, uint32_t ARGB)`
    - Sets color of the specified entry of the specified background palette. 
    - `palette_num` can be only 0 to 3. `entry_num` can be only 0 to 255.
    - On success will return 1, upon failure will return -1.

 - *`int backgroundDrawRec(uint8_t background_num, uint32_t xy, uint32_t wh, uint8_t colorEntry)`
    - Draw rectangles onto specified background_num.
    - Call `setBackgroundControl()` upon first time drawing on this background_num.
    - On success will return 1, upon failure will return -1.

 - *`int setBackgroundControl(uint8_t background_num, uint32_t xy, uint32_t z, uint8_t palette_num)`
    - Sets the control value of specified background_num.
    - On success will return 1, upon failure will return -1.

 - `int changeBackgroundPalette(uint8_t background_num, uint8_t palette_num)`
    - Changes the palette number of specified background.
    - `background_num` should be 0 to 3.
    - `palette_num` should be 0 to 3.
    - On success will return 1, upon failure will return -1.

### Sprites
 - `int setSpritePalette(uint8_t palette_num, uint32_t entry_num, uint32_t ARGB)`
    - Sets color of the specified entry of the specified sprite palette.
    - palette_num can be only 0 to 3. entry_num can be only 0 to 255.
    - On success will return 1, upon failure will return -1.

 - *`uint16_t createRecSprite(uint32_t xy, uint32_t wh, uint8_t palette_num, uint8_t colorEntry)`
    - Creates a rectangular sprite of specified width and height at the desired position.
    - If the input width and height are both smaller than 16, then a small sprite will be created; otherwise creates a larges sprite.
    - If total number of small sprites exceeds 128, then the first small sprite will be overwritten.
    - If total number of large sprites exceeds 64, then the first large sprite will be overwritten.
    - Color(entry_num) outside specified width and height will be set to 0. So we recommend setting the 0 entry of the palette to transparent for desired rectangle.
    - Returns a unique sprite index. Numbers 0 to 127 are small sprites, 128 to 191 are large sprites.

 - `int moveSprite(uint16_t sprite_num, uint32_t d_x, uint32_t d_y)`
    - Moves the specified sprite by the input amount of x and y.
    - `sprite_num` should be the unique sprite index.
    - On success will return 1, upon failure will return -1.

 - `int changeSpritePalette(uint16_t sprite_num, uint8_t palette_num)`
    - Changes the palette number of specified sprite.
    - `sprite_num` should be the unique sprite index.
    - `palette_num` should be 0 to 3.
    - On success will return 1, upon failure will return -1.

 - *`uint32_t calcSmallSpriteControl(uint32_t xy, uint32_t wh, uint8_t p)`
    - Returns the control value of small sprites.
    - the `z` value will be set to 4.

 - *`uint32_t calcLargeSpriteControl(uint32_t xy, uint32_t wh, uint8_t p)`
    - Returns the control value of large sprites.

### Texts
 - `int drawText(char* text, uint32_t length, int32_t x, int32_t y)`
    - Draws text at specified position.
    - x range: 0 to 63; y range: 0 to 35
    - On success will return 1, upon failure will return -1.

 - `void clearTextScreen()`
    - Clears all text data memory.

### Acquire Video Memory Address
   - `uint32_t getBACKGROUND_DATA_ADDRESS(uint32_t background_num)`
   - `uint32_t getSPRITE_DATA_ADDRESS(uint32_t sprite_num)`
   - `uint32_t getBACKGROUND_PALLETE_ADDRESS(uint32_t palette_num, uint32_t entry_num)`
   - `uint32_t getSPRITE_PALLETE_ADDRESS(uint32_t palette_num, uint32_t entry_num)`
   - `uint32_t getTEXT_DATA_ADDRESS()`
   - `uint32_t getBACKGROUND_CONTROL_ADDRESS(uint32_t background_num);` 
   - `uint32_t getSPRITE_CONTROL_ADDRESS(uint32_t sprite_num`)
   - `uint32_t getMODE_CONTROL_REGISTER()`


## System APIs
System utility functions are listed in `systemcall.h`
- `getTimer()`: returns a value that is increased upon every interrupt. Value rounds back to 0 when it reaches 999.
- `getMode()`: get current video mode. 0 = TEXT_MODE; 1 = GRAPHICS_MODE.
- `getStatus()`: get controller status.
- `enableCMD()` & `disableCMD()`: enable & disable CMD button. We hard-coded the CMD button to be used for switching between Text and Graphics mode. Therefore we provide this api to let developers choose whether to enable CMD button or not. 
- `getMachinePeriod()`: reads `Machine Clock Period Register`. This api was requested by another group. 
- `getMachineTime()`: reads `Machine Time (mtime)`. This api was requested by another group.
- `getIntPendingReg()`: reads `Interrupt Pending Flag`. This api was requested by another group. 
- `Rand_sys()`: generates random numbers. ('_sys' to avoid naming conflict with stdlib 'Rand()' function)

- `thread_init()`: initializes a new thread to run specified function. 
   - Read Thread function identifiers (`THREAD_INITIALLIZE`), thread function and thread function's parameters.
   - Sample usage in `cartridge_empty/thread_sample.c`. To run this file please change `main.o` in the Makefile to `thread_sample.o` and re-build the binary. 
- `printtext()`: print out text to screen when OS was in text mode.
   - Read function(`WRITE_TEXT`), and printout parameters.
   - Sample usage: `printtext(WRITE_TEXT, "This is a sample usage \n");`. 
