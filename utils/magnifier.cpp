#include "magnifier.h"

bool operator==(const Magnifier& lhs, const Magnifier& rhs)
{
    return lhs.m_points == rhs.m_points;
}
