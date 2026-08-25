#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <Halide.h>
#include <QtGui>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    int width = 800;
    int height = 600;

    Halide::Func gradient;
    Halide::Var x, y, c;
    Halide::Expr normalized = Halide::cast<float>(x + y) / (width + height - 2);
    Halide::Expr value = Halide::cast<uint8_t>(normalized * 255.0f);

    gradient(x, y, c) = Halide::select(
        c == 0, Halide::cast<uint8_t>((Halide::cast<float>(x) / (width-1)) * 255.0f),
        c == 1, Halide::cast<uint8_t>((Halide::cast<float>(y) / (height-1)) * 255.0f),
        c == 2, Halide::cast<uint8_t>(0),
        c == 3, Halide::cast<uint8_t>(255),
        Halide::cast<uint8_t>(0)
    );
    gradient.output_buffer().dim(0).set_stride(4);
    gradient.output_buffer().dim(2).set_stride(1);

    QImage image(width, height, QImage::Format_RGBA8888);
    auto output = Halide::Buffer<uint8_t>::make_interleaved(
        image.bits(),
        width,
        height,
        4);

    try
    {
        gradient.realize(output);
    }
    catch (const Halide::Error &e)
    {
        qDebug() << "Halide error:" << e.what();
    }

    QQmlApplicationEngine engine;
    engine.loadFromModule("Alkali", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *root = engine.rootObjects().first();
    QObject *viewer = root->findChild<QObject *>("viewer");

    if (viewer)
    {
        viewer->setProperty("image", image);
    }

    return app.exec();
}
