#include <QCoreApplication>
#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QPalette>

#include <Plasma/Theme>

#include "Client.hpp"
#include "TimelinePostModel.hpp"

int main(int argc, char *argv[]) {
     QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

     QApplication app(argc, argv);

     qmlRegisterType<TimelinePostModel>("ch.kotobank", 1, 0, "TimelinePostModel");

     QQmlApplicationEngine engine;
     auto context = engine.rootContext();

     Client client(engine, QUrl("https://pl.kotobank.ch"));
     context->setContextProperty("Client", &client);

     engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

     client.create_app();

     return app.exec();
}
