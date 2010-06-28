#include <vector>
#include "../types/SequenceTypeInfo.hpp"
#include "../types/VectorComposition.hpp"
#include "../types/TypeStream.hpp"

namespace RTT
{
    namespace types
    {
        using namespace std;
        using namespace detail;

        /**
         * This class tells Orocos how to handle std::vector<double>.
         */
        struct StdVectorTypeInfo: public SequenceTypeInfo<std::vector<double>, true>
        {
            StdVectorTypeInfo(std::string name) :
                SequenceTypeInfo<std::vector<double>, true> (name)
            {
            }

            //!Override default in order to take legacy formats into account.
            bool composeTypeImpl(const PropertyBag& bag, std::vector<double>& result) const
            {
                return composeProperty(bag, result) || SequenceTypeInfo<std::vector<double>, true>::composeTypeImpl(bag, result);
            }

        };
    }
}
