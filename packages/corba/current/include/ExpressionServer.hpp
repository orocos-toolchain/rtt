#ifndef ORO_CORBA_EXPRESSIONSERVER_HPP
#define ORO_CORBA_EXPRESSIONSERVER_HPP

#include <corelib/DataSourceBase.hpp>
#include "ExecutionC.h"


// forward decl.
class Orocos_AnyExpression_i;

namespace ORO_Corba
{

    /**
     * This class manages the access of local Expression Corba Servants and Servers.
     * Especially, it also takes care of the copy/clone semantics and informs the
     * servants when a DataSource copy() operation changed the location of a DataSource it was using.
     */
    class ExpressionServer
    {
    protected:
        typedef std::map<ORO_CoreLib::DataSourceBase::const_ptr, Orocos_AnyExpression_i* > EServantMap;
        typedef std::map<ORO_CoreLib::DataSourceBase::const_ptr, Orocos::Expression_ptr > EServerMap;
        typedef std::map<ORO_CoreLib::DataSourceBase::shared_ptr, Orocos::AssignableExpression_ptr> AServerMap;
        typedef std::map<ORO_CoreLib::DataSourceBase::shared_ptr, Orocos::Method_ptr> MServerMap;

        /**
         * All created servants end up in this map.
         */
        static EServantMap EServants;

        /**
         * All created Expressions (or subclasses) end up in this map.
         */
        static EServerMap EServers;
        /**
         * The AssignableExpressions end up in this map as well.
         */
        static AServerMap AServers;
        /**
         * The Methods end up in this map as well.
         */
        static MServerMap MServers;

        /**
         * Private constructor. Not used.
         */
        ExpressionServer();

    public:

        /**
         * Factory method: create a CORBA server to an expression
         * @param expr The expression to serve
         * @return A new or previously created CORBA server for \a expr.
         */
        static Orocos::Expression_ptr CreateExpression( ORO_CoreLib::DataSourceBase::const_ptr expr );


        /**
         * Factory method: create a CORBA server to an assignable expression.
         * @param expr The expression to serve
         * @return A new or previously created CORBA server for \a expr.
         */
        static Orocos::AssignableExpression_ptr CreateAssignableExpression( ORO_CoreLib::DataSourceBase::shared_ptr expr );

        /**
         * Factory method: create a CORBA server to a method
         * @param expr The expression to serve
         * @return A new or previously created CORBA server for \a expr.
         */
        static Orocos::Method_ptr CreateMethod( ORO_CoreLib::DataSourceBase::shared_ptr expr );

        /**
         * This method informs the servants that a new DataSource needs to be used
         * because of a copy operation. It also updates the internal maps such that
         * each new lookup will resolve to the new DataSource instead of the old.
         * The old DataSource becomes 'unserved' and may thus become available for clean-up.
         */
        static void copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned );
    };

}

#endif
