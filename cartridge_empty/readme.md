Sample usage of our API are demonstrated inside `main.c`
## Graphics APIs
graphics utility functions are implemented and listed in `graphic.h`
- `createRecSprite()`: sets the color(entry_num) outside of specified width and height to 0. So we recommend setting the 0 entry of the palette to transparent for desired rectangle.

 - `void setVideoMode(int mode)`: Changes the mode of video controller. 0 = TEXT_MODE 1 = GRAPHICS_MODE
 - `void setRefreshRate(uint8_t rate)`: Changes the refresh rate of the screen. A bug appears when setting the refrsh value to oher than default 0, cannot move sprites.

 - `void setBackgroundPalette(int32_t palette_num, int32_t entry_num, uint32_t ARGB)`: 
 - `void setSpritePalette(int32_t palette_num, int32_t entry_num, uint32_t ARGB)`

 - `int16_t createRecSprite(int32_t x, int32_t y, uint32_t w, uint32_t h, int32_t palette_num, int8_t colorEntry)`
 - `void moveSprite(int16_t sprite_num, uint32_t d_x, uint32_t d_y)`
 - `void changeSpritePalette(int16_t sprite_num, int32_t palette_num)`
 - `uint32_t calcLargeSpriteControl(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t p)`
 - `uint32_t calcSmallSpriteControl(int32_t x, int32_t y, int32_t z, uint32_t w, uint32_t h, uint32_t p)`

 - `void drawText(char* text, uint32_t length, int32_t x, int32_t y)`
 - `void clearTextScreen()`

 - `void backgroundDrawRec(int8_t backgroundNum, int32_t x, int32_t y, uint32_t w, uint32_t h, int8_t colorEntry)`
 - `void setBackgroundControl(int8_t backgroundNum, int32_t x, int32_t y, int32_t z, int32_t palette_num)`
 - `void changebackgroundPalette(int16_t sprite_num, int32_t palette_num)`

## System call APIs
- `getTicks`
- `getTimer`
- `getMode`: get current video mode
- `getStatus`: get controller status
- multithreading api not merged yet


## notes for us: 
- `spriteDrop()` doesn't need to be implemented as a system call
- sample usage of multithreading
- why is `SystemCall()` declared inside `graphic.h`?
- what is the difference between `getTicks` and `getTimer`?
- update API documentation
