/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionInterface.hpp

                        ConditionInterface.hpp -  description
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


#ifndef CONDITIONINTERFACE_HPP
#define CONDITIONINTERFACE_HPP

#include <map>

namespace ORO_Execution {
    class DataSourceBase;
}

namespace ORO_CoreLib
{
    using ORO_Execution::DataSourceBase;
    /**
     * @brief This interface represents the concept of
     * a condition which can be evaluated and return
     * true or false.
     */
    class ConditionInterface
    {
    public:
        virtual ~ConditionInterface();

        /**
         * @brief Evaluate the Condition and return the outcome
         *
         * @return true if the condition is satisfied,
         * false otherwise.
         */
        virtual bool evaluate() = 0;

        /**
         * Some conditions need to be reset at some points.
         * E.g. a ConditionDuration counts the time since the
         * first time a Command was executed, and if this time
         * exceeds a certain preset time, returns true.
         * Therefore, it needs to be reset, i.e. it needs to start
         * counting, when the command is first executed..
         * ConditionOnce has a similar need.  This function is
         * called at such times.
         */
        virtual void reset();

        /**
         * The Clone Software Pattern.
         */
        virtual ConditionInterface* clone() const = 0;

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
         * ConditionInterface uses a DataSource, it is important that
         * you reimplement this function correctly though.
         */
        virtual ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    };

}

#endif
