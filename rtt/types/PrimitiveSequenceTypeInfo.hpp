/***************************************************************************
  tag: Peter Soetens Tue Jul 5 17:12:56 2011 +0200 PrimitiveSequenceTypeInfo.hpp

                        PrimitiveSequenceTypeInfo.hpp -  description
                           -------------------
    begin                : Tue Jul 5 2011
    copyright            : (C) 2011 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_PRIMITIVE_SEQUENCE_TYPE_INFO_HPP
#define ORO_PRIMITIVE_SEQUENCE_TYPE_INFO_HPP

#include "PrimitiveTypeInfo.hpp"
#include "SequenceTypeInfoBase.hpp"
#include "MemberFactory.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * New-style SequenceTypeInfo which offers type info for sequences
         * which can \b NOT be sent over flow ports. This has huge code size
         * reduction, while still being able to use these sequences in structs
         * which \b are sent over flow ports.
         */
        template<class T, bool has_ostream=false>
        class PrimitiveSequenceTypeInfo
            : public PrimitiveTypeInfo<T,has_ostream>,
              public SequenceTypeInfoBase<T>,
              public MemberFactory,
              public CompositionFactory
        {
        public:
            PrimitiveSequenceTypeInfo(std::string name)
                : PrimitiveTypeInfo<T,has_ostream>(name), SequenceTypeInfoBase<T>()
            {}

        bool installTypeInfoObject(TypeInfo* ti) {
            // aquire a shared reference to the this object
            boost::shared_ptr< PrimitiveSequenceTypeInfo<T,has_ostream> > mthis = boost::dynamic_pointer_cast<PrimitiveSequenceTypeInfo<T,has_ostream> >( this->getSharedPtr() );
            assert(mthis);
            // Allow base to install first
            PrimitiveTypeInfo<T,has_ostream>::installTypeInfoObject(ti);
            SequenceTypeInfoBase<T>::installTypeInfoObject(ti);
            
            // Install the factories for primitive types
            ti->setMemberFactory( mthis );
            ti->setCompositionFactory( mthis );

            // Don't delete us, we're memory-managed.
            return false;
        }

            using PrimitiveTypeInfo<T,has_ostream>::buildVariable;
            base::AttributeBase* buildVariable(std::string name,int size) const
            {
                return SequenceTypeInfoBase<T>::buildVariable(name,size);
            }

            virtual bool composeType( base::DataSourceBase::shared_ptr dssource, base::DataSourceBase::shared_ptr dsresult) const
            {
                return SequenceTypeInfoBase<T>::composeType(dssource, dsresult);
            }

            virtual bool resize(base::DataSourceBase::shared_ptr arg, int size) const
            {
                return SequenceTypeInfoBase<T>::resize(arg,size);
            }
            virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const
            {
                return SequenceTypeInfoBase<T>::decomposeType(source);
            }
            virtual std::vector<std::string> getMemberNames() const {
                return SequenceTypeInfoBase<T>::getMemberNames();
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item, const std::string& name) const {
                return SequenceTypeInfoBase<T>::getMember(item,name);
            }

            virtual base::DataSourceBase::shared_ptr getMember(base::DataSourceBase::shared_ptr item,
                                                             base::DataSourceBase::shared_ptr id) const {
                return SequenceTypeInfoBase<T>::getMember(item, id);
            }
        };
    }
}

#endif
