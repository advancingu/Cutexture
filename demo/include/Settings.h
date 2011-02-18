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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "Prerequisites.h"

namespace Cutexture
{
	
	
	/** Allows loading and writing configuration values 
	 * to/from persistent storage. */
	class Settings: public Ogre::Singleton<Settings>
	{
	public:
		Settings();
		virtual ~Settings();

		QVariant getValue(const QString &category, const QString &key,
				const QVariant &defaultValue = QVariant()) const;

		QHash<QString, QVariant> getCategoryValues(const QString &category) const;

		/** Adds each given default value to the settings store unless a 
		 * value already exists in the store.
		 * @param aCategory
		 * @param aDefaults Key,value pairs of defaults.
		 */
		void setDefaultValues(const QString &aCategory, const QHash<QString, QVariant> &aDefaults);

	private:
		QSettings *mQtSettings;
	};

}

#endif /* SETTINGS_H_ */
