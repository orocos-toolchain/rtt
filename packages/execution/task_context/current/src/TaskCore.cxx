
#include "execution/TaskCore.hpp"

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    using namespace std;

    TaskCore::TaskCore(const std::string& name)
        :  _task_name(name),
           ee(this)
    {
    }

    TaskCore::TaskCore(const std::string& name, ExecutionEngine* parent )
        :  _task_name(name),
           ee(this, parent )
    {
    }


    TaskCore::~TaskCore()
    {
    }

    bool TaskCore::startup()
    {
        return true;
    }
    void TaskCore::update()
    {}
    void TaskCore::shutdown()
    {}

}

