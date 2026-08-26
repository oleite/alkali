#pragma once

#include <QQuickItem>
#include <QImage>
#include <QMutex>
#include <qqml.h>

class ImageViewer : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit ImageViewer(QQuickItem *parent = nullptr);

    QImage image() const;
    void setImage(const QImage &image);

Q_SIGNALS:
    void imageChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    mutable QMutex m_mutex;
    QImage m_image;
    bool m_textureDirty = false;
};