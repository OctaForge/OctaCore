#ifndef SAUERLIB_ENDIAN_HH
#define SAUERLIB_ENDIAN_HH

#include <cstdint>

/* we only support modern toolchains, this should be enough */
#if (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)) || \
    defined(__BIG_ENDIAN__)
#define SAUERLIB_BIG_ENDIAN 1
#else
#define SAUERLIB_LITTLE_ENDIAN 1
#endif

#if defined(__GNUC__)
inline uint16_t endianswap16(uint16_t n) { return __builtin_bswap16(n); }
inline uint32_t endianswap32(uint32_t n) { return __builtin_bswap32(n); }
inline uint32_t endianswap64(uint64_t n) { return __builtin_bswap64(n); }
#elif defined(_MSC_VER)
inline uint16_t endianswap16(uint16_t n) { return _byteswap_ushort(n); }
inline uint32_t endianswap32(uint32_t n) { return _byteswap_ulong(n); }
inline uint32_t endianswap64(uint64_t n) { return _byteswap_uint64(n); }
#else
inline uint16_t endianswap16(uint16_t n) { return (n<<8) | (n>>8); }
inline uint32_t endianswap32(uint32_t n) {
    return (n << 24) | (n >> 24) | ((n >> 8) & 0xFF00) | ((n << 8) & 0xFF0000);
}
inline uint64_t endianswap64(uint64_t n) {
    return endianswap32(uint32_t(n >> 32)) | (
        uint64_t(endianswap32(uint32_t(n))) << 32
    );
}
#endif

template<typename T> inline T endianswap(T n) {
    union { T t; uint32_t i; } conv;
    conv.t = n;
    conv.i = endianswap32(conv.i);
    return conv.t;
}
template<> inline uint16_t endianswap<uint16_t>(uint16_t n) {
    return endianswap16(n);
}
template<> inline int16_t endianswap<int16_t>(int16_t n) {
    return endianswap16(n);
}
template<> inline uint32_t endianswap<uint32_t>(uint32_t n) {
    return endianswap32(n);
}
template<> inline int32_t endianswap<int32_t>(int32_t n) {
    return endianswap32(n);
}
template<> inline uint64_t endianswap<uint64_t>(uint64_t n) {
    return endianswap64(n);
}
template<> inline int64_t endianswap<int64_t>(int64_t n) {
    return endianswap64(n);
}
template<> inline double endianswap<double>(double n) {
    union { double t; uint i; } conv;
    conv.t = n;
    conv.i = endianswap64(conv.i);
    return conv.t;
}
template<typename T> inline void endianswap(T *buf, size_t len) {
    for (T *end = &buf[len]; buf < end; buf++) {
        *buf = endianswap(*buf);
    }
}

#ifdef SAUERLIB_BIG_ENDIAN
template<typename T> inline T lilswap(T n) { return endianswap(n); }
template<typename T> inline T bigswap(T n) { return n; }
template<typename T> inline void lilswap(T *buf, size_t len) {
    endianswap(buf, len);
}
template<typename T> inline void bigswap(T *buf, size_t len) {}
#else
template<typename T> inline T lilswap(T n) { return n; }
template<typename T> inline T bigswap(T n) { return endianswap(n); }
template<typename T> inline void lilswap(T *buf, size_t len) {}
template<typename T> inline void bigswap(T *buf, size_t len) {
    endianswap(buf, len);
}
#endif

#endif
