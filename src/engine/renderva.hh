#ifndef ENGINE_RENDERVA_HH
#define ENGINE_RENDERVA_HH

#include <shared/geom.hh>

#include "octa.hh"
#include "renderlights.hh" /* LIGHTTILE_MAXH */

extern vtxarray *visibleva;

bool isfoggedsphere(float rad, const vec &cv);
int isvisiblesphere(float rad, const vec &cv);
int isvisiblebb(const ivec &bo, const ivec &br);

void visiblecubes(bool cull = true);

extern int deferquery;

int getnumqueries();
void flipqueries();
void resetqueries();
occludequery *newquery(void *owner);
void startquery(occludequery *query);
void endquery(occludequery *query);
bool checkquery(occludequery *query, bool nowait = false);

void startbb(bool mask = true);
void endbb(bool mask = true);
void drawbb(const ivec &bo, const ivec &br);
bool bboccluded(const ivec &bo, const ivec &br);

void rendermapmodels();
void renderoutline();
void renderblendbrush(GLuint tex, float x, float y, float w, float h);

int calcspheresidemask(const vec &p, float radius, float bias);
int cullfrustumsides(const vec &lightpos, float lightradius, float size, float border);

extern vec shadoworigin, shadowdir;
extern float shadowradius, shadowbias;
extern int shadowside, shadowspot, shadowtransparent;

void findshadowvas(bool transparent = false);
void findshadowmms();

int dynamicshadowvas();
int dynamicshadowvabounds(int mask, vec &bbmin, vec &bbmax);

void rendershadowmapworld();
void batchshadowmapmodels(bool skipmesh = false);
void renderrsmgeom(bool dyntex = false);

struct shadowmesh;
struct extentity; /* FIXME: remove */

void clearshadowmeshes();
void genshadowmeshes();
shadowmesh *findshadowmesh(int idx, extentity &e);
void rendershadowmesh(shadowmesh *m);

extern int oqfrags;
extern float alphafrontsx1, alphafrontsx2, alphafrontsy1, alphafrontsy2, alphabacksx1, alphabacksx2, alphabacksy1, alphabacksy2, alpharefractsx1, alpharefractsx2, alpharefractsy1, alpharefractsy2;
extern uint alphatiles[LIGHTTILE_MAXH];

int findalphavas();
void renderrefractmask();
void renderalphageom(int side);
void renderalphashadow(bool cullside = false);

bool renderexplicitsky(bool outline = false);

void renderdecals();

void cleanupva();
void rendergeom();

#endif
