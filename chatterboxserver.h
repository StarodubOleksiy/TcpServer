#ifndef CHATTERBOXSERVER_H
#define CHATTERBOXSERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QTime>

class ChatterBoxServer : public QTcpServer
{
    Q_OBJECT

    public:
        ChatterBoxServer(QObject *parent=0);
        void sendToClient(QTcpSocket* pSocket, const QString& str);

    private slots:
        void readyRead();
        void disconnected();
        void sendUserList();

    protected:
        void incomingConnection(int socketfd);

    private:
        QSet<QTcpSocket*> clients;
        QMap<QTcpSocket*,QString> users;
        quint16     m_nNextBlockSize;
};

#endif // CHATTERBOXSERVER_H
