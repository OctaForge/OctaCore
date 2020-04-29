#ifndef ENGINE_MATERIAL_HH
#define ENGINE_MATERIAL_HH

#include "renderlights.hh"
#include "octa.hh"

extern uint matliquidtiles[LIGHTTILE_MAXH], matsolidtiles[LIGHTTILE_MAXH];

extern float matliquidsx1, matliquidsy1, matliquidsx2, matliquidsy2;
extern float matsolidsx1, matsolidsy1, matsolidsx2, matsolidsy2;
extern float matrefractsx1, matrefractsy1, matrefractsx2, matrefractsy2;

extern int showmat;

int findmaterial(const char *name);
const char *findmaterialname(int mat);
const char *getmaterialdesc(int mat, const char *prefix = "");

void genmatsurfs(const cube &c, const ivec &co, int size, vector<materialsurface> &matsurfs);
void calcmatbb(vtxarray *va, const ivec &co, int size, vector<materialsurface> &matsurfs);
int optimizematsurfs(materialsurface *matbuf, int matsurfs);

void setupmaterials(int start = 0, int len = 0);
int findmaterials();
void rendermaterialmask();
void renderliquidmaterials();
void rendersolidmaterials();
void rendereditmaterials();
void renderminimapmaterials();

#define GETMATIDXVAR(name, var, type) \
    type get##name##var(int mat) \
    { \
        switch(mat&MATF_INDEX) \
        { \
            default: case 0: return name##var; \
            case 1: return name##2##var; \
            case 2: return name##3##var; \
            case 3: return name##4##var; \
        } \
    }

#endif
