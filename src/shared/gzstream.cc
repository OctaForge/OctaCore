#include <zlib.h>

#include <sauerlib/encoding.hh>

#include "command.hh"
#include "tools.hh"

#include <engine/console.hh> /* conoutf */

#ifndef STANDALONE
VAR(dbggz, 0, 0, 1);
#endif

struct gzstream : stream
{
    enum
    {
        MAGIC1   = 0x1F,
        MAGIC2   = 0x8B,
        BUFSIZE  = 16384,
        OS_UNIX  = 0x03
    };

    enum
    {
        F_ASCII    = 0x01,
        F_CRC      = 0x02,
        F_EXTRA    = 0x04,
        F_NAME     = 0x08,
        F_COMMENT  = 0x10,
        F_RESERVED = 0xE0
    };

    stream *file;
    z_stream zfile;
    uchar *buf;
    bool reading, writing, autoclose;
    uint crc;
    size_t headersize;

    gzstream() : file(nullptr), buf(nullptr), reading(false), writing(false), autoclose(false), crc(0), headersize(0)
    {
        zfile.zalloc = nullptr;
        zfile.zfree = nullptr;
        zfile.opaque = nullptr;
        zfile.next_in = zfile.next_out = nullptr;
        zfile.avail_in = zfile.avail_out = 0;
    }

    ~gzstream()
    {
        close();
    }

    void writeheader()
    {
        uchar header[] = { MAGIC1, MAGIC2, Z_DEFLATED, 0, 0, 0, 0, 0, 0, OS_UNIX };
        file->write(header, sizeof(header));
    }

    void readbuf(size_t size = BUFSIZE)
    {
        if(!zfile.avail_in) zfile.next_in = (Bytef *)buf;
        size = min(size, size_t(&buf[BUFSIZE] - &zfile.next_in[zfile.avail_in]));
        size_t n = file->read(zfile.next_in + zfile.avail_in, size);
        if(n > 0) zfile.avail_in += n;
    }

    uchar readbyte(size_t size = BUFSIZE)
    {
        if(!zfile.avail_in) readbuf(size);
        if(!zfile.avail_in) return 0;
        zfile.avail_in--;
        return *(uchar *)zfile.next_in++;
    }

    void skipbytes(size_t n)
    {
        while(n > 0 && zfile.avail_in > 0)
        {
            size_t skipped = min(n, size_t(zfile.avail_in));
            zfile.avail_in -= skipped;
            zfile.next_in += skipped;
            n -= skipped;
        }
        if(n <= 0) return;
        file->seek(n, SEEK_CUR);
    }

    bool checkheader()
    {
        readbuf(10);
        if(readbyte() != MAGIC1 || readbyte() != MAGIC2 || readbyte() != Z_DEFLATED) return false;
        uchar flags = readbyte();
        if(flags & F_RESERVED) return false;
        skipbytes(6);
        if(flags & F_EXTRA)
        {
            size_t len = readbyte(512);
            len |= size_t(readbyte(512))<<8;
            skipbytes(len);
        }
        if(flags & F_NAME) while(readbyte(512));
        if(flags & F_COMMENT) while(readbyte(512));
        if(flags & F_CRC) skipbytes(2);
        headersize = size_t(file->tell() - zfile.avail_in);
        return zfile.avail_in > 0 || !file->end();
    }

    bool open(stream *f, const char *mode, bool needclose, int level)
    {
        if(file) return false;
        for(; *mode; mode++)
        {
            if(*mode=='r') { reading = true; break; }
            else if(*mode=='w') { writing = true; break; }
        }
        if(reading)
        {
            if(inflateInit2(&zfile, -MAX_WBITS) != Z_OK) reading = false;
        }
        else if(writing && deflateInit2(&zfile, level, Z_DEFLATED, -MAX_WBITS, min(MAX_MEM_LEVEL, 8), Z_DEFAULT_STRATEGY) != Z_OK) writing = false;
        if(!reading && !writing) return false;

        file = f;
        crc = crc32(0, nullptr, 0);
        buf = new uchar[BUFSIZE];

        if(reading)
        {
            if(!checkheader()) { stopreading(); return false; }
        }
        else if(writing) writeheader();

        autoclose = needclose;
        return true;
    }

    uint getcrc() { return crc; }

    void finishreading()
    {
        if(!reading) return;
#ifndef STANDALONE
        if(dbggz)
        {
            uint checkcrc = 0, checksize = 0;
            loopi(4) checkcrc |= uint(readbyte()) << (i*8);
            loopi(4) checksize |= uint(readbyte()) << (i*8);
            if(checkcrc != crc)
                conoutf(CON_DEBUG, "gzip crc check failed: read %X, calculated %X", checkcrc, crc);
            if(checksize != zfile.total_out)
                conoutf(CON_DEBUG, "gzip size check failed: read %u, calculated %u", checksize, uint(zfile.total_out));
        }
#endif
    }

    void stopreading()
    {
        if(!reading) return;
        inflateEnd(&zfile);
        reading = false;
    }

    void finishwriting()
    {
        if(!writing) return;
        for(;;)
        {
            int err = zfile.avail_out > 0 ? deflate(&zfile, Z_FINISH) : Z_OK;
            if(err != Z_OK && err != Z_STREAM_END) break;
            flushbuf();
            if(err == Z_STREAM_END) break;
        }
        uchar trailer[8] =
        {
            uchar(crc&0xFF), uchar((crc>>8)&0xFF), uchar((crc>>16)&0xFF), uchar((crc>>24)&0xFF),
            uchar(zfile.total_in&0xFF), uchar((zfile.total_in>>8)&0xFF), uchar((zfile.total_in>>16)&0xFF), uchar((zfile.total_in>>24)&0xFF)
        };
        file->write(trailer, sizeof(trailer));
    }

    void stopwriting()
    {
        if(!writing) return;
        deflateEnd(&zfile);
        writing = false;
    }

    void close()
    {
        if(reading) finishreading();
        stopreading();
        if(writing) finishwriting();
        stopwriting();
        DELETEA(buf);
        if(autoclose) DELETEP(file);
    }

    bool end() { return !reading && !writing; }
    offset tell() { return reading ? zfile.total_out : (writing ? zfile.total_in : offset(-1)); }
    offset rawtell() { return file ? file->tell() : offset(-1); }

    offset size()
    {
        if(!file) return -1;
        offset pos = tell();
        if(!file->seek(-4, SEEK_END)) return -1;
        uint isize = file->getlil<uint>();
        return file->seek(pos, SEEK_SET) ? isize : offset(-1);
    }

    offset rawsize() { return file ? file->size() : offset(-1); }

    bool seek(offset pos, int whence)
    {
        if(writing || !reading) return false;

        if(whence == SEEK_END)
        {
            uchar skip[512];
            while(read(skip, sizeof(skip)) == sizeof(skip));
            return !pos;
        }
        else if(whence == SEEK_CUR) pos += zfile.total_out;

        if(pos >= (offset)zfile.total_out) pos -= zfile.total_out;
        else if(pos < 0 || !file->seek(headersize, SEEK_SET)) return false;
        else
        {
            if(zfile.next_in && zfile.total_in <= uint(zfile.next_in - buf))
            {
                zfile.avail_in += zfile.total_in;
                zfile.next_in -= zfile.total_in;
            }
            else
            {
                zfile.avail_in = 0;
                zfile.next_in = nullptr;
            }
            inflateReset(&zfile);
            crc = crc32(0, nullptr, 0);
        }

        uchar skip[512];
        while(pos > 0)
        {
            size_t skipped = (size_t)min(pos, (offset)sizeof(skip));
            if(read(skip, skipped) != skipped) { stopreading(); return false; }
            pos -= skipped;
        }

        return true;
    }

    size_t read(void *buf, size_t len)
    {
        if(!reading || !buf || !len) return 0;
        zfile.next_out = (Bytef *)buf;
        zfile.avail_out = len;
        while(zfile.avail_out > 0)
        {
            if(!zfile.avail_in)
            {
                readbuf(BUFSIZE);
                if(!zfile.avail_in) { stopreading(); break; }
            }
            int err = inflate(&zfile, Z_NO_FLUSH);
            if(err == Z_STREAM_END) { crc = crc32(crc, (Bytef *)buf, len - zfile.avail_out); finishreading(); stopreading(); return len - zfile.avail_out; }
            else if(err != Z_OK) { stopreading(); break; }
        }
        crc = crc32(crc, (Bytef *)buf, len - zfile.avail_out);
        return len - zfile.avail_out;
    }

    bool flushbuf(bool full = false)
    {
        if(full) deflate(&zfile, Z_SYNC_FLUSH);
        if(zfile.next_out && zfile.avail_out < BUFSIZE)
        {
            if(file->write(buf, BUFSIZE - zfile.avail_out) != BUFSIZE - zfile.avail_out || (full && !file->flush()))
                return false;
        }
        zfile.next_out = buf;
        zfile.avail_out = BUFSIZE;
        return true;
    }

    bool flush() { return flushbuf(true); }

    size_t write(const void *buf, size_t len)
    {
        if(!writing || !buf || !len) return 0;
        zfile.next_in = (Bytef *)buf;
        zfile.avail_in = len;
        while(zfile.avail_in > 0)
        {
            if(!zfile.avail_out && !flushbuf()) { stopwriting(); break; }
            int err = deflate(&zfile, Z_NO_FLUSH);
            if(err != Z_OK) { stopwriting(); break; }
        }
        crc = crc32(crc, (Bytef *)buf, len - zfile.avail_in);
        return len - zfile.avail_in;
    }
};

stream *opengzfile(const char *filename, const char *mode, stream *file)
{
    stream *source = file ? file : openfile(filename, mode);
    if(!source) return nullptr;
    gzstream *gz = new gzstream;
    if(!gz->open(source, mode, !file, Z_BEST_COMPRESSION)) { if(!file) delete source; delete gz; return nullptr; }
    return gz;
}
