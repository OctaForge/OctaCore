#ifndef ENGINE_STAIN_HH
#define ENGINE_STAIN_HH

#include <shared/geom.hh>

enum { STAINBUF_OPAQUE = 0, STAINBUF_TRANSPARENT, STAINBUF_MAPMODEL, NUMSTAINBUFS };

struct stainrenderer;

void initstains();
void clearstains();
bool renderstains(int sbuf, bool gbuf, int layer = 0);
void cleanupstains();
void genstainmmtri(stainrenderer *s, const vec v[3]);

#endif