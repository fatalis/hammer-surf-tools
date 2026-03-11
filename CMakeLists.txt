cmake_minimum_required(VERSION 3.10)
set(CMAKE_POLICY_VERSION_MINIMUM 3.10) # force subprojects

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/toolchain-mingw64.cmake")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_C_FLAGS_RELEASE "-O2")

project(hammer-surf-tools LANGUAGES C)

add_subdirectory(extern/minhook)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wdouble-promotion -Wconversion -Wno-address-of-packed-member")

set(SOURCES 
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

# main dll (version.dll)
add_library(hammer-surf-tools SHARED
  src/dll.c
  ${SOURCES}
)

target_include_directories(hammer-surf-tools PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hammer-surf-tools PRIVATE psapi minhook)
target_link_options(hammer-surf-tools PRIVATE
    -Wl,--export-all-symbols
)
set_target_properties(hammer-surf-tools PROPERTIES
  OUTPUT_NAME version
  PREFIX ""
  SUFFIX ".dll"
)

if(NOT DEFINED ENV{GITHUB_ACTIONS})
    # move to where hammerplusplus.exe is
    set_target_properties(hammer-surf-tools PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../.."
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../../.."
    )
endif()

# pattern test program
if(NOT DEFINED ENV{GITHUB_ACTIONS})
    add_executable(test-patterns
      src/test-patterns.c
      ${SOURCES}
    )

    target_include_directories(test-patterns PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(test-patterns PRIVATE minhook)
    target_compile_definitions(test-patterns PRIVATE TEST_PATTERNS)
    set_target_properties(test-patterns PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()
