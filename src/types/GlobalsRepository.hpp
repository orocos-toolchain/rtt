#ifndef ORO_GLOBALS_REPOSITORY_HPP
#define ORO_GLOBALS_REPOSITORY_HPP

#include "../interface/AttributeRepository.hpp"

namespace RTT {
    namespace types {
        /**
         * An AttributeRepository for holding global variables.
         * The RTT uses this to export enumerations to scripting,
         * but users may extend this as well for their own applications.
         *
         * It is recommended to use the setValue() function to add
         * globals, such that they become owned by the GlobalsRepository.
         */
        class RTT_API GlobalsRepository
            : public interface::AttributeRepository
        {
        public:
            typedef boost::shared_ptr<GlobalsRepository> shared_ptr;

            static shared_ptr Instance();
        private:
            static shared_ptr mInstance;
        };            
    }
}

#endif
