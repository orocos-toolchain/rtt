#include "SequenceTypeInfo.hpp"

namespace RTT
{
    namespace types
    {
        bool get_container_item(std::vector<bool> & cont, int index)
        {
            if (index >= (int) (cont.size()) || index < 0)
                return false;
            return cont[index];
        }
    }
}
