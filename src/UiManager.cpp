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

#include "UiManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Constants.h"
#include "TextureMath.h"
#include "Exception.h"
#include "OgreCore.h"

using namespace Cutexture::Utility;

template<> Cutexture::UiManager* Ogre::Singleton<Cutexture::UiManager>::ms_Singleton = 0;

namespace Cutexture
{
	
	UiManager::UiManager() :
		mWidgetBuffer(NULL), mWidgetScene(NULL), mWidgetView(NULL), mTopLevelWidget(NULL),
				mFocusedWidget(NULL)
	{
		mWidgetBuffer = new QImage(QSize(512, 512), QImage::Format_ARGB32);
		
		connect(this, SIGNAL(uiRepaintRequired()), this, SLOT(updateUiTexture()),
				Qt::QueuedConnection);
		
		mWidgetScene = new QGraphicsScene(this);
		mWidgetView = new QGraphicsView(mWidgetScene);
		mWidgetView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
		
		
		// for debugging, show Qt's window with
		// mWidgetView->show();

		// We need to manually tell the scene that a visible view is watching.
		// A QGraphicsView doesn't do that when it is not visible as 
		// a widget on screen.
		QEvent wsce(QEvent::WindowActivate);
		QApplication::sendEvent(mWidgetScene, &wsce);
		
		connect(mWidgetScene, SIGNAL(changed(const QList<QRectF> &)), this, SLOT(updateUiTexture()));

		connect(InputManager::getSingletonPtr(), SIGNAL(mousePressEvent(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
		connect(InputManager::getSingletonPtr(), SIGNAL(mouseReleaseEvent(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
		connect(InputManager::getSingletonPtr(), SIGNAL(mouseMoveEvent(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));

		connect(InputManager::getSingletonPtr(), SIGNAL(keyPressEvent(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
		connect(InputManager::getSingletonPtr(), SIGNAL(keyReleaseEvent(QKeyEvent*)), this, SLOT(keyReleaseEvent(QKeyEvent*)));
	}

	UiManager::~UiManager()
	{
		delete mWidgetBuffer;
	}
	
	void UiManager::setupUserInterfaceWidgets()
	{
		if (!mWidgetScene)
		{
			EXCEPTION("Cannot create widgets. mWidgetScene uninitialized.", "UiManager::setupUserInterfaceWidgets()");
		}
	
		mTopLevelWidget = loadUiFile("game.ui");
		mTopLevelWidget->setAttribute(Qt::WA_TranslucentBackground);
	
		mWidgetScene->addWidget(mTopLevelWidget);
	}
	
	void UiManager::resizeEvent(QResizeEvent *event)
	{
		// get the smallest power of two dimension that is at least as large as the new window size
		Ogre::uint newTexWidth = nextHigherPowerOfTwo(event->size().width());
		Ogre::uint newTexHeight = nextHigherPowerOfTwo(event->size().height());
	
		mTopLevelWidget->resize(event->size());
	
		mWidgetView->setGeometry(QRect(0, 0, newTexWidth, newTexHeight));
	
		Ogre::TexturePtr txtr = Ogre::TextureManager::getSingletonPtr()->getByName(Constants::UI_TEXTURE_NAME);
		if (!txtr.isNull())
		{
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("RttMat");
	
			// remove the old texture
			txtr->unload();
			mat->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
			Ogre::TextureManager::getSingleton().remove(Constants::UI_TEXTURE_NAME);
	
			txtr = Ogre::TextureManager::getSingleton().createManual(
					Constants::UI_TEXTURE_NAME, "General", Ogre::TEX_TYPE_2D, newTexWidth, newTexHeight, 0, Ogre::PF_A8R8G8B8,
					Ogre::TU_DYNAMIC_WRITE_ONLY);
	
			// add the new texture
			Ogre::TextureUnitState* txtrUstate = mat->getTechnique(0)->getPass(0)->createTextureUnitState(Constants::UI_TEXTURE_NAME);
	
			// adjust it to stay aligned and scaled to the window
			Ogre::Real txtrUScale = (Ogre::Real)newTexWidth / event->size().width();
			Ogre::Real txtrVScale = (Ogre::Real)newTexHeight / event->size().height();
			txtrUstate->setTextureScale(txtrUScale, txtrVScale);
			txtrUstate->setTextureScroll((1 / txtrUScale) / 2 - 0.5, (1 / txtrVScale) / 2 - 0.5);
		}
	
		delete mWidgetBuffer;
		mWidgetBuffer = new QImage(QSize(newTexWidth, newTexHeight), QImage::Format_ARGB32);
	}
	
	void UiManager::mousePressEvent(QMouseEvent *event)
	{
		QWidget *pressedWidget = NULL;
	
		// get the clicked item through the view (respects view and item transformations)
		QGraphicsItem* itemAt = mWidgetView->itemAt(event->pos());
		if ((itemAt) && (itemAt->isWidget()))
		{
			QGraphicsProxyWidget *proxyWidget = qgraphicsitem_cast<QGraphicsProxyWidget *>(itemAt);
			if (proxyWidget)
			{
				QWidget *embeddedWidget = proxyWidget->widget();
	
				// if the widget has children, use them, otherwise use the widget directly
				if (embeddedWidget->children().size() > 0)
				{
					QPoint widgetPoint = proxyWidget->mapFromScene(mWidgetView->mapToScene(event->pos())).toPoint();
					pressedWidget = embeddedWidget->childAt(widgetPoint);
				}
				else
				{
					pressedWidget = embeddedWidget;
				}
			}
		}
	
		// if there was a focused widget and there is none or a different one now, defocus
		if (mFocusedWidget && (!pressedWidget || pressedWidget != mFocusedWidget))
		{
			QEvent foe(QEvent::FocusOut);
			QApplication::sendEvent(mFocusedWidget, &foe);
			mFocusedWidget = NULL;
		}
	
		// set the new focus
		if (pressedWidget)
		{
			QEvent fie(QEvent::FocusIn);
			QApplication::sendEvent(pressedWidget, &fie);
			pressedWidget->setFocus(Qt::MouseFocusReason);
			mFocusedWidget = pressedWidget;
		}
	
		QApplication::sendEvent(mWidgetView->viewport(), event);
	}
	
	void UiManager::mouseReleaseEvent(QMouseEvent *event)
	{
		QApplication::sendEvent(mWidgetView->viewport(), event);
	}
	
	void UiManager::mouseMoveEvent(QMouseEvent *event)
	{
		QApplication::sendEvent(mWidgetView->viewport(), event);
	}
	
	void UiManager::keyPressEvent(QKeyEvent *event)
	{
		QApplication::sendEvent(mWidgetView->viewport(), event);
	}
	
	void UiManager::keyReleaseEvent(QKeyEvent *event)
	{
		QApplication::sendEvent(mWidgetView->viewport(), event);
	}
	
	QWidget* UiManager::loadUiFile(const QString &aUiFile, QWidget *aParent)
	{
		QUiLoader uiLoader;
	
		QFile file(aUiFile);
		assert(file.exists());
		file.open(QFile::ReadOnly);
	
		QWidget *generatedWidget = uiLoader.load(&file, aParent);
	
		file.close();
	
		return generatedWidget;
	}
	
	void UiManager::updateUiTexture()
	{
		// clear the previous buffer
		mWidgetBuffer->fill(0);
	
		// update the Ogre texture
		Ogre::TexturePtr txtr = Ogre::TextureManager::getSingletonPtr()->getByName(Constants::UI_TEXTURE_NAME);
		if (!txtr.isNull())
		{
			// render into buffer
			QPainter painter(mWidgetBuffer);
			mWidgetView->render(&painter, QRect(QPoint(0, 0), mWidgetView->size()), QRect(QPoint(0, 0), mWidgetView->size()));
	
			OgreCore::getSingletonPtr()->copyToTexture(mWidgetBuffer, txtr);
		}
	}
}
