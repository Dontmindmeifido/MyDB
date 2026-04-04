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