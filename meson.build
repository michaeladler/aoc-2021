project('aoc', 'cpp',
  version : '0.1',
  default_options : [
        'warning_level=3',
        'cpp_std=c++20',
        'b_lto=true',
        'b_lto_mode=thin',
        'b_ndebug=if-release'])

add_project_arguments(['-march=native'], language : 'cpp')

if get_option('buildtype') == 'release'
  add_project_arguments('-fno-math-errno', language : 'cpp')
endif

shared_src = [
  'share/cpp/parse.cpp',
  'share/cpp/aoc.cpp',
]

all_days_c = [
  'src/day01.cpp',
  'src/day02.cpp',
  'src/day03.cpp',
  'src/day04.cpp',
  'src/day05.cpp',
  'src/day06.cpp',
  'src/day07.cpp',
  'src/day08.cpp',
  'src/day09.cpp',
  'src/day10.cpp',
  'src/day11.cpp',
  'src/day12.cpp',
  'src/day13.cpp',
  'src/day14.cpp',
  'src/day15.cpp',
  'src/day16.cpp',
  'src/day17.cpp',
  'src/day18.cpp',
  'src/day19.cpp',
  'src/day20.cpp',
  'src/day21.cpp',
  'src/day22.cpp',
  'src/day23.cpp',
  'src/day24.cpp',
  'src/day25.cpp',
  # marker
]

prog_sh = find_program('sh')

all_days_h = custom_target(
  'all_days.h',
  input:  ['share/scripts/gen_header.sh'] + all_days_c,
  output: 'all_days.h',
  command : [prog_sh, '-c', '$0 $1 > $2', '@INPUT0@', '@SOURCE_ROOT@', '@OUTPUT@'],
)

incdir = include_directories('share/cpp', 'include')

fmt_dep = dependency('fmt')
threads_dep = dependency('threads')
all_deps = [fmt_dep, threads_dep]

executable('aoc',
  ['src/main.cpp'] + shared_src + all_days_c + [all_days_h],
  include_directories: incdir,
  dependencies: all_deps,
)

foreach day_src : all_days_c
  day = day_src.strip('src/').substring(0, -4)

  executable('@0@'.format(day),
    shared_src + [day_src],
    include_directories: incdir,
    dependencies: all_deps,
    cpp_args: [ '-DIS_MAIN' ])

  testexe = executable('@0@_test'.format(day),
    shared_src + [day_src],
    include_directories: incdir,
    dependencies: all_deps,
    cpp_args: ['-DIS_TEST', '-DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN'])
  test(day, testexe)
endforeach
