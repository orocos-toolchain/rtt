
#pragma implementation
#include "execution/TaskBrowser.hpp"

namespace ORO_Execution
{
    std::vector<std::string> TaskBrowser::completes;
    std::vector<std::string>::iterator TaskBrowser::complete_iter;
    std::string TaskBrowser::component;
    std::string TaskBrowser::method;
    std::string TaskBrowser::datasource;
    std::string TaskBrowser::text;
    TaskContext* TaskBrowser::taskcontext = 0;
}
