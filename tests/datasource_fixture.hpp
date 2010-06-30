#ifndef ORO_DATASOURCE_FIXTURE_HPP_
#define ORO_DATASOURCE_FIXTURE_HPP_

#include "unit.hpp"
#include <boost/array.hpp>

/**
 * AType uses simple C types and C++ containers
 * for any sequence type.
 *
 * Tests cases like ROS messages.
 */
struct AType
{
    AType() :
        a(3), b(9.9), c("hello")
    {
        ai.assign(0);
        ai[3] = 99;
        vd.resize(10, 5.0);
        vd[3] = 101;
    }
    int a;
    double b;
    string c;
    boost::array<int, 5> ai;
    vector<double> vd;
};

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

/**
 * BType uses simple C types and C arrays
 * for any sequence type. Identical values as AType.
 *
 * Tests cases like KDL::Frame
 */
struct BType
{
    BType() :
        a(3), b(9.9)
    {
        strcpy(c,"hello");
        for(int i = 0; i<5; ++i) ai[i] = 0;
        ai[3] = 99;
        for(int i = 0; i<10; ++i) vd[i] = 5;
        vd[3] = 101;
    }
    int    a;
    double b;
    char   c[10];
    int    ai[5];
    double vd[10];
    //int    vvi[2][2];
};

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

/**
 * CType uses composite A/B types
 *
 * Tests cases like vector<KDL::Frame>
 */
struct CType
{
    CType() :
        a(), b(), av(10), bv(10)
    {
    }
    AType a;
    BType b;
    vector<AType> av;
    vector<BType> bv;
};

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
    /**
     * Only for unit tests: only returns new value in get() after updated() has been
     * called. Use this to test the calling of updated() after a set().
     */
    template<typename T>
    class UpdatedReferenceDataSource
        : public DataSource<T>
    {
        // a reference to a value_t
        typename AssignableDataSource<T>::reference_t mref;
        typename DataSource<T>::value_t mcopy;
    public:
        /**
         * Use shared_ptr.
         */
        ~UpdatedReferenceDataSource() {}

        typedef boost::intrusive_ptr<UpdatedReferenceDataSource<T> > shared_ptr;

        UpdatedReferenceDataSource( typename AssignableDataSource<T>::reference_t ref ) : mref(ref), mcopy(ref) {}

        typename DataSource<T>::result_t get() const
        {
            return mcopy;
        }

        typename DataSource<T>::result_t value() const
        {
            return mcopy;
        }

        typename DataSource<T>::const_reference_t rvalue() const
        {
            return mcopy;
        }

        void updated() { mcopy = mref; }

        virtual UpdatedReferenceDataSource<T>* clone() const { return new UpdatedReferenceDataSource(mref); }

        virtual UpdatedReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const { return clone(); }

    };

#endif /* ORO_DATASOURCE_FIXTURE_HPP_ */
