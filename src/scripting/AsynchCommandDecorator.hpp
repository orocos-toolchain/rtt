/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  AsynchCommandDecorator.hpp

                        AsynchCommandDecorator.hpp -  description
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

#ifndef ASYNCHCOMMANDDECORATOR_HPP
#define ASYNCHCOMMANDDECORATOR_HPP


#include "../base/ActionInterface.hpp"


namespace RTT
{



    /**
     * Based on the software patterns 'command' and
     * 'decorator', this interface represents a
     * wrapper around a given command, which decorates
     * that command as an asynchronous command.
     *
     * The command will be executed once, until \a reset()
     * and the result of this invocation will be returned.
     *
     * If the command fails when executed, no more attempts
     * will be done to execute it again, and \a execute()
     * will return false until the AsynchCommandDecorator
     * is \a reset().
     */
    class RTT_API AsynchCommandDecorator: public ActionInterface
    {

    public:

        /**
         * Construct a new decorator with given command.
         *
         * @pre cmd != 0
         * @param cmd The command to be decorated
         * @post The given command is decorated as an
         * 		 asynchronous command.
         */
        AsynchCommandDecorator(ActionInterface* cmd);

        AsynchCommandDecorator(const AsynchCommandDecorator& orig);

        virtual ~AsynchCommandDecorator();

        void readArguments();

        /**
         * Execute the functionality of the corresponding command.
         */
        virtual bool execute();

        virtual void reset();

        const ActionInterface* getCommand() const;

        virtual ActionInterface* clone() const;

        virtual ActionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    private:

        /**
         * Asynchronous command being decorated.
         */
        ActionInterface* command;

        /**
         * Variable indicating whether corresponding command
         * has been executed or not.
         */
        bool executed;
        bool status;
    };

}

#endif
