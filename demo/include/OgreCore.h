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
#include "SceneManager.h"
#include "UiManager.h"

namespace Cutexture
{
	static const QString SETTINGS_CATEGORY_RENDERER_ENGINE = "Renderer Engine";
	static const QString SETTINGS_CATEGORY_RENDERER_PARAMS = "Renderer Parameters";
	
	static const QString SETTINGS_RENDERER_ENGINE_TYPE_KEY = "Type";
	static const QString SETTINGS_RENDERER_ENGINE_TYPE_OPENGL_VAL = "OpenGL Rendering Subsystem";
	static const QString SETTINGS_RENDERER_ENGINE_TYPE_D3D_VAL = "Direct3D9 Rendering Subsystem";
#if defined(Q_WS_X11)
	static const QString SETTINGS_RENDERER_ENGINE_TYPE_VAL = SETTINGS_RENDERER_ENGINE_TYPE_OPENGL_VAL;
#elif defined(Q_WS_WIN)
	static const QString SETTINGS_RENDERER_ENGINE_TYPE_VAL = SETTINGS_RENDERER_ENGINE_TYPE_D3D_VAL;
#endif
	
	
	// common values
	static const QString SETTINGS_RENDERER_FULLSCREEN_KEY = "Full Screen";
	static const QString SETTINGS_RENDERER_FULLSCREEN_VAL = "No";
	static const QString SETTINGS_RENDERER_VERTICAL_SYNC_KEY = "VSync";
	static const QString SETTINGS_RENDERER_VERTICAL_SYNC_VAL = "No";
	static const QString SETTINGS_RENDERER_SRGB_GAMMA_CONVERSION_KEY = "sRGB Gamma Conversion";
	static const QString SETTINGS_RENDERER_SRGB_GAMMA_CONVERSION_VAL = "No";
	//	static const QString SETTINGS_RENDERER_WINDOW_RESOLUTION_KEY = "Video Mode";

	// engine-specific values
#if defined(Q_WS_X11)
	//	static const QString SETTINGS_RENDERER_DISPLAY_FREQUENCY_KEY = "Display Frequency";
	//	static const QString SETTINGS_RENDERER_DISPLAY_FREQUENCY_VAL = "60 MHz";
	static const QString SETTINGS_RENDERER_FULLSCREEN_ANTIALIAS_KEY = "FSAA";
	static const QString SETTINGS_RENDERER_FULLSCREEN_ANTIALIAS_VAL = "0";
	static const QString SETTINGS_RENDERER_RENDER_TO_TEXTURE_MODE_KEY = "RTT Preferred Mode";
	static const QString SETTINGS_RENDERER_RENDER_TO_TEXTURE_MODE_VAL = "FBO";
	//	static const QString SETTINGS_RENDERER_WINDOW_RESOLUTION_VAL = "1280 x  720";
#elif defined(Q_WS_WIN)
	static const QString SETTINGS_RENDERER_NV_PERF_HUD_KEY = "Allow NVPerfHUD";
	static const QString SETTINGS_RENDERER_NV_PERF_HUD_VAL = "No";
	static const QString SETTINGS_RENDERER_ANTIALIAS_KEY = "Anti aliasing";
	static const QString SETTINGS_RENDERER_ANTIALIAS_VAL = "None";
	static const QString SETTINGS_RENDERER_FLOATINGPOINT_MODE_KEY = "Floating-point mode";
	static const QString SETTINGS_RENDERER_FLOATINGPOINT_MODE_VAL = "Fastest";
	//	static const QString SETTINGS_RENDERER_WINDOW_RESOLUTION_VAL = "1280 x  720 @ 32-bit colour";
#endif
	
	
	/** Responsible for setting up and shutting down the OGRE rendering 
	 * engine. */
	class OgreCore: public Ogre::Singleton<OgreCore>
	{
	public:
		OgreCore();
		virtual ~OgreCore();

		bool setupOgre();

		/** Creates the scene elements needed for the user interface and 
		 * then creates the user interface widgets. */
		void setupUserInterface();

		/** Tell Ogre to render one frame. */
		void renderFrame();

		Ogre::RenderWindow* const getOgreRenderWindow() const;

		/** Checks if the Ogre render window was resized and notifies the 
		 * UiManager if needed. */
		void processWindowEvents();

	protected:
		
	private:
		
		
		/** Manager for the User Interface. */
		UiManager* mUiManager;

		/** Pointer to Ogre's render window Ogre::RenderWindow. Owned by Ogre. */
		Ogre::RenderWindow *mOgreRenderWindow;

		/** Pointer to Ogre's top management class. It owns all Ogre-created
		 * object instances (Ex: sceneNode->createChildNode()). */
		Ogre::Root* mOgreRoot;

		/** SceneManager for the Ogre scene. Owned by us. */
		SceneManager *mSceneManager;

		/** ViewManager for the render window. Owned by us */
		ViewManager* mViewManager;

		/** Width of Ogre's renderer window at the last query. */
		unsigned int mRenderWindowWidth;
		/** Height of Ogre's renderer window at the last query. */
		unsigned int mRenderWindowHeight;

		/** Load resources (e.g. meshes, sounds, xml files).
		 @param resourcePath Path to directory where resources.cfg is located.
		 */
		void setupResources(const QString &resourcePath);

		bool setupRenderer();
	};
}
