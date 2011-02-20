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
#include "Constants.h"
#include "TextureMath.h"
#include "Exception.h"

using namespace Cutexture::Utility;

namespace Cutexture
{
	
	UiManager::UiManager() :
		mWidgetScene(NULL), mWidgetView(NULL), mTopLevelWidget(NULL),
				mFocusedWidget(NULL), mUiDirty(false),
				mInputManager(NULL)
	{
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
		
		connect(mWidgetScene, SIGNAL(changed(const QList<QRectF> &)), this, SLOT(setUiDirty()));
	}

	UiManager::~UiManager()
	{
		QEvent wsce(QEvent::WindowDeactivate);
		QApplication::sendEvent(mWidgetScene, &wsce);
		
		// Note: For ~QGraphicsScene to be able to run, qApp must still be valid.
	}
	
	void UiManager::setActiveWidget(QWidget *aWidget)
	{
		assert(mWidgetScene);
		
		if (mTopLevelWidget && mTopLevelWidget != aWidget)
		{
			if (mFocusedWidget)
			{
				QEvent foe(QEvent::FocusOut);
				QApplication::sendEvent(mFocusedWidget, &foe);
				mFocusedWidget = NULL;
			}

			mWidgetScene->clear();
			mTopLevelWidget = NULL;
		}
	
		mWidgetScene->addWidget(aWidget);
		mTopLevelWidget = aWidget;
	}
	
	void UiManager::setInputManager(InputManager *aInputManager)
	{
		if (mInputManager && aInputManager != mInputManager)
		{
			disconnect(mInputManager, 0, this, 0);
		}
		
		if (aInputManager == 0 || !aInputManager->isInitialized())
		{
			EXCEPTION("InputManager not initialized.", "UiManager::setInputManager(InputManager *)");
		}
		
		mInputManager = aInputManager;
		
		if (mInputManager)
		{
			connect(mInputManager, SIGNAL(mousePressEvent(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
			connect(mInputManager, SIGNAL(mouseReleaseEvent(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
			connect(mInputManager, SIGNAL(mouseMoveEvent(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));

			connect(mInputManager, SIGNAL(keyPressEvent(QKeyEvent*)), this, SLOT(keyPressEvent(QKeyEvent*)));
			connect(mInputManager, SIGNAL(keyReleaseEvent(QKeyEvent*)), this, SLOT(keyReleaseEvent(QKeyEvent*)));			
		}
	}
	
	void UiManager::resizeTexture(const QSize &aSize, const Ogre::MaterialPtr &aMaterial, const Ogre::TexturePtr &aTexture)
	{
		assert(!aMaterial.isNull());
		assert(!aTexture.isNull());
		
		// get the smallest power of two dimension that is at least as large as the new UI size
		Ogre::uint newTexWidth = nextHigherPowerOfTwo(aSize.width());
		Ogre::uint newTexHeight = nextHigherPowerOfTwo(aSize.height());
	
		if (!aTexture.isNull())
		{
			std::string txtrName = aTexture->getName();
			
			// remove the old texture
			aTexture->unload();
			aMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
			Ogre::TextureManager::getSingleton().remove(aTexture->getHandle());
	
			Ogre::TexturePtr newTxtr = Ogre::TextureManager::getSingleton().createManual(
					txtrName, "General", Ogre::TEX_TYPE_2D, newTexWidth, newTexHeight, 0, Ogre::PF_A8R8G8B8,
					Ogre::TU_DYNAMIC_WRITE_ONLY);
	
			// add the new texture
			Ogre::TextureUnitState* txtrUstate = aMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(txtrName);
	
			// adjust it to stay aligned and scaled to the window
			Ogre::Real txtrUScale = (Ogre::Real)newTexWidth / aSize.width();
			Ogre::Real txtrVScale = (Ogre::Real)newTexHeight / aSize.height();
			txtrUstate->setTextureScale(txtrUScale, txtrVScale);
			txtrUstate->setTextureScroll((1 / txtrUScale) / 2 - 0.5, (1 / txtrVScale) / 2 - 0.5);
		}
	}
	
	void UiManager::resizeUi(QResizeEvent *aEvent)
	{
		if (mTopLevelWidget)
		{
			mTopLevelWidget->resize(aEvent->size());
		}
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
			mTopLevelWidget->setFocus();
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
	
	void UiManager::setUiDirty(bool aDirty)
	{
		mUiDirty = aDirty;
	}
	
	bool UiManager::isViewSizeMatching(const Ogre::TexturePtr &aTexture) const
	{
		assert(!aTexture.isNull());
		
		return (aTexture->getWidth() == mWidgetView->width() && aTexture->getHeight() == mWidgetView->height());
	}
	
	void UiManager::setViewSize(const Ogre::TexturePtr &aTexture)
	{
		// make sure that the view size matches the texture size
		if (!aTexture.isNull() && !isViewSizeMatching(aTexture))
		{
			mWidgetView->setGeometry(QRect(0, 0, aTexture->getWidth(), aTexture->getHeight()));
		}
	}
	
	void UiManager::renderIntoTexture(const Ogre::TexturePtr &aTexture)
	{
		assert(!aTexture.isNull());
		assert(isViewSizeMatching(aTexture));
		
		Ogre::HardwarePixelBufferSharedPtr hwBuffer = aTexture->getBuffer(0, 0);
		hwBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
		
		const Ogre::PixelBox &pb = hwBuffer->getCurrentLock();
		
		// render into texture buffer
		QImage textureImg((uchar *)pb.data, pb.getWidth(), pb.getHeight(), QImage::Format_ARGB32);
		textureImg.fill(0);
		
		QPainter painter(&textureImg);
		mWidgetView->render(&painter, QRect(QPoint(0, 0), mWidgetView->size()), QRect(QPoint(0, 0), mWidgetView->size()));
		
		hwBuffer->unlock();
	}
}
