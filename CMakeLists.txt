cmake_minimum_required(VERSION 3.15)
project(MyDB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------------------------------------------
# 1. STRICT COMPILER FLAGS & SANITIZERS
# -------------------------------------------------------------------
if(DEFINED WARNINGS_AS_ERRORS)
elseif(DEFINED ENV{WARNINGS_AS_ERRORS})
    set(WARNINGS_AS_ERRORS $ENV{WARNINGS_AS_ERRORS})
else()
    set(WARNINGS_AS_ERRORS ON)
endif()

if(GITHUB_ACTIONS)
    message("NOTE: GITHUB_ACTIONS defined")
    add_compile_definitions(GITHUB_ACTIONS)
endif()

# Compiler flags
message("Compiler: ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
if(MSVC)
    add_compile_options(/W4 /permissive- /wd4244 /wd4267 /wd4996 /external:anglebrackets /external:W0)
    if(WARNINGS_AS_ERRORS)
        add_compile_options(/WX)
    endif()
else()
    add_compile_options(-Wall -Wextra -pedantic)
    if(WARNINGS_AS_ERRORS)
        add_compile_options(-Werror)
    endif()
endif()

# Sanitizers Configuration
set(is_debug "$<CONFIG:Debug>")
set(is_rel_with_deb "$<CONFIG:RelWithDebInfo>")
set(debug_mode "$<OR:${is_debug},${is_rel_with_deb}>")

if(MSVC)
    add_compile_options("$<${debug_mode}:/fsanitize=address>")
elseif(APPLE)
    add_compile_options("$<${debug_mode}:-fsanitize=address,undefined>")
    add_link_options("$<${debug_mode}:-fsanitize=address,undefined>")
elseif(UNIX)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        message("GCC detected: Sanitizers disabled to allow Valgrind usage.")
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER_EQUAL "14.0")
            add_compile_options("$<${debug_mode}:-fsanitize=address,undefined>")
            add_link_options("$<${debug_mode}:-fsanitize=address,undefined>")
        else()
            message("Legacy Clang detected: Sanitizers skipped.")
        endif()
    endif()
endif()

# -------------------------------------------------------------------
# 2. DEPENDENCY INJECTION (GLFW & OPENGL)
# -------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

# -------------------------------------------------------------------
# 3. SOURCE TREE MAPPING
# -------------------------------------------------------------------
file(GLOB_RECURSE USER_SRCS "src/*.cpp")

set(IMGUI_SRCS
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
    external/imgui/misc/cpp/imgui_stdlib.cpp
)

# -------------------------------------------------------------------
# THE EXECUTABLE CONTRACT
# -------------------------------------------------------------------
add_executable(db_engine ${USER_SRCS} ${IMGUI_SRCS})

# -------------------------------------------------------------------
# STRICT COMPILER FLAGS (Target-Scoped)
# -------------------------------------------------------------------
if(DEFINED WARNINGS_AS_ERRORS)
elseif(DEFINED ENV{WARNINGS_AS_ERRORS})
    set(WARNINGS_AS_ERRORS $ENV{WARNINGS_AS_ERRORS})
else()
    set(WARNINGS_AS_ERRORS ON)
endif()

if(GITHUB_ACTIONS)
    target_compile_definitions(db_engine PRIVATE GITHUB_ACTIONS)
endif()

if(MSVC)
    target_compile_options(db_engine PRIVATE /W4 /permissive- /wd4244 /wd4267 /wd4996 /external:anglebrackets /external:W0)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(db_engine PRIVATE /WX)
    endif()
else()
    target_compile_options(db_engine PRIVATE -Wall -Wextra -pedantic)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(db_engine PRIVATE -Werror)
    endif()
endif()

# -------------------------------------------------------------------
# SANITIZERS (Target-Scoped)
# -------------------------------------------------------------------
set(is_debug "$<CONFIG:Debug>")
set(is_rel_with_deb "$<CONFIG:RelWithDebInfo>")
set(debug_mode "$<OR:${is_debug},${is_rel_with_deb}>")

if(MSVC)
    target_compile_options(db_engine PRIVATE "$<${debug_mode}:/fsanitize=address>")
elseif(APPLE)
    target_compile_options(db_engine PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
    target_link_options(db_engine PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
elseif(UNIX)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        # GCC left clean for Valgrind
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER_EQUAL "14.0")
            target_compile_options(db_engine PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
            target_link_options(db_engine PRIVATE "$<${debug_mode}:-fsanitize=address,undefined>")
        endif()
    endif()
endif()

# -------------------------------------------------------------------
# 5. COMPILER INCLUDE FLAGS (-I equivalents)
# -------------------------------------------------------------------
target_include_directories(db_engine PRIVATE
    external/imgui
    external/imgui/backends
    src/Crud
    src/Database
    src/Interface
    src/Interpreter
    src/Misc
)

# -------------------------------------------------------------------
# 6. LINKER FLAGS (-l equivalents)
# -------------------------------------------------------------------
target_link_libraries(db_engine PRIVATE glfw OpenGL::GL)

install(TARGETS db_engine DESTINATION bin)