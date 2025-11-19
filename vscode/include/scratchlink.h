#ifndef __SCRATCHLINK__
#define __SCRATCHLINK__

const int ScratchLink_Say = 0x81;
const int ScratchLink_Led = 0x82;

extern void init_scratchlink(const char *devicename);
extern void scratchlink_update(int accx, int accy, bool buttonA, bool buttonB, bool touch0, bool touch1, bool touch2, int gesture);
extern void scratchlink_callback(byte command, byte *payload, unsigned int size);
extern bool scratchlink_isconnecting();

#endif
