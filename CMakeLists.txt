cmake_minimum_required(VERSION 3.10.0)
project(hello VERSION 0.1.0 LANGUAGES C CXX)

set(FLTK_DIR /home/peter/fltk/build CACHE STRING 
    "FLTK binary folder with FLTKConfig.cmake")

find_package(FLTK REQUIRED CONFIG)

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


add_executable(pr-desktop-tools
    src/smain.cxx
    src/CountDown.cxx
    src/TrackMouse.cxx
    src/Stocks.cxx
    src/Robot.cxx
)

target_include_directories(pr-desktop-tools PRIVATE ${FLTK_INCLUDE_DIRS})
target_link_libraries(pr-desktop-tools fltk fltk_gl fltk_images)


include(CTest)
enable_testing()


