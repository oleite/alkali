#pragma once

#include <QQuickItem>
#include <QImage>
#include <QMutex>
#include <qqml.h>

class ImageRenderItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(int imageWidth READ imageWidth NOTIFY imageChanged)
    Q_PROPERTY(int imageHeight READ imageHeight NOTIFY imageChanged)

public:
    explicit ImageRenderItem(QQuickItem *parent = nullptr);

    QImage image() const;
    void setImage(const QImage &image);
    int imageWidth() const;
    int imageHeight() const;

Q_SIGNALS:
    void imageChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    mutable QMutex m_mutex;
    QImage m_image;
    bool m_textureDirty = false;
};