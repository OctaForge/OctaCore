#include "tools.hh"

#include "encoding.hh"

stream::offset stream::size()
{
    offset pos = tell(), endpos;
    if(pos < 0 || !seek(0, SEEK_END)) return -1;
    endpos = tell();
    return pos == endpos || seek(pos, SEEK_SET) ? endpos : -1;
}

bool stream::getline(char *str, size_t len)
{
    loopi(len-1)
    {
        if(read(&str[i], 1) != 1) { str[i] = '\0'; return i > 0; }
        else if(str[i] == '\n') { str[i+1] = '\0'; return true; }
    }
    if(len > 0) str[len-1] = '\0';
    return true;
}

size_t stream::printf(const char *fmt, ...)
{
    char buf[512];
    char *str = buf;
    va_list args;
#if defined(WIN32) && !defined(__GNUC__)
    va_start(args, fmt);
    int len = _vscprintf(fmt, args);
    if(len <= 0) { va_end(args); return 0; }
    if(len >= (int)sizeof(buf)) str = new char[len+1];
    _vsnprintf(str, len+1, fmt, args);
    va_end(args);
#else
    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if(len <= 0) return 0;
    if(len >= (int)sizeof(buf))
    {
        str = new char[len+1];
        va_start(args, fmt);
        vsnprintf(str, len+1, fmt, args);
        va_end(args);
    }
#endif
    size_t n = write(str, len);
    if(str != buf) delete[] str;
    return n;
}

struct filestream : stream
{
    FILE *file;

    filestream() : file(nullptr) {}
    ~filestream() { close(); }

    bool open(const char *name, const char *mode)
    {
        if(file) return false;
        file = fopen(name, mode);
        return file!=nullptr;
    }

    bool opentemp(const char *name, const char *mode)
    {
        if(file) return false;
#ifdef WIN32
        file = fopen(name, mode);
#else
        file = tmpfile();
#endif
        return file!=nullptr;
    }

    void close()
    {
        if(file) { fclose(file); file = nullptr; }
    }

    bool end() { return feof(file)!=0; }
    offset tell()
    {
#ifdef WIN32
#if defined(__GNUC__) && !defined(__MINGW32__)
        offset off = ftello64(file);
#else
        offset off = _ftelli64(file);
#endif
#else
        offset off = ftello(file);
#endif
        // ftello returns LONG_MAX for directories on some platforms
        return off + 1 >= 0 ? off : -1;
    }
    bool seek(offset pos, int whence)
    {
#ifdef WIN32
#if defined(__GNUC__) && !defined(__MINGW32__)
        return fseeko64(file, pos, whence) >= 0;
#else
        return _fseeki64(file, pos, whence) >= 0;
#endif
#else
        return fseeko(file, pos, whence) >= 0;
#endif
    }

    size_t read(void *buf, size_t len) { return fread(buf, 1, len, file); }
    size_t write(const void *buf, size_t len) { return fwrite(buf, 1, len, file); }
    bool flush() { return !fflush(file); }
    int getchar() { return fgetc(file); }
    bool putchar(int c) { return fputc(c, file)!=EOF; }
    bool getline(char *str, size_t len) { return fgets(str, len, file)!=nullptr; }
    bool putstring(const char *str) { return fputs(str, file)!=EOF; }

    size_t printf(const char *fmt, ...)
    {
        va_list v;
        va_start(v, fmt);
        int result = vfprintf(file, fmt, v);
        va_end(v);
        return max(result, 0);
    }
};

struct utf8stream : stream
{
    enum
    {
        BUFSIZE = 4096
    };
    stream *file;
    offset pos;
    size_t bufread, bufcarry, buflen;
    bool reading, writing, autoclose;
    uchar buf[BUFSIZE];

    utf8stream() : file(nullptr), pos(0), bufread(0), bufcarry(0), buflen(0), reading(false), writing(false), autoclose(false)
    {
    }

    ~utf8stream()
    {
        close();
    }

    bool readbuf(size_t size = BUFSIZE)
    {
        if(bufread >= bufcarry) { if(bufcarry > 0 && bufcarry < buflen) memmove(buf, &buf[bufcarry], buflen - bufcarry); buflen -= bufcarry; bufread = bufcarry = 0; }
        size_t n = file->read(&buf[buflen], min(size, BUFSIZE - buflen));
        if(n <= 0) return false;
        buflen += n;
        size_t carry = bufcarry;
        bufcarry += decodeutf8(&buf[bufcarry], BUFSIZE-bufcarry, &buf[bufcarry], buflen-bufcarry, &carry);
        if(carry > bufcarry && carry < buflen) { memmove(&buf[bufcarry], &buf[carry], buflen - carry); buflen -= carry - bufcarry; }
        return true;
    }

    bool checkheader()
    {
        size_t n = file->read(buf, 3);
        if(n == 3 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) return true;
        buflen = n;
        return false;
    }

    bool open(stream *f, const char *mode, bool needclose)
    {
        if(file) return false;
        for(; *mode; mode++)
        {
            if(*mode=='r') { reading = true; break; }
            else if(*mode=='w') { writing = true; break; }
        }
        if(!reading && !writing) return false;

        file = f;

        if(reading) checkheader();

        autoclose = needclose;
        return true;
    }

    void finishreading()
    {
        if(!reading) return;
    }

    void stopreading()
    {
        if(!reading) return;
        reading = false;
    }

    void stopwriting()
    {
        if(!writing) return;
        writing = false;
    }

    void close()
    {
        stopreading();
        stopwriting();
        if(autoclose) DELETEP(file);
    }

    bool end() { return !reading && !writing; }
    offset tell() { return reading || writing ? pos : offset(-1); }

    bool seek(offset off, int whence)
    {
        if(writing || !reading) return false;

        if(whence == SEEK_END)
        {
            uchar skip[512];
            while(read(skip, sizeof(skip)) == sizeof(skip));
            return !off;
        }
        else if(whence == SEEK_CUR) off += pos;

        if(off >= pos) off -= pos;
        else if(off < 0 || !file->seek(0, SEEK_SET)) return false;
        else
        {
            bufread = bufcarry = buflen = 0;
            pos = 0;
            checkheader();
        }

        uchar skip[512];
        while(off > 0)
        {
            size_t skipped = (size_t)min(off, (offset)sizeof(skip));
            if(read(skip, skipped) != skipped) { stopreading(); return false; }
            off -= skipped;
        }

        return true;
    }

    size_t read(void *dst, size_t len)
    {
        if(!reading || !dst || !len) return 0;
        size_t next = 0;
        while(next < len)
        {
            if(bufread >= bufcarry) { if(readbuf(BUFSIZE)) continue; stopreading(); break; }
            size_t n = min(len - next, bufcarry - bufread);
            memcpy(&((uchar *)dst)[next], &buf[bufread], n);
            next += n;
            bufread += n;
        }
        pos += next;
        return next;
    }

    bool getline(char *dst, size_t len)
    {
        if(!reading || !dst || !len) return false;
        --len;
        size_t next = 0;
        while(next < len)
        {
            if(bufread >= bufcarry) { if(readbuf(BUFSIZE)) continue; stopreading(); if(!next) return false; break; }
            size_t n = min(len - next, bufcarry - bufread);
            uchar *endline = (uchar *)memchr(&buf[bufread], '\n', n);
            if(endline) { n = endline+1 - &buf[bufread]; len = next + n; }
            memcpy(&((uchar *)dst)[next], &buf[bufread], n);
            next += n;
            bufread += n;
        }
        dst[next] = '\0';
        pos += next;
        return true;
    }

    size_t write(const void *src, size_t len)
    {
        if(!writing || !src || !len) return 0;
        uchar dst[512];
        size_t next = 0;
        while(next < len)
        {
            size_t carry = 0, n = encodeutf8(dst, sizeof(dst), &((uchar *)src)[next], len - next, &carry);
            if(n > 0 && file->write(dst, n) != n) { stopwriting(); break; }
            next += carry;
        }
        pos += next;
        return next;
    }

    bool flush() { return file->flush(); }
};

stream *openrawfile(const char *filename, const char *mode)
{
    const char *found = findfile(filename, mode);
    if(!found) return nullptr;
    filestream *file = new filestream;
    if(!file->open(found, mode)) { delete file; return nullptr; }
    return file;
}

stream *openfile(const char *filename, const char *mode)
{
#ifndef STANDALONE
    stream *s = openzipfile(filename, mode);
    if(s) return s;
#endif
    return openrawfile(filename, mode);
}

stream *opentempfile(const char *name, const char *mode)
{
    const char *found = findfile(name, mode);
    filestream *file = new filestream;
    if(!file->opentemp(found ? found : name, mode)) { delete file; return nullptr; }
    return file;
}

stream *openutf8file(const char *filename, const char *mode, stream *file)
{
    stream *source = file ? file : openfile(filename, mode);
    if(!source) return nullptr;
    utf8stream *utf8 = new utf8stream;
    if(!utf8->open(source, mode, !file)) { if(!file) delete source; delete utf8; return nullptr; }
    return utf8;
}

char *loadfile(const char *fn, size_t *size, bool utf8)
{
    stream *f = openfile(fn, "rb");
    if(!f) return nullptr;
    stream::offset fsize = f->size();
    if(fsize <= 0) { delete f; return nullptr; }
    size_t len = fsize;
    char *buf;
    try {
        buf = new char[len + 1];
    } catch (...) {
        delete f;
        return nullptr;
    }
    size_t offset = 0;
    if(utf8 && len >= 3)
    {
        if(f->read(buf, 3) != 3) { delete f; delete[] buf; return nullptr; }
        if(((uchar *)buf)[0] == 0xEF && ((uchar *)buf)[1] == 0xBB && ((uchar *)buf)[2] == 0xBF) len -= 3;
        else offset += 3;
    }
    size_t rlen = f->read(&buf[offset], len-offset);
    delete f;
    if(rlen != len-offset) { delete[] buf; return nullptr; }
    if(utf8) len = decodeutf8((uchar *)buf, len, (uchar *)buf, len);
    buf[len] = '\0';
    if(size!=nullptr) *size = len;
    return buf;
}
