#ifndef ORO_SEQUENCE_TYPE_INFO_HPP
#define ORO_SEQUENCE_TYPE_INFO_HPP

#include "TemplateTypeInfo.hpp"
#include "SequenceTypeInfoBase.hpp"

namespace RTT
{
    namespace types
    {
        /**
         * Old-style SequenceTypeInfo which offers type info for sequences
         * which can be sent over flow ports.
         */
        template<class T, bool has_ostream=false>
        class SequenceTypeInfo
        : public TemplateTypeInfo<T,has_ostream>, public SequenceTypeInfoBase<T>
        {
        public:
            SequenceTypeInfo(std::string name)
            : TemplateTypeInfo<T,has_ostream>(name), SequenceTypeInfoBase<T>(this)
            {}

            using TemplateTypeInfo<T,has_ostream>::buildVariable;
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
