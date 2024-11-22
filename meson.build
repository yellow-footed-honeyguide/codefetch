project('codefetch', 
        'cpp',
        version : '0.9.7',
        default_options : [
            'cpp_std=c++23',
            'buildtype=release',
        ])

add_project_arguments('-DPROJECT_VERSION="' + meson.project_version() + '"',
                     language : 'cpp')

threads_dep = dependency('threads')
libgit2_dep = dependency('libgit2')

sources = [
    'src/main.cpp',
    'src/file_utils.cpp',
    'src/line_count_util.cpp',
    'src/stats_printer.cpp',
    'src/thread_safe_queue.cpp',
    'src/output_formatter.cpp',
    'modules/line_counter_module.cpp',
    'modules/language_stats.cpp',
    'modules/language_stats_module.cpp',
    'modules/metabuild_system_module.cpp',
    'modules/license_module.cpp',
    'modules/git_module.cpp',
]

include_dirs = include_directories('src', 'modules')

executable('codefetch',
          sources,
          include_directories : include_dirs,
          dependencies : [threads_dep, libgit2_dep],
          install : true,
          cpp_args : ['-Wall', '-Wextra', '-Wpedantic'])
