/***************************************************************************
  tag: Peter Soetens  Wed Apr 17 16:15:55 CEST 2002  ReportListenerInterface.h 

                        ReportListenerInterface.h -  description
                           -------------------
    begin                : Wed April 17 2002
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


#ifndef REPORTLISTENERINTERFACE_HPP
#define REPORTLISTENERINTERFACE_HPP

#include "ReportExporterInterface.hpp"

namespace ORO_CoreLib
{
    /**
     * This interface defines an object that collects one or
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
             * Add an object to listen to 
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
