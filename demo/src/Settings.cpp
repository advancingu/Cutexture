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

#include "Settings.h"
#include "DemoConstants.h"

template<> Cutexture::Settings* Ogre::Singleton<Cutexture::Settings>::ms_Singleton = 0;

namespace Cutexture
{
	Settings::Settings() :
		mQtSettings(NULL)
	{
		// example path: /home/user/Cutexture/ct.ini
		QString path = QDir::currentPath();
		path += QDir::separator();
		
		path += DemoConstants::SETTINGS_FILENAME;
		
		
		// check if path is valid
		const QFile file(path);
		
		bool fileExists = file.exists();
		
		mQtSettings = new QSettings(path, QSettings::IniFormat);
		
		if (!fileExists)
		{
			// file didn't exist

			// TODO retrieve default settings from each component and store them
			// mQtSettings->setValue("cat/key", QVariant("val"));
		}
	}
	
	Settings::~Settings()
	{
		delete mQtSettings;
	}
	
	QVariant Settings::getValue(const QString &category, const QString &key,
			const QVariant &defaultValue) const
	{
		if (mQtSettings)
		{
			return mQtSettings->value(category + "/" + key, defaultValue);
		}
		else
			return QVariant();
	}
	
	QHash<QString, QVariant> Settings::getCategoryValues(const QString &category) const
	{
		QHash < QString, QVariant > returnValues;
		
		if (mQtSettings->childGroups().contains(category))
		{
			mQtSettings->beginGroup(category);
			
			const QStringList &keys = mQtSettings->childKeys();
			foreach(QString key, keys)
			{
				returnValues.insert(key, mQtSettings->value(key));
			}

			mQtSettings->endGroup();
		}
		
		return returnValues;
	}
	
	void Settings::setDefaultValues(const QString &aCategory,
			const QHash<QString, QVariant> &aDefaults)
	{
		QHash < QString, QVariant > existingValues = getCategoryValues(aCategory);
		
		mQtSettings->beginGroup(aCategory);
		foreach(QString defKey, aDefaults.keys())
		{
			if (!existingValues.keys().contains(defKey))
			{
				mQtSettings->setValue(defKey, aDefaults.value(defKey));
			}
		}
		mQtSettings->endGroup();
	}
}
