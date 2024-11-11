#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QSet>
#include <QFileInfo>
#include <QFile>
#include <QProcess>
#include <iostream>
#include <QThread>

// RTSP 서버를 별도의 스레드에서 실행하는 클래스
class RTSPServerThread : public QThread {
    Q_OBJECT

public:
    RTSPServerThread(QObject *parent = nullptr) : QThread(parent) {}

    void run() override {
        GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
        if (!loop) {
            qCritical() << "GMainLoop를 생성할 수 없습니다.";
            return;
        }
        qInfo() << "GMainLoop 생성됨.";

        // GstRTSPServer 생성
        GstRTSPServer *server = gst_rtsp_server_new();
        if (!server) {
            qCritical() << "RTSP 서버를 생성할 수 없습니다.";
            return;
        }
        qInfo() << "RTSP 서버 생성됨.";

        // Mount points 설정
        GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);
        if (!mounts) {
            qCritical() << "Mount points를 얻을 수 없습니다.";
            return;
        }
        qInfo() << "Mount points 설정됨.";

        // 미디어 팩토리 생성 및 파이프라인 설정
        GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();
        if (!factory) {
            qCritical() << "미디어 팩토리를 생성할 수 없습니다.";
            g_object_unref(mounts);
            return;
        }
        qInfo() << "미디어 팩토리 생성됨.";
	// nvarguscamerasrc 파이프라인 설정
        const char *pipeline_desc = "( nvarguscamerasrc ! video/x-raw(memory:NVMM), width=640, height=480, format=NV12, framerate=21/1 ! "
                                    "nvvidconv ! videoconvert ! x264enc tune=zerolatency ! rtph264pay name=pay0 pt=96 )";
	// 아래의 코드에서 GST-RTSP-SERVER를 이용하여 PICAMERA를 STREAMING하려고하는데, gst-launch-1.0 rtspsrc location=rtsp://127.0.0.1:8554/test ! decodebin ! autovideosink 명령어를 실행하면 아래의 gst error가 발생해 서버 코드를 수정해줘




        gst_rtsp_media_factory_set_launch(factory, pipeline_desc);
        gst_rtsp_mount_points_add_factory(mounts, "/test", factory);
        qInfo() << "/test 경로에 미디어 팩토리 마운트됨.";

        g_object_unref(mounts);

        guint id = gst_rtsp_server_attach(server, nullptr);
        if (id == 0) {
            qCritical() << "RTSP 서버를 네트워크에 연결할 수 없습니다.";
            return;
        }
        qInfo() << "RTSP 서버가 rtsp://127.0.0.1:8554/test 에서 실행 중입니다.";

        // GMainLoop 실행 (별도의 스레드에서)
        g_main_loop_run(loop);
    }
};

// QT 서버 클래스
class ChatServer : public QTcpServer {
    Q_OBJECT

public:
    ChatServer(QObject *parent = nullptr) : QTcpServer(parent) {}

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QTcpSocket *clientSocket = new QTcpSocket(this);
        if (!clientSocket->setSocketDescriptor(socketDescriptor)) {
            delete clientSocket;
            return;
        }
        clients.insert(clientSocket);

        // 클라이언트로부터 메시지를 받았을 때
        connect(clientSocket, &QTcpSocket::readyRead, [this, clientSocket]() {
            QByteArray message = clientSocket->readAll();
            QString messageStr = QString::fromUtf8(message);

            QStringList messageList = messageStr.split("\n", QString::SkipEmptyParts);  // '\n'으로 메시지 구분
            for (const QString& msg : messageList) {
                qInfo() << "Received message from client: " << msg;

                if (msg == "WEBCAM_STREAM") {
                    startRTSPServer();
                    qInfo() << "RTSP 서버 실행 및 웹캠 스트림 시작";
                } else if (msg == "PLAY") {
                    qInfo() << "Play button clicked";
                    handlePlayRequest(clientSocket);
                } else {
                    broadcastMessageToClients(msg, clientSocket);  // 다른 클라이언트에게 메시지 전송
                }

            }
        });


        // 클라이언트 연결 해제 시
        connect(clientSocket, &QTcpSocket::disconnected, [this, clientSocket]() {
            clients.remove(clientSocket);
            clientSocket->deleteLater();
        });
    }

private:
    QSet<QTcpSocket *> clients;    // 클라이언트에 RTSP URL 전송
    // RTSP 서버를 별도의 스레드에서 실행
    void startRTSPServer() {
        RTSPServerThread *rtspThread = new RTSPServerThread(this);
        rtspThread->start();  // 별도의 스레드에서 GMainLoop 실행
    }

    void handlePlayRequest(QTcpSocket *clientSocket) {
        QString rtspUrl = "rtsp://127.0.0.1:8554/test";
        clientSocket->write(rtspUrl.toUtf8());  // 클라이언트에 RTSP URL 전송
        qInfo() << "클라이언트에게 RTSP 스트림 URL 전송: " << rtspUrl;
    }
    // 다른 클라이언트에게 메시지를 브로드캐스트하는 함수
    void broadcastMessageToClients(const QString &message, QTcpSocket *sender) {
        for (QTcpSocket *client : clients) {
            if (client != sender) {  // 메시지를 보낸 클라이언트는 제외
                client->write(message.toUtf8());
            }
        }
        qInfo() << "Broadcasted message to other clients: " << message;
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // GStreamer 초기화
    gst_init(&argc, &argv);

    ChatServer server;
    if (!server.listen(QHostAddress::Any, 1234)) {
        qCritical() << "서버를 시작할 수 없습니다.";
        return 1;
    }

    qInfo() << "Chat 서버가 포트 1234에서 시작되었습니다.";
    return app.exec();
}

#include "main.moc"
