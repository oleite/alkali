#include <filesystem>
#include <QtTest>

#include "core/ImageReader.h"

static std::filesystem::path fixturePath(const char *relativePath)
{
    const QByteArray fixture = QByteArray("fixtures/images/") + relativePath;
    const QString testPath = QFINDTESTDATA(fixture.constData());
    Q_ASSERT(!testPath.isEmpty());

    return std::filesystem::path{testPath.toStdU16String()};
}

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
        ImageReader reader(fixturePath("oiio/tahoe-gps.jpg"));

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
        ImageReader reader(fixturePath("oiio/grid-overscan.exr"));

        QVERIFY(reader);
        QVERIFY(reader.error().empty());

        QCOMPARE(reader.width(), 1500);
        QCOMPARE(reader.height(), 1500);
        QCOMPARE(reader.channelCount(), 4);

        const auto channels = reader.channelNames();

        QCOMPARE(channels.size(), 4);
        QCOMPARE(channels[0], "R");
        QCOMPARE(channels[1], "G");
        QCOMPARE(channels[2], "B");
        QCOMPARE(channels[3], "A");
    }

    void reads_exr_channel_names()
    {
        ImageReader reader(fixturePath("openexr/Beachball/singlepart.0001.exr"));

        QCOMPARE(reader.channelCount(), 20);

        const auto channels = reader.channelNames();

        QCOMPARE(channels[0], "R");
        QCOMPARE(channels[4], "Z");
        QCOMPARE(channels[5], "disparityL.x");
        QCOMPARE(channels[13], "left.R");
        QCOMPARE(channels[18], "whitebarmask.left.mask");
    }
};

QTEST_MAIN(CoreTests)
#include "tst_core.moc"