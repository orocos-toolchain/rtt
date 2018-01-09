/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CommandComposite.hpp

                        CommandComposite.hpp -  description
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


#include "../base/ActionInterface.hpp"
#include <vector>
#include "rtt-scripting-config.h"

namespace RTT
{ namespace scripting {

    /**
     * @brief Based on the software pattern 'composite', this
     * class RTT_SCRIPTING_API allows composing command objects into one command object
     */
    class RTT_SCRIPTING_API CommandComposite
        : public base::ActionInterface
    {
        public:
        CommandComposite() {}

        CommandComposite(std::vector<base::ActionInterface*> cv )
            : vect( cv )
        {}

        /**
         * Copy-Construct a clone() of all commands
         */
        CommandComposite( const CommandComposite& orig )
            : base::ActionInterface()
        {
            for (std::vector<base::ActionInterface*>::const_iterator it=orig.vect.begin();it!=orig.vect.end();it++)
                this->add( (*it)->clone() );
        }

        virtual ~CommandComposite() {
            for (std::vector<base::ActionInterface*>::iterator it=vect.begin();it!=vect.end();it++) {
                base::ActionInterface * cif=*it;
                delete cif;
            }
        }

        /**
         * Execute the functionality of all commands.
         * Commands will be executed in the order they have been added
         */
        virtual bool execute() {
            for (std::vector<base::ActionInterface*>::iterator it=vect.begin();it!=vect.end();it++) {
                if ( !(*it)->execute() )
                    return false;
            }
            return true;
        }

        void readArguments() {
            for (std::vector<base::ActionInterface*>::iterator it=vect.begin();it!=vect.end();it++)
                (*it)->readArguments();
        }

        bool valid() const {
            // valid if all are valid.
            bool v = true;
            for (std::vector<base::ActionInterface*>::const_iterator it=vect.begin();it!=vect.end();it++)
                v = v && (*it)->valid();
            return v;
        }

        /**
         * add a command to the vect
         *
         */
        virtual void add(base::ActionInterface * com) {
            vect.push_back(com);
        }

        virtual base::ActionInterface* clone() const
        {
            return new CommandComposite( *this );
        }

        virtual base::ActionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
        {
            CommandComposite* res = new CommandComposite();
            for (std::vector<base::ActionInterface*>::const_iterator it=vect.begin();it!=vect.end();it++)
                res->add( (*it)->copy(alreadyCloned) );
            return res;
        }

    private:
        std::vector<base::ActionInterface*> vect;
    };

}}
