#
# Find the KDevelop Platform modules and sets various variables accordingly
#
# KDEVPLATFORM_INCLUDE_DIR and KDEVPLATFORM_LIBRARY_DIR can be set to
# adjust the directories where the platform headers and libraries are searched
#
# KDEVPLATFORM_FOUND - set to TRUE if the platform was found (that is all its modules where found)
#                      FALSE otherwise
#
# KDEVPLATFORM_INCLUDE_DIR         - include dir of the platform, for example /usr/include/kdevplatform
#                                    can be set by the user to select different include dirs
# KDEVPLATFORM_INTERFACES_LIBRARY  - interfaces module library
# KDEVPLATFORM_LANGUAGE_LIBRARY    - language module library
# KDEVPLATFORM_OUTPUTVIEW_LIBRARY  - outputview module library
# KDEVPLATFORM_PROJECT_LIBRARY     - project module library
# KDEVPLATFORM_SUBLIME_LIBRARY     - sublime module library
# KDEVPLATFORM_SHELL_LIBRARY       - shell module library
# KDEVPLATFORM_UTIL_LIBRARY        - util module library
# KDEVPLATFORM_VCS_LIBRARY         - vcs module library
#
# KDEVPLATFORM_LIBRARIES           - all platform module libraries
# KDEVPLATFORM_LIBRARY_DIR         - the directory containing the KDevPlatform libraries
#
# Copyright 2007 Andreas Pakulat <apaku@gmx.de>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if( KDEVPLATFORM_INCLUDE_DIR AND KDEVPLATFORM_LIBRARIES )
    set(KDevPlatform_FIND_QUIETLY TRUE)
endif( KDEVPLATFORM_INCLUDE_DIR AND KDEVPLATFORM_LIBRARIES )

if(WIN32)
    file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _progFiles)
    set(_KDEVPLATFORM_INCLUDE_DIR ${KDE4_INCLUDE_DIR} ${_progFiles}/kdevplatform/include)
    set(_KDEVPLATFORM_LIB_DIR ${KDE4_LIB_DIR} ${_progFiles}/kdevplatform/lib)
endif(WIN32)

if( NOT KDEVPLATFORM_INCLUDE_DIR )
    find_path( _platformIncDir kdevplatform/interfaces/iplugin.h
        PATHS
        ${CMAKE_INSTALL_PREFIX}/include
        ${_KDEVPLATFORM_INCLUDE_DIR}
	${KDE4_INCLUDE_DIR}
    )
    if(_platformIncDir)
        set(KDEVPLATFORM_INCLUDE_DIR ${_platformIncDir}/kdevplatform)
        set(KDEVPLATFORM_INCLUDE_DIR ${KDEVPLATFORM_INCLUDE_DIR} CACHE PATH "kdevplatform include directory containing the various platform modules")
    endif(_platformIncDir)
else( NOT KDEVPLATFORM_INCLUDE_DIR )
    set(KDEVPLATFORM_INCLUDE_DIR ${KDEVPLATFORM_INCLUDE_DIR} CACHE PATH "kdevplatform include directory containing the various platform modules")
endif( NOT KDEVPLATFORM_INCLUDE_DIR )

if( NOT KDEVPLATFORM_LIBRARY_DIR )
    find_library( _platforminterfaces_lib NAMES kdevplatforminterfaces
        PATHS
        ${LIB_INSTALL_DIR}
	${KDE4_LIB_DIR}
        ${_KDEVPLATFORM_LIB_DIR}
    )
    if( _platforminterfaces_lib )
        get_filename_component(KDEVPLATFORM_LIBRARY_DIR ${_platforminterfaces_lib} PATH )
        set( KDEVPLATFORM_LIBRARY_DIR ${KDEVPLATFORM_LIBRARY_DIR} CACHE PATH "path for kdevplatform libraries" )
    endif( _platforminterfaces_lib )
else( NOT KDEVPLATFORM_LIBRARY_DIR )
    set( KDEVPLATFORM_LIBRARY_DIR ${KDEVPLATFORM_LIBRARY_DIR} CACHE PATH "path for kdevplatform libraries" )
endif( NOT KDEVPLATFORM_LIBRARY_DIR )

find_library(KDEVPLATFORM_INTERFACES_LIBRARY kdevplatforminterfaces
    PATHS
    ${KDEVPLATFORM_LIBRARY_DIR}
    NO_DEFAULT_PATH
)

if(NOT KDEVPLATFORM_INTERFACES_LIBRARY)
message("Didn't find INTERFACES")
endif(NOT KDEVPLATFORM_INTERFACES_LIBRARY)


find_library(KDEVPLATFORM_LANGUAGE_LIBRARY kdevplatformlanguage
    PATHS
    ${KDEVPLATFORM_LIBRARY_DIR}
    NO_DEFAULT_PATH
)
if(NOT KDEVPLATFORM_LANGUAGE_LIBRARY)
message("Didn't find LANGUAGE")
endif(NOT KDEVPLATFORM_LANGUAGE_LIBRARY)
find_library(KDEVPLATFORM_OUTPUTVIEW_LIBRARY kdevplatformoutputview
    PATHS
    ${KDEVPLATFORM_LIBRARY_DIR}
    NO_DEFAULT_PATH
)
if(NOT KDEVPLATFORM_OUTPUTVIEW_LIBRARY)
message("Didn't find OUTPUTVIEW")
endif(NOT KDEVPLATFORM_OUTPUTVIEW_LIBRARY)
find_library(KDEVPLATFORM_PROJECT_LIBRARY kdevplatformproject
    PATHS
    ${KDEVPLATFORM_LIBRARY_DIR}
    NO_DEFAULT_PATH
)
if(NOT KDEVPLATFORM_PROJECT_LIBRARY)
message("Didn't find PROJECT")
endif(NOT KDEVPLATFORM_PROJECT_LIBRARY)
find_library(KDEVPLATFORM_SUBLIME_LIBRARY sublime
    PATHS
    ${KDEVPLATFORM_LIBRARY_DIR}
    NO_DEFAULT_PATH
)
if(NOT KDEVPLATFORM_SUBLIME_LIBRARY)
message("Didn't find SUBLIME")
endif(NOT KDEVPLATFORM_SUBLIME_LIBRARY)
find_library(KDEVPLATFORM_SHELL_LIBRARY kdevplatformshell
    PATHS
    ${KDEVPLATFORM_LIBRARY_DIR}
    NO_DEFAULT_PATH
)
if(NOT KDEVPLATFORM_SHELL_LIBRARY)
message("Didn't find SHELL")
endif(NOT KDEVPLATFORM_SHELL_LIBRARY)
find_library(KDEVPLATFORM_UTIL_LIBRARY kdevplatformutil
    PATHS
    ${KDEVPLATFORM_LIBRARY_DIR}
    NO_DEFAULT_PATH
)
if(NOT KDEVPLATFORM_UTIL_LIBRARY)
message("Didn't find UTIL")
endif(NOT KDEVPLATFORM_UTIL_LIBRARY)
find_library(KDEVPLATFORM_VCS_LIBRARY kdevplatformvcs
    PATHS
    ${KDEVPLATFORM_LIBRARY_DIR}
    NO_DEFAULT_PATH
)
if(NOT KDEVPLATFORM_VCS_LIBRARY)
message("Didn't find VCS")
endif(NOT KDEVPLATFORM_VCS_LIBRARY)

if( KDEVPLATFORM_INCLUDE_DIR
 AND KDEVPLATFORM_INTERFACES_LIBRARY
 AND KDEVPLATFORM_LANGUAGE_LIBRARY
 AND KDEVPLATFORM_OUTPUTVIEW_LIBRARY
 AND KDEVPLATFORM_PROJECT_LIBRARY
 AND KDEVPLATFORM_SUBLIME_LIBRARY
 AND KDEVPLATFORM_SHELL_LIBRARY
 AND KDEVPLATFORM_UTIL_LIBRARY
 AND KDEVPLATFORM_VCS_LIBRARY )

    if( NOT KDevPlatform_FIND_QUIETLY )
        message(STATUS "Found all KDevPlatform modules")
    endif( NOT KDevPlatform_FIND_QUIETLY )

# if all modules found
    set(KDEVPLATFORM_FOUND TRUE)
    set(KDEVPLATFORM_LIBRARIES
        ${KDEVPLATFORM_INTERFACES_LIBRARY}
        ${KDEVPLATFORM_LANGUAGE_LIBRARY}
        ${KDEVPLATFORM_OUTPUTVIEW_LIBRARY}
        ${KDEVPLATFORM_PROJECT_LIBRARY}
        ${KDEVPLATFORM_SUBLIME_LIBRARY}
        ${KDEVPLATFORM_SHELL_LIBRARY}
        ${KDEVPLATFORM_UTIL_LIBRARY}
        ${KDEVPLATFORM_VCS_LIBRARY}
    )

    set(KDEVPLATFORM_LIBRARIES ${KDEVPLATFORM_LIBRARIES} CACHE STRING "List of all kdevplatform module libraries")

else( KDEVPLATFORM_INCLUDE_DIR
 AND KDEVPLATFORM_INTERFACES_LIBRARY
 AND KDEVPLATFORM_LANGUAGE_LIBRARY
 AND KDEVPLATFORM_OUTPUTVIEW_LIBRARY
 AND KDEVPLATFORM_PROJECT_LIBRARY
 AND KDEVPLATFORM_SUBLIME_LIBRARY
 AND KDEVPLATFORM_SHELL_LIBRARY
 AND KDEVPLATFORM_UTIL_LIBRARY
 AND KDEVPLATFORM_VCS_LIBRARY )

    message(STATUS "You can set KDEVPLATFORM_INCLUDE_DIR and KDEVPLATFORM_LIBRARY_DIR to help cmake find KDevPlatform")
    if( KDevPlatform_FIND_REQUIRED)
        message(FATAL_ERROR "Couldn't find all platform modules.")
    else( KDevPlatform_FIND_REQUIRED)
        message(STATUS "Couldn't find all platform modules")
    endif( KDevPlatform_FIND_REQUIRED)
# not all platform modules found
    set(KDEVPLATFORM_FOUND FALSE)

endif(KDEVPLATFORM_INCLUDE_DIR
 AND KDEVPLATFORM_INTERFACES_LIBRARY
 AND KDEVPLATFORM_LANGUAGE_LIBRARY
 AND KDEVPLATFORM_OUTPUTVIEW_LIBRARY
 AND KDEVPLATFORM_PROJECT_LIBRARY
 AND KDEVPLATFORM_SUBLIME_LIBRARY
 AND KDEVPLATFORM_SHELL_LIBRARY
 AND KDEVPLATFORM_UTIL_LIBRARY
 AND KDEVPLATFORM_VCS_LIBRARY )

if(KDEVPLATFORM_FOUND)
    message(STATUS "Using kdevplatform include dir: ${KDEVPLATFORM_INCLUDE_DIR}")
    message(STATUS "Using kdevplatform library dir: ${KDEVPLATFORM_LIBRARY_DIR}")
endif(KDEVPLATFORM_FOUND)


MARK_AS_ADVANCED(
    KDEVPLATFORM_INTERFACES_LIBRARY
    KDEVPLATFORM_LANGUAGE_LIBRARY
    KDEVPLATFORM_OUTPUTVIEW_LIBRARY
    KDEVPLATFORM_PROJECT_LIBRARY
    KDEVPLATFORM_SUBLIME_LIBRARY
    KDEVPLATFORM_SHELL_LIBRARY
    KDEVPLATFORM_UTIL_LIBRARY
    KDEVPLATFORM_VCS_LIBRARY
)
