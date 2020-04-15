add_languages('c')

libenet_src = [
    'callbacks.c',
    'compress.c',
    'host.c',
    'list.c',
    'packet.c',
    'peer.c',
    'protocol.c',
    'unix.c',
    'win32.c'
]

cc = meson.get_compiler('c')

libenet_cflags = []

if target_machine.system() != 'windows'
    if cc.has_function('getaddrinfo', prefix: '#include <netdb.h>')
        libenet_cflags += ['-DHAS_GETADDRINFO']
    endif
    if cc.has_function('getnameinfo', prefix: '#include <netdb.h>')
        libenet_cflags += ['-DHAS_GETNAMEINFO']
    endif
    if cc.has_function('gethostbyaddr_r', prefix: '#include <netdb.h>')
        libenet_cflags += ['-DHAS_GETHOSTBYADDR_R']
    endif
    if cc.has_function('gethostbyname_r', prefix: '#include <netdb.h>')
        libenet_cflags += ['-DHAS_GETHOSTBYNAME_R']
    endif
    if cc.has_function('poll', prefix: '#include <poll.h>')
        libenet_cflags += ['-DHAS_POLL']
    endif
    if cc.has_function('fcntl', prefix: '#include <fcntl.h>')
        libenet_cflags += ['-DHAS_FCNTL']
    endif
    if cc.has_function('inet_pton', prefix: '#include <arpa/inet.h>')
        libenet_cflags += ['-DHAS_INET_PTON']
    endif
    if cc.has_function('inet_ntop', prefix: '#include <arpa/inet.h>')
        libenet_cflags += ['-DHAS_INET_NTOP']
    endif
    if cc.has_member('struct msghdr', 'msg_flags', prefix: '#include <sys/socket.h>')
        libenet_cflags += ['-DHAS_MSGHDR_FLAGS']
    endif
    if cc.has_type('socklen_t', prefix: '#include <sys/socket.h>')
        libenet_cflags += ['-DHAS_SOCKLEN_T']
    endif
endif

libenet_lib = static_library('enet',
    libenet_src,
    include_directories: include_directories('include'),
    c_args: libenet_cflags,
    install: false
)

libenet = declare_dependency(
    include_directories: include_directories('include'),
    link_with: libenet_lib
)