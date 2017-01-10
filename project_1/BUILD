package(default_visibility = ['//visibility:public'])

cc_library(
  name = 'rats_lib',
  visibility = ['//visibility:public'],
  srcs = [
    'utilities.cpp',
    'Rat.cpp',
    'Arena.cpp',
    'Player.cpp',
    'Game.cpp',
    'History.cpp',
  ],
  hdrs = [
    'globals.h',
    'Arena.h',
    'Rat.h',
    'Player.h',
    'Game.h',
    'History.h',
  ],
)

cc_binary(
  name = 'rats',
  visibility = ['//visibility:public'],
  srcs = [
    'main.cpp',
  ],
  deps = [
    ':rats_lib',
  ],
)
