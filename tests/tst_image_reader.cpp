#include <algorithm>
#include <cmath>
#include <filesystem>
#include <initializer_list>
#include <source_location>
#include <string>
#include <utility>
#include <vector>

#include <QtTest>

#include "core/ImageReader.h"

namespace QTest
{
    template <>
    char *toString(const Rect &rect)
    {
        const QByteArray text = QByteArrayLiteral("Rect{") + QByteArray::number(rect.x) + QByteArrayLiteral(", ") + QByteArray::number(rect.y) + QByteArrayLiteral(", ") + QByteArray::number(rect.w) + QByteArrayLiteral(", ") + QByteArray::number(rect.h) + QByteArrayLiteral("}");

        return qstrdup(text.constData());
    }
}

static std::filesystem::path fixturePath(const char *relativePath)
{
    const QByteArray fixture = QByteArray("fixtures/images/") + relativePath;
    const QString testPath = QFINDTESTDATA(fixture.constData());
    Q_ASSERT(!testPath.isEmpty());

    return std::filesystem::path{testPath.toStdU16String()};
}

static void compareNames(
    std::vector<std::string> actual,
    std::vector<std::string> expected,
    std::source_location location = std::source_location::current())
{
    std::sort(actual.begin(), actual.end());
    std::sort(expected.begin(), expected.end());

    QTest::qCompare(
        actual,
        expected,
        "actual names",
        "expected names",
        location.file_name(),
        static_cast<int>(location.line()));
}

static void compareChannels(
    const ImageReader &reader,
    std::initializer_list<std::pair<LayerName, std::vector<ChannelName>>> expected,
    std::source_location location = std::source_location::current())
{
    for (const auto &[layer, channels] : expected)
    {
        const auto actual = reader.channels(layer);
        const QByteArray actualExpression =
            QByteArrayLiteral("reader.channels(\"") +
            QByteArray::fromStdString(layer) +
            QByteArrayLiteral("\")");

        QTest::qCompare(
            actual,
            channels,
            actualExpression.constData(),
            "expected channels",
            location.file_name(),
            static_cast<int>(location.line()));
    }
}

static void comparePixelBlock(
    const PixelBlock &block,
    const Rect &dataBounds,
    const std::vector<ChannelName> &channels,
    std::size_t pixelCount,
    std::source_location location = std::source_location::current())
{
    QTest::qCompare(block.dataBounds, dataBounds,
                    "block.dataBounds", "dataBounds",
                    location.file_name(), static_cast<int>(location.line()));
    QTest::qCompare(block.channels, channels,
                    "block.channels", "channels",
                    location.file_name(), static_cast<int>(location.line()));
    QTest::qCompare(block.pixels.size(), pixelCount,
                    "block.pixels.size()", "pixelCount",
                    location.file_name(), static_cast<int>(location.line()));
}

class ImageReaderTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void rejects_missing_file()
    {
        ImageReader reader("non/existing/file.exr");

        QVERIFY(!reader);
        QVERIFY(!reader.error().empty());
        QVERIFY(reader.layers().empty());
    }

    void reads_jpeg_as_rgba_layer()
    {
        ImageReader reader(fixturePath("oiio/tahoe-gps.jpg"));

        QVERIFY(reader);
        QVERIFY(reader.error().empty());
        compareNames(reader.layers(), {"rgba"});
        QCOMPARE(reader.width(), 2048);
        QCOMPARE(reader.height(), 1536);
        compareChannels(reader, {{"rgba", {"R", "G", "B"}}});

        const PixelBlock block = reader.read("rgba");

        comparePixelBlock(block, {0, 0, 2048, 1536},
                          {"R", "G", "B"}, 2048 * 1536 * 3);
        QVERIFY(reader.error().empty());
    }

    void reads_ordinary_rgba_exr()
    {
        ImageReader reader(fixturePath("openexr/TestImages/stripes.exr"));

        QVERIFY(reader);
        compareNames(reader.layers(), {"rgba"});
        QCOMPARE(reader.width(), 100);
        QCOMPARE(reader.height(), 50);

        const PixelBlock block = reader.read("rgba");

        comparePixelBlock(block, {0, 0, 100, 50},
                          {"R", "G", "B", "A"}, 100 * 50 * 4);
    }

    void preserves_exr_overscan_data_bounds()
    {
        ImageReader reader(fixturePath("oiio/grid-overscan.exr"));

        QVERIFY(reader);
        QCOMPARE(reader.width(), 1000);
        QCOMPARE(reader.height(), 1000);

        const PixelBlock block = reader.read("rgba");

        comparePixelBlock(block, {-250, -250, 1500, 1500},
                          {"R", "G", "B", "A"}, 1500 * 1500 * 4);
    }

    void normalizes_nonzero_exr_display_origin()
    {
        ImageReader reader(fixturePath("openexr/DisplayWindow/t07.exr"));

        QVERIFY(reader);
        QCOMPARE(reader.width(), 481);
        QCOMPARE(reader.height(), 371);

        const PixelBlock block = reader.read("rgba");

        comparePixelBlock(block, {40, 40, 400, 300},
                          {"R", "G", "B"}, 400 * 300 * 3);
    }

    void interprets_singlepart_exr_layers()
    {
        ImageReader reader(fixturePath("openexr/Beachball/singlepart.0001.exr"));

        QVERIFY(reader);
        compareNames(reader.layers(), {
                                          "rgba",
                                          "depth",
                                          "disparityL",
                                          "disparityR",
                                          "forward.left",
                                          "forward.right",
                                          "left",
                                          "whitebarmask.left",
                                          "whitebarmask.right",
                                      });
        compareChannels(reader, {
                                    {"rgba", {"R", "G", "B", "A"}},
                                    {"depth", {"Z"}},
                                    {"disparityL", {"x", "y"}},
                                    {"forward.left", {"u", "v"}},
                                    {"forward.right", {"u", "v"}},
                                    {"left", {"R", "G", "B", "A", "Z"}},
                                    {"whitebarmask.left", {"mask"}},
                                    {"whitebarmask.right", {"mask"}},
                                });

        const PixelBlock forward = reader.read("forward.left");

        comparePixelBlock(forward, {654, 245, 911, 876},
                          {"u", "v"}, 911 * 876 * 2);
    }

    void puts_unqualified_nonstandard_channel_in_other()
    {
        ImageReader reader(
            fixturePath("openexr/TestImages/GrayRampsHorizontal.exr"));

        QVERIFY(reader);
        compareNames(reader.layers(), {"other"});
        compareChannels(reader, {{"other", {"Y"}}});

        const PixelBlock block = reader.read("other");

        comparePixelBlock(block, {0, 0, 800, 800}, {"Y"}, 800 * 800);
    }

    void reconstructs_multipart_exr_layers()
    {
        ImageReader reader(fixturePath("openexr/Beachball/multipart.0001.exr"));

        QVERIFY(reader);
        compareNames(reader.layers(), {
                                          "rgba_right",
                                          "depth_left",
                                          "forward_left",
                                          "whitebarmask_left",
                                          "rgba_left",
                                          "depth_right",
                                          "forward_right",
                                          "disparityL",
                                          "disparityR",
                                          "whitebarmask_right",
                                      });
        QCOMPARE(reader.width(), 2048);
        QCOMPARE(reader.height(), 1556);
        compareChannels(reader, {
                                    {"rgba_right", {"R", "G", "B", "A"}},
                                    {"depth_left", {"Z"}},
                                    {"forward_left", {"u", "v"}},
                                    {"forward_right", {"u", "v"}},
                                    {"whitebarmask_left", {"mask"}},
                                    {"whitebarmask_right", {"mask"}},
                                });
    }

    void reconstructs_multiview_layers()
    {
        ImageReader reader(fixturePath("openexr/MultiView/Adjuster.exr"));

        QVERIFY(reader);
        compareNames(reader.layers(), {"rgba", "left", "right"});
        compareChannels(reader, {
                                    {"rgba", {"R", "G", "B"}},
                                    {"left", {"R", "G", "B"}},
                                    {"right", {"R", "G", "B"}},
                                });

        const PixelBlock left = reader.read("left");

        comparePixelBlock(left, {0, 0, 775, 678},
                          {"R", "G", "B"}, 775 * 678 * 3);
    }

    void reads_base_level_from_mipmapped_exr()
    {
        ImageReader reader(
            fixturePath("openexr/MultiResolution/ColorCodedLevels.exr"));

        QVERIFY(reader);
        QCOMPARE(reader.width(), 512);
        QCOMPARE(reader.height(), 512);

        const PixelBlock block = reader.read("rgba");

        comparePixelBlock(block, {0, 0, 512, 512},
                          {"R", "G", "B", "A"}, 512 * 512 * 4);
    }

    void preserves_cube_map_data_outside_display_window()
    {
        ImageReader reader(
            fixturePath("openexr/MultiResolution/KernerEnvCube.exr"));

        QVERIFY(reader);
        QCOMPARE(reader.width(), 256);
        QCOMPARE(reader.height(), 256);

        const PixelBlock block = reader.read("rgba");

        comparePixelBlock(block, {0, 0, 256, 1536},
                          {"R", "G", "B", "A"}, 256 * 1536 * 4);
    }

    void preserves_nonfinite_float_values()
    {
        ImageReader reader(
            fixturePath("openexr/TestImages/BrightRingsNanInf.exr"));

        QVERIFY(reader);

        const PixelBlock block = reader.read("rgba");
        const auto nanCount = std::count_if(
            block.pixels.cbegin(), block.pixels.cend(),
            [](float value) { return std::isnan(value); });
        const auto infCount = std::count_if(
            block.pixels.cbegin(), block.pixels.cend(),
            [](float value) { return std::isinf(value); });

        comparePixelBlock(block, {0, 0, 800, 800},
                          {"R", "G", "B"}, 800 * 800 * 3);
        QCOMPARE(nanCount, std::ptrdiff_t{6});
        QCOMPARE(infCount, std::ptrdiff_t{12});
    }

    void reads_multipart_layers_as_independent_pixel_blocks()
    {
        ImageReader reader(fixturePath("openexr/Beachball/multipart.0001.exr"));

        QVERIFY(reader);
        QCOMPARE(reader.layers(), (std::vector<std::string>{"rgba_right",
                                                            "depth_left",
                                                            "forward_left",
                                                            "whitebarmask_left",
                                                            "rgba_left",
                                                            "depth_right",
                                                            "forward_right",
                                                            "disparityL",
                                                            "disparityR",
                                                            "whitebarmask_right"}));

        const PixelBlock rgba = reader.read("rgba_right");
        comparePixelBlock(rgba, {654, 245, 877, 876},
                          {"R", "G", "B", "A"}, 877 * 876 * 4);

        const PixelBlock depth = reader.read("depth_left");
        comparePixelBlock(depth, {688, 245, 877, 876},
                          {"Z"}, 877 * 876);
    }

    void reports_unknown_layer_reads_without_returning_pixels()
    {
        ImageReader reader(fixturePath("openexr/TestImages/stripes.exr"));

        const PixelBlock block = reader.read("missing");

        comparePixelBlock(block, {}, {}, 0);
        QCOMPARE(reader.warnings().size(), std::size_t{1});
        QVERIFY(reader.warnings().front().find("missing") != std::string::npos);
    }
};

QTEST_MAIN(ImageReaderTests)
#include "tst_image_reader.moc"
