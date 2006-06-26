/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  TryCommand.cxx 

                        TryCommand.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#include <corelib/Logger.hpp>
#include "execution/TryCommand.hpp"
#include <corelib/DataSources.hpp>

namespace ORO_Execution 
{
    using namespace ORO_CoreLib;
    using namespace ORO_CoreLib::detail;

        TryCommand::TryCommand( CommandInterface* command,
                                AssignableDataSource<bool>::shared_ptr storage /*=0*/,
                                AssignableDataSource<bool>::shared_ptr execstat/*=0*/ )
            :_result( storage == 0 ? new UnboundDataSource< ValueDataSource<bool> >(true) : storage ),
             _executed( execstat == 0 ? new UnboundDataSource< ValueDataSource<bool> >(false) : execstat ),
             c(command) {}

        TryCommand::~TryCommand() {
            delete c;
        }
        bool TryCommand::execute() {
            //ORO_CoreLib::Logger::In in("TryCommand");
            //ORO_CoreLib::Logger::log() <<ORO_CoreLib::Logger::RealTime << "execute()"<<ORO_CoreLib::Logger::endl;
            _result->set( c->execute() );
            _executed->set(true);
            return true;
        }
        void TryCommand::reset() {
            c->reset();
            _result->set(true);
            _executed->set(false);
        }

        void TryCommand::readArguments() {
            c->readArguments();
        }

        CommandInterface* TryCommand::theCommand() const {
            return c;
        }

        AssignableDataSource<bool>::shared_ptr TryCommand::result() {
            return _result;
        }

        AssignableDataSource<bool>::shared_ptr TryCommand::executed() {
            return _executed;
        }

        TryCommand* TryCommand::clone() const {
            return new TryCommand( c->clone(),
                                   _result, _executed );
        }

        TryCommand* TryCommand::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new TryCommand( c->copy( alreadyCloned ),
                                   _result->copy(alreadyCloned),
                                   _executed->copy(alreadyCloned) );
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
        _ds->evaluate();
    }

        bool EvalCommand::execute() {
            _cache->set( _ds->value() );
            return true;
        }

        void EvalCommand::reset() {
            _cache->set(false);
            _ds->reset();
        }

        AssignableDataSource<bool>::shared_ptr EvalCommand::cache() {
            return _cache;
        }

        CommandInterface* EvalCommand::clone() const {
            return new EvalCommand( _ds,
                                    _cache );
        }

        CommandInterface* EvalCommand::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
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
