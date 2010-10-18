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

#include "ViewManager.h"

using namespace Ogre;

template<> Cutexture::ViewManager* Ogre::Singleton<Cutexture::ViewManager>::ms_Singleton = 0;

namespace Cutexture
{
	ViewManager::ViewManager(Ogre::RenderWindow& aRenderWindow) :
		mIsWireframe(false), mRenderWindow(&aRenderWindow), mPrimaryViewport(NULL)
	{
		
	}
	
	ViewManager::~ViewManager()
	{
		
	}
	
	void ViewManager::setIsWireframe(const bool& aIsWireframe)
	{
		Ogre::Camera* pCamera = mPrimaryViewport->getCamera();
		
		if (aIsWireframe)
		{
			pCamera->setPolygonMode(PM_WIREFRAME);
			pCamera->getViewport()->setClearEveryFrame(true, FBT_DEPTH | FBT_COLOUR); //Wireframe needs the screen cleared every frame
		}
		else
		{
			pCamera->setPolygonMode(PM_SOLID);
			//TODO: setClearEveryFrame(false);
			//pCamera->getViewport()->setClearEveryFrame(false);
			pCamera->getViewport()->setClearEveryFrame(true, FBT_DEPTH | FBT_COLOUR); //Wireframe needs the screen cleared every frame			
		}
		
		mIsWireframe = aIsWireframe;
	}
	
	void ViewManager::createDefaultView(Camera * const aCamera)
	{
		// Shouldn't be called more than once
		assert(mPrimaryViewport == NULL);
		
		// Set up viewport
		mPrimaryViewport = mRenderWindow->addViewport(aCamera);
		mPrimaryViewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
		
		// Alter the camera aspect ratio to match the viewport
		aCamera->setAspectRatio(Ogre::Real(mPrimaryViewport->getActualWidth()) / Ogre::Real(
				mPrimaryViewport->getActualHeight()));
		
		// Set auto aspect ratio
		aCamera->setAutoAspectRatio(true);
		
		// 1 = 1m, set far clip distance to 500 km.
		aCamera->setFarClipDistance(500000.0);
		
		
		// Near clip distance of 1m
		aCamera->setNearClipDistance(Ogre::Real(1));
	}
}
