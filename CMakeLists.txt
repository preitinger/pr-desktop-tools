
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

###
### ACHTUNG, VORUEBERGEHEND RELEASE!
###
set(MY_RELEASE ON)
if (MY_RELEASE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()


# Nur nötig, wenn er ihn trotz C++23 nicht findet:
#include_directories("/usr/local/include/c++/14.3.0")
#include_directories("/usr/local/include/c++/14.3.0/x86_64-pc-linux-gnu")



# END empfohlen von KI


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

  # using Clang
  message(("Clang"))
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
    if (MY_RELEASE)
        add_compile_options(-Wall -Wextra -Wno-parentheses -Wno-unused-parameter -Wredundant-move -O5 -fdump-tree-tailr1-details -fmax-errors=1)
        add_link_options(-O5)
    else ()
        add_compile_options(-Wall -Wextra -Wno-parentheses -Wno-unused-parameter -Wredundant-move -O0 -fdump-tree-tailr1-details -fmax-errors=1)
        add_link_options(-O0)
    endif()
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

add_executable(dataIO5
    # src/dataIO2/TypeDecl.cxx
    # src/dataIO2/templates.cxx
    src/dataIO5/main.cxx
    src/dataIO5/utils/StackTraceException.cxx
    src/dataIO5/TypeExp.cxx
    src/dataIO5/TSLiteral.cxx
    src/dataIO5/TSReference.cxx
    src/dataIO5/TSAttributeList.cxx
    src/dataIO5/TSUnion.cxx
    src/dataIO5/TSArray.cxx
    src/dataIO5/Decl.cxx
    src/dataIO5/DeclCol.cxx
    src/dataIO5/parse.cxx
    src/dataIO5/TSAttribute.cxx
    src/dataIO5/maps.cxx
    src/dataIO5/Substitution.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    # src/dataIO5/.cxx
    src/dataIO5/utils/utils.cxx
    src/dataIO5/utils/SRange.cxx
    src/dataIO5/utils/Output.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    src/dataIO5/logics/Func.cxx
    src/dataIO5/logics/Exp.cxx
    src/dataIO5/logics/FuncExp.cxx
    src/dataIO5/logics/VarExp.cxx
    src/dataIO5/logics/test.cxx
    src/dataIO5/logics/unification.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
    # src/dataIO5/utils/.cxx
)
target_include_directories(dataIO5 PRIVATE src/dataIO5)

# Wichtig für std::stacktrace in GCC 14
target_link_libraries(dataIO5 PRIVATE stdc++exp)

message("Breche nach dataIO5 ab!")
return()

add_executable(dataIO4
    # src/dataIO2/TypeDecl.cxx
    # src/dataIO2/templates.cxx
    src/dataIO4/main.cxx
    src/dataIO4/utils/StackTraceException.cxx
    src/dataIO4/TypeExp.cxx
    src/dataIO4/TSLiteral.cxx
    src/dataIO4/TSReference.cxx
    src/dataIO4/TSAttributeList.cxx
    src/dataIO4/TSUnion.cxx
    src/dataIO4/TSArray.cxx
    src/dataIO4/Decl.cxx
    src/dataIO4/DeclCol.cxx
    src/dataIO4/parse.cxx
    src/dataIO4/TSAttribute.cxx
    src/dataIO4/maps.cxx
    src/dataIO4/Substitution.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    # src/dataIO4/.cxx
    src/dataIO4/utils/utils.cxx
    src/dataIO4/utils/SRange.cxx
    src/dataIO4/utils/Output.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    src/dataIO4/logics/Func.cxx
    src/dataIO4/logics/Exp.cxx
    src/dataIO4/logics/FuncExp.cxx
    src/dataIO4/logics/VarExp.cxx
    src/dataIO4/logics/test.cxx
    src/dataIO4/logics/unification.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
    # src/dataIO4/utils/.cxx
)
target_include_directories(dataIO4 PRIVATE src/dataIO4)

# Wichtig für std::stacktrace in GCC 14
target_link_libraries(dataIO4 PRIVATE stdc++exp)

message("Breche nach dataIO4 ab!")
return()

add_subdirectory(src/polymorphic-templates bins)

# add_executable(test_static-cast
#     src/test/static-cast.cxx
# )

# return()

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

# add_executable(dataIO-codegen
#     src/dataIO-codegen/Type.cxx
#     src/dataIO-codegen/utils.cxx
#     src/dataIO-codegen/codeGen.cxx
#     src/dataIO-codegen/Indent.cxx 
#     src/dataIO-codegen/TestInitializer.cxx
#     src/dataIO-codegen/testExampleType.cxx
# )
# # Wichtig für std::stacktrace in GCC 14
# target_link_libraries(dataIO-codegen PRIVATE stdc++exp)

add_executable(dataIO2
    # src/dataIO2/TypeDecl.cxx
    # src/dataIO2/templates.cxx
    src/dataIO2/main.cxx
    src/dataIO-codegen/utils.cxx
    src/dataIO-codegen/Indent.cxx
)
# Wichtig für std::stacktrace in GCC 14
target_link_libraries(dataIO2 PRIVATE stdc++exp)


add_executable(dataIO3
    # src/dataIO2/TypeDecl.cxx
    # src/dataIO2/templates.cxx
    src/dataIO3/main.cxx
    src/dataIO3/Literal.cxx
    src/dataIO3/Reference.cxx
    src/dataIO3/utils.cxx
    src/dataIO3/Attribute.cxx
    src/dataIO3/AttributeList.cxx
    src/dataIO3/AnyTSType.cxx
    src/dataIO3/Union.cxx
    src/dataIO3/Array.cxx
)
# Wichtig für std::stacktrace in GCC 14
target_link_libraries(dataIO3 PRIVATE stdc++exp)



include(CTest)
enable_testing()


