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

    m_sourceUrl = resolvedSource;

    if (prepareBuffers())
    {
        Q_EMIT sourceChanged();
        render();
    }
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
    return m_displayQImage;
}

void Processor::buildPipeline()
{
    Halide::Var x, y, c;

    m_inputParam.dim(0).set_stride(Halide::Expr{});
    m_inputParam.dim(2).set_stride(1);

    Halide::Func bounded = Halide::BoundaryConditions::constant_exterior(m_inputParam, 0.0f);
    Halide::Expr val = Halide::select(
        c >= m_inputParam.channels(),
        Halide::select(c == 3, 1.0f, 0.0f),
        bounded(x, y, c));

    m_process(x, y, c) = Halide::select(c == 3, val, val * m_intensityParam);

    m_display(x, y, c) = m_process(x, y, c);

    m_display.output_buffer().dim(0).set_stride(4);
    m_display.output_buffer().dim(2).set_stride(1);
}

bool Processor::prepareBuffers()
{
    if (!readInput())
        return false;

    const Rect &bounds = m_inputPixels.dataBounds;
    const int nchannels = m_inputPixels.channels.size();

    auto buffer = Halide::Buffer<float>::make_interleaved(
        m_inputPixels.pixels.data(),
        bounds.w,
        bounds.h,
        nchannels);

    buffer.set_min(bounds.x, bounds.y);

    m_inputParam.set(buffer);

    // TODO: Revisit frame ownership before async rendering.
    // Rn Halide is writing directly to the QImage storage that is shared with the viewer.
    m_displayQImage = QImage(m_displayWidth, m_displayHeight, QImage::Format_RGBA32FPx4);
    m_displayHalideBuffer = Halide::Buffer<float>::make_interleaved(
        reinterpret_cast<float *>(m_displayQImage.bits()),
        m_displayWidth,
        m_displayHeight,
        4);

    return true;
}

bool Processor::readInput()
{
    const auto path = QQmlFile::urlToLocalFileOrQrc(m_sourceUrl).toStdString();
    qDebug() << "Loading: " << path;

    auto reader = ImageReader(path);
    if (!reader)
    {
        qWarning() << "Failed to load '" << path << "' Warnings: "
                   << reader.warnings() << " Errors: "
                   << reader.error();

        return false;
    }

    const std::string layer = "rgba";
    m_inputPixels = reader.read(layer);
    if (m_inputPixels.pixels.empty())
    {
        qWarning() << "Failed to read layer '" << layer << "' Warnings: "
                   << reader.warnings() << " Errors: "
                   << reader.error();

        return false;
    }

    m_displayWidth = reader.width();
    m_displayHeight = reader.height();

    return true;
}

void Processor::render()
{
    if (m_displayQImage.isNull())
        return;

    try
    {
        m_display.realize(m_displayHalideBuffer);
        Q_EMIT outputChanged();
    }
    catch (const Halide::Error &e)
    {
        qCritical() << "Failed to Render - Halide realize error: "
                    << e.what();
        return;
    }
}
