/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  StateMachineTree.hpp 

                        StateMachineTree.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef HIERARCHICAL_STATE_CONTEXT_HPP
#define HIERARCHICAL_STATE_CONTEXT_HPP

#include <corelib/StateMachine.hpp>
#include <vector>
#include <string>

namespace ORO_Execution
{
    /**
     * @brief A hierarchical StateMachine which is
     * loaded in the Program Processor.
     *
     * A StateMachineTree can have children and one parent.
     */
    class StateMachineTree
        :public ORO_CoreLib::StateMachine
    {
        std::vector<StateMachineTree*> _children;
        StateMachineTree* _parent;
    protected:
        std::string _name;
    public:
        typedef std::vector<StateMachineTree*> ChildList;

        virtual ~StateMachineTree() {}

        StateMachineTree(StateMachineTree* parent, const std::string& name="Default")
            : _parent (parent) , _name(name)
        {}

        /**
         * Get the parent, returns zero if no parent.
         */
        virtual StateMachineTree* getParent() 
        {
            return _parent;
        }

        virtual void setParent(StateMachineTree* parent)
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

        virtual void addChild( StateMachineTree* child ) {
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
