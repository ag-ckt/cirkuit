/***************************************************************************
*   Copyright (C) 2009 by Matteo Agostinelli                              *
*   agostinelli@gmail.com                                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "circuitmacrosmanager.h"

#include <KStandardDirs>
#include <KTar>
#include <KNotification>
#include <KLocale>

#include <QFile>
#include <QUrl>
#include <QHttp>
#include <QDebug>
#include <QMessageBox>
#include <QtNetwork>
#include <QProcess>
#include <QBuffer>
#include "httpdownloader.h"

CircuitMacrosManager::CircuitMacrosManager(): QObject()
{
	//progressNotify = new KNotification("Download progress");
	//progressNotify->setComponentData(KComponentData("cirkuit"));
}

bool CircuitMacrosManager::checkExistence() const
{
	QString cm_path = KStandardDirs::locateLocal("data", "cirkuit/circuit_macros/libcct.m4", true);
	
	if (QFile::exists(cm_path))
		return true;
	else
		return false;
}

void CircuitMacrosManager::downloadLatest()
{
	QString filename = KStandardDirs::locateLocal("data", "cirkuit/Circuit_macros.tar.gz", true);
	file = new QFile(filename);
	
	if (!file->open(QIODevice::WriteOnly))
	{
		QMessageBox::information(0, tr("HTTP"),
												 tr("Unable to save the file %1: %2.")
												 .arg(filename).arg(file->errorString()));
												 delete file;
												 file = 0;
												 return;
	}
	
	dnldr = new HttpDownloader;
	
	connect(dnldr, SIGNAL(done()), this, SLOT(done()));
	connect(dnldr, SIGNAL(progress(int,int)), this, SLOT(progress(int,int)));
	//progressNotify->setText(i18n("Circuit Macros download in progress... please wait"));
	//progressNotify->sendEvent();
	
	dnldr->addDownload(QUrl(QString("http://www.ece.uwaterloo.ca/~aplevich/Circuit_macros/Circuit_macros.tar.gz")), file);
}

void CircuitMacrosManager::done()
{
	//progressNotify->setText(i18n("Circuit Macros download finished"));
	//progressNotify->sendEvent();
	
	file->close();
	
	KTar tarfile(file->fileName());
	tarfile.open(QIODevice::ReadOnly);
	
	const KArchiveDirectory* root = tarfile.directory();
	
	const KArchiveDirectory* mainDir = (KArchiveDirectory*) root->entry(root->entries().at(0));
	mainDir->copyTo(KStandardDirs::locateLocal("data", "cirkuit/circuit_macros/", true), true);
	
	configureCircuitMacros();
	
	delete file;
	file = 0;
	
	//progressNotify->setText(i18n("Circuit Macros download finished!"));
	//progressNotify->sendEvent();
	
	
	emit(configured());
}

void CircuitMacrosManager::progress(int done, int total)
{	
	//qDebug() << "Progress: " << 100*done/total;
	
	if (100*done/total % 10 == 0)
	{
		//progressNotify->setText(i18n("Circuit Macros download in progress... %1\% complete").arg(100*done/total));
		//progressNotify->sendEvent();
	}
}


void CircuitMacrosManager::readmeDone()
{
	buffer->close();
	
	QString onlineVersion = findVersion(barray);
	qDebug() << "ONLINE version: " << onlineVersion;
	
	if (onlineVersion > installedVersion())
		emit newVersionAvailable(onlineVersion);
	
	delete buffer;
	buffer = 0;
}

void CircuitMacrosManager::configureCircuitMacros()
{
	QString homelibFilename = KStandardDirs::locateLocal("data", "cirkuit/circuit_macros/homelib.txt", false);
	QString defineString = QString("`define(`HOMELIB_',`%1')')").arg(KStandardDirs::locateLocal("data", "cirkuit/circuit_macros/", false));
	
	QFile homelibFile(homelibFilename);
	if (!homelibFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	
	QTextStream out(&homelibFile);
	out << defineString << "\n";
	
	homelibFile.close();
	
	QStringList args;
	args << "homelib" << "psdefault";
	QProcess::startDetached("make", args, KStandardDirs::locateLocal("data", "cirkuit/circuit_macros/", false));
	
	QFile::remove(KStandardDirs::locateLocal("data", "cirkuit/Circuit_macros.tar.gz", false));
	
	qDebug() << "Circuit macros configured";
}

QString CircuitMacrosManager::installedVersion() const
{
	if (!checkExistence())
		return "";
	
	QString filename = KStandardDirs::locateLocal("data", "cirkuit/circuit_macros/README", false);
	
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return "";
	
	return findVersion(file.readAll());
}

void CircuitMacrosManager::checkOnlineVersion()
{
	barray.clear();
	buffer = new QBuffer(&barray);
	buffer->open(QIODevice::WriteOnly);
	
	dnldr = new HttpDownloader;
	connect(dnldr, SIGNAL(done()), this, SLOT(readmeDone()));
	dnldr->addDownload(QUrl(QString("http://www.ece.uwaterloo.ca/~aplevich/Circuit_macros/README")), buffer);
}

QString CircuitMacrosManager::findVersion(const QByteArray& byteArray) const
{
	bool versionStringFound = false;
	
	QRegExp rx("Version (\\d\\.\\d+)");
	QString version = "";
	
	QString content(byteArray);
	
	if (content.contains(rx))
	{
		versionStringFound = true;
		version = rx.cap(1);
	}
	
	return version;
}
