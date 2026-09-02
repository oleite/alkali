#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <source_location>

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

    const auto toString = [](const std::vector<std::string> &names)
    {
        QStringList result;

        for (const auto &name : names)
            result.append(QString::fromStdString(name));

        return result.join(", ");
    };

    const QString actualString = toString(actual);
    const QString expectedString = toString(expected);

    QTest::qCompare(
        actualString,
        expectedString,
        "actual",
        "expected",
        location.file_name(),
        static_cast<int>(location.line()));
}

class CoreTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void formats_rect_values_for_test_failures()
    {
        const std::unique_ptr<char[]> formatted{QTest::toString(Rect{1, 2, 3, 4})};

        QVERIFY(formatted);
        QCOMPARE(formatted.get(), "Rect{1, 2, 3, 4}");
    }

    void value_initialized_rect_is_empty()
    {
        const Rect empty{};

        QCOMPARE(empty, (Rect{0, 0, 0, 0}));
    }

    void unites_overlapping_rects()
    {
        const Rect a{0, 0, 10, 10};
        const Rect b{5, -5, 10, 20};

        QCOMPARE(Rect::getUnion(a, b), (Rect{0, -5, 15, 20}));
    }

    void unites_disjoint_rects()
    {
        const Rect a{0, 0, 10, 10};
        const Rect b{20, 30, 5, 5};

        QCOMPARE(Rect::getUnion(a, b), (Rect{0, 0, 25, 35}));
    }

    void uses_empty_rect_as_union_identity()
    {
        const Rect empty{};
        const Rect area{10, 20, 30, 40};

        QCOMPARE(Rect::getUnion(empty, area), area);
        QCOMPARE(Rect::getUnion(area, empty), area);
    }

    void treats_nonpositive_rects_as_empty_for_union()
    {
        const Rect area{10, 20, 30, 40};

        QCOMPARE(Rect::getUnion(Rect{5, 5, 0, 10}, area), area);
        QCOMPARE(Rect::getUnion(Rect{5, 5, 10, 0}, area), area);
        QCOMPARE(Rect::getUnion(Rect{5, 5, -10, 10}, area), area);
        QCOMPARE(Rect::getUnion(Rect{5, 5, 10, -10}, area), area);
    }

    void intersects_overlapping_rects()
    {
        const Rect a{0, 0, 10, 10};
        const Rect b{5, -5, 10, 20};

        QCOMPARE(Rect::getIntersection(a, b), (Rect{5, 0, 5, 10}));
    }

    void returns_empty_for_disjoint_intersection()
    {
        const Rect a{0, 0, 10, 10};
        const Rect b{20, 30, 5, 5};

        QCOMPARE(Rect::getIntersection(a, b), Rect{});
        QCOMPARE(Rect::getIntersection(b, a), Rect{});
    }

    void returns_empty_for_edge_touching_intersection()
    {
        const Rect area{0, 0, 10, 10};
        const Rect touchesRight{10, 2, 5, 4};
        const Rect touchesBottom{2, 10, 4, 5};

        QCOMPARE(Rect::getIntersection(area, touchesRight), Rect{});
        QCOMPARE(Rect::getIntersection(area, touchesBottom), Rect{});
    }

    void handles_negative_rect_origins()
    {
        const Rect a{-10, -20, 20, 30};
        const Rect b{-5, -25, 15, 10};

        QCOMPARE(Rect::getUnion(a, b), (Rect{-10, -25, 20, 35}));
        QCOMPARE(Rect::getIntersection(a, b), (Rect{-5, -20, 15, 5}));
    }

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

        const Image image = reader.readAll();

        QCOMPARE(image.width(), 2048);
        QCOMPARE(image.height(), 1536);
        QCOMPARE(image.dataBounds(), (Rect{0, 0, 2048, 1536}));
        compareNames(image.layers(), {"rgba"});
        compareNames(image.channels("rgba"), {"R", "G", "B"});
    }

    void reads_ordinary_rgba_exr()
    {
        ImageReader reader(fixturePath("openexr/TestImages/stripes.exr"));

        QVERIFY(reader);
        compareNames(reader.layers(), {"rgba"});

        const Image image = reader.readAll();

        QCOMPARE(image.width(), 100);
        QCOMPARE(image.height(), 50);
        QCOMPARE(image.dataBounds(), (Rect{0, 0, 100, 50}));
        compareNames(image.layers(), {"rgba"});
        compareNames(image.channels("rgba"), {"R", "G", "B", "A"});
    }

    void preserves_exr_overscan_data_bounds()
    {
        ImageReader reader(fixturePath("oiio/grid-overscan.exr"));

        QVERIFY(reader);

        const Image image = reader.readAll();

        QCOMPARE(image.width(), 1000);
        QCOMPARE(image.height(), 1000);
        QCOMPARE(image.dataBounds(), (Rect{-250, -250, 1500, 1500}));
        compareNames(image.layers(), {"rgba"});
        compareNames(image.channels("rgba"), {"R", "G", "B", "A"});
    }

    void normalizes_nonzero_exr_display_origin()
    {
        ImageReader reader(fixturePath("openexr/DisplayWindow/t07.exr"));

        QVERIFY(reader);

        const Image image = reader.readAll();

        QCOMPARE(image.width(), 481);
        QCOMPARE(image.height(), 371);
        QCOMPARE(image.dataBounds(), (Rect{40, 40, 400, 300}));
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

        const Image image = reader.readAll();

        compareNames(image.layers(), reader.layers());
        compareNames(image.channels("rgba"), {"R", "G", "B", "A"});
        compareNames(image.channels("depth"), {"Z"});
        compareNames(image.channels("disparityL"), {"x", "y"});
        compareNames(image.channels("forward.left"), {"u", "v"});
        compareNames(image.channels("forward.right"), {"u", "v"});
        compareNames(image.channels("left"), {"R", "G", "B", "A", "Z"});
        compareNames(image.channels("whitebarmask.left"), {"mask"});
        compareNames(image.channels("whitebarmask.right"), {"mask"});
    }

    void puts_unqualified_nonstandard_channel_in_other()
    {
        ImageReader reader(
            fixturePath("openexr/TestImages/GrayRampsHorizontal.exr"));

        QVERIFY(reader);
        compareNames(reader.layers(), {"other"});

        const Image image = reader.readAll();

        compareNames(image.layers(), {"other"});
        compareNames(image.channels("other"), {"Y"});
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

        const Image image = reader.readAll();

        QCOMPARE(image.width(), 2048);
        QCOMPARE(image.height(), 1556);
        //  Union of all ten source data windows. The display origin is already (0, 0).
        QCOMPARE(image.dataBounds(), (Rect{654, 245, 911, 876}));
        compareNames(image.layers(), reader.layers());
        compareNames(image.channels("rgba_right"), {"R", "G", "B", "A"});
        compareNames(image.channels("depth_left"), {"Z"});
        compareNames(image.channels("forward_left"), {"u", "v"});
        compareNames(image.channels("forward_right"), {"u", "v"});
        compareNames(image.channels("whitebarmask_left"), {"mask"});
        compareNames(image.channels("whitebarmask_right"), {"mask"});
    }

    void reads_selected_multipart_layers()
    {
        ImageReader reader(fixturePath("openexr/Beachball/multipart.0001.exr"));

        QVERIFY(reader);

        const Image rgba = reader.read({"rgba_right"});
        QCOMPARE(rgba.width(), 2048);
        QCOMPARE(rgba.height(), 1556);
        compareNames(rgba.layers(), {"rgba_right"});
        compareNames(rgba.channels("rgba_right"), {"R", "G", "B", "A"});
        QCOMPARE(rgba.dataBounds(), (Rect{654, 245, 877, 876}));

        const Image depth = reader.read({"depth_left"});
        QCOMPARE(depth.width(), 2048);
        QCOMPARE(depth.height(), 1556);
        compareNames(depth.layers(), {"depth_left"});
        compareNames(depth.channels("depth_left"), {"Z"});
        QCOMPARE(depth.dataBounds(), (Rect{688, 245, 877, 876}));

        const Image combined = reader.read({"rgba_right", "depth_left"});
        QCOMPARE(combined.width(), 2048);
        QCOMPARE(combined.height(), 1556);
        compareNames(combined.layers(), {"rgba_right", "depth_left"});
        compareNames(combined.channels("rgba_right"), {"R", "G", "B", "A"});
        compareNames(combined.channels("depth_left"), {"Z"});
        QCOMPARE(combined.dataBounds(), (Rect{654, 245, 911, 876}));
    }
};

QTEST_MAIN(CoreTests)
#include "tst_core.moc"
