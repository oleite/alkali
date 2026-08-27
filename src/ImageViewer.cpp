#include "ImageViewer.h"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QMutexLocker>

ImageViewer::ImageViewer(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

QImage ImageViewer::image() const
{
    QMutexLocker lock(&m_mutex);
    return m_image;
}

void ImageViewer::setImage(const QImage &image)
{
    {
        QMutexLocker lock(&m_mutex);
        m_image = image;
        m_textureDirty = true;
    }

    Q_EMIT imageChanged();
    update();
}

QSGNode *ImageViewer::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto *node = static_cast<QSGSimpleTextureNode *>(oldNode);

    if (!node)
    {
        node = new QSGSimpleTextureNode();
        node->setOwnsTexture(true);
        // QImage image{100, 100, QImage::Format_RGBA32FPx4};
        // image.fill(Qt::magenta);
        // node->setTexture(window()->createTextureFromImage(image));
    }

    node->setRect(boundingRect());

    QImage imageCopy;

    {
        QMutexLocker lock(&m_mutex);
        if (!m_textureDirty)
        {
            return node;
        }
        imageCopy = m_image;
        m_textureDirty = false;
    }

    if (imageCopy.isNull() || !window())
    {
        return node;
    }

    QSGTexture *newTexture = window()->createTextureFromImage(imageCopy);

    node->setTexture(newTexture);
    return node;
}
