#include "Paths.h"

#include <QDir>
#include <QCoreApplication>

QString Paths::resourcesPath() const
{
    QDir root{QCoreApplication::applicationDirPath()};
#ifdef Q_OS_MACOS
    return root.absoluteFilePath("../Resources");
#else
    return root.absoluteFilePath("resources");
#endif
}

QUrl Paths::testImage(const QString &name) const
{
    return QUrl::fromLocalFile(QDir(resourcesPath()).filePath("test_images/" + name));
}
