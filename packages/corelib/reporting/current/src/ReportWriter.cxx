/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  ReportWriter.cxx 

                        ReportWriter.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
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

#include "corelib/ReportWriter.hpp"
#include "corelib/TimeService.hpp"
//#include "corelib/rtconversions.hpp"


namespace ORO_CoreLib
{
     ReportWriter::ReportWriter(WriteInterface<std::string>* f)
         : rtf(f), localTime( TimeService::Instance()->getTicks() )
     {
         expList.reserve(DEFAULT_EXPLIST_SIZE);
         message.reserve(DEFAULT_MESSAGE_SIZE);
     }

     ReportWriter::~ReportWriter()
     {
     }
     void ReportWriter::exporterAdd(StringExporterInterface* exp)
     {
         expList.push_back(exp);
     }
     void ReportWriter::exporterRemove(StringExporterInterface* exp)
     {
         std::vector<StringExporterInterface*>::iterator itl;
         itl = find( expList.begin(), expList.end(), exp);
         expList.erase(itl);
     }

     bool ReportWriter::trigger()
     {
         // Trigger rereading the data
         std::vector<StringExporterInterface*>::iterator itl;
         for (itl= expList.begin(); itl != expList.end(); ++itl)
             (*itl)->refresh( TimeService::Instance()->secondsSince(localTime) );
         return true;
     }

     void ReportWriter::writeOut()
     {
         std::string s;
         // get all data and then flush the fifo
         std::vector<StringExporterInterface*>::iterator itl;
         for (itl= expList.begin(); itl != expList.end(); ++itl)
             {
                 (*itl)->reportGet(message);
                 //s = inttostring( message.length() ); NO LONGER NEEDED
                 //rtf->write(s.data(), s.length() );
                 rtf->Push( message );
             }

     }

     void ReportWriter::step()
     {
         trigger();
         writeOut();
     }

     void ReportWriter::resetTime( Seconds s)
     {
         localTime = TimeService::Instance()->getTicks();
         localTime += TimeService::nsecs2ticks( long(s * 1000.*1000*1000) );
     }

 }

