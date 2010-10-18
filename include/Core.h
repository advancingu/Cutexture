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
	/** Responsible for setting up and shutting down all game subsystems. */
	class Core: public Ogre::Singleton<Core>
	{
	public:
		Core();
		virtual ~Core();

		void go();

		/** Shutdown the application framework. Only call this 
		 * after the game logic has been shutdown. 
		 * @see Game::initiateShutdown() */
		void shutdown();

		inline Ogre::Real getFrameUpdateRate()
		{
			return mFrameUpdateRate;
		}
		
	protected:
		
	private:
		bool mEndCoreLoop;

		// Owner of the following singletons
		OgreCore *mOgreCore;
		Game *mGame;
		InputManager* mInputManager; // Handle mouse and keyboard input
		Settings* mSettings; // Store and retrieve application settings

		/** Used to measure the time delta between two 
		 * iterations of the entire main loop. The time is 
		 * reset at the end of each loop.
		 * TODO Might have to be subclassed to provide more accurate 
		 * nanosecond timing.
		 * @see Chapter 7.5.6 Game Engine Architecture book
		 * @see man clock_gettime(3) for Linux */
		QTime mFrameTime;

		/** Stores how long it took for the previous game loop 
		 * iteration to execute, as a fraction of one second.
		 * Initialized to 1/30 (i.e. 30 FPS) before first frame is 
		 * rendered. */
		Ogre::Real mFrameUpdateRate;
	};
}
