/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:59 CEST 2002  ReportWriter.hpp 

                        ReportWriter.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
 
 
#ifndef REPORTWRITER_HPP
#define REPORTWRITER_HPP

#include "ReportCollectorInterface.hpp"
#include "WriteInterface.hpp"
#include "ReportExporterInterface.hpp"
#include "TaskPreemptible.hpp"
#include "HeartBeatGenerator.hpp"

namespace ORO_CoreLib
{

    /**
     * Implements an Active Report Listener.
     * Listens to added Exporters and publishes their info
     * on a WriteInterface object.
     */
    class ReportWriter 
        : public StringCollectorInterface,
          public TaskPreemptible
    {
        public:

            /**
             * Create a ReportWriter object
             *
             * @param f The object to write the results to.
             * @param period The periodicity of reporting results.
             */
            ReportWriter(WriteInterface* f, double period=0.01);

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
            ReportWriter() : TaskPreemptible(0) {}

            WriteInterface* rtf;

            const static int DEFAULT_EXPLIST_SIZE = 32;
            const static int DEFAULT_MESSAGE_SIZE = 1024;

            std::vector<StringExporterInterface*> expList;

            std::string message;

            HeartBeatGenerator::ticks localTime;
    };

}

#endif

