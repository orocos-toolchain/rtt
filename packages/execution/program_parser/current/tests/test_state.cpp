#define BOOST_SPIRIT_DEBUG 1

#include "FakeComponent.hpp"
#include "execution/StateGraphParser.hpp"
#include "execution/ParsedStateContext.hpp"
#include "execution/GlobalFactory.hpp"
#include "execution/Processor.hpp"
#include "control_kernel/ExecutionExtension.hpp"
#include "control_kernel/KernelInterfaces.hpp"

#include <algorithm>
#include <iterator>
#include <fstream>
#include <iomanip>

using namespace ORO_Execution;
using ORO_ControlKernel::ExecutionExtension;
using ORO_ControlKernel::ControlKernelInterface;

class MyControlKernel
  : public ControlKernelInterface
{
  void step()
    {
    }

  bool initialize()
    {
      return true;
    }

  void finalize()
    {
    }
};

int main ()
{
  FakeComponent comp;

  MyControlKernel kernelint;

  ExecutionExtension fact( &kernelint );

  fact.commandFactory().registerObject( "fakecomp", comp.createCommandFactory() );
  fact.dataFactory().registerObject( "fakecomp", comp.createDataSourceFactory() );
  fact.commandFactory().registerObject( "Execution", fact.createCommandFactory() );
  fact.dataFactory().registerObject( "Execution", fact.createDataSourceFactory() );

  std::string statedesc;
  statedesc.reserve( 1000 );
  std::ifstream inputfile( "test.osd" );
  inputfile.unsetf( std::ios_base::skipws );
  std::copy( std::istream_iterator<char>( inputfile ),
             std::istream_iterator<char>(),
             std::back_inserter( statedesc ) );

  iter_t begin( statedesc.begin(), statedesc.end(), "file", 1, 1 );
  iter_t end;
  StateGraphParser stateparser( begin, fact.getProcessor(), &fact );

  std::vector<ParsedStateContext*> stategraph = stateparser.parse( begin, end );

  for ( std::vector<ParsedStateContext*>::iterator i = stategraph.begin();
        i != stategraph.end(); ++i )
    ( *i )->registerWithProcessor( fact.getProcessor() );

  std::cout << "stategraph.size()" << stategraph.size() << std::endl;
  std::cout << "stategraph.front()->startRunning..." << std::endl;
  fact.startStateContext( stategraph[0]->getName() );
  std::cout << "going through 20 states" << std::endl;
  for ( int i = 0; i < 20; ++i )
    fact.step();
};
