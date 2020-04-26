#ifndef __GAME_H__
#define __GAME_H__

#include "cube.hh"

// console message types

enum
{
    CON_CHAT       = 1<<8
};

enum                            // static entity types
{
    NOTUSED = ET_EMPTY,         // entity slot not in use in map
    LIGHT = ET_LIGHT,           // lightsource, attr1 = radius, attr2 = intensity
    MAPMODEL = ET_MAPMODEL,     // attr1 = idx, attr2 = yaw, attr3 = pitch, attr4 = roll, attr5 = scale
    PLAYERSTART,                // attr1 = angle, attr2 = team
    ENVMAP = ET_ENVMAP,         // attr1 = radius
    PARTICLES = ET_PARTICLES,
    MAPSOUND = ET_SOUND,
    SPOTLIGHT = ET_SPOTLIGHT,
    DECAL = ET_DECAL,
    MAXENTTYPES,

    I_FIRST = 0,
    I_LAST = -1
};

#define MAXNAMELEN 15

namespace entities
{
    extern vector<extentity *> ents;
    extern void preloadentities();
}

namespace game
{
    // game
    extern string clientmap;

    extern dynent *player1;

    // client
    extern bool connected;

    // render

    extern void saveragdoll(dynent *d);
    extern void clearragdolls();
    extern void moveragdolls();
}

#endif
