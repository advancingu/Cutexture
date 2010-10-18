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

#include "InputManager.h"
#include "UiManager.h"
#include "Exception.h"
#include "OgreCore.h"
#include "Constants.h"

template<> Cutexture::InputManager* Ogre::Singleton<Cutexture::InputManager>::ms_Singleton = 0;

namespace Cutexture
{
	InputManager::InputManager() :
		mOis(NULL), mOisKeyboard(NULL), mOisMouse(NULL), mMouseButtonsPressed(0),
				mModifiersPressed(0)
	{
		// set up keymap
		mMovementKeys.insert(Qt::Key_W, Enums::Forward);
		mMovementKeys.insert(Qt::Key_A, Enums::StrafeLeft);
		mMovementKeys.insert(Qt::Key_S, Enums::Backward);
		mMovementKeys.insert(Qt::Key_D, Enums::StrafeRight);
		mMovementKeys.insert(Qt::Key_Up, Enums::PitchDown);
		mMovementKeys.insert(Qt::Key_Down, Enums::PitchUp);
		mMovementKeys.insert(Qt::Key_Left, Enums::YawCounterClock);
		mMovementKeys.insert(Qt::Key_Right, Enums::YawClock);
	}
	
	InputManager::~InputManager()
	{
		if (mOis)
		{
			if (mOisKeyboard)
			{
				mOis->destroyInputObject(mOisKeyboard);
				mOisKeyboard = 0;
			}

			if (mOisMouse)
			{
				mOis->destroyInputObject(mOisMouse);
				mOisMouse = 0;
			}

			OIS::InputManager::destroyInputSystem(mOis);
			mOis = 0;
		}
	}
	
	void InputManager::initialize(Ogre::RenderWindow *aRenderWindow)
	{
		// initialize the OpenInputSystem (OIS)
		size_t windowHnd = 0;
		aRenderWindow->getCustomAttribute("WINDOW", &windowHnd);
		std::ostringstream windowHndStr;
		windowHndStr << windowHnd;
		OIS::ParamList paramList;
		paramList.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#if defined(OIS_WIN32_PLATFORM)
		// if activated, can break absolute mouse positioning
		// TODO test if this works correctly with foreground and nonexclusive flags active
		paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		//		paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		//		paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined(OIS_LINUX_PLATFORM)
		paramList.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		paramList.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		paramList.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		paramList.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
		mOis = OIS::InputManager::createInputSystem(paramList);
		
		try
		{
			mOisKeyboard = static_cast<OIS::Keyboard*> (mOis->createInputObject(OIS::OISKeyboard,
					true));
			mOisKeyboard->setEventCallback(this);
			mOisKeyboard->setTextTranslation(OIS::Keyboard::Unicode);
		}
		catch (...)
		{
			EXCEPTION("Could not initialize keyboard", "InputManager::InputManager()");
		}
		
		try
		{
			mOisMouse = static_cast<OIS::Mouse*> (mOis->createInputObject(OIS::OISMouse, true));
			
			
			//Set mouse clipping area
			unsigned int width, height, depth;
			int left, top;
			
			OgreCore::getSingleton().getOgreRenderWindow()->getMetrics(width, height, depth, left,
					top);
			mOisMouse->getMouseState().width = width;
			mOisMouse->getMouseState().height = height;
			mOisMouse->setEventCallback(this);
		}
		catch (...)
		{
			EXCEPTION("Could not initialize mouse", "InputManager::InputManager()");
		}
	}
	
	void InputManager::update()
	{
		assert(mOis && mOisKeyboard && mOisMouse);
		
		
		// set new state
		mOisKeyboard->capture();
		mOisMouse->capture();
	}
	
	void InputManager::resizeEvent(QResizeEvent *event)
	{
		// update allowed mouse area
		const OIS::MouseState &mouseState = mOisMouse->getMouseState();
		mouseState.width = event->size().width();
		mouseState.height = event->size().height();
	}
	
	QPoint InputManager::getRelativeMouseMovement() const
	{
		const OIS::MouseState& mouseState = mOisMouse->getMouseState();
		return QPoint(mouseState.X.rel, mouseState.Y.rel);
	}
	
	bool InputManager::mouseMoved(const OIS::MouseEvent &arg)
	{
		QPoint eventPoint(arg.state.X.abs, arg.state.Y.abs);
		eventPoint += QPoint(Constants::INPUT_MANAGER_MOUSE_OFFSET_X,
				Constants::INPUT_MANAGER_MOUSE_OFFSET_Y);
		//		qDebug() << "mouseMoved" << eventPoint;

		QMouseEvent mouseEvent(QEvent::MouseMove, eventPoint, eventPoint, Qt::NoButton,
				mMouseButtonsPressed, Qt::NoModifier);
		emit(mouseMoveEvent(&mouseEvent));
		return true;
	}
	
	bool InputManager::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	{
		QPoint eventPoint(arg.state.X.abs, arg.state.Y.abs);
		eventPoint += QPoint(Constants::INPUT_MANAGER_MOUSE_OFFSET_X,
				Constants::INPUT_MANAGER_MOUSE_OFFSET_Y);
		//		qDebug() << "mousePressed" << eventPoint;

		mMouseButtonsPressed |= toQtMouseButton(id);
		
		QMouseEvent mouseEvent(QEvent::MouseButtonPress, eventPoint, eventPoint,
				toQtMouseButton(id), mMouseButtonsPressed, Qt::NoModifier);
		emit(mousePressEvent(&mouseEvent));
		return true;
	}
	
	bool InputManager::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
	{
		QPoint eventPoint(arg.state.X.abs, arg.state.Y.abs);
		eventPoint += QPoint(Constants::INPUT_MANAGER_MOUSE_OFFSET_X,
				Constants::INPUT_MANAGER_MOUSE_OFFSET_Y);
		//		qDebug() << "mouseReleased" << eventPoint;

		mMouseButtonsPressed &= ~Qt::MouseButtons(toQtMouseButton(id));
		
		
		// do not include the released button in the mMouseButtonsPressed enum; @see http://doc.qt.nokia.com/4.5/qmouseevent.html#buttons
		QMouseEvent mouseEvent(QEvent::MouseButtonRelease, eventPoint, eventPoint, toQtMouseButton(
				id), mMouseButtonsPressed, Qt::NoModifier);
		emit(mouseReleaseEvent(&mouseEvent));
		return true;
	}
	
	bool InputManager::keyPressed(const OIS::KeyEvent &arg)
	{
		Qt::KeyboardModifier modKey = toQtModifier(arg);
		
		if (modKey != Qt::NoModifier)
		{
			mModifiersPressed |= modKey;
		}

		Qt::Key pressedKey = toQtKey(arg);
		
		if (mMovementKeys.keys().contains(pressedKey))
		{
			mMovementsActive |= mMovementKeys.value(pressedKey);
		}

		if (modKey == Qt::NoModifier || modKey == Qt::KeypadModifier)
		{
			QKeyEvent keyEvent(QEvent::KeyPress, toQtKey(arg), mModifiersPressed, toQtKeyText(arg));
			emit(keyPressEvent(&keyEvent));
		}
		
		return true;
	}
	
	bool InputManager::keyReleased(const OIS::KeyEvent &arg)
	{
		Qt::KeyboardModifier modKey = toQtModifier(arg);
		Qt::Key releasedKey = toQtKey(arg);
		
		mMovementsActive &= ~Enums::Movements(mMovementKeys.value(releasedKey));
		
		if (modKey == Qt::NoModifier || modKey == Qt::KeypadModifier)
		{
			QKeyEvent
					keyEvent(QEvent::KeyRelease, releasedKey, mModifiersPressed, toQtKeyText(arg));
			emit(keyReleaseEvent(&keyEvent));
		}

		// release the modifier after the keyReleased event was sent
		if (modKey != Qt::NoModifier)
		{
			mModifiersPressed &= ~Qt::KeyboardModifiers(modKey);
		}
		
		return true;
	}
	
	Qt::Key InputManager::toQtKey(const OIS::KeyEvent &aEvent) const
	{
		switch (aEvent.key)
		{
		case OIS::KC_UNASSIGNED:
			return Qt::Key_unknown;
			break;
		case OIS::KC_ESCAPE:
			return Qt::Key_Escape;
			break;
		case OIS::KC_1:
			return Qt::Key_1;
			break;
		case OIS::KC_2:
			return Qt::Key_2;
			break;
		case OIS::KC_3:
			return Qt::Key_3;
			break;
		case OIS::KC_4:
			return Qt::Key_4;
			break;
		case OIS::KC_5:
			return Qt::Key_5;
			break;
		case OIS::KC_6:
			return Qt::Key_6;
			break;
		case OIS::KC_7:
			return Qt::Key_7;
			break;
		case OIS::KC_8:
			return Qt::Key_8;
			break;
		case OIS::KC_9:
			return Qt::Key_9;
			break;
		case OIS::KC_0:
			return Qt::Key_0;
			break;
		case OIS::KC_MINUS:
			return Qt::Key_Minus;
			break;
		case OIS::KC_EQUALS:
			return Qt::Key_Equal;
			break;
		case OIS::KC_BACK:
			return Qt::Key_Backspace;
			break;
		case OIS::KC_TAB:
			return Qt::Key_Tab;
			break;
		case OIS::KC_Q:
			return Qt::Key_Q;
			break;
		case OIS::KC_W:
			return Qt::Key_W;
			break;
		case OIS::KC_E:
			return Qt::Key_E;
			break;
		case OIS::KC_R:
			return Qt::Key_R;
			break;
		case OIS::KC_T:
			return Qt::Key_T;
			break;
		case OIS::KC_Y:
			return Qt::Key_Y;
			break;
		case OIS::KC_U:
			return Qt::Key_U;
			break;
		case OIS::KC_I:
			return Qt::Key_I;
			break;
		case OIS::KC_O:
			return Qt::Key_O;
			break;
		case OIS::KC_P:
			return Qt::Key_P;
			break;
		case OIS::KC_LBRACKET:
			return Qt::Key_BracketLeft;
			break;
		case OIS::KC_RBRACKET:
			return Qt::Key_BracketRight;
			break;
		case OIS::KC_RETURN:
			return Qt::Key_Return;
			break;
		case OIS::KC_A:
			return Qt::Key_A;
			break;
		case OIS::KC_S:
			return Qt::Key_S;
			break;
		case OIS::KC_D:
			return Qt::Key_D;
			break;
		case OIS::KC_F:
			return Qt::Key_F;
			break;
		case OIS::KC_G:
			return Qt::Key_G;
			break;
		case OIS::KC_H:
			return Qt::Key_H;
			break;
		case OIS::KC_J:
			return Qt::Key_J;
			break;
		case OIS::KC_K:
			return Qt::Key_K;
			break;
		case OIS::KC_L:
			return Qt::Key_L;
			break;
		case OIS::KC_SEMICOLON:
			return Qt::Key_Semicolon;
			break;
		case OIS::KC_APOSTROPHE:
			return Qt::Key_Apostrophe;
			break;
		case OIS::KC_GRAVE:
			return Qt::Key_Agrave;
			break;
		case OIS::KC_BACKSLASH:
			return Qt::Key_Backslash;
			break;
		case OIS::KC_Z:
			return Qt::Key_Z;
			break;
		case OIS::KC_X:
			return Qt::Key_X;
			break;
		case OIS::KC_C:
			return Qt::Key_C;
			break;
		case OIS::KC_V:
			return Qt::Key_V;
			break;
		case OIS::KC_B:
			return Qt::Key_B;
			break;
		case OIS::KC_N:
			return Qt::Key_N;
			break;
		case OIS::KC_M:
			return Qt::Key_M;
			break;
		case OIS::KC_COMMA:
			return Qt::Key_Comma;
			break;
		case OIS::KC_PERIOD:
			return Qt::Key_Period;
			break;
		case OIS::KC_SLASH:
			return Qt::Key_Slash;
			break;
		case OIS::KC_MULTIPLY:
			return Qt::Key_multiply;
			break;
		case OIS::KC_LMENU:
			return Qt::Key_Menu;
			break;
		case OIS::KC_SPACE:
			return Qt::Key_Space;
			break;
		case OIS::KC_CAPITAL:
			return Qt::Key_CapsLock;
			break;
		case OIS::KC_F1:
			return Qt::Key_F1;
			break;
		case OIS::KC_F2:
			return Qt::Key_F2;
			break;
		case OIS::KC_F3:
			return Qt::Key_F3;
			break;
		case OIS::KC_F4:
			return Qt::Key_F4;
			break;
		case OIS::KC_F5:
			return Qt::Key_F5;
			break;
		case OIS::KC_F6:
			return Qt::Key_F6;
			break;
		case OIS::KC_F7:
			return Qt::Key_F7;
			break;
		case OIS::KC_F8:
			return Qt::Key_F8;
			break;
		case OIS::KC_F9:
			return Qt::Key_F9;
			break;
		case OIS::KC_F10:
			return Qt::Key_F10;
			break;
		case OIS::KC_NUMLOCK:
			return Qt::Key_NumLock;
			break;
		case OIS::KC_SCROLL:
			return Qt::Key_ScrollLock;
			break;
		case OIS::KC_NUMPAD7:
			return Qt::Key_7;
			break;
		case OIS::KC_NUMPAD8:
			return Qt::Key_8;
			break;
		case OIS::KC_NUMPAD9:
			return Qt::Key_9;
			break;
		case OIS::KC_SUBTRACT:
			return Qt::Key_Minus;
			break;
		case OIS::KC_NUMPAD4:
			return Qt::Key_4;
			break;
		case OIS::KC_NUMPAD5:
			return Qt::Key_5;
			break;
		case OIS::KC_NUMPAD6:
			return Qt::Key_6;
			break;
		case OIS::KC_ADD:
			return Qt::Key_Plus;
			break;
		case OIS::KC_NUMPAD1:
			return Qt::Key_1;
			break;
		case OIS::KC_NUMPAD2:
			return Qt::Key_2;
			break;
		case OIS::KC_NUMPAD3:
			return Qt::Key_3;
			break;
		case OIS::KC_NUMPAD0:
			return Qt::Key_0;
			break;
		case OIS::KC_DECIMAL:
			return Qt::Key_Period;
			break;
			//    	case OIS::KC_OEM_102:
			//    		return Qt::Key_
			//    		break;
		case OIS::KC_F11:
			return Qt::Key_F11;
			break;
		case OIS::KC_F12:
			return Qt::Key_F12;
			break;
		case OIS::KC_F13:
			return Qt::Key_F13;
			break;
		case OIS::KC_F14:
			return Qt::Key_F14;
			break;
		case OIS::KC_F15:
			return Qt::Key_F15;
			break;
			//    	case OIS::KC_KANA:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_ABNT_C1:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_CONVERT:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_NOCONVERT:
			//    		return Qt::Key_
			//    		break;
		case OIS::KC_YEN:
			return Qt::Key_yen;
			break;
			//    	case OIS::KC_ABNT_C2:
			//    		return Qt::Key_
			//    		break;
		case OIS::KC_NUMPADEQUALS:
			return Qt::Key_Equal;
			break;
		case OIS::KC_PREVTRACK:
			return Qt::Key_MediaPrevious;
			break;
		case OIS::KC_AT:
			return Qt::Key_At;
			break;
		case OIS::KC_COLON:
			return Qt::Key_Colon;
			break;
		case OIS::KC_UNDERLINE:
			return Qt::Key_Underscore;
			break;
		case OIS::KC_KANJI:
			return Qt::Key_Kanji;
			break;
		case OIS::KC_STOP:
			return Qt::Key_MediaStop;
			break;
			//    	case OIS::KC_AX:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_UNLABELED:
			//    		return Qt::Key_
			//    		break;
		case OIS::KC_NEXTTRACK:
			return Qt::Key_MediaNext;
			break;
			//    	case OIS::KC_NUMPADENTER:
			//    		return Qt::Key_
			//    		break;
		case OIS::KC_MUTE:
			return Qt::Key_VolumeMute;
			break;
			//    	case OIS::KC_CALCULATOR:
			//    		return Qt::Key_
			//    		break;
		case OIS::KC_PLAYPAUSE:
			return Qt::Key_MediaPlay;
			break;
		case OIS::KC_MEDIASTOP:
			return Qt::Key_MediaStop;
			break;
		case OIS::KC_VOLUMEDOWN:
			return Qt::Key_VolumeDown;
			break;
		case OIS::KC_VOLUMEUP:
			return Qt::Key_VolumeUp;
			break;
		case OIS::KC_WEBHOME:
			return Qt::Key_HomePage;
			break;
		case OIS::KC_NUMPADCOMMA:
			return Qt::Key_Colon;
			break;
		case OIS::KC_DIVIDE:
			return Qt::Key_Slash;
			break;
		case OIS::KC_SYSRQ:
			return Qt::Key_SysReq;
			break;
		case OIS::KC_RMENU:
			return Qt::Key_Menu;
			break;
		case OIS::KC_PAUSE:
			return Qt::Key_Pause;
			break;
		case OIS::KC_HOME:
			return Qt::Key_Home;
			break;
		case OIS::KC_UP:
			return Qt::Key_Up;
			break;
		case OIS::KC_PGUP:
			return Qt::Key_PageUp;
			break;
		case OIS::KC_LEFT:
			return Qt::Key_Left;
			break;
		case OIS::KC_RIGHT:
			return Qt::Key_Right;
			break;
		case OIS::KC_END:
			return Qt::Key_End;
			break;
		case OIS::KC_DOWN:
			return Qt::Key_Down;
			break;
		case OIS::KC_PGDOWN:
			return Qt::Key_PageDown;
			break;
		case OIS::KC_INSERT:
			return Qt::Key_Insert;
			break;
		case OIS::KC_DELETE:
			return Qt::Key_Delete;
			break;
			//    	case OIS::KC_APPS:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_POWER:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_SLEEP:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_WAKE:
			//    		return Qt::Key_
			//    		break;
		case OIS::KC_WEBSEARCH:
			return Qt::Key_Search;
			break;
		case OIS::KC_WEBFAVORITES:
			return Qt::Key_Favorites;
			break;
		case OIS::KC_WEBREFRESH:
			return Qt::Key_Refresh;
			break;
		case OIS::KC_WEBSTOP:
			return Qt::Key_Stop;
			break;
		case OIS::KC_WEBFORWARD:
			return Qt::Key_Forward;
			break;
		case OIS::KC_WEBBACK:
			return Qt::Key_Back;
			break;
			//    	case OIS::KC_MYCOMPUTER:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_MAIL:
			//    		return Qt::Key_
			//    		break;
			//    	case OIS::KC_MEDIASELECT:
			//    		return Qt::Key_
			//    		break;
		default:
			return Qt::Key_unknown;
			break;
		}
		
		return Qt::Key_unknown;
	}
	
	Qt::KeyboardModifier InputManager::toQtModifier(const OIS::KeyEvent &aEvent) const
	{
		switch (aEvent.key)
		{
		case OIS::KC_LCONTROL:
			return Qt::ControlModifier;
			break;
		case OIS::KC_RCONTROL:
			return Qt::ControlModifier;
			break;
		case OIS::KC_LSHIFT:
			return Qt::ShiftModifier;
			break;
		case OIS::KC_RSHIFT:
			return Qt::ShiftModifier;
			break;
		case OIS::KC_NUMPAD0:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD1:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD2:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD3:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD4:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD5:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD6:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD7:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD8:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_NUMPAD9:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_LEFT:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_RIGHT:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_UP:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_DOWN:
			return Qt::KeypadModifier;
			break;
		case OIS::KC_LWIN:
			return Qt::MetaModifier;
			break;
		case OIS::KC_RWIN:
			return Qt::ShiftModifier;
			break;
		default:
			break;
		}
		
		return Qt::NoModifier;
	}
	
	Qt::MouseButton InputManager::toQtMouseButton(const OIS::MouseButtonID &aButton) const
	{
		switch (aButton)
		{
		case OIS::MB_Left:
			return Qt::LeftButton;
		case OIS::MB_Middle:
			return Qt::MidButton;
		case OIS::MB_Right:
			return Qt::RightButton;
		default:
			return Qt::NoButton;
		}
	}
	
	QString InputManager::toQtKeyText(const OIS::KeyEvent &aEvent) const
	{
		return QString::fromUcs4(&aEvent.text, 1);
	}
}
