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

#include "Core.h"
#include "Exception.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
	QScopedPointer<Cutexture::Core> corePtr;
	
	try
	{
		QApplication app(argc, argv);
		
		//Run the game
		corePtr.reset(new Cutexture::Core());
		// blocking call; we advance Qt's event loop in this method; no need to call app.exec()
		corePtr->go();
		
		Ogre::LogManager::getSingleton().logMessage("Cutexture finished running, exiting...");
		
		return 0;
	}
	catch (Ogre::Exception &e)
	{
		//Rendering error
		Ogre::String fullDesc = "OGRE exception at \"" + e.getSource() + "\" in \"" + e.getFile()
				+ "\": " + e.getDescription();
		Ogre::LogManager::getSingleton().logMessage("Error: " + fullDesc);
		
		return 1;
	}
	catch (Cutexture::Exception &e)
	{
		//Game engine error
		Ogre::LogManager::getSingleton().logMessage("Error: " + e.getFullDescription());
		
		return 1;
	}
}

