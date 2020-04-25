#ifndef BLEND_HH
#define BLEND_HH

#include <shared/gl.hh>

#include <shared/tools.hh>
#include <shared/geom.hh>

extern int worldsize; /* FIXME: remove */

void optimizeblendmap();
void resetblendmap();
void enlargeblendmap();
void shrinkblendmap(int octant);
bool usesblendmap(int x1 = 0, int y1 = 0, int x2 = worldsize, int y2 = worldsize);
void cleanupblendmap();

int calcblendlayer(int x1, int y1, int x2, int y2);
void bindblendtexture(const ivec &p);
void updateblendtextures(int x1 = 0, int y1 = 0, int x2 = worldsize, int y2 = worldsize);
void renderblendtexture(uchar *dst, int dsize, int dx, int dy, int dw, int dh);
void clearblendtextures();

extern int blendpaintmode;

void stoppaintblendmap();
void trypaintblendmap();

void renderblendbrush(GLuint tex, float x, float y, float w, float h);
void renderblendbrush();

bool loadblendmap(stream *f, int info);
void saveblendmap(stream *f);
uchar shouldsaveblendmap();

/* currently not used */

struct BlendMapCache;
BlendMapCache *newblendmapcache();
void freeblendmapcache(BlendMapCache *&cache);
bool setblendmaporigin(BlendMapCache *cache, const ivec &o, int size);
bool hasblendmap(BlendMapCache *cache);
uchar lookupblendmap(BlendMapCache *cache, const vec &pos);

#endif
