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

#include "Prerequisites.h"

namespace Cutexture
{
	/** Manages the views on a render window
	 */
	class ViewManager: public Ogre::Singleton<ViewManager>
	{
	public:
		ViewManager(Ogre::RenderWindow& aRenderWindow);
		virtual ~ViewManager();

		/** Returns the current wireframe status
		 */
		bool getIsWireframe() const
		{
			return mIsWireframe;
		}
		
		
		/** Sets the wireframe status for all views attached 
		 to the render window.
		 */
		void setIsWireframe(const bool &aIsWireframe);

		inline const Ogre::Viewport* const getPrimaryViewport() const
		{
			return mPrimaryViewport;
		}
		
		
		/** Creates the default viewport on the render window 
		 */
		void createDefaultView(Ogre::Camera * const aCamera);
	protected:
		bool mIsWireframe;
		Ogre::RenderWindow* const mRenderWindow;
		Ogre::Viewport* mPrimaryViewport;
	};
}
