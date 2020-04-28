#ifndef ENGINE_WORLD_HH
#define ENGINE_WORLD_HH

#include <shared/tools.hh>
#include <shared/geom.hh>

#include <shared/ents.hh>

/* FIXME: move these defines/enums */

enum                            // hardcoded texture numbers
{
    DEFAULT_SKY = 0,
    DEFAULT_GEOM,
    NUMDEFAULTSLOTS
};

#define WATER_AMPLITUDE 0.4f
#define WATER_OFFSET 1.1f

enum
{
    MATSURF_NOT_VISIBLE = 0,
    MATSURF_VISIBLE,
    MATSURF_EDIT_ONLY
};

#define TEX_SCALE 16.0f

struct vertex { vec pos; bvec4 norm; vec tc; bvec4 tangent; };

struct selinfo
{
    int corner;
    int cx, cxs, cy, cys;
    ivec o, s;
    int grid, orient;
    selinfo() : corner(0), cx(0), cxs(0), cy(0), cys(0), o(0, 0, 0), s(0, 0, 0), grid(8), orient(0) {}
    int size() const    { return s.x*s.y*s.z; }
    int us(int d) const { return s[d]*grid; }
    bool operator==(const selinfo &sel) const { return o==sel.o && s==sel.s && grid==sel.grid && orient==sel.orient; }
    bool validate()
    {
        extern int worldsize;
        if(grid <= 0 || grid >= worldsize) return false;
        if(o.x >= worldsize || o.y >= worldsize || o.z >= worldsize) return false;
        if(o.x < 0) { s.x -= (grid - 1 - o.x)/grid; o.x = 0; }
        if(o.y < 0) { s.y -= (grid - 1 - o.y)/grid; o.y = 0; }
        if(o.z < 0) { s.z -= (grid - 1 - o.z)/grid; o.z = 0; }
        s.x = clamp(s.x, 0, (worldsize - o.x)/grid);
        s.y = clamp(s.y, 0, (worldsize - o.y)/grid);
        s.z = clamp(s.z, 0, (worldsize - o.z)/grid);
        return s.x > 0 && s.y > 0 && s.z > 0;
    }
};

#include "octa.hh"

extern vector<int> outsideents;
extern vector<int> entgroup;

extern int spotlights;
extern int volumetriclights;
extern int nospeclights;

void freeoctaentities(cube &c);
void entitiesinoctanodes();

bool pointinsel(const selinfo &sel, const vec &o);
bool haveselent();
void entcancel();

void attachentities();

vec getselpos();

undoblock *copyundoents(undoblock *u);
void pasteundoent(int idx, const entity &ue);
void pasteundoents(undoblock *u);

void entselectionbox(const entity &e, vec &eo, vec &es);

void entdrag(const vec &ray);

void renderentselection(const vec &o, const vec &ray, bool entmoving);

bool hoveringonent(int ent, int orient);

void resetmap();
void startmap(const char *name);

bool emptymap(int factor, bool force, const char *mname = "", bool usecfg = true);
bool enlargemap(bool force);

int getworldsize();

#endif
