/***************************************************************************
  tag: FMTC  do nov 2 13:06:09 CET 2006  TaskObject.hpp 

                        TaskObject.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_TASK_OBJECT_HPP
#define ORO_TASK_OBJECT_HPP

#include <string>
#include "OperationInterface.hpp"

namespace RTT
{
    /**
     * A task object groups a set of commands and methods (operations)
     * which may be invoked.
     */
    class TaskObject
        : public OperationInterface
    {
    protected:
        std::string mname;
        std::string mdescription;

        OperationInterface* mparent;

    public:
        /**
         * Create a TaskObject with a given \a name and \a description.
         */
        TaskObject(std::string name, std::string description ="A Task Object.");

        /**
         * Create a TaskObject with a given \a name and \a description and tie it to a \a parent.
         * @param parent The OperationInterface to tie the new TaskObject to. In case
         * a TaskObject with the same name is already present in \a parent, the TaskObject
         * will not be tied and new->getParent() == 0.
         */
        TaskObject(OperationInterface* parent, std::string name, std::string description ="A Task Object.");

        ~TaskObject();

        virtual OperationInterface* getParent() { return mparent; }

        virtual void setParent(OperationInterface* newparent) { mparent = newparent; }

        virtual void setEngine(ExecutionEngine* newengine);

        const std::string& getName() const { return mname; }

        const std::string& getDescription() const { return mdescription; }

        void setDescription(const std::string& d) { mdescription = d;}

        void setName(const std::string& n) { mname = n;}
    };
}


#endif
