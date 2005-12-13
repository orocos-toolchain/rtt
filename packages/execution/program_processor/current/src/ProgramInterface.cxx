
#include "execution/ProgramInterface.hpp"
#include <corelib/Logger.hpp>

namespace ORO_Execution
{
    using namespace ORO_CoreLib;

    void ProgramInterface::handleUnload() {}

    ProgramInterface::ProgramInterface(ProgramProcessor* progp/* = 0*/) : pStatus(Status::unloaded), pp(progp) {}

    ProgramInterface::~ProgramInterface()
    {}

    void ProgramInterface::setProgramProcessor(ProgramProcessor* progp) {
        pp = progp;
        if (pp) {
            Logger::log() << Logger::Debug <<"Program "+this->getName()+" loaded in ProgramProcessor."<<Logger::endl;
            pStatus = Status::stopped;
        }else {
            Logger::log() << Logger::Debug <<"Program "+this->getName()+" unloaded from ProgramProcessor."<<Logger::endl;
            pStatus = Status::unloaded;
            this->handleUnload();
        }
    }
}
