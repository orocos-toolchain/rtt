/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  TryCommand.cxx

                        TryCommand.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
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


#include <Logger.hpp>
#include "TryCommand.hpp"
#include "../internal/DataSources.hpp"

namespace RTT
{

    using namespace detail;

        TryCommand::TryCommand( ActionInterface* command,
                                AssignableDataSource<bool>::shared_ptr storage /*=0*/)
            :_result( storage == 0 ? new UnboundDataSource< ValueDataSource<bool> >(true) : storage ),
             c(command) {}

        TryCommand::~TryCommand() {
            delete c;
        }
        bool TryCommand::execute() {
            //Logger::In in("TryCommand");
            //Logger::log() <<Logger::RealTime << "execute()"<<Logger::endl;
            if (_result->get() == false) // has thrown in readArguments
                return false;
            try {
                c->execute();
            } catch( ... ) {
                _result->set( false );
                return true;
            }
            _result->set( true );
            return true;
        }
        void TryCommand::reset() {
            c->reset();
            _result->set(true);
        }

        bool TryCommand::valid() const {
            // ok to check conditions if command is valid or it failed.
            // we assume here that c behaves as a DispatchAction:

            return _result->get() == false || c->valid();
        }

        void TryCommand::readArguments() {
            try {
                c->readArguments();
            } catch( ... ) {
                _result->set( false );
                return;
            }
            _result->set( true );
        }

        ActionInterface* TryCommand::theCommand() const {
            return c;
        }

        AssignableDataSource<bool>::shared_ptr TryCommand::result() {
            return _result;
        }

        TryCommand* TryCommand::clone() const {
            return new TryCommand( c->clone(),
                                   _result );
        }

        TryCommand* TryCommand::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommand( c->copy( alreadyCloned ),
                                   _result->copy(alreadyCloned));
        }


        TryCommandResult::TryCommandResult( DataSource<bool>::shared_ptr ec, bool invert)
            :c(ec), _invert(invert) {}

        TryCommandResult::~TryCommandResult() {
            // do not delete !
        }

        bool TryCommandResult::evaluate() {
            // by default true means reject
            return  _invert != c->get();
        }

        ConditionInterface* TryCommandResult::clone() const {
            return new TryCommandResult( c, _invert ); // do not clone c !
        }

        ConditionInterface* TryCommandResult::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommandResult( c->copy(alreadyCloned), _invert );
        }

    EvalCommand::EvalCommand( DataSource<bool>::shared_ptr ds, AssignableDataSource<bool>::shared_ptr cache /*=0*/)
        :_cache( cache == 0 ? new UnboundDataSource<ValueDataSource<bool> >(false) : cache ),
         _ds(ds) {}

        EvalCommand::~EvalCommand() {
        }

    void EvalCommand::readArguments() {
    }

        bool EvalCommand::execute() {
            _ds->evaluate();
            _cache->set( _ds->rvalue() );
            return true;
        }

        void EvalCommand::reset() {
            _cache->set(false);
            _ds->reset();
        }

        AssignableDataSource<bool>::shared_ptr EvalCommand::cache() {
            return _cache;
        }

        ActionInterface* EvalCommand::clone() const {
            return new EvalCommand( _ds,
                                    _cache );
        }

        ActionInterface* EvalCommand::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new EvalCommand( _ds->copy( alreadyCloned ),
                                    _cache->copy(alreadyCloned) );
        }

        EvalCommandResult::EvalCommandResult( DataSource<bool>::shared_ptr ec)
            :c(ec) {}

        EvalCommandResult::~EvalCommandResult() {
            // do not delete !
        }

        bool EvalCommandResult::evaluate() {
            return c->get();
        }

        ConditionInterface* EvalCommandResult::clone() const {
            return new EvalCommandResult( c ); // do not clone c !
        }

        ConditionInterface* EvalCommandResult::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new EvalCommandResult( c->copy( alreadyCloned ) );
        }

}
