#ifndef ENGINE_WORLD_HH
#define ENGINE_WORLD_HH

#include <shared/geom.hh>
#include <shared/ents.hh>

/* FIXME: move these defines/enums */

enum                            // hardcoded texture numbers
{
    DEFAULT_SKY = 0,
    DEFAULT_GEOM,
    NUMDEFAULTSLOTS
};

enum
{
    MATSURF_NOT_VISIBLE = 0,
    MATSURF_VISIBLE,
    MATSURF_EDIT_ONLY
};

#define TEX_SCALE 16.0f

#include "octa.hh"

extern vector<int> outsideents;
extern vector<int> entgroup;

extern int worldscale, worldsize;

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
