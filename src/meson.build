add_global_arguments('-D_FILE_OFFSET_BITS=64', language: 'cpp')

if get_option('buildtype') != 'debug'
    add_global_arguments('-DNDEBUG', language: 'cpp')
endif

octacore_includes = [
    include_directories('.', 'shared', 'engine', 'game')
]

subdir('client')
