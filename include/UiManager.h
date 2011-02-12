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

#include <QtCore/QObject>

namespace Cutexture
{
	/** Responsible for setting up the user interface, managing 
	 * user interface states and reacting to user and system 
	 * events which relate to the user interface.
	 */
	class UiManager: public QObject, public Ogre::Singleton<UiManager>
	{
	Q_OBJECT
	public:
		UiManager();
		virtual ~UiManager();

		/** Initializes the user interface widgets and triggers a repaint
		 * of the user interface texture. */
		void setupUserInterfaceWidgets();

	public slots:
		void resizeEvent(QResizeEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);

	signals:
	/** Signal which is fired when the UI widgets need to be repainted.
	 * If the receiver wants to render the widgets, only a
	 * Qt::QueuedConnection must be used to connect to this signal. */
	void uiRepaintRequired();

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

		QWidget* loadUiFile(const QString &aUiFile, QWidget *aParent = 0);

private	slots:
	/** Renders mTopLevelWidget into mWidgetBuffer and copies the
	 * result to an Ogre texture.
	 * @see uiRepaintRequired() */
	void updateUiTexture();
};
}
