/***************************************************************************
  tag: FMTC  do nov 2 13:06:09 CET 2006  OperationInterface.hpp

                        OperationInterface.hpp -  description
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


#ifndef ORO_OPERATION_INTERFACE_HPP
#define ORO_OPERATION_INTERFACE_HPP

#include <string>
#include <vector>
#include "rtt-config.h"
#include "../internal/CommandRepository.hpp"
#include "MethodRepository.hpp"
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
#include "EventService.hpp"
#endif
#include "AttributeRepository.hpp"

namespace RTT
{
    class ExecutionEngine;

    /**
     * The interface for accessing and executing 'operations', being commands,
     * methods, events and attributes. OperationInterface objects can be
     * hierarchically nested.
     */
    class RTT_API OperationInterface
    {
    protected:
        CommandRepository mcommands;

        MethodRepository mmethods;

#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
        EventService mevents;
#endif

        AttributeRepository mattributes;

        typedef std::vector< OperationInterface* > Objects;
        /// the Child TaskObjects.
        Objects mobjects;
    public:
        OperationInterface();

        OperationInterface(ExecutionEngine* ee);
        /**
         * A list of all child objects of this interface.
         */
        typedef std::vector< std::string > ObjectList;

        /**
         * Returns the commands of this interface.
         */
        CommandRepository* commands() { return &mcommands; }

        const CommandRepository* commands() const { return &mcommands; }

        /**
         * Returns the methods of this interface.
         */
        MethodRepository* methods() { return &mmethods; }

        const MethodRepository* methods() const { return &mmethods; }

        /**
         * The task-local events ( 'signals' ) of this TaskContext.
         */
        EventService* events() {
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
            return &mevents;
#else
            return 0;
#endif
        }

        /**
         * The task-local events ( 'signals' ) of this TaskContext.
         */
        const EventService* events() const {
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
            return &mevents;
#else
            return 0;
#endif
        }

        /**
         * Returns the attributes of this interface.
         */
        AttributeRepository* attributes() { return &mattributes; }

        const AttributeRepository* attributes() const { return &mattributes; }

        virtual ~OperationInterface();

        virtual void clear();

        /**
         * Returns the name of this interface.
         */
        virtual const std::string& getName() const = 0;

        /**
         * Returns the description of this interface.
         */
        virtual const std::string& getDescription() const = 0;

        /**
         * Add a new child interface to this interface.
         *
         * @param obj This object becomes owned by this interface.
         *
         * @return true if it cuold be added, false if such
         * object already exists.
         */
        virtual bool addObject( OperationInterface *obj );

        /**
         * Get a pointer to a previously added TaskObject
         *
         * @param obj_name The name of the TaskObject
         *
         * @return the pointer
         */
        virtual OperationInterface* getObject(const std::string& obj_name );

        /**
         * Get a list of all the object names of this interface
         *
         * @return a list of string names.
         */
        virtual ObjectList getObjectList() const;

        /**
         * Remove and delete a previously added TaskObject.
         * Deletion will only occur if \a obj_name's parent is \a this.
         * You can avoid deletion by first calling
         * @code this->getObject( obj_name )->setParent(0); @endcode
         *
         * @param obj_name The name of the TaskObject to remove
         *
         * @return true if found and removed, false otherwise.
         */
        virtual bool removeObject(const std::string& obj_name );

        /**
         * Returns the parent OperationInterface in which this TaskObject lives.
         * A TaskObject can have only one parent.
         * @return null in case no parent is set, the parent otherwise.
         */
        virtual OperationInterface* getParent() = 0;

        /**
         * Set a new parent for this interface.  Do not call this
         * method directly. This function is automatically called when
         * a TaskObject is added to another TaskObject.
         */
        virtual void setParent(OperationInterface* newparent) = 0;

        /**
         * Set the execution engine of the parent TaskContext.  Do not
         * call this method directly. This function is automatically
         * called when a TaskObject is added to a TaskContext.
         */
        virtual void setEngine(ExecutionEngine* newengine) = 0;

    };
}


#endif
