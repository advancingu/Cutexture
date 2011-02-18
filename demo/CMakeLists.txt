set(DEMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/demo)

set(QT_USE_QTWEBKIT TRUE)
set(QT_USE_QTUITOOLS TRUE)

include(${QT_USE_FILE})

######################################################################
# Configure includes											     #
######################################################################

set(CUTEXTURE_INCLUDE_DIR 
    ${DEMO_DIR}/include
)

if(WIN32)
	if(MSVC)
		set(OGRE3D_INCLUDE_DIR
			${CMAKE_CURRENT_SOURCE_DIR}/../ogre-1.6.4-msvc/OgreMain/include
			${CMAKE_CURRENT_SOURCE_DIR}/../ogre-1.6.4-msvc/Dependencies/include
		)
		set(OIS_INCLUDE_DIR
			${CMAKE_CURRENT_SOURCE_DIR}/../OIS-1.2.0/include
		)
	endif(MSVC)
elseif(UNIX)
    set(OGRE3D_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../Ogre3d-sdk-1.7/include/OGRE
	)
	set(OIS_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}/../OIS-1.2.0/include
	)
endif(WIN32)

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
			${CMAKE_CURRENT_SOURCE_DIR}/../ogre-1.6.4-msvc/lib				
			${CMAKE_CURRENT_SOURCE_DIR}/../ogre-1.6.4-msvc/Dependencies/lib/Release			
			${CMAKE_CURRENT_SOURCE_DIR}/../ogre-1.6.4-msvc/Dependencies/lib/Debug
		)
		set(OGRE3D_SUPPORT_LIB_DIR
			.
		)	
		set(OIS_LIB_DIR
			${CMAKE_CURRENT_SOURCE_DIR}/../OIS-1.2.0/lib
		)
	endif(MSVC)
elseif(UNIX)
    set(OGRE3D_LIB_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../Ogre3d-sdk-1.7/lib
    )
    set(OGRE3D_SUPPORT_LIB_DIR
        /usr/lib64
    )
    set(OIS_LIB_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../OIS-1.2.0/lib
    )
    set(CUTEXTURE_LIB_DIR
    	${CMAKE_CURRENT_SOURCE_DIR}
	)
endif(WIN32)

link_directories(
    ${OGRE3D_LIB_DIR}
    ${OGRE3D_SUPPORT_LIB_DIR}
    ${OIS_LIB_DIR}
    ${QT_LIB_DIR}
    ${CUTEXTURE_LIB_DIR}
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
# Configure sources												     #
######################################################################

#file(GLOB CUTEXTURE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
AUX_SOURCE_DIRECTORY("${DEMO_DIR}/src" CUTEXTURE_SOURCES)

set(CUTEXTURE_MOC_HEADERS
    ${CUTEXTURE_INCLUDE_DIR}/Game.h
)

qt4_wrap_cpp(CUTEXTURE_MOC_SOURCES ${CUTEXTURE_MOC_HEADERS})

######################################################################
# Configure application outputs									     #
######################################################################

add_executable(
	CUTEXTURE ${CUTEXTURE_SOURCES} ${CUTEXTURE_MOC_SOURCES}
)

target_link_libraries(
    CUTEXTURE ${OGRE3D_LIBS_STRINGS} OIS ${QT_LIBRARIES} cutexture
)

if(CMAKE_BUILD_TYPE MATCHES "Release")
	set(CUTEXTURE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/output/bin/release)
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
	set(CUTEXTURE_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/output/bin/debug)
endif(CMAKE_BUILD_TYPE MATCHES "Release")

install(
	TARGETS CUTEXTURE
	DESTINATION ${CUTEXTURE_INSTALL_DIR}
)

######################################################################
# Install libraries and plugins									     #
######################################################################

# OGRE libraries
foreach(LIB ${OGRE3D_LIBS})
	if(UNIX)
		# resolve unix file symlinks
		execute_process(COMMAND 
			readlink ${OGRE3D_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${LIB}${CMAKE_SHARED_LIBRARY_SUFFIX}
			OUTPUT_VARIABLE RESOLVED_LIB
		)
		# if the file was a symlink
		if(RESOLVED_LIB)
			# remove linebreak and write back to LIB
			string(REGEX REPLACE "^(.+)\n" "\\1" LIB ${RESOLVED_LIB})
			#set(LIB ${RESOLVED_LIB})
		endif(RESOLVED_LIB)
	endif(UNIX)
	install(FILES 
		${OGRE3D_LIB_DIR}/${LIB}
		DESTINATION ${CUTEXTURE_INSTALL_DIR}
	)
endforeach(LIB)

# OGRE plugins
file(GLOB OGRE3D_PLUGINS ${OGRE3D_LIB_DIR}/OGRE/*${CMAKE_SHARED_LIBRARY_SUFFIX})
foreach(LIB ${OGRE3D_PLUGINS})
	install(FILES 
		${LIB}
		DESTINATION ${CUTEXTURE_INSTALL_DIR}
	)
endforeach(LIB)

# OIS library
foreach(LIB ${OIS_LIBS})
	if(UNIX)
		# resolve unix file symlinks
		execute_process(COMMAND 
			readlink ${OIS_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${LIB}${CMAKE_SHARED_LIBRARY_SUFFIX}
			OUTPUT_VARIABLE RESOLVED_LIB
		)
		# if the file was a symlink
		if(RESOLVED_LIB)
			# remove 'lib', '.so' as well as linebreak and write back to LIB
			string(REGEX REPLACE "^(.+)\n" "\\1" LIB ${RESOLVED_LIB})
		endif(RESOLVED_LIB)
	endif(UNIX)
	install(FILES 
		${OIS_LIB_DIR}/${LIB}
		DESTINATION ${CUTEXTURE_INSTALL_DIR}
	)
endforeach(LIB)

######################################################################
# Install configuration files									     #
######################################################################

if(WIN32)
	set(CUTEXTURE_OGRE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/configurations/ogre-plugins-win32.cfg)
elseif(UNIX)
	set(CUTEXTURE_OGRE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/configurations/ogre-plugins-unix.cfg)
endif(WIN32)

if(WIN32)
	set(CUTEXTURE_OGRE_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/configurations/ogre-resources-win32.cfg)
elseif(UNIX)
	set(CUTEXTURE_OGRE_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/configurations/ogre-resources-unix.cfg)
endif(WIN32)

install(FILES ${CUTEXTURE_OGRE_CONFIG}
	DESTINATION ${CUTEXTURE_INSTALL_DIR}
	RENAME plugins.cfg
)

install(FILES ${CUTEXTURE_OGRE_RESOURCES}
	DESTINATION ${CUTEXTURE_INSTALL_DIR}
	RENAME resources.cfg
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ui/game.ui
	DESTINATION ${CUTEXTURE_INSTALL_DIR}
)
