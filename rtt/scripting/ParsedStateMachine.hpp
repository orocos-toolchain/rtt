/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:33:09 CEST 2004  ParsedStateMachine.hpp

                        ParsedStateMachine.hpp -  description
                           -------------------
    begin                : Tue July 20 2004
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


#ifndef CURRENT_INCLUDE_PARSEDSTATECONTEXT_HPP
#define CURRENT_INCLUDE_PARSEDSTATECONTEXT_HPP

#include "StateMachine.hpp"
#include "../internal/DataSource.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{ namespace scripting {
    typedef boost::shared_ptr<ParsedStateMachine> ParsedStateMachinePtr;
    typedef boost::weak_ptr<ParsedStateMachine> ParsedStateMachineWPtr;

    /**
     * State machine created by the scripting engine which represents a parsed state machine.
     */
    class ParsedStateMachine
        : public StateMachine
    {
        typedef std::map<std::string, base::AttributeBase*> VisibleWritableValuesMap;
    protected:
        virtual void handleUnload();
    public:
        ParsedStateMachine();
        virtual ~ParsedStateMachine();

        /**
         * Create a copy, set instantiate to 'true' if instantiating a RootMachine.
         */
        ParsedStateMachinePtr copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacements, bool instantiate = false ) const;

        void addParameter( const std::string& name, base::AttributeBase* var );

        base::AttributeBase* getParameter( const std::string& name ) const;
        VisibleWritableValuesMap getParameters() const;
        std::vector<std::string> getParameterNames() const;

        /**
         * Set the name of this machine. If \a recurisive == true, this also sets subMachines'
         * names, to the given name + "." + the name they have been
         * instantiated by in this machine.
         */
        void setName( const std::string& name, bool recursive );

        std::string getText() const;

        void setText( std::string text);

        StateMachineTask* getTaskObject() const;

        void setTaskObject(StateMachineTask* tc);

        bool inState( const std::string& name );
        /**
         * Call this function if the state machine is parsed.
         */
        void finish();
    private:
        VisibleWritableValuesMap parametervalues;

        boost::shared_ptr<std::string> _text;

        StateMachineTask* object;
    };
}}

#endif
