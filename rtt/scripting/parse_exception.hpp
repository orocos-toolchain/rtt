/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  parse_exception.hpp

                        parse_exception.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be

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
#ifndef ORO_PARSE_EXCEPTION_HPP
#define ORO_PARSE_EXCEPTION_HPP

#include <string>
#include <vector>
#include "../scripting/rtt-scripting-config.h"
#include "../base/DataSourceBase.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    class parse_exception;

    /**
     * This is an exception class that keeps a parse_exception pointer
     * along with the location in the file and the file where it
     * occurred.
     */
    class RTT_SCRIPTING_EXPORT file_parse_exception
    {
        parse_exception* mpe;
        std::string mfile;
        int mline;
        int mcolumn;
        // make this private to prevent the compiler from adding it...
        file_parse_exception& operator=( const file_parse_exception& rhs );
    public:
        file_parse_exception( const file_parse_exception& rhs );
        file_parse_exception( parse_exception* pe, const std::string& file,
                              int line, int column )
            : mpe( pe ), mfile( file ), mline( line ), mcolumn( column )
        {
        }
        ~file_parse_exception();

        const std::string what() const;
    };

    /**
     * This is the uppermost exception class in the parser system.
     * Throughout the parser system, exceptions are thrown that inherit
     * from this.
     */
    class RTT_SCRIPTING_EXPORT parse_exception
    {
        // make these private
        parse_exception& operator=( const parse_exception& );
    protected:
        parse_exception() {};
    public:
        virtual ~parse_exception() {};
        virtual const std::string what() const = 0;
        virtual parse_exception* copy() const = 0;
    };

    namespace scripting {

        /**
         * A Semantic parse exception means the parser recognised
         * a part of the string, but got into trouble lateron,
         * for example, a missing argument or non existing component.
         */
        class RTT_SCRIPTING_EXPORT semantic_parse_exception
            : public parse_exception
        {
            // make these private
            semantic_parse_exception& operator=( const semantic_parse_exception& );
        protected:
            semantic_parse_exception() {};
        };

        /**
         * A Fatal Syntactic parse exception means the parser
         * knows the input is plain wrong and no further attemts
         * should be made to parse it.
         */
        class RTT_SCRIPTING_EXPORT fatal_syntactic_parse_exception
            : public parse_exception
        {
            // make these private
            fatal_syntactic_parse_exception& operator=( const fatal_syntactic_parse_exception& );
        protected:
            fatal_syntactic_parse_exception() {};
        };

        /**
         * A Fatal Semantic parse exception means the parser
         * knows that the parsing failed dramatically and
         * should not be passed to another parser. for
         * example, a missing argument, or the incorrect type.
         */
        class RTT_SCRIPTING_EXPORT fatal_semantic_parse_exception
            : public parse_exception
        {
            // make these private
            fatal_semantic_parse_exception& operator=( const fatal_syntactic_parse_exception& );
        protected:
            fatal_semantic_parse_exception() {};
        };

        /**
         * A normal syntactic parse exception means the parser
         * recognised the input, but got stuck later due to a
         * syntactic error, like a missing brace.
         */
        class RTT_SCRIPTING_EXPORT syntactic_parse_exception
            : public parse_exception
        {
            // make these private
            syntactic_parse_exception& operator=( const syntactic_parse_exception& );
        protected:
            syntactic_parse_exception() {};
        };



        class RTT_SCRIPTING_EXPORT parse_exception_illegal_identifier
            : public syntactic_parse_exception
        {
            std::string mident;
        public:
            parse_exception_illegal_identifier( const std::string& ident )
                : mident( ident )
            {
            };

            const std::string what() const
            {
                return "The string \"" + mident + "\" cannot be used as an identifer.";
            }

            parse_exception_illegal_identifier* copy() const
            {
                return new parse_exception_illegal_identifier( *this );
            }

            const std::string& identifier() const
            {
                return mident;
            }
        };

        /**
         * parse_exception class that is used for various semantic errors
         * for which it was not worth defining a proper exception class.
         */
        class RTT_SCRIPTING_EXPORT parse_exception_semantic_error
            : public semantic_parse_exception
        {
            std::string mdesc;
        public:
            parse_exception_semantic_error( const std::string& desc )
                : mdesc( desc )
            {
            };

            const std::string what() const
            {
                return "Semantic error: " + mdesc;
            }

            parse_exception_semantic_error* copy() const
            {
                return new parse_exception_semantic_error( *this );
            }

            const std::string& desc() const
            {
                return mdesc;
            }
        };

        /**
         * parse_exception class that is used for fatal semantic errors
         * for which it was not worth defining a proper exception class.
         */
        class RTT_SCRIPTING_EXPORT parse_exception_fatal_semantic_error
            : public fatal_semantic_parse_exception
        {
            std::string mdesc;
        public:
            parse_exception_fatal_semantic_error( const std::string& desc )
                : mdesc( desc )
            {
            };

            const std::string what() const
            {
                return "Fatal Semantic error: " + mdesc;
            }

            parse_exception_fatal_semantic_error* copy() const
            {
                return new parse_exception_fatal_semantic_error( *this );
            }

            const std::string& desc() const
            {
                return mdesc;
            }
        };

        /**
         * An exception which a parser may throw to indicate that
         * it failed to understand the input, and thus can not
         * interpret its validity. It is the equivalent of a
         * failing parser rule.
         */
        class RTT_SCRIPTING_EXPORT parse_exception_parser_fail
            : public parse_exception
        {
        public:
            parse_exception_parser_fail()
            {
            }

            const std::string what() const
            {
                return "Parse Failure Exception ( This _specific_ parser could not parse the input )";
            }

            parse_exception_parser_fail* copy() const
            {
                return new parse_exception_parser_fail( *this );
            }
        };

        /**
         * parse_exception class that is used for various syntactic errors
         * for which it was not worth defining a proper exception class.
         */
        class RTT_SCRIPTING_EXPORT parse_exception_syntactic_error
            : public syntactic_parse_exception
        {
            std::string mdesc;
        public:
            parse_exception_syntactic_error( const std::string& desc )
                : mdesc( desc )
            {
            };

            const std::string what() const
            {
                return "Syntactic error: " + mdesc;
            }

            parse_exception_syntactic_error* copy() const
            {
                return new parse_exception_syntactic_error( *this );
            }

            const std::string& desc() const
            {
                return mdesc;
            }
        };

        class RTT_SCRIPTING_EXPORT parse_exception_no_such_component
            : public semantic_parse_exception
        {
            std::string mname;
            std::string mmeth;
        public:
            parse_exception_no_such_component( const std::string& name, const std::string& meth )
                : mname( name ), mmeth(meth)
            {
            }

            const std::string what() const
            {
                return "Object or task \"" + mname + "\" registered no method "+mmeth+" (or "+mname+" was not found at all).";
            }

            parse_exception_no_such_component* copy() const
            {
                return new parse_exception_no_such_component( *this );
            }

            const std::string& componentName() const
            {
                return mname;
            }
        };

        class RTT_SCRIPTING_EXPORT parse_exception_no_such_method_on_component
            : public semantic_parse_exception
        {
            std::string mcomponentname;
            std::string mmethodname;
        public:
            parse_exception_no_such_method_on_component(
                                                        const std::string& componentname, const std::string& methodname )
                : mcomponentname( componentname ), mmethodname( methodname )
            {
            };

            const std::string what() const
            {
                return "No method \"" + mmethodname + "\" registered for the object or task \"" + mcomponentname + "\".";
            }

            parse_exception_no_such_method_on_component* copy() const
            {
                return new parse_exception_no_such_method_on_component( *this );
            }

            const std::string& componentName() const
            {
                return mcomponentname;
            }

            const std::string& methodName() const
            {
                return mmethodname;
            }
        };

        class RTT_SCRIPTING_EXPORT parse_exception_wrong_number_of_arguments
            : public fatal_semantic_parse_exception
        {
            std::string mcomponentname;
            std::string mmethodname;
            int mexpectednumber;
            int mreceivednumber;
        public:
            parse_exception_wrong_number_of_arguments(
                                                      const std::string& componentname, const std::string& methodname,
                                                      int expectednumber, int receivednumber )
                : mcomponentname( componentname ), mmethodname( methodname ),
                  mexpectednumber( expectednumber ),
                  mreceivednumber( receivednumber )
            {
            };

            const std::string what() const;

            parse_exception_wrong_number_of_arguments* copy() const
            {
                return new parse_exception_wrong_number_of_arguments( *this );
            }

            const std::string& componentName() const
            {
                return mcomponentname;
            }

            const std::string& methodName() const
            {
                return mmethodname;
            }

            int expectedNumber() const
            {
                return mexpectednumber;
            }

            int receivedNumber() const
            {
                return mreceivednumber;
            }
        };

        class RTT_SCRIPTING_EXPORT parse_exception_wrong_type_of_argument
            : public fatal_semantic_parse_exception
        {
            std::string mcomponentname;
            std::string mmethodname;
            int margnumber;
            std::string mexpected;
            std::string mreceived;
        public:
            parse_exception_wrong_type_of_argument(
                                                   const std::string& componentname, const std::string& methodname,
                                                   int argnumber, const std::string& expected, const std::string& received )
                : mcomponentname( componentname ), mmethodname( methodname ),
                  margnumber( argnumber ), mexpected( expected), mreceived( received )
            {
            };

            const std::string what() const;

            parse_exception_wrong_type_of_argument* copy() const
            {
                return new parse_exception_wrong_type_of_argument( *this );
            }

            const std::string& componentName() const
            {
                return mcomponentname;
            }

            const std::string& methodName() const
            {
                return mmethodname;
            }

            int argumentNumber() const
            {
                return margnumber;
            }
        };

        class RTT_SCRIPTING_EXPORT parse_exception_undefined_value
            : public fatal_semantic_parse_exception
        {
            std::string mname;
        public:
            parse_exception_undefined_value( const std::string& name )
                : mname( name )
            {
            }

            const std::string what() const throw()
            {
                return "Use of undefined value: \"" + mname + "\".";
            }

            parse_exception_undefined_value* copy() const throw()
            {
                return new parse_exception_undefined_value( *this );
            }

            const std::string& name() {
                return mname;
            }
        };

        class RTT_SCRIPTING_EXPORT parse_exception_no_such_constructor
            : public fatal_semantic_parse_exception
        {
            std::string margsig;
        public:
            parse_exception_no_such_constructor(const std::string& tname,
                                                std::vector<base::DataSourceBase::shared_ptr> args);

            const std::string what() const { return margsig; }

            parse_exception_no_such_constructor* copy() const
            {
                return new parse_exception_no_such_constructor( *this );
            }
        };
    }
}

#endif
