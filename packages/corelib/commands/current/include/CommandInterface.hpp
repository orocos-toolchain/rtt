/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  CommandInterface.hpp

                        CommandInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef COMMANDINTERFACE_HPP
#define COMMANDINTERFACE_HPP

#include <string>

namespace ORO_CoreLib
{

    /**
     * Based on the software pattern 'command', this
     * interface allows execution of command objects.
     */
    class CommandInterface
    {
        public:
            virtual ~CommandInterface()
            {}

            /**
             * Execute the functionality of this command.
             */
            virtual void execute() = 0;

            /**
             * Return a std::string identifying the name
             * of this Command.
             */
            virtual std::string toString() = 0;

            /**
             * Reset this command.  This method is called when the
             * ProgramNode containing this command is entered.  Then,
             * the command continues to become executed until one of
             * the termination clauses becomes true, and this method
             * is not called.  Only when the ProgramNode is left and
             * after some time reentered, this method is called
             * again.  AsynchCommandDecorator needs this, and also
             * Commands that have DataSource arguments need to reset
             * them..
             * @see ConditionInterface::reset
             * @see DataSource::reset
             */
            virtual void reset() {};

        /**
         * The Clone Software Pattern.
         */
        virtual CommandInterface* clone() const = 0;
    };

}

#endif // COMMANDINTERFACE_HPP
