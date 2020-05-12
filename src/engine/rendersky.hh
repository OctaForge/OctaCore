#ifndef ENGINE_RENDERSKY_HH
#define ENGINE_RENDERSKY_HH

extern int skyshadow, explicitsky;

void cleanupsky();
bool limitsky();
void drawskybox(bool clear = false);
bool hasskybox();

#endif
