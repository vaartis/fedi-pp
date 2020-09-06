#include <QApplication>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QPalette>

#include <Plasma/Theme>

#include "client.hpp"

int main(int argc, char *argv[]) {
     QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

     QGuiApplication app(argc, argv);

     QQmlApplicationEngine engine;
     auto context = engine.rootContext();

     Client client(engine, QUrl("https://pl.kotobank.ch/"));
     context->setContextProperty("Client", &client);

     engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

     client.create_app();

     return app.exec();
}
