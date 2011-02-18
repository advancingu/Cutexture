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

namespace Cutexture
{
	namespace Enums
	{
		/** Camera movement actions. These are flags so 
		 * that combined movements such as Forward + 
		 * StrafeLeft are supported.
		 * @see For rotation axes, see http://en.wikipedia.org/wiki/Aircraft_principal_axes */
		enum Movement
		{
			//			NoMovement = 0x0,
			Forward = 0x1,
			Backward = 0x2,
			StrafeLeft = 0x4,
			StrafeRight = 0x8,
			RollLeft = 0x10,
			RollRight = 0x20,
			PitchDown = 0x40,
			PitchUp = 0x80,
			YawCounterClock = 0x100,
			YawClock = 0x200,
		};
		Q_DECLARE_FLAGS	(Movements, Movement)
		Q_DECLARE_OPERATORS_FOR_FLAGS(Movements)
	}
}
