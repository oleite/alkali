#include "Processor.h"

Processor::Processor()
{
    initHalide();
}

float Processor::intensity() const
{
    return m_intensity.get();
}

void Processor::setIntensity(float intensity)
{
    if (intensity == m_intensity.get())
        return;

    m_intensity.set(intensity);
    Q_EMIT intensityChanged();
    repaint();
}

QImage Processor::output() const
{
    return m_outputImage;
}

void Processor::initHalide()
{
    Halide::Var x, y, c;
    m_gradient(x, y, c) = Halide::select(
        c == 0, Halide::cast<uint8_t>((Halide::cast<float>(x) / (m_width - 1)) * 255.0f),
        c == 1, Halide::cast<uint8_t>((Halide::cast<float>(y) / (m_height - 1)) * 255.0f),
        c == 2, Halide::cast<uint8_t>(m_intensity * 255.0f),
        c == 3, Halide::cast<uint8_t>(255),
        Halide::cast<uint8_t>(0));
    m_gradient.output_buffer().dim(0).set_stride(4);
    m_gradient.output_buffer().dim(2).set_stride(1);

    m_outputImage = QImage(m_width, m_height, QImage::Format_RGBA8888);

    m_outputBuffer = Halide::Buffer<uint8_t>::make_interleaved(
        m_outputImage.bits(),
        m_width,
        m_height,
        4);

    repaint();
}

void Processor::repaint()
{
    try
    {
        m_gradient.realize(m_outputBuffer);
        Q_EMIT outputChanged();
    }
    catch (const Halide::Error &e)
    {
        qCritical() << "Halide realize error:" << e.what();
        return;
    }
}
