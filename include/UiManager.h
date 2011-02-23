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

#include "InputManager.h"

#include <QtCore/QObject>

namespace Cutexture
{
	/** Responsible for setting up the user interface, managing 
	 * user interface states and reacting to user and system 
	 * events which relate to the user interface.
	 */
	class UiManager: public QObject
	{
	Q_OBJECT
	public:
		UiManager();
		virtual ~UiManager();

		/** Sets aWidget as the currently visible UI widget. */
		void setActiveWidget(QWidget *aWidget);
		
		/** Sets the InputManager which will provide input events 
		 * to the UI. */
		void setInputManager(InputManager *aInputManager);
		
		/** @return True, if the UI texture needs to be repainted. */
		inline bool isUiDirty() const { return mUiDirty; }
		
		/** Renders mTopLevelWidget into the texture specified by 
		 * aTexture. */
		void renderIntoTexture(const Ogre::TexturePtr &aTexture);

		/** Recreates the texture aTexture with a power-of-two 
		 * sized texture whose size is greater or equal to aSize.
		 * @param aSize Minimum size of the texture aTexture.
		 * @param aMaterial Material to assign aTexture to.
		 * @param aTexture The texture to resize.
		 */
		void resizeTexture(const QSize &aSize, const Ogre::MaterialPtr &aMaterial, 
				const Ogre::TexturePtr &aTexture);
		
		/** Resizes the active UI widget to the size in 
		 * aEvent->size(). Note: This is not the same as setting 
		 * the view size. Resizing the UI changes the actual 
		 * size of the widgets whereas changing the view size 
		 * simply changes the size of the viewport which displays 
		 * the UI.
		 * @param aEvent Target widget size. 
		 * @see setViewSize() */
		void resizeUi(QResizeEvent *aEvent);
		
		/** @return True, if the size of mWidgetView is equal to the 
		 * size of aTexture. */
		bool isViewSizeMatching(const Ogre::TexturePtr &aTexture) const;
		
		/** Sets mWidgetView's geometry to aTexture's dimensions 
		 * if it is not already of this size.
		 * @param aTexture The texture to fit mWidgetView to. */
		void setViewSize(const Ogre::TexturePtr &aTexture);
		
	public slots:
		/** @see QWidget::mousePressEvent() */
		void mousePressEvent(QMouseEvent *event);
		/** @see QWidget::mouseReleaseEvent() */
		void mouseReleaseEvent(QMouseEvent *event);
		/** @see QWidget::mouseMoveEvent() */
		void mouseMoveEvent(QMouseEvent *event);
		/** @see QWidget::keyPressEvent() */
		void keyPressEvent(QKeyEvent *event);
		/** @see QWidget::keyReleaseEvent() */
		void keyReleaseEvent(QKeyEvent *event);
		/** Sets or unsets the UI dirty flag. If the flag is set, 
		 * the UI has changed since the last time the flag was 
		 * set to false. This flag needs to be manually reset to 
		 * false by the application. */
		void setUiDirty(bool aDirty = true);

	private:
		
		/** Scene which contains all the user interface widgets
		 * as QGraphicsWidget items. */
		QGraphicsScene *mWidgetScene;

		/** View which visualizes the scene containing UI widgets. */
		QGraphicsView *mWidgetView;

		/** Top-level widget in the graphics scene. */
		QWidget *mTopLevelWidget;

		/** Pointer to the widget currently possessing keyboard focus. 
		 * Null if no focus set. */
		QWidget *mFocusedWidget;
		
		/** Indicates if the UI texture needs to be updated due to a  
		 * change in mWidgetScene. */
		bool mUiDirty;
		
		/** Pointer to InputManager which provides input events to 
		 * the UI. */
		InputManager *mInputManager;
	};
}
