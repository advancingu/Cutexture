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

#include "OgreCore.h"
#include "UiManager.h"
#include "Exception.h"
#include "Settings.h"
#include "Constants.h"
#include "InputManager.h"
#include "ViewManager.h"

template<> Cutexture::OgreCore* Ogre::Singleton<Cutexture::OgreCore>::ms_Singleton = 0;

namespace Cutexture
{
	OgreCore::OgreCore() :
		mUiManager(NULL), mOgreRenderWindow(NULL), mOgreRoot(NULL), mViewManager(NULL),
				mSceneManager(NULL), mRenderWindowWidth(0), mRenderWindowHeight(0)
	{
		// set default configuration values for this component
		QHash < QString, QVariant > engineType;
		engineType.insert(SETTINGS_RENDERER_ENGINE_TYPE_KEY, SETTINGS_RENDERER_ENGINE_TYPE_VAL);
		Settings::getSingletonPtr()->setDefaultValues(SETTINGS_CATEGORY_RENDERER_ENGINE, engineType);
		
		QHash < QString, QVariant > engineParams;
		
		engineParams.insert(SETTINGS_RENDERER_FULLSCREEN_KEY, SETTINGS_RENDERER_FULLSCREEN_VAL);
		engineParams.insert(SETTINGS_RENDERER_VERTICAL_SYNC_KEY,
				SETTINGS_RENDERER_VERTICAL_SYNC_VAL);
		engineParams.insert(SETTINGS_RENDERER_SRGB_GAMMA_CONVERSION_KEY,
				SETTINGS_RENDERER_SRGB_GAMMA_CONVERSION_VAL);
#if defined(Q_WS_X11)
		engineParams.insert(SETTINGS_RENDERER_FULLSCREEN_ANTIALIAS_KEY, SETTINGS_RENDERER_FULLSCREEN_ANTIALIAS_VAL);
		engineParams.insert(SETTINGS_RENDERER_RENDER_TO_TEXTURE_MODE_KEY, SETTINGS_RENDERER_RENDER_TO_TEXTURE_MODE_VAL);
#elif defined(Q_WS_WIN)
		engineParams.insert(SETTINGS_RENDERER_NV_PERF_HUD_KEY, SETTINGS_RENDERER_NV_PERF_HUD_VAL);
		engineParams.insert(SETTINGS_RENDERER_ANTIALIAS_KEY, SETTINGS_RENDERER_ANTIALIAS_VAL);
		engineParams.insert(SETTINGS_RENDERER_FLOATINGPOINT_MODE_KEY, SETTINGS_RENDERER_FLOATINGPOINT_MODE_VAL);
#endif
		Settings::getSingletonPtr()->setDefaultValues(SETTINGS_CATEGORY_RENDERER_PARAMS,
				engineParams);
	}
	
	OgreCore::~OgreCore()
	{
		delete mSceneManager;
		delete mViewManager;
		delete mUiManager;
		
		
		// deleting Ogre::Root will automatically delete all other Ogre-managed objects
		delete mOgreRoot;
	}
	
	bool OgreCore::setupOgre()
	{
		QString resourcePath = QCoreApplication::instance()->applicationDirPath()
				+ QDir::separator();
		
		QString pluginsPath;
#ifndef OGRE_STATIC_LIB
		// only use plugins.cfg if not static
		pluginsPath = resourcePath + "plugins.cfg";
#endif
		mOgreRoot = new Ogre::Root();
		
		setupResources(resourcePath);
		
		if (!setupRenderer())
		{
			return false;
		}

		// initialize Ogre and create a render window
		mOgreRenderWindow = mOgreRoot->initialise(true);
		
		
		// create the UI widget overlay manager
		mUiManager = new UiManager();
		
		mViewManager = new ViewManager(*mOgreRenderWindow);
		mSceneManager = new SceneManager();
		
		return true;
	}
	
	void OgreCore::setupUserInterface()
	{
		if (!mSceneManager)
		{
			EXCEPTION("Cannot setup user interface. No mSceneManager.", "OgreCore::setupUserInterface()");
		}
		if (!mUiManager)
		{
			EXCEPTION("Cannot setup user interface. No mOgreWidget.", "OgreCore::setupUserInterface()");
		}

		mSceneManager->setupUserInterfaceElements();
		mUiManager->setUiTexture(Ogre::TextureManager::getSingletonPtr()->getByName(Constants::UI_TEXTURE_NAME));
	}
	
	void OgreCore::renderFrame()
	{
		Ogre::Root::getSingleton().renderOneFrame();
	}
	
	Ogre::RenderWindow* const OgreCore::getOgreRenderWindow() const
	{
		if (mOgreRenderWindow == NULL)
		{
			EXCEPTION("mOgreRenderWindow is null.", "OgreCore::getOgreRenderWindow()");
		}
		
		return mOgreRenderWindow;
	}
	
	void OgreCore::processWindowEvents()
	{
		unsigned int currWidth = 0;
		unsigned int currHeight = 0;
		unsigned int unused = 0;
		int unused2 = 0;
		getOgreRenderWindow()->getMetrics(currWidth, currHeight, unused, unused2, unused2);
		
		
		// if the window size has changed
		if (currWidth != mRenderWindowWidth || currHeight != mRenderWindowHeight)
		{
			QResizeEvent resizeEvent(QSize(currWidth, currHeight), QSize(mRenderWindowWidth,
					mRenderWindowHeight));
			
			mUiManager->resizeEvent(&resizeEvent);
			InputManager::getSingletonPtr()->resizeEvent(&resizeEvent);
			
			mRenderWindowWidth = currWidth;
			mRenderWindowHeight = currHeight;
		}
	}
	
	void OgreCore::setupResources(const QString &resourcePath)
	{
		// Load resource paths from config file
		Ogre::ConfigFile cf;
		cf.load((resourcePath + "resources.cfg").toStdString());
		
		
		// Go through all sections & settings in the file
		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
		
		QString secName, typeName, archName;
		
		while (seci.hasMoreElements())
		{
			secName = QString::fromStdString(seci.peekNextKey());
			Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = QString::fromStdString(i->first);
				archName = QString::fromStdString(i->second);
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
				// OS X does not set the working directory relative to the app,
				// In order to make things portable on OS X we need to provide
				// the loading with it's own bundle path location
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(QString(
						qApp->applicationDirPath() + "/" + archName).toStdString(),
						typeName.toStdString(), secName.toStdString());
#else
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
						archName.toStdString(), typeName.toStdString(), secName.toStdString());
#endif
			}
		}
	}
	
	bool OgreCore::setupRenderer()
	{
		assert(Ogre::Root::getSingletonPtr());
		
		QString renderSystemSetting = Settings::getSingletonPtr()->getValue(
				SETTINGS_CATEGORY_RENDERER_ENGINE, SETTINGS_RENDERER_ENGINE_TYPE_KEY).toString();
		
		Ogre::RenderSystem* targetRenderSystem = mOgreRoot->getRenderSystemByName(
				renderSystemSetting.toStdString());
		
		if (!targetRenderSystem)
		{
			return false;
		}

		QHash < QString, QVariant > targetRenderSettings
				= Settings::getSingletonPtr()->getCategoryValues(SETTINGS_CATEGORY_RENDERER_PARAMS);
		foreach(QString key, targetRenderSettings.keys())
		{
			targetRenderSystem->setConfigOption(key.toStdString(), targetRenderSettings.value(key).toString().toStdString());
		}

		// Use a WBuffer if possible. Better for deep scenes.
		//targetRenderSystem->setWBufferEnabled(true);

		mOgreRoot->setRenderSystem(targetRenderSystem);
		
		return true;
	}
}
