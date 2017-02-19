/* Standard I/O implementation for OctaSTD.
 *
 * This file is part of OctaSTD. See COPYING.md for futher information.
 */

#ifndef OSTD_IO_HH
#define OSTD_IO_HH

#include <stdio.h>

#include <vector>
#include <stdexcept>
#include <system_error>

#include "ostd/platform.hh"
#include "ostd/string.hh"
#include "ostd/stream.hh"
#include "ostd/format.hh"

namespace ostd {

enum class stream_mode {
    READ = 0, WRITE, APPEND, READ_U, WRITE_U, APPEND_U
};

namespace detail {
    static char const *filemodes[] = {
        "rb", "wb", "ab", "rb+", "wb+", "ab+"
    };
}

/* TODO: inherit from system_error and come up with the proper infra for it */
struct io_error: std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct file_stream: stream {
    file_stream(): p_f(), p_owned(false) {}
    file_stream(file_stream const &) = delete;
    file_stream(file_stream &&s): p_f(s.p_f), p_owned(s.p_owned) {
        s.p_f = nullptr;
        s.p_owned = false;
    }

    file_stream(string_range path, stream_mode mode = stream_mode::READ): p_f() {
        open(path, mode);
    }

    file_stream(FILE *f): p_f(f), p_owned(false) {}

    ~file_stream() { close(); }

    file_stream &operator=(file_stream const &) = delete;
    file_stream &operator=(file_stream &&s) {
        close();
        swap(s);
        return *this;
    }

    bool open(string_range path, stream_mode mode = stream_mode::READ) {
        if (p_f || (path.size() > FILENAME_MAX)) {
            return false;
        }
        char buf[FILENAME_MAX + 1];
        memcpy(buf, &path[0], path.size());
        buf[path.size()] = '\0';
        p_owned = false;
#ifndef OSTD_PLATFORM_WIN32
        p_f = fopen(buf, detail::filemodes[size_t(mode)]);
#else
        if (fopen_s(&p_f, buf, detail::filemodes[size_t(mode)]) != 0) {
            return false;
        }
#endif
        p_owned = !!p_f;
        return is_open();
    }

    bool open(FILE *f) {
        if (p_f) {
            return false;
        }
        p_f = f;
        p_owned = false;
        return is_open();
    }

    bool is_open() const { return p_f != nullptr; }
    bool is_owned() const { return p_owned; }

    void close() {
        if (p_f && p_owned) {
            fclose(p_f);
        }
        p_f = nullptr;
        p_owned = false;
    }

    bool end() const {
        return feof(p_f) != 0;
    }

    void seek(stream_off_t pos, stream_seek whence = stream_seek::SET) {
#ifndef OSTD_PLATFORM_WIN32
        if (fseeko(p_f, pos, int(whence)))
#else
        if (_fseeki64(p_f, pos, int(whence)))
#endif
        {
            throw io_error{"file_stream seek error"};
        }
    }

    stream_off_t tell() const {
#ifndef OSTD_PLATFORM_WIN32
        auto ret = ftello(p_f);
#else
        auto ret = _ftelli64(p_f);
#endif
        if (ret < 0) {
            throw io_error{"file_stream tell error"};
        }
        return ret;
    }

    void flush() {
        if (fflush(p_f)) {
            throw io_error{"file_stream flush error"};
        }
    }

    void read_bytes(void *buf, size_t count) {
        if (fread(buf, 1, count, p_f) != count) {
            throw io_error{"file_stream read error"};
        }
    }

    void write_bytes(void const *buf, size_t count) {
        if (fwrite(buf, 1, count, p_f) != count) {
            throw io_error{"file_stream write error"};
        }
    }

    int getchar() {
        int ret = fgetc(p_f);
        if (ret == EOF) {
            throw io_error{"file_stream EOF"};
        }
        return ret;
    }

    void putchar(int c) {
        if (fputc(c, p_f) == EOF) {
            throw io_error{"file_stream EOF"};
        }
    }

    void swap(file_stream &s) {
        using std::swap;
        swap(p_f, s.p_f);
        swap(p_owned, s.p_owned);
    }

    FILE *get_file() { return p_f; }

private:
    FILE *p_f;
    bool p_owned;
};

inline void swap(file_stream &a, file_stream &b) {
    a.swap(b);
}

static file_stream in(stdin);
static file_stream out(stdout);
static file_stream err(stderr);

/* no need to call anything from file_stream, prefer simple calls... */

namespace detail {
    /* lightweight output range for direct stdout */
    struct stdout_range: output_range<stdout_range> {
        using value_type      = char;
        using reference       = char &;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;

        stdout_range() {}
        void put(char c) {
            if (putchar(c) == EOF) {
                throw io_error{"stdout EOF"};
            }
        }
    };

    template<typename R>
    inline void range_put_all(stdout_range &r, R range) {
        if constexpr(
            is_contiguous_range<R> &&
            std::is_same_v<std::remove_const_t<range_value_t<R>>, char>
        ) {
            if (fwrite(range.data(), 1, range.size(), stdout) != range.size()) {
                throw io_error{"stdout write error"};
            }
        } else {
            for (; !range.empty(); range.pop_front()) {
                r.put(range.front());
            }
        }
    }
}

template<typename T>
inline void write(T const &v) {
    format(detail::stdout_range{}, format_spec{'s'}, v);
}

template<typename T, typename ...A>
inline void write(T const &v, A const &...args) {
    write(v);
    write(args...);
}

template<typename T>
inline void writeln(T const &v) {
    write(v);
    if (putchar('\n') == EOF) {
        throw io_error{"stdout EOF"};
    }
}

template<typename T, typename ...A>
inline void writeln(T const &v, A const &...args) {
    write(v);
    write(args...);
    if (putchar('\n') == EOF) {
        throw io_error{"stdout EOF"};
    }
}

template<typename ...A>
inline void writef(string_range fmt, A const &...args) {
    format(detail::stdout_range{}, fmt, args...);
}

template<typename ...A>
inline void writefln(string_range fmt, A const &...args) {
    writef(fmt, args...);
    if (putchar('\n') == EOF) {
        throw io_error{"stdout EOF"};
    }
}

} /* namespace ostd */

#endif
