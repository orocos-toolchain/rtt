/***************************************************************************
  tag: Peter Soetens  Wed Apr 17 16:16:06 CEST 2002  ReportExporterInterface.hpp 

                        ReportExporterInterface.hpp -  description
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


#ifndef REPORTEXPORTERINTERFACE_HPP
#define REPORTEXPORTERINTERFACE_HPP

#include "Time.hpp"
#include "NameServerRegistrator.hpp"

namespace ORO_CoreLib
{

    /**
     * This interfaces defines an object that exports 
     * reports of a certain type.
     */
    template <class _ReportType>
    class ReportExporterInterface
                :private NameServerRegistrator<ReportExporterInterface<_ReportType>*,std::string>
    {
        public:
            /**
             * The type of the report used.
             */
            typedef _ReportType ReportType;
            
            /**
             * Construct a default not nameserved ReportExporterInterface.
             */
            ReportExporterInterface() {}

            /**
             * Construct a default nameserved ReportExporterInterface.
             *
             * @param name The name of this ReportExporterInterface.
             */
            ReportExporterInterface(const std::string& name)
                    : NameServerRegistrator<ReportExporterInterface<ReportType>*,std::string>( nameserver, name, this )
            {}

            inline virtual ~ReportExporterInterface() {}

            /**
             * Get a copy of the last collected report.
             * 
             * @param A copy of the resulting report.
             */
            virtual void reportGet( ReportType& result) = 0;

            /**
             * Get a valid reference to the last collected report.
             * 
             * @param A reference to the resulting report.
             */
            virtual ReportType& reportGet() = 0;

            /**
             * Instructs to read a new report
             * @param timeStamp
             *        The time stamp which should be used to mark the
             *        data with.
             */
            virtual void refresh( Seconds timeStamp ) = 0;

            /**
             * The nameserver of this interface.
             *
             * @see NameServer
             */
            static NameServer< ReportExporterInterface<ReportType> *> nameserver;
    };

    template< class T>
    NameServer<ReportExporterInterface<T>*> ReportExporterInterface<T>::nameserver;

    class PropertyBag;

    template< class T >
    class Property;

    /**
     * A common used report format are strings.
     */
    typedef ReportExporterInterface<std::string> StringExporterInterface;

    /**
     * A common used report format are PropertyBags.
     */
    typedef ReportExporterInterface<Property<PropertyBag> > PropertyExporterInterface;
}

#endif
