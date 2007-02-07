/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  CommandProcessor.cxx 

                        CommandProcessor.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#include "CommandProcessor.hpp"
#include <CommandInterface.hpp>
#include <AtomicQueue.hpp>
#include <Logger.hpp>

namespace RTT
{

    using namespace std;
    
    using namespace OS;

    CommandProcessor::CommandProcessor(int queue_size)
        :a_queue( new AtomicQueue<CommandInterface*>(queue_size) ),
         coms_processed(0),
         accept(false)
    {
    }

    CommandProcessor::~CommandProcessor()
    {
        delete a_queue;
    }

    bool CommandProcessor::initialize()
    {
        a_queue->clear();
        accept = true;
        return true;
    }

    void CommandProcessor::finalize()
    {
        accept = false;
    }

	void CommandProcessor::step()
    {
        // execute one command from the AtomicQueue.
        CommandInterface* com(0);
        int res = a_queue->dequeueCounted( com );
        if ( com ) {
            // note : the command's result is discarded.
            // Wrap your command (ie CommandDispatch) to keep track
            // of the result of enqueued commands.
            com->execute();
            // let the user know this command was processed.
            coms_processed = res;
        }
    }

    int CommandProcessor::process( CommandInterface* c )
    {
        if (accept && c) {
            int result = a_queue->enqueueCounted( c );
            if ( this->getActivity() )
                this->getActivity()->trigger();
            return result;
        }
        return 0;
    }

    bool CommandProcessor::isProcessed( int nr ) const
    {
        return ( nr <= coms_processed );
    }
}

