/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  RealTimeTypekit.cxx

                        RealTimeTypekit.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "rtt-typekit-config.h"
#include "RealTimeTypekit.hpp"
#include "../types/Types.hpp"
#include "../FlowStatus.hpp"
#include "../internal/DataSources.hpp"
#include "../typekit/Types.hpp"
#include "../rtt-fwd.hpp"
#include "../internal/mystd.hpp"
#include "../types/TemplateConstructor.hpp"
#ifdef OS_RT_MALLOC
#include "../rt_string.hpp"
#endif

namespace RTT
{
    using namespace std;
    using namespace detail;

    namespace {
#ifndef ORO_EMBEDDED
        // CONSTRUCTORS
        struct array_ctor
            : public std::unary_function<int, const std::vector<double>&>
        {
            typedef const std::vector<double>& (Signature)( int );
            mutable boost::shared_ptr< std::vector<double> > ptr;
            array_ctor()
                : ptr( new std::vector<double>() ) {}
            const std::vector<double>& operator()( int size ) const
            {
                ptr->resize( size );
                return *(ptr);
            }
        };

        /**
         * See NArityDataSource which requires a function object like
         * this one.
         */
        struct array_varargs_ctor
        {
            typedef const std::vector<double>& result_type;
            typedef double argument_type;
            result_type operator()( const std::vector<double>& args ) const
            {
                return args;
            }
        };

        /**
         * Helper DataSource for constructing arrays with a variable number of
         * parameters.
         */
        typedef NArityDataSource<array_varargs_ctor> ArrayDataSource;

        /**
         * Constructs an array with \a n elements, which are given upon
         * construction time.
         */
        struct ArrayBuilder
            : public TypeConstructor
        {
            virtual DataSourceBase::shared_ptr build(const std::vector<DataSourceBase::shared_ptr>& args) const {
                if (args.size() == 0 )
                    return DataSourceBase::shared_ptr();
                ArrayDataSource::shared_ptr vds = new ArrayDataSource();
                for(unsigned int i=0; i != args.size(); ++i) {
                    DataSource<double>::shared_ptr dsd = boost::dynamic_pointer_cast< DataSource<double> >( args[i] );
                    if (dsd)
                        vds->add( dsd );
                    else
                        return DataSourceBase::shared_ptr();
                }
                return vds;
            }

        };

        struct array_ctor2
            : public std::binary_function<int, double, const std::vector<double>&>
        {
            typedef const std::vector<double>& (Signature)( int, double );
            mutable boost::shared_ptr< std::vector<double> > ptr;
            array_ctor2()
                : ptr( new std::vector<double>() ) {}
            const std::vector<double>& operator()( int size, double value ) const
            {
                ptr->resize( size );
                ptr->assign( size, value );
                return *(ptr);
            }
        };

        double float_to_double( float val ) {return double(val);}
        float double_to_float( double val ) {return float(val);}

        int float_to_int(float f) { return int(f); }
        float int_to_float(int i) { return float(i); }
        int double_to_int(double f) { return int(f); }
        double int_to_double(int i) { return double(i); }
        unsigned int int_to_uint(int i) { return (unsigned int)(i); }
        int uint_to_int(unsigned int ui) { return int(ui); }

        // llong
        long long float_to_llong(float f) { return (long long)(f); }
        float llong_to_float(long long i) { return (float)(i); }
        long long double_to_llong(double f) { return (long long)(f); }
        double llong_to_double(long long i) { return (double)(i); }
        long long int_to_llong(int i) { return (long long)(i); }
        int llong_to_int(long long i) { return (int)(i); }
        long long uint_to_llong(unsigned int i) { return (long long)(i); }
        unsigned int llong_to_uint(long long i) { return (unsigned int)(i); }
        bool llong_to_bool(long long i) { return i != 0; }

        // ullong
        unsigned long long float_to_ullong(float f) { return (unsigned long long)(f); }
        float ullong_to_float(unsigned long long i) { return (float)(i); }
        unsigned long long double_to_ullong(double f) { return (unsigned long long)(f); }
        double ullong_to_double(unsigned long long i) { return (double)(i); }
        unsigned long long int_to_ullong(int i) { return (unsigned long long)(i); }
        int ullong_to_int(unsigned long long i) { return (int)(i); }
        unsigned long long uint_to_ullong(unsigned int i) { return (unsigned long long)(i); }
        unsigned int ullong_to_uint(unsigned long long i) { return (unsigned int)(i); }
        unsigned long long llong_to_ullong(long long i) { return (unsigned long long)(i); }
        long long ullong_to_llong(unsigned long long i) { return (long long)(i); }

#endif
        bool flowstatus_to_bool(FlowStatus fs) { return fs != NoData ; }
        bool writestatus_to_bool(WriteStatus fs) { return fs == WriteSuccess ; }
        bool send_to_bool(SendStatus ss) { return ss == SendSuccess; }
        bool int_to_bool(int i) { return i != 0; }
        int bool_to_int(bool b) { return int(b); }

        struct string_ctor
            : public std::unary_function<int, std::string>
        {
            typedef std::string (Signature)( int );
            std::string operator()( int size ) const
            {
                return std::string( size, std::string::value_type() );
            }
        };

#ifdef OS_RT_MALLOC
        struct rt_string_ctor_int
            : public std::unary_function<int, RTT::rt_string>
        {
            typedef rt_string (Signature)( int );
            rt_string operator()( int size ) const
            {
                return rt_string( size, rt_string::value_type() );
            }
        };

        struct rt_string_ctor_string
            : public std::unary_function<const std::string&, RTT::rt_string>
        {
            typedef rt_string (Signature)( std::string const& );
            rt_string operator()( std::string const& arg ) const
            {
                return rt_string( arg.c_str() );
            }
        };

        struct string_ctor_rt_string
            : public std::unary_function<const rt_string&, std::string>
        {
            typedef std::string (Signature)( rt_string const& );
            std::string operator()( rt_string const& arg ) const
            {
                return std::string( arg.c_str() );
            }
        };

#endif
    }

    bool RealTimeTypekitPlugin::loadConstructors()
    {
        TypeInfoRepository::shared_ptr ti = TypeInfoRepository::Instance();
#ifndef ORO_EMBEDDED
        ti->type("double")->addConstructor( newConstructor( &float_to_double, true ));
        ti->type("double")->addConstructor( newConstructor( &int_to_double, true ));
        ti->type("float")->addConstructor( newConstructor( &int_to_float, true ));
        ti->type("float")->addConstructor( newConstructor( &double_to_float, true ));
        ti->type("int")->addConstructor( newConstructor( &float_to_int, false ));
        ti->type("int")->addConstructor( newConstructor( &double_to_int, false ));
        ti->type("int")->addConstructor( newConstructor( &uint_to_int, true ));
        ti->type("int")->addConstructor( newConstructor( &bool_to_int, true ));
        ti->type("uint")->addConstructor( newConstructor( &int_to_uint, true ));

        // llong
        ti->type("llong")->addConstructor( newConstructor( &float_to_llong, true ));
        ti->type("float")->addConstructor( newConstructor( &llong_to_float, true ));
        ti->type("llong")->addConstructor( newConstructor( &double_to_llong, true ));
        ti->type("double")->addConstructor( newConstructor( &llong_to_double, true ));
        ti->type("llong")->addConstructor( newConstructor( &int_to_llong, true ));
        ti->type("int")->addConstructor( newConstructor( &llong_to_int, true ));
        ti->type("llong")->addConstructor( newConstructor( &uint_to_llong, true ));
        ti->type("uint")->addConstructor( newConstructor( &llong_to_uint, true ));
        ti->type("bool")->addConstructor( newConstructor( &llong_to_bool, true ));

        // ullong
        ti->type("ullong")->addConstructor( newConstructor( &float_to_ullong, true ));
        ti->type("float")->addConstructor( newConstructor( &ullong_to_float, true ));
        ti->type("ullong")->addConstructor( newConstructor( &double_to_ullong, true ));
        ti->type("double")->addConstructor( newConstructor( &ullong_to_double, true ));
        ti->type("ullong")->addConstructor( newConstructor( &int_to_ullong, true ));
        ti->type("int")->addConstructor( newConstructor( &ullong_to_int, true ));
        ti->type("ullong")->addConstructor( newConstructor( &uint_to_ullong, true ));
        ti->type("uint")->addConstructor( newConstructor( &ullong_to_uint, true ));
        ti->type("ullong")->addConstructor( newConstructor( &llong_to_ullong, true ));
        ti->type("llong")->addConstructor( newConstructor( &ullong_to_llong, true ));

        ti->type("string")->addConstructor( newConstructor( string_ctor() ) );
#ifdef OS_RT_MALLOC
        ti->type("rt_string")->addConstructor( newConstructor( rt_string_ctor_int() ) );
        ti->type("rt_string")->addConstructor( newConstructor( rt_string_ctor_string() ) );
        ti->type("string")->addConstructor( newConstructor( string_ctor_rt_string() ) );
#endif
        ti->type("bool")->addConstructor( newConstructor( &flowstatus_to_bool, true ) );
        ti->type("bool")->addConstructor( newConstructor( &writestatus_to_bool, true ) );
        ti->type("bool")->addConstructor( newConstructor( &send_to_bool, true ) );
        ti->type("bool")->addConstructor( newConstructor( &int_to_bool, true ) );
#endif
        return true;
    }
}
