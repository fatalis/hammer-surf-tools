cmake_minimum_required(VERSION 3.10)

# On Linux, cross-compile for Windows so windows.h and MinGW are used
if(UNIX AND NOT APPLE AND NOT CMAKE_CROSSCOMPILING)
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/toolchain-mingw64.cmake")
endif()

project(hammer-surf-utils LANGUAGES C)

set(CMAKE_C_STANDARD 23)

add_subdirectory(extern/minhook)

# Default to Release (compile.sh used -O2)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# Match compile.sh: -O2 -Wall
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wdouble-promotion -Wconversion")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Werror")

add_library(hammer-surf-utils SHARED
  src/dll.c
  src/common.c
  src/wrapper.c
  src/patterns.c
  src/hooks.c
  src/hammerfuncs.c
  src/contextmenu.c
  src/brushsync.c
  src/rampgen.c
  src/measure.c
  src/anglefix.c
  src/triggergen.c
  src/scriptfuncs.c
  src/hotkeys.c
  src/util.c
  src/offsets.c
  src/testincludes.c
)

add_executable(test-patterns
  src/patterns.c
  src/test-patterns.c

  src/common.c
  # src/wrapper.c
  src/patterns.c
  src/hooks.c
  src/hammerfuncs.c
  src/contextmenu.c
  # src/brushsync.c
  src/rampgen.c
  src/measure.c
  src/anglefix.c
  src/triggergen.c
  src/scriptfuncs.c
  src/hotkeys.c
  src/util.c
  src/offsets.c
  # src/testincludes.c
)

target_include_directories(hammer-surf-utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(test-patterns PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(hammer-surf-utils PRIVATE psapi minhook)
target_link_libraries(test-patterns PRIVATE minhook)

target_compile_definitions(test-patterns PRIVATE TEST_PATTERNS)

target_link_options(hammer-surf-utils PRIVATE
    # -static
    # -static-libgcc
    # -static-libstdc++
)

# Match compile.sh: output version.dll into ../../.. (same location as script)
set_target_properties(hammer-surf-utils PROPERTIES
  OUTPUT_NAME version
  PREFIX ""
  SUFFIX ".dll"
  # LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../.."
)

# set_target_properties(test-patterns PROPERTIES
#   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
# )

# Build as Windows DLL (WIN32 is set when using mingw toolchain)
if(WIN32)
  set_target_properties(hammer-surf-utils PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
  )
endif()
