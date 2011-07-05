#ifndef ORO_PRIMITIVE_SEQUENCE_TYPE_INFO_HPP
#define ORO_PRIMITIVE_SEQUENCE_TYPE_INFO_HPP

#include "PrimitiveTypeInfo.hpp"
#include "SequenceTypeInfoBase.hpp"

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
        : public PrimitiveTypeInfo<T,has_ostream>, public SequenceTypeInfoBase<T>
        {
        public:
            PrimitiveSequenceTypeInfo(std::string name)
            : PrimitiveTypeInfo<T,has_ostream>(name), SequenceTypeInfoBase(this)
            {}

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
