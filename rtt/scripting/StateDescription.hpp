/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  StateDescription.hpp

                        StateDescription.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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

#ifndef STATE_DESCRIPTION_HPP
#define STATE_DESCRIPTION_HPP

#include "StateInterface.hpp"
#include "../base/ProgramInterface.hpp"

namespace RTT
{ namespace scripting {

    /**
     * @brief This class represents a state with all actions stored
     * in an external program.
     */
    class RTT_API StateDescription
        : public StateInterface
    {
        base::ProgramInterfacePtr mentry;
        base::ProgramInterfacePtr mexit;
        base::ProgramInterfacePtr mhandle;
        base::ProgramInterfacePtr mrun;
        std::string name;
        int entrypoint;
        bool inited;
    public:
        /**
         * Construct a new State with entry, exit and handle nodes.
         * The StateGraph owning the nodes is needed for processing each state.
         */
        StateDescription(const std::string& _name, int linenr )
            : mentry(), mexit(), mhandle(), mrun(),
              name(_name), entrypoint(linenr), inited(false)
        {
        }

        virtual ~StateDescription();

        const std::string& getName() const { return name; }
        void setName(const std::string& newname) { name = newname; }

        void setEntryPoint(int line) { entrypoint = line; }
        int getEntryPoint() const { return entrypoint; }

        /**
         * This function returns a new state that contains all the
         * data of this state ( its handle, entry, exit programs, and
         * its name and inited state ), while this state's data is
         * reset ( i.e. its handle, entry, exit programs are set to
         * null, its name is cleared, and it is set to not inited ).
         * This is used by the parser when it suddenly notices that it
         * needs to insert a dummy state before the current to check
         * preconditions.
         * @deprecated by new precondition semantics.
         */
        StateDescription* postponeState();

        base::ProgramInterface* getEntryProgram() const {
            return mentry.get();
        }

        base::ProgramInterface* getRunProgram() const {
            return mrun.get();
        }

        base::ProgramInterface* getHandleProgram() const {
            return mhandle.get();
        }

        base::ProgramInterface* getExitProgram() const {
            return mexit.get();
        }

        void setEntryProgram( base::ProgramInterfacePtr entry ) {
            mentry = entry;
        }

        void setRunProgram( base::ProgramInterfacePtr run ) {
            mrun = run;
        }

        void setHandleProgram( base::ProgramInterfacePtr handle ) {
            mhandle = handle;
        }

        void setExitProgram( base::ProgramInterfacePtr exit ) {
            mexit = exit;
        }

        bool isDefined() const
        {
            return inited;
        }

        void setDefined( bool d ) {
            inited = d;
        }

        StateDescription* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacementdss ) const;

    };
}};

#endif
