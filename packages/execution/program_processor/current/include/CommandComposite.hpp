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
 

#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{
	using ORO_CoreLib::CommandInterface;
	using std::vector;
	
    /**
     * @brief Based on the software pattern 'composite', this
     * class allows composing command objects into one command object
     */
    class CommandComposite
        : public CommandInterface
    {
        public:
        CommandComposite() {}

        CommandComposite( const CommandComposite& orig )
        {
            for (vector<CommandInterface*>::iterator zjakky=list.begin();zjakky!=list.end();zjakky++)
                add( (*zjakky)->clone() );
        }

        virtual ~CommandComposite() {
            for (vector<CommandInterface*>::iterator zjakky=list.begin();zjakky!=list.end();zjakky++) {
                CommandInterface * cif=*zjakky;
                delete cif;
            }
        }

            /**
             * Execute the functionality of all commands.
             * Commands will be executed in the order they have been added
             */
            virtual void execute() {
            	for (vector<CommandInterface*>::iterator zjakky=list.begin();zjakky!=list.end();zjakky++) {
            		(*zjakky)->execute();
            	}
			};

        /**
         * add a command to the list
         * 
         */
        virtual int add(CommandInterface * com) {		
            list.push_back(com);
        };

        virtual CommandInterface* clone() const
        {
            return new CommandComposite( *this );
        }
			
    private:
        vector<CommandInterface*> list;
    };

}
