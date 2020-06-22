#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "cube.hh"
#include "world.hh"
#include "light.hh"

extern dynent *player;
extern physent *camera1;                // special ent that acts as camera, same object as player1 in FPS mode

extern int worldscale, worldsize;
extern int xtraverts, xtravertsva;

#endif
