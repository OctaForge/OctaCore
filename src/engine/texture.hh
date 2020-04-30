#ifndef ENGINE_TEXTURE_HH
#define ENGINE_TEXTURE_HH

#include <shared/gl.hh>
#include <shared/tools.hh>

#include "shader.hh"

struct ImageData
{
    int w, h, bpp, levels, align, pitch;
    GLenum compressed;
    uchar *data;
    void *owner;
    void (*freefunc)(void *);

    ImageData()
        : data(NULL), owner(NULL), freefunc(NULL)
    {}


    ImageData(int nw, int nh, int nbpp, int nlevels = 1, int nalign = 0, GLenum ncompressed = GL_FALSE)
    {
        setdata(NULL, nw, nh, nbpp, nlevels, nalign, ncompressed);
    }

    ImageData(int nw, int nh, int nbpp, uchar *data)
        : owner(NULL), freefunc(NULL)
    {
        setdata(data, nw, nh, nbpp);
    }

    ImageData(SDL_Surface *s) { wrap(s); }
    ~ImageData() { cleanup(); }

    void setdata(uchar *ndata, int nw, int nh, int nbpp, int nlevels = 1, int nalign = 0, GLenum ncompressed = GL_FALSE)
    {
        w = nw;
        h = nh;
        bpp = nbpp;
        levels = nlevels;
        align = nalign;
        pitch = align ? 0 : w*bpp;
        compressed = ncompressed;
        data = ndata ? ndata : new uchar[calcsize()];
        if(!ndata) { owner = this; freefunc = NULL; }
    }

    int calclevelsize(int level) const { return ((max(w>>level, 1)+align-1)/align)*((max(h>>level, 1)+align-1)/align)*bpp; }

    int calcsize() const
    {
        if(!align) return w*h*bpp;
        int lw = w, lh = h,
            size = 0;
        loopi(levels)
        {
            if(lw<=0) lw = 1;
            if(lh<=0) lh = 1;
            size += ((lw+align-1)/align)*((lh+align-1)/align)*bpp;
            if(lw*lh==1) break;
            lw >>= 1;
            lh >>= 1;
        }
        return size;
    }

    void disown()
    {
        data = NULL;
        owner = NULL;
        freefunc = NULL;
    }

    void cleanup()
    {
        if(owner==this) delete[] data;
        else if(freefunc) (*freefunc)(owner);
        disown();
    }

    void replace(ImageData &d)
    {
        cleanup();
        *this = d;
        if(owner == &d) owner = this;
        d.disown();
    }

    void wrap(SDL_Surface *s)
    {
        setdata((uchar *)s->pixels, s->w, s->h, s->format->BytesPerPixel);
        pitch = s->pitch;
        owner = s;
        freefunc = (void (*)(void *))SDL_FreeSurface;
    }
};

// management of texture slots
// each texture slot can have multiple texture frames, of which currently only the first is used
// additional frames can be used for various shaders

struct Texture
{
    enum
    {
        IMAGE      = 0,
        CUBEMAP    = 1,
        TYPE       = 0xFF,

        STUB       = 1<<8,
        TRANSIENT  = 1<<9,
        COMPRESSED = 1<<10,
        ALPHA      = 1<<11,
        MIRROR     = 1<<12,
        FLAGS      = 0xFF00
    };

    char *name;
    int type, w, h, xs, ys, bpp, clamp;
    bool mipmap, canreduce;
    GLuint id;
    uchar *alphamask;

    Texture() : alphamask(NULL) {}
};

enum
{
    TEX_DIFFUSE = 0,
    TEX_NORMAL,
    TEX_GLOW,
    TEX_ENVMAP,

    TEX_SPEC,
    TEX_DEPTH,
    TEX_ALPHA,
    TEX_UNKNOWN,

    TEX_DETAIL = TEX_SPEC
};

enum
{
    VSLOT_SHPARAM = 0,
    VSLOT_SCALE,
    VSLOT_ROTATION,
    VSLOT_OFFSET,
    VSLOT_SCROLL,
    VSLOT_LAYER,
    VSLOT_ALPHA,
    VSLOT_COLOR,
    VSLOT_RESERVED, // used by RE
    VSLOT_REFRACT,
    VSLOT_DETAIL,
    VSLOT_NUM
};

struct VSlot
{
    Slot *slot;
    VSlot *next;
    int index, changed;
    vector<SlotShaderParam> params;
    bool linked;
    float scale;
    int rotation;
    ivec2 offset;
    vec2 scroll;
    int layer, detail;
    float alphafront, alphaback;
    vec colorscale;
    vec glowcolor;
    float refractscale;
    vec refractcolor;

    VSlot(Slot *slot = NULL, int index = -1) : slot(slot), next(NULL), index(index), changed(0)
    {
        reset();
        if(slot) addvariant(slot);
    }

    void addvariant(Slot *slot);

    void reset()
    {
        params.setsize(0);
        linked = false;
        scale = 1;
        rotation = 0;
        offset = ivec2(0, 0);
        scroll = vec2(0, 0);
        layer = detail = 0;
        alphafront = 0.5f;
        alphaback = 0;
        colorscale = vec(1, 1, 1);
        glowcolor = vec(1, 1, 1);
        refractscale = 0;
        refractcolor = vec(1, 1, 1);
    }

    void cleanup()
    {
        linked = false;
    }

    bool isdynamic() const;
};

struct Slot
{
    enum { OCTA, MATERIAL, DECAL };

    struct Tex
    {
        int type;
        Texture *t;
        string name;
        int combined;

        Tex() : t(NULL), combined(-1) {}
    };

    int index, smooth;
    vector<Tex> sts;
    Shader *shader;
    vector<SlotShaderParam> params;
    VSlot *variants;
    bool loaded;
    uint texmask;
    char *grass;
    Texture *grasstex, *thumbnail;

    Slot(int index = -1) : index(index), variants(NULL), grass(NULL) { reset(); }
    virtual ~Slot() {}

    virtual int type() const { return OCTA; }
    virtual const char *name() const;
    virtual const char *texturedir() const { return "media/texture"; }

    virtual VSlot &emptyvslot();

    virtual int cancombine(int type) const;
    virtual bool shouldpremul(int type) const { return false; }

    int findtextype(int type, int last = -1) const;

    void load(int index, Slot::Tex &t);
    void load();

    Texture *loadthumbnail();

    void reset()
    {
        smooth = -1;
        sts.setsize(0);
        shader = NULL;
        params.setsize(0);
        loaded = false;
        texmask = 0;
        DELETEA(grass);
        grasstex = NULL;
        thumbnail = NULL;
    }

    void cleanup()
    {
        loaded = false;
        grasstex = NULL;
        thumbnail = NULL;
        loopv(sts)
        {
            Tex &t = sts[i];
            t.t = NULL;
            t.combined = -1;
        }
    }
};

inline void VSlot::addvariant(Slot *slot)
{
    if(!slot->variants) slot->variants = this;
    else
    {
        VSlot *prev = slot->variants;
        while(prev->next) prev = prev->next;
        prev->next = this;
    }
}

inline bool VSlot::isdynamic() const
{
    return !scroll.iszero() || slot->shader->isdynamic();
}

struct MatSlot : Slot, VSlot
{
    MatSlot();

    int type() const { return MATERIAL; }
    const char *name() const;

    VSlot &emptyvslot() { return *this; }

    int cancombine(int type) const { return -1; }

    void reset()
    {
        Slot::reset();
        VSlot::reset();
    }

    void cleanup()
    {
        Slot::cleanup();
        VSlot::cleanup();
    }
};

struct DecalSlot : Slot, VSlot
{
    float depth, fade;

    DecalSlot(int index = -1) : Slot(index), VSlot(this), depth(1), fade(0.5f) {}

    int type() const { return DECAL; }
    const char *name() const;
    const char *texturedir() const { return "media/decal"; }

    VSlot &emptyvslot() { return *this; }

    int cancombine(int type) const;
    bool shouldpremul(int type) const;

    void reset()
    {
        Slot::reset();
        VSlot::reset();
        depth = 1;
        fade = 0.5f;
    }

    void cleanup()
    {
        Slot::cleanup();
        VSlot::cleanup();
    }
};

struct texrotation
{
    bool flipx, flipy, swapxy;
};

struct cubemapside
{
    GLenum target;
    const char *name;
    bool flipx, flipy, swapxy;
};

extern const texrotation texrotations[8];
extern const cubemapside cubemapsides[6];

extern int hwtexsize, hwcubetexsize, hwmaxaniso, maxtexsize, hwtexunits, hwvtexunits;

#define MAXBLURRADIUS 7
extern float blursigma;
void setupblurkernel(int radius, float *weights, float *offsets);
void setblurshader(int pass, int size, int radius, float *weights, float *offsets, GLenum target = GL_TEXTURE_2D);

void savepng(const char *filename, ImageData &image, bool flip = false);
void savetga(const char *filename, ImageData &image, bool flip = false);
bool loadimage(const char *filename, ImageData &image);

extern Slot dummyslot;
extern VSlot dummyvslot;
extern DecalSlot dummydecalslot;
extern vector<Slot *> slots;
extern vector<VSlot *> vslots;

MatSlot &lookupmaterialslot(int slot, bool load = true);
Slot &lookupslot(int slot, bool load = true);
VSlot &lookupvslot(int slot, bool load = true);
DecalSlot &lookupdecalslot(int slot, bool load = true);
VSlot *findvslot(Slot &slot, const VSlot &src, const VSlot &delta);
VSlot *editvslot(const VSlot &src, const VSlot &delta);
void mergevslot(VSlot &dst, const VSlot &src, const VSlot &delta);
void packvslot(vector<uchar> &buf, const VSlot &src);
void packvslot(vector<uchar> &buf, int index);
void packvslot(vector<uchar> &buf, const VSlot *vs);
bool unpackvslot(ucharbuf &buf, VSlot &dst, bool delta);

struct cube;

void compactvslots(cube *c, int n = 8);
void compactvslot(int &index);
void compactvslot(VSlot &vs);
int compactvslots(bool cull = false);

void clearslots();

void linkslotshaders();

extern Texture *notexture;

int texalign(const void *data, int w, int bpp);
bool floatformat(GLenum format);
uchar *loadalphamask(Texture *t);
void setuptexcompress();
void createtexture(int tnum, int w, int h, const void *pixels, int clamp, int filter, GLenum component = GL_RGB, GLenum target = GL_TEXTURE_2D, int pw = 0, int ph = 0, int pitch = 0, bool resize = true, GLenum format = GL_FALSE, bool swizzle = false);
void create3dtexture(int tnum, int w, int h, int d, const void *pixels, int clamp, int filter, GLenum component = GL_RGB, GLenum target = GL_TEXTURE_3D, bool swizzle = false);
Texture *textureload(const char *name, int clamp = 0, bool mipit = true, bool msg = true);
bool settexture(const char *name, int clamp = 0);
bool reloadtexture(Texture &tex);
bool reloadtexture(const char *name);
void reloadtextures();
void cleanuptextures();

Texture *cubemapload(const char *name, bool mipit = true, bool msg = true, bool transient = false);
void initenvmaps();
void genenvmaps();
ushort closestenvmap(const vec &o);
ushort closestenvmap(int orient, const ivec &o, int size);
GLuint lookupenvmap(ushort emid);
GLuint lookupenvmap(Slot &slot);

#endif