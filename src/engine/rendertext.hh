#ifndef ENGINE_RENDERTEXT_HH
#define ENGINE_RENDERTEXT_HH

#include <shared/tools.hh>
#include <shared/geom.hh>

struct Texture;
struct Shader;

// rendertext
struct font
{
    struct charinfo
    {
        float x, y, w, h, offsetx, offsety, advance;
        int tex;
    };

    char *name;
    vector<Texture *> texs;
    vector<charinfo> chars;
    int charoffset, defaultw, defaulth, scale;
    float bordermin, bordermax, outlinemin, outlinemax;

    font() : name(nullptr) {}
    ~font() { DELETEA(name); }
};

#define FONTH (curfont->scale)
#define FONTW (FONTH/2)
#define MINRESW 640
#define MINRESH 480

extern font *curfont;
extern Shader *textshader;
extern const matrix4x3 *textmatrix;
extern float textscale;

static inline void setfont(font *f) { if(f) curfont = f; }

bool setfont(const char *name);
void pushfont();
bool popfont();
void reloadfonts();

void gettextres(int &w, int &h);

void draw_text(const char *str, float left, float top, int r = 255, int g = 255, int b = 255, int a = 255, int cursor = -1, int maxwidth = -1);
void draw_textf(const char *fstr, float left, float top, ...) PRINTFARGS(1, 4);
float text_widthf(const char *str);
void text_boundsf(const char *str, float &width, float &height, int maxwidth = -1);
void text_posf(const char *str, int cursor, float &cx, float &cy, int maxwidth);

static inline int text_width(const char *str)
{
    return int(ceil(text_widthf(str)));
}

static inline void text_bounds(const char *str, int &width, int &height, int maxwidth = -1)
{
    float widthf, heightf;
    text_boundsf(str, widthf, heightf, maxwidth);
    width = int(ceil(widthf));
    height = int(ceil(heightf));
}

static inline void text_pos(const char *str, int cursor, int &cx, int &cy, int maxwidth)
{
    float cxf, cyf;
    text_posf(str, cursor, cxf, cyf, maxwidth);
    cx = int(cxf);
    cy = int(cyf);
}

#endif
