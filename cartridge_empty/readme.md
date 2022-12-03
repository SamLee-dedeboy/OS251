Sample usage of our API are demonstrated inside `main.c`
## Graphics utility functions are implemented and listed in `graphic.h`
- `createRecSprite()`: sets the color(entry_num) outside of specified width and height to 0. So we recommend setting the 0 entry of the palette to transparent for desired rectangle.
- Bug when using `setRefreshRate()`: sprites cannot move if the refresh rate is set to values other than default 0.

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
