if(WIN32)
	if(MSVC)
		set(OGRE3D_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../ogre-1.6.4-msvc)
	endif(MSVC)	
elseif(UNIX)
	set(OGRE3D_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Ogre3d-sdk-1.7)
endif(WIN32)

set(OIS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../OIS-1.2.0)

