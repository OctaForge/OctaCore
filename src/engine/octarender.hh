#ifndef ENGINE_OCTARENDER_HH
#define ENGINE_OCTARENDER_HH

#include <shared/tools.hh>
#include <shared/geom.hh>

#include "octa.hh"

extern int wtris, wverts, vtris, vverts, glde, gbatches;
extern int allocva;

extern ivec worldmin, worldmax, nogimin, nogimax;
extern vector<tjoint> tjoints;
extern vector<vtxarray *> varoot, valist;

ushort encodenormal(const vec &n);
void guessnormals(const vec *pos, int numverts, vec *normals);
void reduceslope(ivec &n);
void findtjoints();
void octarender();
void allchanged(bool load = false);
void clearvas(cube *c);
void destroyva(vtxarray *va, bool reparent = true);
void updatevabb(vtxarray *va, bool force = false);
void updatevabbs(bool force = false);

#endif
