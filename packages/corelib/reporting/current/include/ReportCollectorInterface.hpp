/***************************************************************************
  tag: Peter Soetens  Wed Apr 17 16:15:55 CEST 2002  ReportListenerInterface.h 

                        ReportListenerInterface.h -  description
                           -------------------
    begin                : Wed April 17 2002
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

#ifndef REPORTLISTENERINTERFACE_HPP
#define REPORTLISTENERINTERFACE_HPP

#include "ReportExporterInterface.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    /**
     * @brief This interface defines an object that collects one or
     * more reports from ReportExporters.
     */
    template <class _ReportType>
    class ReportCollectorInterface
    {
        public:
        virtual ~ReportCollectorInterface() {}
            /**
             * The type of the report used.
             */
            typedef _ReportType ReportType;
            
            /**
             * Add an object to read reports from. 
             */
            virtual void exporterAdd(ReportExporterInterface<ReportType>*) = 0;

            /**
             * remove an object that exports data
             */
            virtual void exporterRemove(ReportExporterInterface<ReportType>*) = 0;

            /**
             * Force exporting of data of all the exporters.
             * This is a non blocking operation.
             *
             * @return true if the operation could proceed, false otherwise.
             */
            virtual bool trigger() = 0;

        /**
         * Return the number of exporter clients.
         * If zero, this PropertyReporter may safely be destroyed.
         */
        virtual int nbOfExporters() const = 0;
    };

    /**
     * A common used report format are strings.
     */
    typedef ReportCollectorInterface<std::string> StringCollectorInterface;

    /**
     * A common used report format are PropertyBags.
     */
    typedef ReportCollectorInterface<Property<PropertyBag> > PropertyCollectorInterface;

}

#endif
