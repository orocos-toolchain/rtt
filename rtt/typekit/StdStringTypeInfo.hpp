#include <string>
#include "../types/SequenceTypeInfo.hpp"

namespace RTT
{
    namespace types
    {

        /**
         * Standard string specialisation that removes decomposition.
         */
        struct StdStringTypeInfo: public SequenceTypeInfo<std::string, true>
        {
            StdStringTypeInfo() :
                SequenceTypeInfo<std::string, true> ("string")
            {
            }

            base::AttributeBase* buildVariable(std::string name, int size) const
            {
                string t_init(size, ' '); // we can't use the default char(), which is null !

                // returned type is identical to parent, but we set spaces.
                base::AttributeBase* ret = SequenceTypeInfo<std::string, true>::buildVariable(name, size);
                Attribute<std::string> tt = ret;
                tt.set(t_init);
                return ret;
            }

            virtual bool composeType(base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const
            {
                // First, try a plain update.
                if (result->update(source.get()))
                    return true;
                return false;
            }

        };
    }
}
