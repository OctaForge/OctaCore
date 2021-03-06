#ifndef ENGINE_PVS_HH
#define ENGINE_PVS_HH

#include <sauerlib/stream.hh>

#include <shared/geom.hh>

void clearpvs();
bool pvsoccluded(const ivec &bbmin, const ivec &bbmax);
bool pvsoccludedsphere(const vec &center, float radius);
bool waterpvsoccluded(int height);
void setviewcell(const vec &p);
void savepvs(stream *f);
void loadpvs(stream *f, int numpvs);
int getnumviewcells();

static inline bool pvsoccluded(const ivec &bborigin, int size)
{
    return pvsoccluded(bborigin, ivec(bborigin).add(size));
}

#endif
