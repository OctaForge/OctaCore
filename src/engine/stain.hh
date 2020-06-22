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

enum
{
    STAIN_BLOOD = 0,
    STAIN_PULSE_SCORCH,
    STAIN_RAIL_HOLE,
    STAIN_PULSE_GLOW,
    STAIN_RAIL_GLOW
};

extern void addstain(int type, const vec &center, const vec &surface, float radius, const bvec &color = bvec(0xFF, 0xFF, 0xFF), int info = 0);

static inline void addstain(int type, const vec &center, const vec &surface, float radius, int color, int info = 0)
{
    addstain(type, center, surface, radius, bvec::hexcolor(color), info);
}

#endif
