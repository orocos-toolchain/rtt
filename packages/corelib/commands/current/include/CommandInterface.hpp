/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  CommandInterface.hpp

                        CommandInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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

#ifndef COMMANDINTERFACE_HPP
#define COMMANDINTERFACE_HPP

#include <map>

#pragma interface

namespace ORO_Execution {
    class DataSourceBase;
}

namespace ORO_CoreLib
{
    using ORO_Execution::DataSourceBase;

    /**
     * @brief Based on the software pattern 'command', this
     * interface allows execution of command objects.
     *
     * The command may return true or false to indicate
     * if it was accepted (true) or rejected (false).
     *
     * @section cc Copy versus clone semantics
     * @par Copy is used to copy a whole tree of commands,
     * storing common information in web of \a DataSource's. 
     * The DataSource 'web' will be copied such that an
     * entirely new command tree references the new DataSources.
     * @par When clone is used, the Datasources remain
     * in place, and only the command is cloned, thus,
     * the original and the clone point to the same DataSource.
     * The deletion of DataSources must thus be managed with
     * smart pointers. A clone is thus used for within the
     * existing DataSource web.
     */
    class CommandInterface
    {
    public:
        virtual ~CommandInterface();

        /**
         * Execute the functionality of this command.
         * @return true if the command was accepted, false otherwise
         */
        virtual bool execute() = 0;

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
        virtual void reset();

        /**
         * Execute the functionality of this command.
         * Overload of \a execute().
         * @return true if the command was accepted, false otherwise
         *
         */
        virtual bool operator()();

        /**
         * The Clone Software Pattern.
         */
        virtual CommandInterface* clone() const = 0;

        /**
         * When copying an &orocos; program, we want identical
         * DataSource's to be mapped to identical DataSources, in
         * order for the program to work correctly.  This is different
         * from the clone function, where we simply want a new Command
         * that can replace the old one directly.
         *
         * This function takes a map that maps the old DataSource's
         * onto their new replacements.  This way, it is possible to
         * check before cloning a DataSource, whether it has already
         * been copied, and if so, reuse the existing copy.
         *
         * To keep old source working, the standard implementation of
         * this function simply calls the clone function.  If your
         * CommandInterface uses a DataSource, it is important that
         * you reimplement this function correctly though.
         */
        virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

}

#endif // COMMANDINTERFACE_HPP
