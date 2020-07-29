#ifndef ENGINE_PHYSICS_HH
#define ENGINE_PHYSICS_HH

#include <shared/geom.hh>

struct dynent;
struct physent;
struct extentity;
struct entity;

extern vec collidewall;
extern int collideinside;
extern physent *collideplayer;

bool overlapsdynent(const vec &o, float radius);
void rotatebb(vec &center, vec &radius, int yaw, int pitch, int roll = 0);
float shadowray(const vec &o, const vec &ray, float radius, int mode, extentity *t = NULL);

void moveplayer(physent *pl, int moveres, bool local);
bool moveplayer(physent *pl, int moveres, bool local, int curtime);
void crouchplayer(physent *pl, int moveres, bool local);
bool collide(physent *d, const vec &dir = vec(0, 0, 0), float cutoff = 0.0f, bool playercol = true, bool insideplayercol = false);
bool bounce(physent *d, float secs, float elasticity, float waterfric, float grav);
bool bounce(physent *d, float elasticity, float waterfric, float grav);
void avoidcollision(physent *d, const vec &dir, physent *obstacle, float space);
bool movecamera(physent *pl, const vec &dir, float dist, float stepdist);
void physicsframe();
void dropenttofloor(entity *e);
bool droptofloor(vec &o, float radius, float height);

void vecfromyawpitch(float yaw, float pitch, int move, int strafe, vec &m);
void vectoyawpitch(const vec &v, float &yaw, float &pitch);
void updatephysstate(physent *d);
void cleardynentcache();
void updatedynentcache(physent *d);
bool entinmap(dynent *d, bool avoidplayers = false);

#endif
