#ifndef ENGINE_RENDERLIGHTS_HH
#define ENGINE_RENDERLIGHTS_HH

#include <shared/gl.hh>
#include <shared/tools.hh>
#include <shared/geom.hh>

#include "aa.hh"

#define LIGHTTILE_MAXW 16
#define LIGHTTILE_MAXH 16

enum { L_NOSHADOW = 1<<0, L_NODYNSHADOW = 1<<1, L_VOLUMETRIC = 1<<2, L_NOSPEC = 1<<3, L_SMALPHA = 1<<4 };

/* only for the inlines below */
extern int lighttilealignw, lighttilealignh, lighttilevieww, lighttileviewh, lighttilew, lighttileh;
extern int vieww, viewh; /* rendergl */

template<class T>
static inline void calctilebounds(float sx1, float sy1, float sx2, float sy2, T &bx1, T &by1, T &bx2, T &by2)
{
    int tx1 = max(int(floor(((sx1 + 1)*0.5f*vieww)/lighttilealignw)), 0),
        ty1 = max(int(floor(((sy1 + 1)*0.5f*viewh)/lighttilealignh)), 0),
        tx2 = min(int(ceil(((sx2 + 1)*0.5f*vieww)/lighttilealignw)), lighttilevieww),
        ty2 = min(int(ceil(((sy2 + 1)*0.5f*viewh)/lighttilealignh)), lighttileviewh);
    bx1 = T((tx1 * lighttilew) / lighttilevieww);
    by1 = T((ty1 * lighttileh) / lighttileviewh);
    bx2 = T((tx2 * lighttilew + lighttilevieww - 1) / lighttilevieww);
    by2 = T((ty2 * lighttileh + lighttileviewh - 1) / lighttileviewh);
}

static inline void masktiles(uint *tiles, float sx1, float sy1, float sx2, float sy2)
{
    int tx1, ty1, tx2, ty2;
    calctilebounds(sx1, sy1, sx2, sy2, tx1, ty1, tx2, ty2);
    for(int ty = ty1; ty < ty2; ty++) tiles[ty] |= ((1<<(tx2-tx1))-1)<<tx1;
}

enum { SM_NONE = 0, SM_REFLECT, SM_CUBEMAP, SM_CASCADE, SM_SPOT };

extern int shadowmapping;
extern matrix4 shadowmatrix;

void loaddeferredlightshaders();
void cleardeferredlightshaders();
void clearshadowcache();

void rendervolumetric();
void cleanupvolumetric();

static inline bool sphereinsidespot(const vec &dir, int spot, const vec &center, float radius)
{
    const vec2 &sc = sincos360[spot];
    float cdist = dir.dot(center), cradius = radius + sc.y*cdist;
    return sc.x*sc.x*(center.dot(center) - cdist*cdist) <= cradius*cradius;
}
static inline bool bbinsidespot(const vec &origin, const vec &dir, int spot, const ivec &bbmin, const ivec &bbmax)
{
    vec radius = vec(ivec(bbmax).sub(bbmin)).mul(0.5f), center = vec(bbmin).add(radius);
    return sphereinsidespot(dir, spot, center.sub(origin), radius.magnitude());
}

extern matrix4 worldmatrix, screenmatrix;

struct extentity; // FIXME

extern int transparentlayer;
extern int gw, gh, ghasstencil;
extern GLuint gdepthtex, gcolortex, gnormaltex, gglowtex;
extern GLuint msdepthtex, mscolortex, msnormaltex, msglowtex;
extern int msaasamples, msaalight;
extern vector<vec2> msaapositions;

int calcshadowinfo(const extentity &e, vec &origin, float &radius, vec &spotloc, int &spotangle, float &bias);
void rendershadowatlas();
bool useradiancehints();
void renderradiancehints();
void clearradiancehintscache();
void cleanuplights();
void workinoq();

int calcbbcsmsplits(const ivec &bbmin, const ivec &bbmax);
int calcspherecsmsplits(const vec &center, float radius);
int calcbbrsmsplits(const ivec &bbmin, const ivec &bbmax);
int calcspherersmsplits(const vec &center, float radius);

void cleanupgbuffer();
void initgbuffer();
bool usepacknorm();
void maskgbuffer(const char *mask);
void bindgdepth();
void preparegbuffer(bool depthclear = true);
void rendergbuffer(bool depthclear = true);
void shadesky();
void shadegbuffer();
void shademinimap(const vec &color = vec(-1, -1, -1));
void shademodelpreview(int x, int y, int w, int h, bool background = true, bool scissor = false);
void rendertransparent();
void renderao();
void loadhdrshaders(int aa = AA_UNUSED);
void processhdr(GLuint outfbo = 0, int aa = AA_UNUSED);
void copyhdr(int sw, int sh, GLuint fbo, int dw = 0, int dh = 0, bool flipx = false, bool flipy = false, bool swapxy = false);
void setuplights();
void setupgbuffer();
GLuint shouldscale();
void doscale(GLuint outfbo = 0);
bool debuglights();

extern int avatarmask;

bool useavatarmask();
void enableavatarmask();
void disableavatarmask();

#endif
