/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  CommandExecFunction.cxx 

                        CommandExecFunction.cxx -  description
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
 
 
#include "execution/CommandExecFunction.hpp"

namespace ORO_Execution
{

    namespace {
        struct ValidFCondition
            : public ORO_CoreLib::ConditionInterface
        {
            DataSource<bool>::shared_ptr _a;
        public:
            ValidFCondition( DataSource<bool>* a)
                : _a( a )
            {}

            bool evaluate()
            {
                return _a->get();
            }

            ORO_CoreLib::ConditionInterface* clone() const
            {
                return new ValidFCondition( _a.get() );
            }

            ORO_CoreLib::ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
            {
                return new ValidFCondition( _a->copy( alreadyCloned ) );
            }
        };
    }

    ORO_CoreLib::ConditionInterface* CommandExecFunction::createValidCondition() const
    {
        return new ValidFCondition( maccept.get() );
    }

}
