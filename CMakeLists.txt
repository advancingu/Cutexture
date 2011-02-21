######################################################################
# Build system for Cutexture										 #
######################################################################

project(Cutexture)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules)

if(CMAKE_BUILD_TYPE)
    message("Building ${CMAKE_BUILD_TYPE} build.")
# TODO: CMAKE_BUILD_TYPE doesn't work for MSVC projects; should use CMake's "debug" and "optimized" flags instead.
elseif(NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "No build type specified. Use -DCMAKE_BUILD_TYPE=\"$BUILDTYPE\" to specify.")
endif(CMAKE_BUILD_TYPE)

# Import dependency path definitions
include(build/paths.cmake)

######################################################################
# Configure Qt													     #
######################################################################

# Find QT and provide a hint on where it might be
find_package(Qt4 REQUIRED)

if (${QT_VERSION_MAJOR}.${QT_VERSION_MINOR} LESS 4.6)
    message(FATAL_ERROR "Qt version 4.6 or higher required. Found version ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.")
endif(${QT_VERSION_MAJOR}.${QT_VERSION_MINOR} LESS 4.6)

message("Using Qt version "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}.)

include(${QT_USE_FILE})

######################################################################
# Configure includes											     #
######################################################################

set(CUTEXTURE_INCLUDE_DIR 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
	if(MSVC)
		set(OGRE3D_INCLUDE_DIR
			${OGRE3D_PATH}/OgreMain/include
			${OGRE3D_PATH}/Dependencies/include
		)
	endif(MSVC)
elseif(UNIX)
    set(OGRE3D_INCLUDE_DIR
        ${OGRE3D_PATH}/include/OGRE
	)
endif(WIN32)
set(OIS_INCLUDE_DIR
	${OIS_PATH}/include
)

include_directories(
    ${CUTEXTURE_INCLUDE_DIR}
    ${OGRE3D_INCLUDE_DIR}
    ${OIS_INCLUDE_DIR}
    ${QT_INCLUDES}
)

######################################################################
# Configure library paths										     #
######################################################################

if(WIN32)
	if(MSVC)
		set(OGRE3D_LIB_DIR
			${OGRE3D_PATH}/lib				
			${OGRE3D_PATH}/Dependencies/lib/Release			
			${OGRE3D_PATH}/Dependencies/lib/Debug
		)
		set(OGRE3D_SUPPORT_LIB_DIR
			.
		)	
		set(OIS_LIB_DIR
			${OIS_PATH}/lib
		)
	endif(MSVC)
elseif(UNIX)
    set(OGRE3D_LIB_DIR
        ${OGRE3D_PATH}/lib
    )
    set(OGRE3D_SUPPORT_LIB_DIR
        /usr/lib64
    )
    set(OIS_LIB_DIR
        ${OIS_PATH}/lib
    )
endif(WIN32)

link_directories(
    ${OGRE3D_LIB_DIR}
    ${OGRE3D_SUPPORT_LIB_DIR}
    ${OIS_LIB_DIR}
    ${QT_LIB_DIR}
)

######################################################################
# Configure library inputs										     #
######################################################################

set(OGRE3D_RELEASE_LIBS
    OgreMain
)

if(WIN32)
    set (OGRE3D_DEBUG_LIBS
        OgreMain_d
    )
elseif(UNIX)
    set (OGRE3D_DEBUG_LIBS
        ${OGRE3D_RELEASE_LIBS}
    )
endif(WIN32)

# defines a list of "[debug|release] $libname" tuples to be passed to target_link_libraries()
foreach(LIB ${OGRE3D_RELEASE_LIBS})
    set(OGRE3D_LIBS_STRINGS
        ${OGRE3D_LIBS_STRINGS}
        optimized ${LIB}
    )
endforeach(LIB)
foreach(LIB ${OGRE3D_DEBUG_LIBS})
    set(OGRE3D_LIBS_STRINGS
        ${OGRE3D_LIBS_STRINGS}
        debug ${LIB}
    )
endforeach(LIB)

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(OGRE3D_LIBS ${OGRE3D_RELEASE_LIBS})
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(OGRE3D_LIBS ${OGRE3D_DEBUG_LIBS})
endif(CMAKE_BUILD_TYPE MATCHES "Release")

set(OIS_RELEASE_LIBS OIS)
# TODO use correct debug lib
set(OIS_DEBUG_LIBS OIS)

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(OIS_LIBS ${OIS_RELEASE_LIBS})
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(OIS_LIBS ${OIS_DEBUG_LIBS})
endif(CMAKE_BUILD_TYPE MATCHES "Release")

######################################################################
# Configure sources                                                  #
######################################################################

#file(GLOB CUTEXTURE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/src" CUTEXTURE_SOURCES)

set(CUTEXTURE_MOC_HEADERS
    ${CUTEXTURE_INCLUDE_DIR}/UiManager.h
    ${CUTEXTURE_INCLUDE_DIR}/InputManager.h
)

qt4_wrap_cpp(CUTEXTURE_MOC_SOURCES ${CUTEXTURE_MOC_HEADERS})

######################################################################
# Configure application outputs                                      #
######################################################################

add_library(
	cutexture SHARED ${CUTEXTURE_SOURCES} ${CUTEXTURE_MOC_SOURCES}
)

set_target_properties(cutexture PROPERTIES VERSION 0.1.0)

if(CMAKE_BUILD_TYPE MATCHES "Release")
	set(CUTEXTURE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/output/lib/release)
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
	set(CUTEXTURE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/output/lib/debug)
endif(CMAKE_BUILD_TYPE MATCHES "Release")

install(
	TARGETS cutexture
	LIBRARY DESTINATION ${CUTEXTURE_INSTALL_DIR}
)

######################################################################
# Configure unit tests                                               #
######################################################################

#include(src/tests/CMakeLists.txt)

######################################################################
# Configure demo project                                             #
######################################################################

include(demo/CMakeLists.txt)
