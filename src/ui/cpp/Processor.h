#pragma once

#include <QObject>
#include <QImage>
#include <qqml.h>
#include <Halide.h>
#include <OpenImageIO/imageio.h>

#include "core/ImageReader.h"

class Processor : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(float intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
    Q_PROPERTY(QImage output READ output NOTIFY outputChanged)

public:
    Processor();

    QUrl source() const;
    void setSource(QUrl source);

    float intensity() const;
    void setIntensity(float intensity);

    QImage output() const;

Q_SIGNALS:
    void sourceChanged();
    void intensityChanged();
    void outputChanged();

private:
    QUrl resolveSourceUrl(const QUrl &source) const;
    void buildPipeline();
    bool prepareBuffers();
    bool readInput();
    void render();

private:
    QUrl m_sourceUrl;

    Halide::Func m_process;
    Halide::Func m_display;

    PixelBlock m_inputPixels;

    Halide::ImageParam m_inputParam{Halide::type_of<float>(), 3, "input_B"};
    Halide::Param<float> m_intensityParam{"intensity", 1.0f};

    Halide::Buffer<float> m_displayHalideBuffer;
    QImage m_displayQImage;

    int m_displayWidth = 0;
    int m_displayHeight = 0;
};