#include "GlobalsRepository.hpp"

using namespace RTT::types;

GlobalsRepository::shared_ptr GlobalsRepository::mInstance;

GlobalsRepository::shared_ptr GlobalsRepository::Instance() {
    if (mInstance)
        return mInstance;
    mInstance.reset( new GlobalsRepository() );
    return mInstance;
}
