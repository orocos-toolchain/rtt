#ifndef HIERARCHICAL_STATE_CONTEXT_HPP
#define HIERARCHICAL_STATE_CONTEXT_HPP

#include <corelib/StateContext.hpp>
#include <vector>
#include <string>

namespace ORO_Execution
{
    /**
     * @brief A hierarchical StateContext which is
     * loaded in the Program Processor.
     *
     * A StateContextTree can have children and one parent.
     */
    class StateContextTree
        :public ORO_CoreLib::StateContext
    {
        std::vector<StateContextTree*> _children;
        StateContextTree* _parent;
    protected:
        std::string _name;
    public:
        typedef std::vector<StateContextTree*> ChildList;

        virtual ~StateContextTree() {}

        StateContextTree(StateContextTree* parent, const std::string& name="Default")
            : _parent (parent) , _name(name)
        {}

        /**
         * Get the parent, returns zero if no parent.
         */
        virtual StateContextTree* getParent() 
        {
            return _parent;
        }

        virtual void setParent(StateContextTree* parent)
        {
            _parent = parent;
        }

        /**
         * Get a list of all child state contexts.
         */
        virtual const ChildList& getChildren()
        {
            return _children;
        }

        virtual void addChild( StateContextTree* child ) {
            _children.push_back( child );
        }

        /**
         * This method must be overloaded to get a useful
         * hierarchy.
         */
        virtual const std::string& getName() const {
            return _name;
        }
    }; 
}

#endif
