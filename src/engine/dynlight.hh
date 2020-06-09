#ifndef ENGINE_DYNLIGHT_HH
#define ENGINE_DYNLIGHT_HH

#include <shared/geom.hh>

struct physent;

enum
{
    DL_SHRINK = 1<<8,
    DL_EXPAND = 1<<9,
    DL_FLASH  = 1<<10
};

void adddynlight(const vec &o, float radius, const vec &color, int fade = 0, int peak = 0, int flags = 0, float initradius = 0, const vec &initcolor = vec(0, 0, 0), physent *owner = NULL, const vec &dir = vec(0, 0, 0), int spot = 0);
void dynlightreaching(const vec &target, vec &color, vec &dir, bool hud = false);
void removetrackeddynlights(physent *owner = NULL);

void updatedynlights();
int finddynlights();
bool getdynlight(int n, vec &o, float &radius, vec &color, vec &dir, int &spot, int &flags);

#endif
