/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  ActionInterface.hpp

                        ActionInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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

#ifndef ACTIONINTERFACE_HPP
#define ACTIONINTERFACE_HPP

#include <map>
#include <boost/shared_ptr.hpp>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT {
    class DataSourceBase;

    /**
     * @brief Based on the software pattern 'action', this
     * interface allows execution of action objects.
     *
     * The action may return true or false to indicate
     * if it was accepted (true) or rejected (false).
     *
     * @section cc Copy versus clone semantics
     * @par Copy is used to copy a whole tree of actions,
     * storing common information in web of \a DataSource's. 
     * The DataSource 'web' will be copied such that an
     * entirely new action tree references the new DataSources.
     * @par When clone is used, the Datasources remain
     * in place, and only the action is cloned, thus,
     * the original and the clone point to the same DataSource.
     * The deletion of DataSources must thus be managed with
     * smart pointers. A clone is thus used for within the
     * existing DataSource web.
     */
    class ActionInterface
    {
    public:
        /**
         * Use this type for shared pointer storage of an
         * ActionInterface object.
         */
        typedef boost::shared_ptr<ActionInterface> shared_ptr;

        virtual ~ActionInterface();

        /**
         * Execute the functionality of this action.
         * @return true if the action was accepted, false otherwise
         */
        virtual bool execute() = 0;

        /**
         * Reset this action. Must be used after execute() failed.
         */
        virtual void reset();

        /**
         * Inspect if this action was \a executed and \a valid.
         * This method may not be called before execute(). The
         * default implementation returns always \a true, i.e.
         * after execute(), it was executed. Override this method
         * if it has more complex state semantics.
         */
        virtual bool valid() const;

        /**
         * This is invoked some time before execute() at a time
         * when the action may read its function arguments.
         */
        virtual void readArguments() = 0;

        /**
         * The Clone Software Pattern.
         */
        virtual ActionInterface* clone() const = 0;

        /**
         * When copying an Orocos program, we want identical
         * DataSource's to be mapped to identical DataSources, in
         * order for the program to work correctly.  This is different
         * from the clone function, where we simply want a new Action
         * that can replace the old one directly.
         *
         * This function takes a map that maps the old DataSource's
         * onto their new replacements.  This way, it is possible to
         * check before cloning a DataSource, whether it has already
         * been copied, and if so, reuse the existing copy.
         *
         * To keep old source working, the standard implementation of
         * this function simply calls the clone function.  If your
         * ActionInterface uses a DataSource, it is important that
         * you reimplement this function correctly though.
         */
        virtual ActionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

}

#endif // ACTIONINTERFACE_HPP
