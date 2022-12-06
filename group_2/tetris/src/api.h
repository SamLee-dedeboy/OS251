// API List
uint32_t getTicks(void);
uint32_t getStatus(void);
uint32_t genRandom(int high);
void setColor(int palette_id, int entry_id, uint32_t rgba);
void setBackgroundColor(int palette_id, int entry_id, uint32_t rgba);
uint32_t calcSmallSpriteControl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t calcLargeSpriteControl(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t p);
uint32_t calcBackgroundControl(uint32_t x, uint32_t y, uint32_t z, uint32_t p);
void setSmallSpriteControl(int sprite_id, uint32_t addr);
void setLargeSpriteControl(int sprite_id, uint32_t addr);
void setBackgroundSpriteControl(int sprite_id, uint32_t addr);
void shiftSmallSpriteControl(int sprite_id, uint32_t x, uint32_t y);
void shiftLargeSpriteControl(int sprite_id, uint32_t x, uint32_t y);
uint32_t getSmallSpriteControl(int sprite_id);
uint32_t getLargeSpriteControl(int sprite_id);
uint32_t getBackgroundSpriteControl(int sprite_id);
void setGraphicsMode(void);
void setTextMode(void);
void printLine(char* string);
// threads
typedef uint32_t *TContext;
typedef void (*TEntry)(void*);
TContext InitContext(uint32_t *stacktop, TEntry entry, void *param);
void SwitchContext(TContext *old, TContext new);
// Video Interrupt
int getVideoInterruptCount();