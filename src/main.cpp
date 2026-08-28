#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Basic");

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.loadFromModule("Alkali", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
