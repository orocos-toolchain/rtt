/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:59 CEST 2002  ReportHandler.hpp 

                        ReportHandler.hpp -  description
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
 

#ifndef REPORTHANDLER_HPP
#define REPORTHANDLER_HPP

#include "MonitorInterface.hpp"
#include "ReportExporterInterface.hpp"
#include "PropertyBag.hpp"
#include "marshalling/SimpleMarshaller.hpp"

#include <sstream>

namespace ORO_CoreLib
{
    /**
     * @brief A stub that monitors variables, puts them in a report and is able to
     * export them in a threadsafe way.
     * @deprecated by PropertyExporter
     */
    class ReportHandler : 
        public StringExporterInterface,
        public MonitorInterface
    {
        struct Updater { virtual void update() = 0; virtual PropertyBase* base() = 0; };
        /**
         * A helper class for storing pointers to monitorred variables
         * and the corresponding property.
         *
         * @template T The type of the property and base type of the pointer.
         */
        template <class T>
        struct ReportPointer : public Updater
        {
            ReportPointer(const T* _d, Property<T>* _p) : d(_d), p(_p) {}
            virtual void update() { *p = *d; }
            virtual PropertyBase* base() { return p; }
            const T* d;
            Property<T>* p;
        };
        
        public:
            /**
             * Create a local not nameserved ReportHandler.
             */
            ReportHandler();

            /**
             * Create a ReportHandler which is nameserved as ReportExporterInterface
             * with a name.
             *
             * @param name The name of the ReportHandler.
             */
            ReportHandler(const std::string& name);
            
            virtual ~ReportHandler();

            virtual void monitor(const double* d, const std::string& name, const std::string& description="");
            virtual void monitor(const int* i, const std::string& name, const std::string& description="");

            virtual void remove(const std::string& name);
            virtual void refresh( Seconds timeStamp );
            virtual void reportGet(std::string& );
            virtual std::string& reportGet();
        protected:

        Property<PropertyBag>* reportSet;
        std::vector<Updater*> updates;
        Seconds time;

        std::stringstream report;
        std::string report_copy;
        SimpleMarshaller<std::stringstream> marshaller;
    };

}

#endif
