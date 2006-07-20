/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CommandComposite.hpp 

                        CommandComposite.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 

#include "CommandInterface.hpp"
#include <vector>

namespace RTT
{
    /**
     * @brief Based on the software pattern 'composite', this
     * class allows composing command objects into one command object
     */
    class CommandComposite
        : public CommandInterface
    {
        public:
        CommandComposite() {}

        CommandComposite(std::vector<CommandInterface*> cv )
            : vect( cv )
        {}

        /**
         * Copy-Construct a clone() of all commands
         */
        CommandComposite( const CommandComposite& orig )
            : CommandInterface()
        {
            for (std::vector<CommandInterface*>::const_iterator it=orig.vect.begin();it!=orig.vect.end();it++)
                this->add( (*it)->clone() );
        }

        virtual ~CommandComposite() {
            for (std::vector<CommandInterface*>::iterator it=vect.begin();it!=vect.end();it++) {
                CommandInterface * cif=*it;
                delete cif;
            }
        }

            /**
             * Execute the functionality of all commands.
             * Commands will be executed in the order they have been added
             */
            virtual bool execute() {
            	for (std::vector<CommandInterface*>::iterator it=vect.begin();it!=vect.end();it++) {
            		if ( !(*it)->execute() )
                        return false;
            	}
                return true;
			};

        void readArguments() {
            for (std::vector<CommandInterface*>::iterator it=vect.begin();it!=vect.end();it++)
                (*it)->readArguments();
        }

        /**
         * add a command to the vect
         * 
         */
        virtual void add(CommandInterface * com) {		
            vect.push_back(com);
        };

        virtual CommandInterface* clone() const
        {
            return new CommandComposite( *this );
        }
			
        virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        {
            CommandComposite* res = new CommandComposite();
            for (std::vector<CommandInterface*>::const_iterator it=vect.begin();it!=vect.end();it++)
                res->add( (*it)->copy(alreadyCloned) );
            return res;
        }
			
    private:
        std::vector<CommandInterface*> vect;
    };

}
