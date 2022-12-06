#if !defined(_VIDEO_API_H_)
#define _VIDEO_API_H_

#include <stdint.h>

void controlLargeSprite(uint32_t idx, uint32_t h, uint32_t w, uint32_t x, uint16_t y);
void controlBackground(uint32_t idx, uint32_t x, uint32_t y, uint32_t z, uint32_t palette);
int backgroundPaletteMemory(uint32_t idx, uint8_t * addr, uint32_t mem_len);
int spritePaletteMemory(uint32_t idx, uint8_t * addr, uint32_t mem_len);
int transparentSpritePalette(uint32_t idx);
int setBackgroundDataImage(uint32_t idx, uint8_t * addr);
int setLargeSpriteDataImage(uint32_t idx, uint8_t * addr);
#define DISPLAY_MODE_TEXT 0
#define DISPLAY_MODE_GRAPHICS 1
void setDisplayMode(uint32_t mode);
int getTicks(void);
int getStatus(void);
// internal interfaces, export for debug only
#if 1
uint32_t writeTargetMem(uint32_t mem_handle, uint32_t source_addr, uint32_t mem_len);
int writeToMemoryWithIndex(uint32_t offset, uint32_t mem_size, uint32_t idx, uint32_t * addr, uint32_t mem_len);
#endif

#endif // _VIDEO_API_H_
