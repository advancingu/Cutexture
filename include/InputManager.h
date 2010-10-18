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
#include "Enums.h"

namespace Cutexture
{
	/** Responsible for all input-device handling. 
	 * Receives mouse and keyboard events from OIS (Open Input System) and 
	 * converts them to Qt mouse and keyboard events. Also separates 
	 * one-time input events (e.g. mouse button clicked) from continuous 
	 * input (e.g. camera movement key is pressed).
	 */
	class InputManager: public QObject,
			public Ogre::Singleton<InputManager>,
			public OIS::MouseListener,
			public OIS::KeyListener
	{
Q_OBJECT		;
	public:
		InputManager();
		virtual ~InputManager();

		void initialize(Ogre::RenderWindow *aRenderWindow);

		/** Updates the current input state. */
		void update();

		/** Call to update the input manager based on the new
		 window size. */
		void resizeEvent(QResizeEvent *event);

		/** Returns the relative mouse movement since the last update. */
		QPoint getRelativeMouseMovement() const;

		/** Returns the currently active movement actions. */
		inline Enums::Movements getMovementsActive()
		{	return mMovementsActive;}

		/** Returns the currently pressed mouse buttons. */
		inline Qt::MouseButtons getMouseButtonsPressed()
		{	return mMouseButtonsPressed;}

		signals:
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	protected:
		bool mouseMoved(const OIS::MouseEvent &arg);
		bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

		bool keyPressed(const OIS::KeyEvent &arg);
		bool keyReleased(const OIS::KeyEvent &arg);

	private:
		/** Pointer to the OpenInputSystem input manager. */
		OIS::InputManager *mOis;
		/** OpenInputSystem keyboard handler. */
		OIS::Keyboard *mOisKeyboard;
		/** OpenInputSystem mouse handler. */
		OIS::Mouse *mOisMouse;

		/** Bitflag of currently pressed mouse buttons. */
		Qt::MouseButtons mMouseButtonsPressed;

		/** Bitflag of currently pressed modifier keys (e.g. CTRL). */
		Qt::KeyboardModifiers mModifiersPressed;

		/** Set for associating key codes to movement actions. */
		QHash<Qt::Key, Enums::Movement> mMovementKeys;

		/** Bitflag of currently active movement actions. */
		Enums::Movements mMovementsActive;

		/** From an OIS key event, convert to a Qt key code. */
		Qt::Key toQtKey(const OIS::KeyEvent &aEvent) const;

		/** From an OIS key event, convert to a Qt modifier key.
		 * @return The keyboard modifier value or Qt::NoModifier if 
		 * not a modifier key. */
		Qt::KeyboardModifier toQtModifier(const OIS::KeyEvent &aEvent) const;

		/** Convert an OIS mouse button ID to a Qt::MouseButton 
		 * enum. */
		Qt::MouseButton toQtMouseButton(const OIS::MouseButtonID &aButton) const;

		/** Retrieves the unicode string for an OIS keyboard 
		 * event. */
		QString toQtKeyText(const OIS::KeyEvent &aEvent) const;
	};
}
