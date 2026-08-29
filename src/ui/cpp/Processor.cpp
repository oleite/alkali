#include "Processor.h"

#include <QQmlFile>
#include <utility>
#include <QQmlContext>

Processor::Processor()
{
    buildPipeline();
}

QUrl Processor::source() const
{
    return m_sourceUrl;
}

QUrl Processor::resolveSourceUrl(const QUrl &source) const
{
    QUrl resolvedSource = source;

    if (source.isRelative())
    {
        if (auto *context = qmlContext(this))
            resolvedSource = context->resolvedUrl(source);
    }
    return resolvedSource;
}

void Processor::setSource(QUrl source)
{
    const QUrl resolvedSource = resolveSourceUrl(source);

    if (resolvedSource == m_sourceUrl)
        return;

    if (!QQmlFile::isLocalFile(resolvedSource))
    {
        qWarning() << "Unsupported image URL: " << resolvedSource;
        return;
    }

    const QString path = QQmlFile::urlToLocalFileOrQrc(resolvedSource);
    qDebug() << "Loading: " << path;
    m_sourceInput = OIIO::ImageInput::open(path.toStdString());
    if (!m_sourceInput)
    {
        qCritical() << "Failed to load image: " << resolvedSource;
        return;
    }

    m_sourceUrl = resolvedSource;

    prepareBuffers();

    Q_EMIT sourceChanged();
    render();
}

float Processor::intensity() const
{
    return m_intensityParam.get();
}

void Processor::setIntensity(float intensity)
{
    if (intensity == m_intensityParam.get())
        return;

    m_intensityParam.set(intensity);
    Q_EMIT intensityChanged();
    render();
}

QImage Processor::output() const
{
    return m_outputImage;
}

void Processor::buildPipeline()
{

    Halide::Var x, y, c;
    Halide::Expr value = Halide::cast<float>(m_inputParam(x, y, c));
    value = Halide::select(
        c == 3, value,
        value * m_intensityParam);
    m_pipeline(x, y, c) = Halide::cast<float>(value);

    m_inputParam.dim(0).set_stride(4);
    m_inputParam.dim(2).set_stride(1);

    m_pipeline.output_buffer().dim(0).set_stride(4);
    m_pipeline.output_buffer().dim(2).set_stride(1);
}

void Processor::prepareBuffers()
{
    if (!m_sourceInput)
        return;

    const auto &spec = m_sourceInput->spec();
    const int width = spec.width;
    const int height = spec.height;
    const int nchannels = spec.nchannels;

    m_inputPixels.resize(width * height * nchannels);

    m_sourceInput->read_image(
        0,
        0,
        0,
        nchannels,
        OIIO::TypeDesc::FLOAT,
        m_inputPixels.data());

    m_inputParam.set(Halide::Buffer<float>::make_interleaved(
        m_inputPixels.data(),
        width,
        height,
        4));

    // TODO: Revisit frame ownership before async rendering.
    // Rn Halide is writing directly to the QImage storage that is shared with the viewer.
    m_outputImage = QImage(width, height, QImage::Format_RGBA32FPx4);
    float *outputPixels = reinterpret_cast<float *>(m_outputImage.bits());
    m_outputBuffer = Halide::Buffer<float>::make_interleaved(
        outputPixels,
        width,
        height,
        4);
}

void Processor::render()
{
    if (!m_sourceInput || m_outputImage.isNull())
        return;

    try
    {
        m_pipeline.realize(m_outputBuffer);
        Q_EMIT outputChanged();
    }
    catch (const Halide::Error &e)
    {
        qCritical() << "Halide realize error:" << e.what();
        return;
    }
}
