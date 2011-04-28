/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  datasource_fixture.hpp

                        datasource_fixture.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_DATASOURCE_FIXTURE_HPP_
#define ORO_DATASOURCE_FIXTURE_HPP_

#include <rtt-config.h>
#include "unit.hpp"
#include <iostream>
#include <RTT.hpp>
#include <boost/array.hpp>
#include <boost/serialization/vector.hpp>

using namespace RTT;
using namespace RTT::detail;
using namespace boost;
using namespace std;

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

    void clear() {
        a = 0;
        b = 0;
        c.clear();
        ai[3] = 0;
        vd.clear();
    }

    int a;
    double b;
    string c;
    boost::array<int, 5> ai;
    vector<double> vd;
};

typedef std::vector<AType> ATypes;


RTT_UNIT_API bool operator==(const AType& a, const AType& b);

RTT_UNIT_API std::ostream& operator<<(std::ostream& os, const AType& a);
RTT_UNIT_API std::ostream& operator<<(std::ostream& os, const ATypes& as);

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
        for(int i = 0; i<10; ++i) c[i] = 0;
        strcpy(c,"hello");
        for(int i = 0; i<5; ++i) ai[i] = 0;
        ai[3] = 99;
        for(int i = 0; i<10; ++i) vd[i] = 5;
        vd[3] = 101;
    }
    void clear() {
        a = 0;
        b = 0;
        for(int i = 0; i<10; ++i) c[i] = 0;
        for(int i = 0; i<5; ++i) ai[i] = 0;
        for(int i = 0; i<10; ++i) vd[i] = 0;
    }

    int    a;
    double b;
    char   c[10];
    int    ai[5];
    double vd[10];
    //int    vvi[2][2];
};
typedef std::vector<BType> BTypes;

RTT_UNIT_API bool operator==(const BType& a, const BType& b);

RTT_UNIT_API std::ostream& operator<<(std::ostream& os, const BType& a);
RTT_UNIT_API std::ostream& operator<<(std::ostream& os, const BTypes& a);

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

    void clear() {
        a.clear();
        b.clear();
        av.clear();
        bv.clear();
    }
};

typedef std::vector<CType> CTypes;

RTT_UNIT_API bool operator==(const CType& a, const CType& b);

RTT_UNIT_API std::ostream& operator<<(std::ostream& os, const CType& a);
RTT_UNIT_API std::ostream& operator<<(std::ostream& os, const CTypes& a);

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, AType & g, const unsigned int version)
{
    ar & make_nvp("a", g.a);
    ar & make_nvp("b", g.b);
    ar & make_nvp("c", g.c );
    ar & make_nvp("ai", g.ai );
    ar & make_nvp("vd", g.vd );
}

template<class Archive>
void serialize(Archive & ar, BType & g, const unsigned int version)
{
    ar & make_nvp("a", g.a);
    ar & make_nvp("b", g.b);
    ar & make_nvp("c", make_array(g.c,10) );
    ar & make_nvp("ai", make_array(g.ai,5) );
    ar & make_nvp("vd", make_array(g.vd,10) );

    //ar & make_nvp("vvi", make_array( g.vvi, 4));
}

template<class Archive>
void serialize(Archive & ar, CType & g, const unsigned int version)
{
    ar & make_nvp("a", g.a);
    ar & make_nvp("b", g.b);
    ar & make_nvp("av", g.av );
    ar & make_nvp("bv", g.bv );
}

} // namespace serialization
} // namespace boost


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
