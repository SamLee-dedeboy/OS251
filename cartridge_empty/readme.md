Sample usage of our API are demonstrated inside `main.c`
## Graphics Utility Functions Implemented and Listed in `graphic.h`
- Bug when using `setRefreshRate()`: sprites cannot move if th refresh rarte is other than default 0.
- `createRecSprite()`: sets the color (entry_num) outside of specified width and height to 0. So we ecommend setting the 0 entry of the palette to transparent for desired rectangle.

## System call APIs
- `getTicks`
- `getTimer`
- `getMode`: get current video mode
- `getStatus`: get controller status
- multithreading api not yet merged


## notes for us: 
- spriteDrop() doesn't need to be implemented as a system call
- sample usage of multithreading
- why is `SystemCall()` declared inside `graphic.h`
- what is the difference between `getTicks` and `getTimer`?
- update API documentation
