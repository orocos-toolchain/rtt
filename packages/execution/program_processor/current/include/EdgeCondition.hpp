#ifndef EDGECONDITION_HPP
#define EDGECONDITION_HPP

#include <corelib/ConditionTrue.hpp>
#include <corelib/ConditionInterface.hpp>

#include <boost/graph/adjacency_list.hpp>
using boost::property;

enum edge_condition_t { edge_condition };

namespace boost {
    BOOST_INSTALL_PROPERTY(edge, condition);
}

namespace ORO_Execution
{
	using ORO_CoreLib::ConditionInterface;
	using ORO_CoreLib::ConditionTrue;

	/**
	 * @brief This class represents a conditional branch in a program
	 * tree.  It contains a condition which must be satisfied
     * to pass to the next node.
	 */
	class EdgeCondition
	{
    public:
        typedef property<edge_condition_t, EdgeCondition,
                         property< boost::edge_index_t, int> > EdgeProperty;

        /**
         * EdgeCondition specifies a condition to
         * check.
         */
        EdgeCondition() : condition ( new ConditionTrue ) {}

        /**
         * EdgeCondition specifies a condition to
         * check.
         * This EdgeCondition takes ownership of cond.
         */
        EdgeCondition(ConditionInterface* cond );

        virtual ~EdgeCondition();

        EdgeCondition( const EdgeCondition& orig )
            : condition( orig.getCondition()->clone() )
        {}

        /**
         * Reset the condition in this
         * EdgeCondition.  See the documentation
         * of ConditionInterface::reset() for more
         * details..
         */
        virtual void reset();

        /**
         * Evaluates the condition coupled with this termination clause.
         */
        virtual bool evaluate();

        /**
         * Return the condition coupled with this termination clause.
         */
        virtual ConditionInterface* getCondition() const;

    protected:

        virtual void setCondition(ConditionInterface* cond);

    private:
        ConditionInterface* condition;

	};

}

#endif
