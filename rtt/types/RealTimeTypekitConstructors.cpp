/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  RealTimeToolkit.cxx

                        RealTimeToolkit.cxx -  description
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



#include "rtt-config.h"
#include "RealTimeToolkit.hpp"
#include "Types.hpp"
#include "TemplateTypeInfo.hpp"
#include "../internal/DataSources.hpp"
#include "../internal/mystd.hpp"
#include "../rtt-fwd.hpp"
#include "../FlowStatus.hpp"
#include "../ConnPolicy.hpp"

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
            : public TypeBuilder
        {
            virtual DataSourceBase::shared_ptr build(const std::vector<DataSourceBase::shared_ptr>& args) const {
                if (args.size() == 0 )
                    return DataSourceBase::shared_ptr();
                ArrayDataSource::shared_ptr vds = new ArrayDataSource();
                for(unsigned int i=0; i != args.size(); ++i) {
                    DataSource<double>::shared_ptr dsd = AdaptDataSource<double>()( args[i] );
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

            double float_to_double( float val ) {return val;}
            float double_to_float( double val ) {return val;}


            int float_to_int(float f) { return int(f); }
            float int_to_float(int i) { return i; }
            int double_to_int(double f) { return int(f); }
            double int_to_double(int i) { return i; }
            unsigned int int_to_uint(int i) { return i; }
            int uint_to_int(unsigned int ui) { return ui; }
#endif
        bool flow_to_bool(FlowStatus fs) { return fs; }
        bool send_to_bool(SendStatus ss) { return ss; }

        struct string_ctor
            : public std::unary_function<int, const std::string&>
        {
            mutable boost::shared_ptr< std::string > ptr;
            typedef const std::string& (Signature)( int );
            string_ctor()
                : ptr( new std::string() ) {}
            const std::string& operator()( int size ) const
            {
                ptr->resize( size );
                return *(ptr);
            }
        };
    }

    bool RealTimeToolkitPlugin::loadConstructors()
    {
        TypeInfoRepository::shared_ptr ti = TypeInfoRepository::Instance();
#ifndef ORO_EMBEDDED
        ti->type("array")->addConstructor( new ArrayBuilder() ); // var number of args
        // fixed number of args:
        ti->type("array")->addConstructor( newConstructor( array_ctor() ) );
        ti->type("array")->addConstructor( newConstructor( array_ctor2() ) );
        ti->type("double")->addConstructor( newConstructor( &float_to_double, true ));
        ti->type("double")->addConstructor( newConstructor( &int_to_double, true ));
        ti->type("float")->addConstructor( newConstructor( &int_to_float, true ));
        ti->type("float")->addConstructor( newConstructor( &double_to_float, true ));
        ti->type("int")->addConstructor( newConstructor( &float_to_int, false ));
        ti->type("int")->addConstructor( newConstructor( &double_to_int, false ));
        ti->type("int")->addConstructor( newConstructor( &uint_to_int, false ));
        ti->type("uint")->addConstructor( newConstructor( &int_to_uint, false ));
        ti->type("string")->addConstructor( newConstructor( string_ctor() ) );
        ti->type("bool")->addConstructor( newConstructor( &flow_to_bool, true ) );
        ti->type("bool")->addConstructor( newConstructor( &send_to_bool, true ) );
#endif
        return true;
    }
}
