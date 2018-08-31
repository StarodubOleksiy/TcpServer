#include "ChatterBoxServer.h"
#include <QDataStream>
#include <QTcpSocket>
#include <QRegExp>

ChatterBoxServer::ChatterBoxServer(QObject *parent) : QTcpServer(parent)
{
}

void ChatterBoxServer::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clients.insert(client);

    qDebug() << "New client from:" << client->peerAddress().toString()
             <<"enter the chat" ;


    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void ChatterBoxServer::readyRead()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
       QDataStream in(pClientSocket);
       in.setVersion(QDataStream::Qt_4_7);
        for (;;) {
            if (!m_nNextBlockSize) {
                if (pClientSocket->bytesAvailable() < (int)sizeof(quint16)) {
                    break;
                }
                in >> m_nNextBlockSize;
            }
            if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
                break;
            }
            QTime   time;
            QString str;
            in >> time >> str;

            QString strMessage =
                time.toString() + " " + "Client has sent - " + str;
            qDebug()<<strMessage;

            m_nNextBlockSize = 0;
            foreach(QTcpSocket *otherClient, clients)
            sendToClient(otherClient,
                         "Server Response: Received \"" + /*sendstr[i]*/str + "\""
                        );
        }
}
void ChatterBoxServer::sendToClient(QTcpSocket* pSocket, const QString& str)
{

    QByteArray  arrBlock;
   QDataStream out(&arrBlock, QIODevice::WriteOnly);
   out.setVersion(QDataStream::Qt_4_7);
    out << quint16(0) << QTime::currentTime()<<str;


    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}


void ChatterBoxServer::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clients.remove(client);

    QString user = users[client];
    users.remove(client);

    sendUserList();
    foreach(QTcpSocket *client, clients)
        client->write(QString("Server:" + user + " has left.\n").toUtf8());
}


void ChatterBoxServer::sendUserList()
{
    QStringList userList;
    foreach(QString user, users.values())
        userList << user;

    foreach(QTcpSocket *client, clients)
        client->write(QString("/users:" + userList.join(",") + "\n").toUtf8());
}
