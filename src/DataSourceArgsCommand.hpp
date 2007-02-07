/***************************************************************************
  tag: FMTC  do nov 2 13:06:11 CET 2006  DataSourceArgsCommand.hpp 

                        DataSourceArgsCommand.hpp -  description
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
 
 
#ifndef ORO_DATASOURCE_ARGS_COMMAND_HPP
#define ORO_DATASOURCE_ARGS_COMMAND_HPP

#include "CommandFunctors.hpp"
#include "CommandProcessor.hpp"
#include "CommandDSFunctors.hpp"
#include "DispatchInterface.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A Command which dispatches locally.
         * The arguments are fetched from DataSources.
         */
        template<class CommandT,
                 class CommandF = detail::Functor<CommandT> >
        class DataSourceArgsCommand
            : public DispatchInterface
        {
            CommandF mcom;
            mutable CommandF mcon;
            CommandProcessor* mcp;
            bool minvoked, maccept, mvalid, mexec, minvert;
        public:
            typedef boost::function_traits<CommandT> traits;
            typedef CommandT Signature;
            typedef bool result_type;

            DataSourceArgsCommand(boost::function<CommandT> com, boost::function<CommandT> con, CommandProcessor* cp, bool inverted)
                : mcom( com ), mcon( con ),
                  mcp( cp ),
                  minvoked(false), maccept(false),
                  mvalid(false), mexec(false), minvert(inverted)
            {
            }

            DataSourceArgsCommand(CommandF com, CommandF con, CommandProcessor* cp, bool inverted)
                : mcom( com ), mcon( con ),
                  mcp( cp ),
                  minvoked(false), maccept(false),
                  mvalid(false), mexec(false), minvert(inverted)
            {
            }

            DataSourceArgsCommand<CommandT,CommandF>* create() const
            {
                return clone();
            }

            template<class Arg1T>
            DataSourceArgsCommand<CommandT,CommandF>* create(DataSource<Arg1T>* a1) const
            {
                DataSourceArgsCommand<CommandT,CommandF>* r =  this->clone();
                r->mcom.setArguments(a1);
                r->mcon.setArguments(a1);
                return r;
            }

            template<class Arg1T, class Arg2T>
            DataSourceArgsCommand<CommandT,CommandF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) const
            {
                DataSourceArgsCommand<CommandT,CommandF>* r =  this->clone();
                r->mcom.setArguments(a1, a2);
                r->mcon.setArguments(a1, a2);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T>
            DataSourceArgsCommand<CommandT,CommandF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) const
            {
                DataSourceArgsCommand<CommandT,CommandF>* r =  this->clone();
                r->mcom.setArguments(a1, a2, a3);
                r->mcon.setArguments(a1, a2, a3);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T>
            DataSourceArgsCommand<CommandT,CommandF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) const
            {
                DataSourceArgsCommand<CommandT,CommandF>* r =  this->clone();
                r->mcom.setArguments(a1, a2, a3, a4);
                r->mcon.setArguments(a1, a2, a3, a4);
                return r;
            }

            bool ready() const {
                return !minvoked;
            }

            virtual bool dispatch() {
                if (minvoked)
                    return false;
                minvoked = true;
                this->readArguments();
                return maccept = mcp->process( this );
            }

            virtual void readArguments() { mcom.readArguments(); }

            virtual bool execute() {
                mvalid = mcom.execute();
                mexec = true;
                return mvalid;
            }
        
            virtual bool done() const {
                if (mexec && mvalid )
                    return mcon.evaluate() != minvert;
                return false;
            }
     
            virtual void reset() {
                minvoked = (false);
                maccept = (false);
                mvalid = (false); 
                mexec = (false);
            }

            virtual bool sent() const {
                return minvoked;
            }

            virtual bool accepted() const {
                return maccept;
            }

            virtual bool executed() const {
                return mexec;
            }

            virtual bool valid() const {
                return mvalid;
            }

            virtual ConditionInterface* createCondition() const
            {
                return new detail::ConditionFunctor<CommandT,CommandF>(mcon, minvert);
            }

            virtual DataSourceArgsCommand<CommandT,CommandF>* clone() const {
                return new DataSourceArgsCommand<CommandT,CommandF>(*this);
            }

            virtual DataSourceArgsCommand<CommandT,CommandF>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new DataSourceArgsCommand<CommandT,CommandF>(CommandF(mcom.copy(alreadyCloned)),
                                                                    CommandF(mcon.copy(alreadyCloned)),mcp, minvert);
            }

        };
    }
}

#endif
