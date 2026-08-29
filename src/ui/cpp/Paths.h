#pragma once

#include <QObject>
#include <QUrl>
#include <qqml.h>

class Paths : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    Q_INVOKABLE QUrl testImage(const QString &name) const;

private:
    QString resourcesPath() const;
};