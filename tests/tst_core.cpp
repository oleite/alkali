#include <filesystem>
#include <QtTest>

#include "core/ImageReader.h"

class CoreTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void rejects_missing_file()
    {
        ImageReader reader("non/existing/file.exr");

        QVERIFY(!reader);
        QVERIFY(!reader.error().empty());

        QVERIFY(reader.channelNames().empty());
        QCOMPARE(reader.width(), 0);
        QCOMPARE(reader.height(), 0);
        QCOMPARE(reader.channelCount(), 0);
    }
    void reads_jpeg_metadata()
    {
        const QString testPath = QFINDTESTDATA("fixtures/images/oiio/tahoe-gps.jpg");
        QVERIFY(!testPath.isEmpty());
        const std::filesystem::path path{testPath.toStdU16String()};

        ImageReader reader(path);

        QVERIFY(reader);
        QVERIFY(reader.error().empty());

        QCOMPARE(reader.width(), 2048);
        QCOMPARE(reader.height(), 1536);
        QCOMPARE(reader.channelCount(), 3);

        const auto channels = reader.channelNames();

        QCOMPARE(channels.size(), 3);
        QCOMPARE(channels[0], "R");
        QCOMPARE(channels[1], "G");
        QCOMPARE(channels[2], "B");
    }

    void reads_exr_metadata()
    {
        const QString testPath = QFINDTESTDATA("fixtures/images/oiio/grid-overscan.exr");
        QVERIFY(!testPath.isEmpty());
        const std::filesystem::path path{testPath.toStdU16String()};

        ImageReader reader(path);

        QVERIFY(reader);
        QVERIFY(reader.error().empty());

        QCOMPARE(reader.width(), 1500);
        QCOMPARE(reader.height(), 1500);
        QCOMPARE(reader.channelCount(), 4);
    }
};

QTEST_MAIN(CoreTests)
#include "tst_core.moc"