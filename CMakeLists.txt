
#cmake_minimum_required(VERSION 3.10.0)
cmake_minimum_required(VERSION 3.20) # Wichtig für moderne C++ Standards

project(pr-desktop-tools VERSION 0.1.0 LANGUAGES C CXX)

# BEGIN empfohlen von KI
# Den C++ Standard auf 23 festlegen
set(CMAKE_CXX_STANDARD 23)

# Sicherstellen, dass der Standard angefordert wird (kein Fallback auf ältere Versionen)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Verhindert die Nutzung von Compiler-spezifischen Erweiterungen (z.B. gnu++23)
# Setzen Sie dies auf OFF, wenn Sie GCC-spezifische Features benötigen
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Nur nötig, wenn er ihn trotz C++23 nicht findet:
#include_directories("/usr/local/include/c++/14.3.0")
#include_directories("/usr/local/include/c++/14.3.0/x86_64-pc-linux-gnu")



# END empfohlen von KI


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

  # using Clang
  message(("Clang"))
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
  message(("GCC") (${CMAKE_CXX_COMPILE_FEATURES}))
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
  message(("Intel C++"))
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
  message(("Visual Studio C++"))
endif()

set(FLTK_DIR /home/peter/fltk/build CACHE STRING 
    "FLTK binary folder with FLTKConfig.cmake")

find_package(FLTK REQUIRED CONFIG)

find_package(PkgConfig)

# find_package(mongocxx REQUIRED)

# these calls create special `PkgConfig::<MODULE>` variables
pkg_check_modules(MONGOCXX REQUIRED IMPORTED_TARGET GLOBAL libmongocxx)
#pkg_check_modules(YOUR_PKG REQUIRED IMPORTED_TARGET GLOBAL ya-package)

# add_executable(pr-desktop-tools_old
#     src/main.cxx
#     src/Robot.cxx 
#     src/Activity.cxx 
#     src/ActivityButton.cxx
#     src/StocksActivity.cxx
#     src/SleepActivity.cxx
#     src/TrackMouseActivity.cxx
#     src/SActivity.cxx
#     src/SButtonAccess.cxx
#     src/Button.cxx
# )

# BEGIN KI empfohlen
set(CMAKE_INSTALL_RPATH "/usr/local/lib64")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# END KI empfohlen

add_executable(pr-desktop-tools
    src/smain.cxx
    src/CountDown.cxx
    src/TrackMouse.cxx
    src/Stocks.cxx
    src/Robot.cxx
    src/TestWidgetAccess.cxx
    src/SmartAction.cxx

    src/BaseAction.cxx
    src/ExampleAction.cxx
    src/StocksAction.cxx
    src/GenActionCodeAction.cxx
    src/TestClass.cxx
    src/BlaBlubb.cxx
    src/pr-newsletter/IncVersions.cxx
    src/pr-newsletter/incVersions.cxx
)

message(("FLTK_INCLUDE_DIRS") ("${FLTK_INCLUDE_DIRS}"))
message(("MONGOCXX_INCLUDE_DIRS") ("${MONGOCXX_INCLUDE_DIRS}"))
target_include_directories(pr-desktop-tools PRIVATE ${FLTK_INCLUDE_DIRS})
target_link_libraries(pr-desktop-tools fltk fltk_gl fltk_images pthread)

add_executable(incVersionsBuildStart
    src/pr-newsletter/main_incVersionsBuildAllStart.cxx
    src/pr-newsletter/incVersions.cxx
)

add_executable(pr-push-me
    src/pr-push-me/main_pr-push-me.cxx
)
target_include_directories(pr-push-me PRIVATE ${MONGOCXX_INCLUDE_DIRS})
target_link_libraries(pr-push-me PUBLIC PkgConfig::MONGOCXX fltk fltk_gl fltk_images pthread)

add_executable(dataIO-codegen
    src/dataIO-codegen/Type.cxx
    src/dataIO-codegen/utils.cxx
    src/dataIO-codegen/codeGen.cxx
    src/dataIO-codegen/Indent.cxx 
    src/dataIO-codegen/TestInitializer.cxx
)
# Wichtig für std::stacktrace in GCC 14
target_link_libraries(dataIO-codegen PRIVATE stdc++exp)



include(CTest)
enable_testing()


