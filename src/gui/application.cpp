// This file is part of the SpeedCrunch project
// Copyright (C) 2009 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "gui/application.hxx"

#include <QtCore/QByteArray>

#include <cstring>

#if QT_VERSION >= 0x040400

#include <QtCore/QSharedMemory>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#define GUI_APPLICATION_SHARED_MEMORY_KEY    "speedcrunch"
#define GUI_APPLICATION_LOCAL_SOCKET_TIMEOUT 1000

struct Application::Private
{
    QObject * p;
    bool isRunning;
    QLocalServer localServer;
    QSharedMemory sharedMemory;

    Private( QObject * parent = 0 );

    bool sendRaiseRequest();
};

Application::Private::Private( QObject * parent )
    : p( parent )
{
    sharedMemory.setKey( GUI_APPLICATION_SHARED_MEMORY_KEY );
    isRunning = sharedMemory.attach() && sendRaiseRequest();
    if ( !isRunning && sharedMemory.create(1) ) {
        connect( &localServer, SIGNAL(newConnection()), p, SLOT(receiveMessage()) );
        localServer.listen( GUI_APPLICATION_SHARED_MEMORY_KEY );
    }
}

void Application::receiveMessage()
{
    qDebug() << "receiveMessage()";
    QLocalSocket * localSocket = d->localServer.nextPendingConnection();

    if ( ! localSocket->waitForReadyRead(GUI_APPLICATION_LOCAL_SOCKET_TIMEOUT) )
        return;

    QByteArray byteArray = localSocket->readAll();
    const QString message = QString::fromUtf8( byteArray.constData() );

    if ( message == "raise" )
        emit raiseRequested();

    localSocket->disconnectFromServer();
    qDebug() << "raise requested";
}

bool Application::Private::sendRaiseRequest()
{
    qDebug() << "sendRaiseRequest()";
    QLocalSocket localSocket;
    localSocket.connectToServer( GUI_APPLICATION_SHARED_MEMORY_KEY, QIODevice::WriteOnly );

    if ( ! localSocket.waitForConnected(GUI_APPLICATION_LOCAL_SOCKET_TIMEOUT) )
        return false;

    localSocket.write( QString("raise").toUtf8() );

    if ( ! localSocket.waitForBytesWritten(GUI_APPLICATION_LOCAL_SOCKET_TIMEOUT) )
        return false;

    localSocket.disconnectFromServer();
    qDebug() << "sent raise request";
    return true;
}

#else // QT_VERSION >= 0x040400

struct Application::Private
{
    bool isRunning;
    Private() : isRunning( false ) {};
};

#endif // QT_VERSION >= 0x040400

Application::Application( int & argc, char * argv[] )
    : QApplication( argc, argv ), d( new Application::Private(this) )
{
}

Application::~Application()
{
}

bool Application::isRunning() const
{
    return d->isRunning;
}

