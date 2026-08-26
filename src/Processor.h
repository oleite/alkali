#pragma once

#include <QObject>
#include <QImage>
#include <qqml.h>
#include <Halide.h>


class Processor : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(float intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
    Q_PROPERTY(QImage output READ output NOTIFY outputChanged)

public:
    Processor();

    float intensity() const;
    void setIntensity(float intensity);

    QImage output() const;
    

Q_SIGNALS:
    void intensityChanged();
    void outputChanged();

private:
    void initHalide();
    void repaint();

private:
    Halide::Func m_gradient;

    Halide::Param<float> m_intensity{"intensity", .5f};
    int m_width = 800;
    int m_height = 600;

    Halide::Buffer<uint8_t> m_outputBuffer;
    QImage m_outputImage;
};