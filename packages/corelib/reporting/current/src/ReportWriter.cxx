
#include "corelib/ReportWriter.hpp"
#include "corelib/FifoRTOut.hpp"
#include "corelib/HeartBeatGenerator.hpp"
//#include "corelib/rtconversions.hpp"


namespace ORO_CoreLib
{
     ReportWriter::ReportWriter(WriteInterface* f, double period)
         :TaskPreemptible(period), rtf(f), localTime( HeartBeatGenerator::Instance()->ticksGet() )
     {
         rtos_printf("Creating ReportWriter\n");
         expList.reserve(DEFAULT_EXPLIST_SIZE);
         message.reserve(DEFAULT_MESSAGE_SIZE);
     }

     ReportWriter::~ReportWriter()
     {
         stop();
         rtos_printf("Destroying ReportWriter\n");
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
             (*itl)->refresh( HeartBeatGenerator::Instance()->secondsSince(localTime) );
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
                 rtf->write(message.data(), message.length() );
             }

     }

     void ReportWriter::step()
     {
         trigger();
         writeOut();
     }

     void ReportWriter::resetTime( Seconds s)
     {
         localTime = HeartBeatGenerator::Instance()->ticksGet();
         localTime += HeartBeatGenerator::nsecs2ticks( long(s * 1000.*1000*1000) );
     }

 }

