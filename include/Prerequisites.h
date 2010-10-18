/** This file is part of Cutexture.
 
 Copyright (c) 2010 Markus Weiland, Kevin Lang

 Portions of this code may be under copyright of authors listed in AUTHORS.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#pragma once

// Shut up 3rd-party warnings
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#pragma warning(push, 0)
#endif

#include <OgreAxisAlignedBox.h>
#include <OgreCamera.h>
#include <OgreCommon.h>
#include <OgreCompositorManager.h>
#include <OgreConfigFile.h>
#include <OgreConfigOptionMap.h>
#include <OgreDataStream.h>
#include <OgreEntity.h>
#include <OgreException.h>
#include <OgreHardwareBuffer.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreImage.h>
#include <OgreLogManager.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreMatrix3.h>
#include <OgreMeshManager.h>
#include <OgreMovableObject.h>
#include <OgrePixelFormat.h>
#include <OgreQuaternion.h>
#include <OgreRenderable.h>
#include <OgreRenderOperation.h>
#include <OgreRenderQueue.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include <OgreSharedPtr.h>
#include <OgreSingleton.h>
#include <OgreSphere.h>
#include <OgreString.h>
#include <OgreStringConverter.h>
#include <OgreTechnique.h>
#include <OgreTexture.h>
#include <OgreVector3.h>
#include <OgreViewport.h>
#include <OgreWindowEventUtilities.h>

/*#include <FreeImage.h>*/

#include <OIS/OISInputManager.h>
#include <OIS/OISMouse.h>
#include <OIS/OISKeyboard.h>

#include <QtCore/QtCore>
#include <QtGui/QtGui>

#include <QtUiTools/QtUiTools>

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#pragma warning(pop)
#endif

// Disable warning about "assignment operator could not be generated."
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#pragma warning(disable: 4512)
#endif

namespace Cutexture
{
	class Core;
	class Exception;
	class InputManager;
	class OgreCore;
	class SceneManager;
	class SleepThread;
	class ViewManager;
	class Game;
	class Settings;
}

