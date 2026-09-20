#include <memory>

#include <QtTest>

#include "core/Rect.h"

namespace QTest
{
    template <>
    char *toString(const Rect &rect)
    {
        const QByteArray text = QByteArrayLiteral("Rect{") + QByteArray::number(rect.x) + QByteArrayLiteral(", ") + QByteArray::number(rect.y) + QByteArrayLiteral(", ") + QByteArray::number(rect.w) + QByteArrayLiteral(", ") + QByteArray::number(rect.h) + QByteArrayLiteral("}");

        return qstrdup(text.constData());
    }
}

class RectTests : public QObject
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
};

QTEST_MAIN(RectTests)
#include "tst_rect.moc"
