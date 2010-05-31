/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  Operators.hpp

                        Operators.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef ORO_OPERATORS_HPP
#define ORO_OPERATORS_HPP

#include <string>
#include <vector>
#include "../internal/DataSource.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT { namespace types
{

  /**
   * \file this file contains operator information objects,
   * These objects contain information on how to apply one operator on
   * a set of arguments, and contain a method that does it..  This is
   * mostly done to allow me to work with template stuff to save me a
   * lot of typing time ;)
   */

  /**
   * UnaryOperator contains information on some unary operator that
   * operates on one arg of a certain type.  Examples of unary
   * operators are logical not for booleans, unary minus for ints,
   * unary minus for doubles, unary minus for vectors, unary plus for
   * ints (internal::identity operator) etc.  We always provide templates that
   * accept a STL style adaptable functor..
   */
  class RTT_API UnaryOp
  {
  public:
    virtual ~UnaryOp();
    /**
     * If op is the operator you are responsible for, and if the
     * argument internal::DataSource is of the correct type, then return an
     * appropriate DataSource. ( i.e. a internal::DataSource that will apply a
     * certain operation on the value it gets from its argument
     * internal::DataSource, and will return that value ).  Otherwise, return 0.
     */
    virtual base::DataSourceBase* build( const std::string& op,
                                   base::DataSourceBase* a ) = 0;
  };

  class RTT_API BinaryOp
  {
  public:
    virtual ~BinaryOp();
    /**
     * If op is the operator you are responsible for, and if the
     * argument internal::DataSource's are of the correct type, then return an
     * appropriate internal::DataSource ( i.e. a internal::DataSource that will apply a
     * certain operation on the values it gets from its argument
     * DataSources, and will return that value ).
     * Otherwise, return 0.
     */
    virtual base::DataSourceBase* build(
      const std::string& op, base::DataSourceBase* a, base::DataSourceBase* b ) = 0;
  };

  /**
   * The Dot Operator allows access to members of composite types,
   * such as in frame.pos.x .
   * This is used if a dot was parsed on a value type. It is different by semantics of the
   * UnaryOp because the dot is parsed as a binary operator but
   * actually is a unary ( hard to explain, look at the code...)
   */
  class RTT_API DotOp
  {
  public:
    virtual ~DotOp();
    /**
     * If op is the operator you are responsible for, and if the
     * argument internal::DataSource is of the correct type, then return an
     * appropriate DataSource. ( i.e. a internal::DataSource that will apply a
     * certain operation on the value it gets from its argument
     * internal::DataSource, and will return that value ).  Otherwise, return 0.
     */
    virtual base::DataSourceBase* build( const std::string& member,
                                   base::DataSourceBase* a ) = 0;
  };

    /**
     * This class builds on upon construction all expression
     * operators known to Orocos. Mainly used for scripting.
     */
    class RTT_API OperatorRepository
    {
        std::vector<DotOp*> dotops;
        std::vector<UnaryOp*> unaryops;
        std::vector<BinaryOp*> binaryops;
        OperatorRepository();
        OperatorRepository( const OperatorRepository& );

    public:
        void add( UnaryOp* o );
        void add( DotOp* o );
        void add( BinaryOp* o );

        typedef boost::shared_ptr<OperatorRepository> shared_ptr;
        ~OperatorRepository();
        /**
         * Returns a shared pointer to the singleton of this class.
         */
        static shared_ptr Instance();
        base::DataSourceBase* applyDot( const std::string& member, base::DataSourceBase* value );
        base::DataSourceBase* applyUnary( const std::string& op, base::DataSourceBase* a );
        base::DataSourceBase* applyBinary(
                                    const std::string& op, base::DataSourceBase* a, base::DataSourceBase* b );
    };

    /**
     * This global function provides the short notation for
     * OperatorRepository::Instance()
     */
    RTT_API OperatorRepository::shared_ptr operators();
}}

#endif
