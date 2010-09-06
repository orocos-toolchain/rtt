/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  SendHandleAlias.hpp

                        SendHandleAlias.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_SENDHANDLEALIAS_HPP_
#define ORO_SENDHANDLEALIAS_HPP_

#include "../base/AttributeBase.hpp"
#include "../OperationInterface.hpp"
#include "../rtt-fwd.hpp"


namespace RTT {
    namespace scripting {
        /**
         * Keeps track of a DataSource which has a SendHandle
         * and the factory for creating the collect functions
         * associated with that handle.
         */
        class SendHandleAlias
            : public base::AttributeBase
        {
            base::DataSourceBase::shared_ptr data;
            OperationInterfacePart* fact;
        public:
            SendHandleAlias(const std::string& name, base::DataSourceBase::shared_ptr d, OperationInterfacePart* opf );

            base::DataSourceBase::shared_ptr getDataSource() const;
            OperationInterfacePart* getFactory() const;

            SendHandleAlias* clone() const;

            SendHandleAlias* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacements, bool );
        };

    }
}

#endif /* ORO_SENDHANDLEALIAS_HPP_ */
