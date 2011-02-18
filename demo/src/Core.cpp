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
#include "Game.h"
#include "OgreCore.h"
#include "SleepThread.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Exception.h"
#include "Settings.h"
#include <iostream>

using namespace std;

template<> Cutexture::Core* Ogre::Singleton<Cutexture::Core>::ms_Singleton = 0;

namespace Cutexture
{
	Core::Core() :
		mEndCoreLoop(false), mOgreCore(NULL), mGame(NULL), mInputManager(NULL), mSettings(NULL),
				mFrameUpdateRate(0)
	{
		
	}
	
	Core::~Core()
	{
		delete mGame;
		delete mOgreCore;
		
		
		// The input manager should be deleted last in case events are still being fired.
		delete mInputManager;
		
		delete mSettings;
	}
	
	void Core::go()
	{
		mSettings = new Settings();
		mInputManager = new InputManager();
		
		mOgreCore = new OgreCore();
		bool setupResult = mOgreCore->setupOgre();
		
		if (!setupResult)
		{
			EXCEPTION("Failed to setup OgreCore.", "Core::go()");
		}

		mInputManager->initialize(mOgreCore->getOgreRenderWindow());
		
		mOgreCore->setupUserInterface();
		
		QWidget *ui = loadUiFile("game.ui");
//		QWidget *ui = new QWidget();
		ui->setAttribute(Qt::WA_TranslucentBackground);
		
//		QWebView *web  = new QWebView();
//		web->load(QUrl("http://mrdoob.com/projects/chromeexperiments/ball_pool/"));
//		ui->layout()->addWidget(web);

		UiManager::getSingletonPtr()->setActiveWidget(ui);
		UiManager::getSingletonPtr()->setInputManager(mInputManager);
		
		QCoreApplication::instance()->processEvents();
		
		SceneManager::getSingletonPtr()->setupDefaultScene();
		
		mGame = new Game();
		
		Ogre::WindowEventUtilities::messagePump();
		
		
		// launch other threads here
		

		mFrameUpdateRate = Ogre::Real(1) / Ogre::Real(30);
		mFrameTime.start();
		
		
		// Main game loop
		while (!mEndCoreLoop)
		{
			QCoreApplication::instance()->processEvents();
			Ogre::WindowEventUtilities::messagePump();
			
			mOgreCore->processWindowEvents();
			
			
			// grab the mouse and keyboard state
			mInputManager->update();
			
			if (mEndCoreLoop)
			{
				break;
			}

			mGame->update();
			
			UiManager *uiMan = UiManager::getSingletonPtr();
			if (uiMan->isUiDirty())
			{
				uiMan->updateUiTexture();
				uiMan->setUiDirty(false);
			}
			
			mOgreCore->renderFrame();
			
			
			// restart game loop timer and update frame duration
			int duration = mFrameTime.restart();
			// TODO compute an average over multiple frames to reduce the effect of spikes
			// @see Chapter 7.5.2.3 Game Engine Architecture book
			// NOTE: For input, the actual frame time delta should be used and not an average.
			mFrameUpdateRate = Ogre::Real(duration) / Ogre::Real(1000);
			
			SleepThread::msleep(1);
		}
		
	}
	
	void Core::shutdown()
	{
		mEndCoreLoop = true;
	}

	QWidget* Core::loadUiFile(const QString &aUiFile, QWidget *aParent)
	{
		QUiLoader uiLoader;
	
		QFile file(aUiFile);
		assert(file.exists());
		file.open(QFile::ReadOnly);
	
		QWidget *generatedWidget = uiLoader.load(&file, aParent);
	
		file.close();
	
		return generatedWidget;
	}
}
