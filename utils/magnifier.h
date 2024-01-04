#ifndef MAGNIFIER_H
#define MAGNIFIER_H

#include <list>

class Magnifier {
    static constexpr int MAX_ITERATION = 3;
public:
    struct Point {
        Point(int _x, int _y) : x(_x), y(_y) {}
        int x;
        int y;
        auto operator<=>(const Point&) const = default;
    };

    const std::list<Point>& getPoints() const { return m_points; }
    void reset() {
        m_points.clear();
    }

    void decrease() {
        if (!m_points.empty()) {
            m_points.pop_front();
        }
    }

    void increase(Point point) {
        if (m_points.size() < MAX_ITERATION) {
            m_points.push_front(point);
        }
    }
    friend bool operator==(const Magnifier&, const Magnifier&);
private:
    std::list<Point> m_points;
};

bool operator==(const Magnifier& lhs, const Magnifier& rhs);

#endif // MAGNIFIER_H
