#include "corelib/CommandNOP.hpp"

namespace ORO_CoreLib
{
    using namespace std;

    void CommandNOP::execute()
    {
#ifdef COMMANDNOPDEBUG
        cout << "CommandNOP::execute()" <<endl;
#endif

    }

    std::string CommandNOP::toString()
    {
        return "CommandNOP";
    }

    CommandInterface* CommandNOP::clone() const
    {
        return new CommandNOP(*this);
    }
}

