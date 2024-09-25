#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QSet>

class ChatServer : public QTcpServer {
    Q_OBJECT

public:
    ChatServer(QObject *parent = nullptr) : QTcpServer(parent) {}

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QTcpSocket *clientSocket = new QTcpSocket(this);
        clientSocket->setSocketDescriptor(socketDescriptor);
        clients.insert(clientSocket);

        // 클라이언트로부터 메시지를 받았을 때
        connect(clientSocket, &QTcpSocket::readyRead, [this, clientSocket]() {
            QByteArray message = clientSocket->readAll();
            broadcastMessage(message, clientSocket);
        });

        // 클라이언트 연결이 끊어졌을 때
        connect(clientSocket, &QTcpSocket::disconnected, [this, clientSocket]() {
            clients.remove(clientSocket);
            clientSocket->deleteLater();
        });
    }

private:
    QSet<QTcpSocket *> clients;

    // 메시지 브로드캐스트 함수
    void broadcastMessage(const QByteArray &message, QTcpSocket *sender) {
        for (QTcpSocket *client : clients) {
            if (client != sender) { // 메시지를 보낸 클라이언트는 제외
                client->write(message);
            }
        }
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    ChatServer server;
    if (!server.listen(QHostAddress::Any, 1234)) {
        qCritical() << "Unable to start server";
        return 1;
    }

    qInfo() << "Chat server started on port 1234";
    return app.exec();
}

#include "main.moc"
