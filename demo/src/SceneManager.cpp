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

#include "SceneManager.h"
#include "ViewManager.h"
#include "Exception.h"
#include "Enums.h"
#include "OgreCore.h"
#include "Constants.h"
#include "DemoConstants.h"

using namespace Ogre;

template<> Cutexture::SceneManager* Ogre::Singleton<Cutexture::SceneManager>::ms_Singleton = 0;

namespace Cutexture
{
	SceneManager::SceneManager()
	{
		// Create the Ogre SceneManager, in this case a generic one. Ogre::Root will hold onto the 
		// reference so we don't need to keep it around.
		Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC,
				DemoConstants::SCENE_MANAGER_NAME);
	}
	
	SceneManager::~SceneManager()
	{
		
	}
	
	void SceneManager::setupDefaultScene()
	{
		Ogre::SceneManager *sceneManager = Ogre::Root::getSingletonPtr()->getSceneManager(
				DemoConstants::SCENE_MANAGER_NAME);
		
		
		// Create the camera
		Ogre::Camera* viewportCamera = sceneManager->createCamera("MainCamera");
		ViewManager::getSingletonPtr()->createDefaultView(viewportCamera);
		
		viewportCamera->setPosition(Vector3(0, 100, -100));
		
		sceneManager->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
		
		Light* l = sceneManager->createLight("MainLight");
		l->setType(Light::LT_DIRECTIONAL);
		l->setDiffuseColour(0.3, 0.3, 0.3);
		l->setDirection(-Vector3::UNIT_Y);
		
		Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
		Ogre::MeshManager::getSingleton().createPlane("ground",
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 100, 100, 20, 20,
				true, 1, 5, 5, Ogre::Vector3::UNIT_X);
		
		Ogre::Entity* ent = sceneManager->createEntity("GroundEntity", "ground");
		Ogre::SceneNode *childNode = sceneManager->getRootSceneNode()->createChildSceneNode();
		childNode->attachObject(ent);
		ViewManager::getSingletonPtr()->getPrimaryViewport()->getCamera()->lookAt(
				childNode->getPosition());
	}
	
	void SceneManager::setupUserInterfaceElements()
	{
		Ogre::SceneManager *sceneManager = Ogre::Root::getSingletonPtr()->getSceneManager(
				DemoConstants::SCENE_MANAGER_NAME);
		
		Ogre::Rectangle2D *miniScreen = new Ogre::Rectangle2D(true);
		miniScreen->setCorners(-1.0, 1.0, 1.0, -1.0);
		miniScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0 * Ogre::Vector3::UNIT_SCALE,
				100000.0 * Ogre::Vector3::UNIT_SCALE));
		miniScreen->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
		
		Ogre::SceneNode *miniScreenNode = sceneManager->getRootSceneNode()->createChildSceneNode(
				"MiniScreenNode");
		miniScreenNode->attachObject(miniScreen);
		
		
		// create a material
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("RttMat",
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::Technique *technique = mat->createTechnique();
		technique->createPass();
		mat->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		mat->getTechnique(0)->getPass(0)->setSceneBlending(SBT_TRANSPARENT_ALPHA);
		//		mat->getTechnique(0)->getPass(0)->setDepthBias(1);
		

		// assign to mini screen
		miniScreen->setMaterial("RttMat");
		
		Ogre::TexturePtr txtr = Ogre::TextureManager::getSingleton().createManual(
				Constants::UI_TEXTURE_NAME, "General", Ogre::TEX_TYPE_2D, 512, 512, 0,
				Ogre::PF_A8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY);
		mat->getTechnique(0)->getPass(0)->createTextureUnitState(Constants::UI_TEXTURE_NAME);
		txtr->load();
	}
}
