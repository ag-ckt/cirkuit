/*
    Copyright (C) 2010  Matteo Agostinelli <agostinelli@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "graphicsgenerator.h"
#include "graphicsdocument.h"
#include "command.h"

#include <poppler-qt4.h>

#include <KStandardDirs>
#include <KTemporaryFile>
#include <QDir>
#include <QImage>

#include <QDebug>

GraphicsGenerator::GraphicsGenerator(QObject* parent): QObject(parent)
{    
    m_workingDir = new QDir(KStandardDirs::locateLocal("data", "cirkuit/build/", true));
    
    m_tempFile = new KTemporaryFile;
    m_tempFile->setPrefix(m_workingDir->absolutePath() + "/");
    m_tempFile->setSuffix(".dvi");
    
    m_tempFile->open();
    m_tempFileInfo = new QFileInfo(m_tempFile->fileName());

    m_input = Unknown;
    m_output = Unknown;
}

bool GraphicsGenerator::convert(GraphicsGenerator::Format in, GraphicsGenerator::Format out)
{
    qDebug() << "Inside the new converter!!...";
    
    // this class doesn't know how to convert from source
    if (in == Source || out == Source || out == Dvi) {
        return false;
    }
    
    // Check that input and output formats are different
    if (in == out) {
        return true;
    }
    
    if (out == QtImage) {
        bool b = true;
        if (!convert(in, Pdf)) b = false;
        if (!render()) b = false;
        return b;
    }
    
    if (out == Svg && in != Pdf) {
        bool b = true;
        if (!convert(in,Pdf)) b = false;
        if (!convert(Pdf,Svg)) b = false;
        return b;
    }
    
    if (!formatExists(in)) {
        return false;
    }
    
    if (in == Dvi) {
        if (out == Postscript) {
            QStringList args;
            args << filePath(Dvi) << QString("-o %1").arg(filePath(Postscript));
            m_commands.append(new Command("dvips", "", args, this));
            return true;
        } else if (out == Eps) {
            QStringList args;
            args << filePath(Dvi) << QString("-E -Ppdf -G0 -o %1").arg(filePath(Eps));
            m_commands.append(new Command("dvips", "", args, this));
            return true;
        } else if (out == Pdf) {
            bool b = true;
            if (!convert(Dvi,Eps)) b = false;
            if (!convert(Eps,Pdf)) b = false;
            return b;
        } else if (out == Png) {
            bool b = true;
            if (!convert(Dvi,Eps)) b = false;
            if (!convert(Eps,Png)) b = false;
            return b;
        } else {
            return false;
        }
    }
    
    if (in == Postscript) {
        if (out == Eps) {
            QStringList args;
            args << filePath(Postscript) << filePath(Eps);
            m_commands.append(new Command("ps2epsi", "", args, this));
            return true;
        } else if (out == Pdf) {
            QStringList args;
            args << filePath(Postscript) << filePath(Pdf);
            m_commands.append(new Command("ps2pdf", "", args, this));
            return true;
        } else if (out == Png) {
            bool b = true;
            if (!convert(Postscript,Eps)) b = false;
            if (!convert(Eps,Png)) b = false;
            return b;
        } else {
            return false;
        }     
    }
    
    if (in == Eps) {
        if (out == Postscript) {
            QStringList args;
            args << filePath(Eps) << filePath(Postscript);
            m_commands.append(new Command("ps2ps", "", args, this));
            return true;
        } else if (out == Pdf) {
            QStringList args;
            args << filePath(Eps) << "--outfile="+filePath(Pdf);
            m_commands.append(new Command("epstopdf", "", args, this));
            return true;
        } else if (out == Png) {
            QStringList args;
            args << "-density 300" << filePath(Eps) << filePath(Png);
            m_commands.append(new Command("epstopdf", "", args, this));
            return true;
        } else {
            return false;
        }     
    }
    
    if (in == Pdf) {
        if (out == Svg) {
            QStringList args;
            args << filePath(Pdf) << filePath(Svg);
            m_commands.append(new Command("pdf2svg", "", args, this));
            return true;
        }
    }
    
    return false;
}

bool GraphicsGenerator::start()
{
    bool success = true;
    
    for (int i = 0; i < m_commands.count(); i++) {
        qDebug() << "Executing " << m_commands[i]->name() << " with arguments " << m_commands[i]->args();
        if (!m_commands[i]->execute()) {
            emit fail();
            success = false;
        }
        QString stderr = m_commands[i]->readAllStandardError();
        QString stdout = m_commands[i]->readAllStandardOutput();
        if (!stderr.isEmpty()) {
            emit error(stderr);
        }
        if (!stdout.isEmpty()) {
            emit output(stdout);
        }
    }
    
    return success;
}

void GraphicsGenerator::readOutput()
{

}


bool GraphicsGenerator::generate(GraphicsGenerator::Format format)
{
    return convert(Source, format);
}

void GraphicsGenerator::clear()
{
    // clear the existing command queue
    foreach (Command* c, m_commands) {
        delete c;
    }
    m_commands.clear();
}

bool GraphicsGenerator::formatExists(GraphicsGenerator::Format format) const
{
    if (m_tempFileInfo == 0) {
        return false;
    }
    
    return m_workingDir->exists(m_tempFileInfo->baseName() + GraphicsGenerator::extension(format));
}

QString GraphicsGenerator::extension(GraphicsGenerator::Format format)
{
    switch (format) {
        case Source:
            return ".cir";
        case Dvi:
            return ".dvi";
        case Postscript:
            return ".ps";
        case Pdf:
            return ".pdf";
        case Eps:
            return ".eps";
        case Png:
            return ".png";
        case Svg:
            return ".svg";
        case QtImage:
        case Unknown:
        default:
            return "";
    }
}

QString GraphicsGenerator::filePath(GraphicsGenerator::Format format) const
{
    return QString("%1/%2%3").arg(m_workingDir->canonicalPath()).arg(m_tempFileInfo->baseName()).arg(GraphicsGenerator::extension(format));
}

bool GraphicsGenerator::render()
{
    if (!formatExists(Pdf)) {
        return false;
    }

    Poppler::Document* document = Poppler::Document::load(filePath(Pdf));
    if (!document || document->isLocked()) {
        delete document;
        return false;
    }

    // Access page of the PDF file
    Poppler::Page* pdfPage = document->page(0);  // Document starts at page 0
    if (pdfPage == 0) {
        return false;
    }

    // Generate a QImage of the rendered page
    QImage m_image = pdfPage->renderToImage(600,600);
    emit previewReady(m_image);

    delete pdfPage;
    delete document;
    
    return true;
}


GeneratorThread::GeneratorThread(GraphicsGenerator::Format in, GraphicsGenerator::Format out, GraphicsDocument* doc, QObject* parent): QThread(parent)
{
    m_input = in;
    m_output = out;
    
    setDocument(doc);
}

void GeneratorThread::run()
{
    GraphicsGenerator* gen;
    switch (m_doc->identify()) {
        default:
            gen = new GraphicsGenerator();
            
    }
    
    connect(gen, SIGNAL(previewReady(QImage)), this, SIGNAL(previewReady(QImage)));
    connect(gen, SIGNAL(error(QString)), this, SLOT(printMessage(QString)));
    gen->convert(m_input,m_output);   
    gen->start();
    exec();
}

void GeneratorThread::setDocument(GraphicsDocument* doc)
{
    m_doc = doc;
}

void GeneratorThread::printMessage(const QString& msg)
{
    qDebug() << msg;
}
