#ifndef SAUERLIB_ENCODING_HH
#define SAUERLIB_ENCODING_HH

#include <cstddef>

enum
{
    CT_PRINT   = 1<<0,
    CT_SPACE   = 1<<1,
    CT_DIGIT   = 1<<2,
    CT_ALPHA   = 1<<3,
    CT_LOWER   = 1<<4,
    CT_UPPER   = 1<<5,
    CT_UNICODE = 1<<6
};
extern const unsigned char cubectype[256];
static inline int iscubeprint(unsigned char c) { return cubectype[c]&CT_PRINT; }
static inline int iscubespace(unsigned char c) { return cubectype[c]&CT_SPACE; }
static inline int iscubealpha(unsigned char c) { return cubectype[c]&CT_ALPHA; }
static inline int iscubealnum(unsigned char c) { return cubectype[c]&(CT_ALPHA|CT_DIGIT); }
static inline int iscubelower(unsigned char c) { return cubectype[c]&CT_LOWER; }
static inline int iscubeupper(unsigned char c) { return cubectype[c]&CT_UPPER; }
static inline int iscubepunct(unsigned char c) { return cubectype[c] == CT_PRINT; }
static inline int cube2uni(unsigned char c)
{
    extern const int cube2unichars[256];
    return cube2unichars[c];
}
static inline unsigned char uni2cube(int c)
{
    extern const int uni2cubeoffsets[8];
    extern const unsigned char uni2cubechars[];
    return static_cast<unsigned int>(c) <= 0x7FF ? uni2cubechars[uni2cubeoffsets[c>>8] + (c&0xFF)] : 0;
}
static inline unsigned char cubelower(unsigned char c)
{
    extern const unsigned char cubelowerchars[256];
    return cubelowerchars[c];
}
static inline unsigned char cubeupper(unsigned char c)
{
    extern const unsigned char cubeupperchars[256];
    return cubeupperchars[c];
}
extern size_t decodeutf8(unsigned char *dst, size_t dstlen, const unsigned char *src, size_t srclen, size_t *carry = nullptr);
extern size_t encodeutf8(unsigned char *dstbuf, size_t dstlen, const unsigned char *srcbuf, size_t srclen, size_t *carry = nullptr);

#endif
