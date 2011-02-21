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

#include "Game.h"

#include "Core.h"
#include "OgreCore.h"
#include "Exception.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ViewManager.h"
#include "Constants.h"
#include "Enums.h"

using namespace Cutexture::Constants;
using namespace Cutexture::Enums;

template<> Cutexture::Game* Ogre::Singleton<Cutexture::Game>::ms_Singleton = 0;

namespace Cutexture
{
	Game::Game()
		: mInputManager(0)
	{
		if (OgreCore::getSingletonPtr() == NULL)
		{
			EXCEPTION("OgreCore is not initialized.", "Game::Game()");
		}
	}

Game::~Game()
{
}

void Game::setInputManager(InputManager *aInputManager)
{
	if (mInputManager && aInputManager != mInputManager)
	{
		disconnect(mInputManager, 0, this, 0);
	}
	
	if (aInputManager == 0 || !aInputManager->isInitialized())
	{
		EXCEPTION("InputManager not initialized.", "Game::setInputManager(InputManager *)");
	}
	
	mInputManager = aInputManager;
	
	if (mInputManager)
	{
		connect(mInputManager, SIGNAL(keyPressEvent(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
	}
}

void Game::applyGameLogic()
{
	assert(mInputManager);
	
	// move the camera x units per second
	const float camMovSpeed = 50;
	// rotate the camera x degrees per second
	const Ogre::Degree camRotSpeed(30);
	// fraction of a second since last update
	const Ogre::Real frameFraction = Core::getSingletonPtr()->getFrameUpdateRate();

	Ogre::Camera* mainCamera = ViewManager::getSingletonPtr()->getPrimaryViewport()->getCamera();

	QPoint mouseDelta = mInputManager->getRelativeMouseMovement();

	Ogre::Degree relativePitch(0);
	Ogre::Degree relativeYaw(0);

	// ***** Handle Movement *****
	Enums::Movements movementsActive = mInputManager->getMovementsActive();

	Ogre::Vector3 vecMovement(0.0f);

	if (movementsActive & Enums::Forward)
	{
		vecMovement += Ogre::Vector3(0.0, 0.0, -camMovSpeed);
	}
	else if (movementsActive & Enums::Backward)
	{
		vecMovement += Ogre::Vector3(0.0, 0.0, camMovSpeed);
	}

	if (movementsActive & Enums::StrafeLeft)
	{
		vecMovement += Ogre::Vector3(-camMovSpeed, 0.0, 0.0);
	}
	else if (movementsActive & Enums::StrafeRight)
	{
		vecMovement += Ogre::Vector3(camMovSpeed, 0.0, 0.0);
	}

	if (movementsActive != 0)
	{
		vecMovement *= frameFraction;
		mainCamera->moveRelative(vecMovement);
	}

	// ***** Handle Rotations *****
	// Keyboard rotations
	if (movementsActive & Enums::YawCounterClock)
	{
		relativeYaw += camRotSpeed * frameFraction;
	}
	else if (movementsActive & Enums::YawClock)
	{
		relativeYaw += -camRotSpeed * frameFraction;
	}

	if (movementsActive & Enums::PitchUp)
	{
		relativePitch += camRotSpeed * frameFraction;
	}
	else if (movementsActive & Enums::PitchDown)
	{
		relativePitch += -camRotSpeed * frameFraction;
	}

	// Mouse rotations                
	if (mInputManager->getMouseButtonsPressed() & Qt::RightButton)
	{
		// The frame rate doesn't need to be compensated for here because this is based on the mouse delta
		// which is constant in time so naturally variable with the frame rate.
		relativeYaw += Ogre::Degree(Ogre::Real(mouseDelta.x()) / Ogre::Real(10));
		relativePitch += Ogre::Degree(Ogre::Real(mouseDelta.y()) / Ogre::Real(10));
	}

	// Rotate camera based on movements
	if ((relativeYaw != Ogre::Degree(0)) || (relativePitch != Ogre::Degree(0)))
	{
		mainCamera->yaw(relativeYaw);
		mainCamera->pitch(relativePitch);
	}
}

void Game::initiateShutdown()
{
	// shut down the game logic here

	// when done, shut down the main loop
	Core::getSingletonPtr()->shutdown();
}

void Game::keyPressEvent(QKeyEvent *event)
{
	if (!event || event->isAccepted())
	{
		return;
	}

	if (event->key() == Qt::Key_F2)
	{
		ViewManager::getSingletonPtr()->setIsWireframe(
				! ViewManager::getSingletonPtr()->getIsWireframe()
		);
	}
	else if (event->key() == Qt::Key_Q)
	{
		initiateShutdown();
	}
}
}
