#include <filesystem>
#include <QtTest>

#include "core/ImageReader.h"

class CoreTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_tahoe_gps()
    {
        const QString testPath = QFINDTESTDATA("fixtures/images/oiio/tahoe-gps.jpg");
        QVERIFY(!testPath.isEmpty());
        const std::filesystem::path path{testPath.toStdU16String()};

        ImageReader reader(path);

        QVERIFY(reader);
        QCOMPARE(reader.width(), 2048);
        QCOMPARE(reader.height(), 1536);
        QCOMPARE(reader.channelCount(), 3);
    }

    void test_grid_overscan()
    {
        const QString testPath = QFINDTESTDATA("fixtures/images/oiio/grid-overscan.exr");
        QVERIFY(!testPath.isEmpty());
        const std::filesystem::path path{testPath.toStdU16String()};

        ImageReader reader(path);

        QVERIFY(reader);
        QCOMPARE(reader.width(), 1500);
        QCOMPARE(reader.height(), 1500);
        QCOMPARE(reader.channelCount(), 4);
    }
};

QTEST_MAIN(CoreTests)
#include "tst_core.moc"