#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "app_controller.hpp"
#include "logger.hpp"

int main(int argc, char *argv[]) {
    // 1. Initialize Logger first to catch startup issues
    talky::Logger::Init();
    TALKY_INFO("Talky Application Launching...");

    QGuiApplication app(argc, argv);
    
    // 2. Setup the bridge
    AppController controller;
    
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("App", &controller);

    // 3. Load UI from relative path
    const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    // 4. Start core logic
    controller.start();

    return app.exec();
}
