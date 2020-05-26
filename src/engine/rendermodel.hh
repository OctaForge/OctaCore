#ifndef ENGINE_RENDERMODEL_HH
#define ENGINE_RENDERMODEL_HH

#include <shared/tools.hh>
#include <shared/geom.hh>

#include "bih.hh"
#include "renderlights.hh"

enum { MDL_CULL_VFC = 1<<0, MDL_CULL_DIST = 1<<1, MDL_CULL_OCCLUDED = 1<<2, MDL_CULL_QUERY = 1<<3, MDL_FULLBRIGHT = 1<<4, MDL_NORENDER = 1<<5, MDL_MAPMODEL = 1<<6, MDL_NOBATCH = 1<<7, MDL_ONLYSHADOW = 1<<8, MDL_NOSHADOW = 1<<9, MDL_FORCESHADOW = 1<<10, MDL_FORCETRANSPARENT = 1<<11 };

extern float transmdlsx1, transmdlsy1, transmdlsx2, transmdlsy2;
extern uint transmdltiles[LIGHTTILE_MAXH];

enum { MDL_MD2 = 0, MDL_MD3, MDL_MD5, MDL_OBJ, MDL_SMD, MDL_IQM, NUMMODELTYPES };

struct Texture;
struct Shader;
struct dynent;
struct modelattach;

// FIXME
#define COLLIDE_OBB 2

struct model
{
    char *name;
    float spinyaw, spinpitch, spinroll, offsetyaw, offsetpitch, offsetroll;
    bool shadow, alphashadow, depthoffset;
    float scale;
    vec translate;
    BIH *bih;
    vec bbcenter, bbradius, bbextend, collidecenter, collideradius;
    float rejectradius, eyeheight, collidexyradius, collideheight;
    char *collidemodel;
    int collide, batch;

    model(const char *name) : name(name ? newstring(name) : NULL), spinyaw(0), spinpitch(0), spinroll(0), offsetyaw(0), offsetpitch(0), offsetroll(0), shadow(true), alphashadow(true), depthoffset(false), scale(1.0f), translate(0, 0, 0), bih(0), bbcenter(0, 0, 0), bbradius(-1, -1, -1), bbextend(0, 0, 0), collidecenter(0, 0, 0), collideradius(-1, -1, -1), rejectradius(-1), eyeheight(0.9f), collidexyradius(0), collideheight(0), collidemodel(NULL), collide(COLLIDE_OBB), batch(-1) {}
    virtual ~model() { DELETEA(name); DELETEP(bih); }
    virtual void calcbb(vec &center, vec &radius) = 0;
    virtual void calctransform(matrix4x3 &m) = 0;
    virtual int intersect(int anim, int basetime, int basetime2, const vec &pos, float yaw, float pitch, float roll, dynent *d, modelattach *a, float size, const vec &o, const vec &ray, float &dist, int mode) = 0;
    virtual void render(int anim, int basetime, int basetime2, const vec &o, float yaw, float pitch, float roll, dynent *d, modelattach *a = NULL, float size = 1, const vec4 &color = vec4(1, 1, 1, 1)) = 0;
    virtual bool load() = 0;
    virtual int type() const = 0;
    virtual BIH *setBIH() { return NULL; }
    virtual bool envmapped() const { return false; }
    virtual bool skeletal() const { return false; }
    virtual bool animated() const { return false; }
    virtual bool pitched() const { return true; }
    virtual bool alphatested() const { return false; }

    virtual void setshader(Shader *shader) {}
    virtual void setenvmap(float envmapmin, float envmapmax, Texture *envmap) {}
    virtual void setspec(float spec) {}
    virtual void setgloss(int gloss) {}
    virtual void setglow(float glow, float glowdelta, float glowpulse) {}
    virtual void setalphatest(float alpha) {}
    virtual void setfullbright(float fullbright) {}
    virtual void setcullface(int cullface) {}
    virtual void setcolor(const vec &color) {}

    virtual void genshadowmesh(vector<triangle> &tris, const matrix4x3 &orient) {}
    virtual void preloadBIH() { if(!bih) setBIH(); }
    virtual void preloadshaders() {}
    virtual void preloadmeshes() {}
    virtual void cleanup() {}

    virtual void startrender() {}
    virtual void endrender() {}

    void boundbox(vec &center, vec &radius)
    {
        if(bbradius.x < 0)
        {
            calcbb(bbcenter, bbradius);
            bbradius.add(bbextend);
        }
        center = bbcenter;
        radius = bbradius;
    }

    float collisionbox(vec &center, vec &radius)
    {
        if(collideradius.x < 0)
        {
            boundbox(collidecenter, collideradius);
            if(collidexyradius)
            {
                collidecenter.x = collidecenter.y = 0;
                collideradius.x = collideradius.y = collidexyradius;
            }
            if(collideheight)
            {
                collidecenter.z = collideradius.z = collideheight/2;
            }
            rejectradius = collideradius.magnitude();
        }
        center = collidecenter;
        radius = collideradius;
        return rejectradius;
    }

    float boundsphere(vec &center)
    {
        vec radius;
        boundbox(center, radius);
        return radius.magnitude();
    }

    float above()
    {
        vec center, radius;
        boundbox(center, radius);
        return center.z+radius.z;
    }
};

// FIXME
#undef COLLIDE_OBB

struct occludequery;

struct mapmodelinfo { string name; model *m, *collide; };

extern vector<mapmodelinfo> mapmodels;

void resetmodelbatches();
void startmodelquery(occludequery *query);
void endmodelquery();
void rendershadowmodelbatches(bool dynmodel = true);
void shadowmaskbatchedmodels(bool dynshadow = true);
void rendermapmodelbatches();
void rendermodelbatches();
void rendertransparentmodelbatches(int stencil = 0);
void rendermapmodel(int idx, int anim, const vec &o, float yaw = 0, float pitch = 0, float roll = 0, int flags = MDL_CULL_VFC | MDL_CULL_DIST, int basetime = 0, float size = 1);
void clearbatchedmapmodels();
void preloadusedmapmodels(bool msg = false, bool bih = false);
int batcheddynamicmodels();
int batcheddynamicmodelbounds(int mask, vec &bbmin, vec &bbmax);
void cleanupmodels();

model *loadmodel(const char *name, int i = -1, bool msg = false);

static inline model *loadmapmodel(int n)
{
    if(mapmodels.inrange(n))
    {
        model *m = mapmodels[n].m;
        return m ? m : loadmodel(NULL, n);
    }
    return NULL;
}

static inline mapmodelinfo *getmminfo(int n) { return mapmodels.inrange(n) ? &mapmodels[n] : NULL; }

void flushpreloadedmodels(bool msg = true);

/* unused for now */

struct modelattach
{
    const char *tag, *name;
    int anim, basetime;
    vec *pos;
    model *m;

    modelattach() : tag(NULL), name(NULL), anim(-1), basetime(0), pos(NULL), m(NULL) {}
    modelattach(const char *tag, const char *name, int anim = -1, int basetime = 0) : tag(tag), name(name), anim(anim), basetime(basetime), pos(NULL), m(NULL) {}
    modelattach(const char *tag, vec *pos) : tag(tag), name(NULL), anim(-1), basetime(0), pos(pos), m(NULL) {}
};

void rendermodel(const char *mdl, int anim, const vec &o, float yaw = 0, float pitch = 0, float roll = 0, int cull = MDL_CULL_VFC | MDL_CULL_DIST | MDL_CULL_OCCLUDED, dynent *d = NULL, modelattach *a = NULL, int basetime = 0, int basetime2 = 0, float size = 1, const vec4 &color = vec4(1, 1, 1, 1));
int intersectmodel(const char *mdl, int anim, const vec &pos, float yaw, float pitch, float roll, const vec &o, const vec &ray, float &dist, int mode = 0, dynent *d = NULL, modelattach *a = NULL, int basetime = 0, int basetime2 = 0, float size = 1);
void abovemodel(vec &o, const char *mdl);
void setbbfrommodel(dynent *d, const char *mdl);
void preloadmodel(const char *name);
bool matchanim(const char *name, const char *pattern);

void moveragdoll(dynent *d);
void cleanragdoll(dynent *d);

#endif
