#include <new>

#include <SDL.h>

#include <sauerlib/encoding.hh>

#include "tools.hh"

///////////////////////// file system ///////////////////////

#ifdef WIN32
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

string homedir = "";
struct packagedir
{
    char *dir, *filter;
    size_t dirlen, filterlen;
};
vector<packagedir> packagedirs;

char *makerelpath(const char *dir, const char *file, const char *prefix, const char *cmd)
{
    static string tmp;
    if(prefix) copystring(tmp, prefix);
    else tmp[0] = '\0';
    if(file[0]=='<')
    {
        const char *end = strrchr(file, '>');
        if(end)
        {
            size_t len = strlen(tmp);
            copystring(&tmp[len], file, min(sizeof(tmp)-len, size_t(end+2-file)));
            file = end+1;
        }
    }
    if(cmd) concatstring(tmp, cmd);
    if(dir)
    {
        defformatstring(pname, "%s/%s", dir, file);
        concatstring(tmp, pname);
    }
    else concatstring(tmp, file);
    return tmp;
}


char *path(char *s)
{
    for(char *curpart = s;;)
    {
        char *endpart = strchr(curpart, '&');
        if(endpart) *endpart = '\0';
        if(curpart[0]=='<')
        {
            char *file = strrchr(curpart, '>');
            if(!file) return s;
            curpart = file+1;
        }
        for(char *t = curpart; (t = strpbrk(t, "/\\")); *t++ = PATHDIV);
        for(char *prevdir = nullptr, *curdir = curpart;;)
        {
            prevdir = curdir[0]==PATHDIV ? curdir+1 : curdir;
            curdir = strchr(prevdir, PATHDIV);
            if(!curdir) break;
            if(prevdir+1==curdir && prevdir[0]=='.')
            {
                memmove(prevdir, curdir+1, strlen(curdir+1)+1);
                curdir = prevdir;
            }
            else if(curdir[1]=='.' && curdir[2]=='.' && curdir[3]==PATHDIV)
            {
                if(prevdir+2==curdir && prevdir[0]=='.' && prevdir[1]=='.') continue;
                memmove(prevdir, curdir+4, strlen(curdir+4)+1);
                if(prevdir-2 >= curpart && prevdir[-1]==PATHDIV)
                {
                    prevdir -= 2;
                    while(prevdir-1 >= curpart && prevdir[-1] != PATHDIV) --prevdir;
                }
                curdir = prevdir;
            }
        }
        if(endpart)
        {
            *endpart = '&';
            curpart = endpart+1;
        }
        else break;
    }
    return s;
}

char *path(const char *s, bool copy)
{
    static string tmp;
    copystring(tmp, s);
    path(tmp);
    return tmp;
}

const char *parentdir(const char *directory)
{
    const char *p = directory + strlen(directory);
    while(p > directory && *p != '/' && *p != '\\') p--;
    static string parent;
    size_t len = p-directory+1;
    copystring(parent, directory, len);
    return parent;
}

bool fileexists(const char *path, const char *mode)
{
    bool exists = true;
    if(mode[0]=='w' || mode[0]=='a') path = parentdir(path);
#ifdef WIN32
    if(GetFileAttributes(path[0] ? path : ".\\") == INVALID_FILE_ATTRIBUTES) exists = false;
#else
    if(access(path[0] ? path : ".", mode[0]=='w' || mode[0]=='a' ? W_OK : (mode[0]=='d' ? X_OK : R_OK)) == -1) exists = false;
#endif
    return exists;
}

bool createdir(const char *path)
{
    size_t len = strlen(path);
    if(path[len-1]==PATHDIV)
    {
        static string strip;
        path = copystring(strip, path, len);
    }
#ifdef WIN32
    return CreateDirectory(path, nullptr)!=0;
#else
    return mkdir(path, 0777)==0;
#endif
}

size_t fixpackagedir(char *dir)
{
    path(dir);
    size_t len = strlen(dir);
    if(len > 0 && dir[len-1] != PATHDIV)
    {
        dir[len] = PATHDIV;
        dir[len+1] = '\0';
    }
    return len;
}

bool subhomedir(char *dst, int len, const char *src)
{
    const char *sub = strstr(src, "$HOME");
    if(!sub) sub = strchr(src, '~');
    if(sub && sub-src < len)
    {
#ifdef WIN32
        char home[MAX_PATH+1];
        home[0] = '\0';
        if(SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, 0, home) != S_OK || !home[0]) return false;
#else
        const char *home = getenv("HOME");
        if(!home || !home[0]) return false;
#endif
        dst[sub-src] = '\0';
        concatstring(dst, home, len);
        concatstring(dst, sub+(*sub == '~' ? 1 : strlen("$HOME")), len);
    }
    return true;
}

const char *sethomedir(const char *dir)
{
    string pdir;
    copystring(pdir, dir);
    if(!subhomedir(pdir, sizeof(pdir), dir) || !fixpackagedir(pdir)) return nullptr;
    copystring(homedir, pdir);
    return homedir;
}

const char *addpackagedir(const char *dir)
{
    string pdir;
    copystring(pdir, dir);
    if(!subhomedir(pdir, sizeof(pdir), dir) || !fixpackagedir(pdir)) return nullptr;
    char *filter = pdir;
    for(;;)
    {
        static int len = strlen("media");
        filter = strstr(filter, "media");
        if(!filter) break;
        if(filter > pdir && filter[-1] == PATHDIV && filter[len] == PATHDIV) break;
        filter += len;
    }
    packagedir &pf = packagedirs.add();
    pf.dir = filter ? newstring(pdir, filter-pdir) : newstring(pdir);
    pf.dirlen = filter ? filter-pdir : strlen(pdir);
    pf.filter = filter ? newstring(filter) : nullptr;
    pf.filterlen = filter ? strlen(filter) : 0;
    return pf.dir;
}

const char *findfile(const char *filename, const char *mode)
{
    static string s;
    if(homedir[0])
    {
        formatstring(s, "%s%s", homedir, filename);
        if(fileexists(s, mode)) return s;
        if(mode[0]=='w' || mode[0]=='a')
        {
            string dirs;
            copystring(dirs, s);
            char *dir = strchr(dirs[0]==PATHDIV ? dirs+1 : dirs, PATHDIV);
            while(dir)
            {
                *dir = '\0';
                if(!fileexists(dirs, "d") && !createdir(dirs)) return s;
                *dir = PATHDIV;
                dir = strchr(dir+1, PATHDIV);
            }
            return s;
        }
    }
    if(mode[0]=='w' || mode[0]=='a') return filename;
    loopv(packagedirs)
    {
        packagedir &pf = packagedirs[i];
        if(pf.filter && strncmp(filename, pf.filter, pf.filterlen)) continue;
        formatstring(s, "%s%s", pf.dir, filename);
        if(fileexists(s, mode)) return s;
    }
    if(mode[0]=='e') return nullptr;
    return filename;
}

bool listdir(const char *dirname, bool rel, const char *ext, vector<char *> &files)
{
    size_t extsize = ext ? strlen(ext)+1 : 0;
#ifdef WIN32
    defformatstring(pathname, rel ? ".\\%s\\*.%s" : "%s\\*.%s", dirname, ext ? ext : "*");
    WIN32_FIND_DATA FindFileData;
    HANDLE Find = FindFirstFile(pathname, &FindFileData);
    if(Find != INVALID_HANDLE_VALUE)
    {
        do {
            if(!ext) files.add(newstring(FindFileData.cFileName));
            else
            {
                size_t namelen = strlen(FindFileData.cFileName);
                if(namelen > extsize)
                {
                    namelen -= extsize;
                    if(FindFileData.cFileName[namelen] == '.' && strncmp(FindFileData.cFileName+namelen+1, ext, extsize-1)==0)
                        files.add(newstring(FindFileData.cFileName, namelen));
                }
            }
        } while(FindNextFile(Find, &FindFileData));
        FindClose(Find);
        return true;
    }
#else
    defformatstring(pathname, rel ? "./%s" : "%s", dirname);
    DIR *d = opendir(pathname);
    if(d)
    {
        struct dirent *de;
        while((de = readdir(d)) != nullptr)
        {
            if(!ext) files.add(newstring(de->d_name));
            else
            {
                size_t namelen = strlen(de->d_name);
                if(namelen > extsize)
                {
                    namelen -= extsize;
                    if(de->d_name[namelen] == '.' && strncmp(de->d_name+namelen+1, ext, extsize-1)==0)
                        files.add(newstring(de->d_name, namelen));
                }
            }
        }
        closedir(d);
        return true;
    }
#endif
    else return false;
}

int listfiles(const char *dir, const char *ext, vector<char *> &files)
{
    string dirname;
    copystring(dirname, dir);
    path(dirname);
    size_t dirlen = strlen(dirname);
    while(dirlen > 1 && dirname[dirlen-1] == PATHDIV) dirname[--dirlen] = '\0';
    int dirs = 0;
    if(listdir(dirname, true, ext, files)) dirs++;
    string s;
    if(homedir[0])
    {
        formatstring(s, "%s%s", homedir, dirname);
        if(listdir(s, false, ext, files)) dirs++;
    }
    loopv(packagedirs)
    {
        packagedir &pf = packagedirs[i];
        if(pf.filter && strncmp(dirname, pf.filter, dirlen == pf.filterlen-1 ? dirlen : pf.filterlen))
            continue;
        formatstring(s, "%s%s", pf.dir, dirname);
        if(listdir(s, false, ext, files)) dirs++;
    }
#ifndef STANDALONE
    dirs += listzipfiles(dirname, ext, files);
#endif
    return dirs;
}

#ifndef STANDALONE
static Sint64 rwopsseek(SDL_RWops *rw, Sint64 pos, int whence)
{
    stream *f = (stream *)rw->hidden.unknown.data1;
    if((!pos && whence==SEEK_CUR) || f->seek(pos, whence)) return (int)f->tell();
    return -1;
}

static size_t rwopsread(SDL_RWops *rw, void *buf, size_t size, size_t nmemb)
{
    stream *f = (stream *)rw->hidden.unknown.data1;
    return f->read(buf, size*nmemb)/size;
}

static size_t rwopswrite(SDL_RWops *rw, const void *buf, size_t size, size_t nmemb)
{
    stream *f = (stream *)rw->hidden.unknown.data1;
    return f->write(buf, size*nmemb)/size;
}

static int rwopsclose(SDL_RWops *rw)
{
    return 0;
}

SDL_RWops *stream_rwops(stream *s)
{
    SDL_RWops *rw = SDL_AllocRW();
    if(!rw) return nullptr;
    rw->hidden.unknown.data1 = s;
    rw->seek = rwopsseek;
    rw->read = rwopsread;
    rw->write = rwopswrite;
    rw->close = rwopsclose;
    return rw;
}
#endif

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

