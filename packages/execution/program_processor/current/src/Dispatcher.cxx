#include "execution/Dispatcher.hpp"

namespace ORO_Execution
{

    using namespace std;
    using namespace ORO_CoreLib;

    NameServer<Dispatcher *> Dispatcher::nameserver;

    Dispatcher::Dispatcher( std::string name )
            : NameServerRegistrator<Dispatcher *>( nameserver, name, this ), procInt( 0 )
    {}

    void Dispatcher::process( std::string toProcess )
    {
        // feed <toProcess> to the parserInterfaces
        //cout << "Received in the Dispatcher, " << toProcess.length() << " bytes: " << toProcess << endl;

        if ( procInt != 0 )
            for ( vector<ParserInterface*>::iterator it = parsers.begin(); it != parsers.end(); ++it )
            {
                ( *it ) ->process( toProcess, procInt );
            }

        //cout << "done feeding the ParserInterfaces" << endl;
    }

}
