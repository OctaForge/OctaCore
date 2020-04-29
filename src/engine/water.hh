#ifndef ENGINE_WATER_HH
#define ENGINE_WATER_HH

#include <shared/geom.hh>

#define WATER_AMPLITUDE 0.4f
#define WATER_OFFSET 1.1f

extern int vertwater;

void loadcaustics(bool force = false);
void renderwaterfog(int mat, float blend);

void preloadwatershaders(bool force = false);

void renderlava();
void renderwater();
void renderwaterfalls();

const bvec &getwatercolour(int mat);
const bvec &getwaterdeepcolour(int mat);
const bvec &getwaterfallcolour(int mat);
int getwaterfog(int mat);
int getwaterdeep(int mat);
float getwaterrefract(int mat);

const bvec &getlavacolour(int mat);
int getlavafog(int mat);

#endif
