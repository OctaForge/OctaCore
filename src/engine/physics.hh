#ifndef ENGINE_PHYSICS_HH
#define ENGINE_PHYSICS_HH

#include <shared/geom.hh>

struct extentity;

bool overlapsdynent(const vec &o, float radius);
void rotatebb(vec &center, vec &radius, int yaw, int pitch, int roll = 0);
float shadowray(const vec &o, const vec &ray, float radius, int mode, extentity *t = NULL);

#endif
