#include "execution/CommandIllegal.hpp"

#include <iostream>
namespace ORO_Execution
{
    using namespace std;

    CommandIllegal::CommandIllegal( const std::string& cmd )
            : illCmd( cmd ), description( "" )
    {}

    CommandIllegal::CommandIllegal( const std::string& cmd, const std::string& descr )
            : illCmd( cmd ), description( descr )
    {}

    void CommandIllegal::execute()
    {
        cout << "Tried to execute an illegal command: " << endl;
        cout << illCmd << endl;
        cout << description << endl;
    }

    void CommandIllegal::warn()
    {
        cout << "Tried to create an illegal command: " << endl;
        cout << illCmd << endl;
        cout << description << endl;
    }

    std::string CommandIllegal::toString()
    {
        return "CommandIllegal";
    }
}
