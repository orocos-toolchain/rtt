/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:59 CEST 2002  ReportWriter.hpp 

                        ReportWriter.hpp -  description
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
 
#ifndef REPORTWRITER_HPP
#define REPORTWRITER_HPP

#include "ReportCollectorInterface.hpp"
#include "WriteInterface.hpp"
#include "ReportExporterInterface.hpp"
#include "RunnableInterface.hpp"
#include "TimeService.hpp"

#include <vector>
#include <string>

namespace RTT
{

    /**
     * @brief A class which periodically collects string messages ('reports')
     * from StringExporterInterface objects and writes them out to a WriteInterface object.
     *
     * Should run in a TaskNonRealTime if the WriteInterface is a non realtime
     * device, like WriteCout. Use this class in conjunction with ReportingPropertyBag for
     * example, which generates the reports.
     * @deprecated
     */
    class ReportWriter 
        : public StringCollectorInterface,
          public RunnableInterface
    {
        public:

            /**
             * Create a ReportWriter object
             *
             * @param f The object to write the results to.
             */
            ReportWriter(WriteInterface<std::string>* f);

            /**
             * Destroy a ReportWriter object
             */
            virtual ~ReportWriter();

            virtual void exporterAdd(StringExporterInterface* exp);

            virtual void exporterRemove(StringExporterInterface* exp);

            virtual bool trigger();

            virtual void writeOut();

            virtual void step();

            virtual void resetTime( Seconds s=0);

        private:
            ReportWriter() {}

            WriteInterface<std::string>* rtf;

            const static int DEFAULT_EXPLIST_SIZE = 32;
            const static int DEFAULT_MESSAGE_SIZE = 1024;

            std::vector<StringExporterInterface*> expList;

            std::string message;

            TimeService::ticks localTime;
    };

}

#endif

