#include "tests/datasource_fixture.hpp"

bool operator==(const AType& a, const AType& b)
{
    return a.a == b.a && a.b == b.b && a.c == b.c && a.ai == b.ai && std::equal(a.vd.begin(), a.vd.end(), b.vd.begin());
}

std::ostream& operator<<(std::ostream& os, const AType& a)
{
    os << "{ " << a.a << ", " << a.b << ", " << a.c << ", < ";
    for (unsigned int i = 0; i != a.ai.size(); ++i)
        os << a.ai[i] << " ";
    os <<">, ( ";
    for (unsigned int i = 0; i != a.vd.size(); ++i)
        os << a.vd[i] << " ";
    os << ") }";
    return os;
}
std::ostream& operator<<(std::ostream& os, const ATypes& as)
{
    os << '[';
    for (unsigned int i = 0; i != as.size(); ++i)
        os << as[i] << (i+1 == as.size() ? "]": ", ");
    return os;
}
bool operator==(const BType& a, const BType& b)
{
    return a.a == b.a && a.b == b.b && strcmp(a.c, b.c) == 0 && a.ai[3] == b.ai[3] && a.vd[3] == b.vd[3];
}

std::ostream& operator<<(std::ostream& os, const BType& a)
{
    os << "{ " << a.a << ", " << a.b << ", " << a.c << ", < ";
    for (unsigned int i = 0; i != 5; ++i)
        os << a.ai[i] << " ";
    os <<">, ( ";
    for (unsigned int i = 0; i != 10; ++i)
        os << a.vd[i] << " ";
    os << ") }";
    return os;
}
std::ostream& operator<<(std::ostream& os, const BTypes& bs)
{
    os << '[';
    for (unsigned int i = 0; i != bs.size(); ++i)
        os << bs[i] << (i+1 == bs.size() ? "]": ", ");
    return os;
}

bool operator==(const CType& a, const CType& b)
{
    return a.a == b.a && a.b == b.b && std::equal(a.av.begin(), a.av.end(), b.av.begin()) && std::equal(a.bv.begin(), a.bv.end(), b.bv.begin());
}

std::ostream& operator<<(std::ostream& os, const CType& a)
{
    os << "{ (" << a.a << ", " << endl <<"   " << a.b << endl<< "  )"<<endl;
    os << "  <";
    for (unsigned int i = 0; i != 5; ++i)
        os << a.av[i] << " ";
    os <<"  >,"<<endl<<"  <";
    for (unsigned int i = 0; i != 10; ++i)
        os << a.bv[i] << " ";
    os <<"  >"<<endl <<"}";
    return os;
}
std::ostream& operator<<(std::ostream& os, const CTypes& cs)
{
    os << '[';
    for (unsigned int i = 0; i != cs.size(); ++i)
        os << cs[i] << (i+1 == cs.size() ? "]": ", ");
    return os;
}
