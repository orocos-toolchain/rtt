#include "TemplateCommandFactory.hpp"
#include "TemplateDataSourceFactory.hpp"

#include <iostream>

using namespace ORO_Execution;

class FakeComponent
{
public:
  CommandFactoryInterface* createCommandFactory() {
    TemplateCommandFactory<FakeComponent>* ret = newCommandFactory( this );
    ret->add( "printString",
              command( &FakeComponent::printString,
                       &FakeComponent::returnTrue,
                       "print the specified string", "s", "string to print" ) );
    ret->add( "printInt",
              command( &FakeComponent::printInt,
                       &FakeComponent::returnTrue,
                       "print the specified int", "a", "int to print" ) );
    ret->add( "printDouble",
              command( &FakeComponent::printDouble,
                       &FakeComponent::returnTrue,
                       "print the specified double", "d", "double to print" ) );
    return ret;
  }

  DataSourceFactoryInterface* createDataSourceFactory() {
    TemplateDataSourceFactory<FakeComponent>* ret =
      newDataSourceFactory( this );
    ret->add( "someString",
              data( &FakeComponent::someString, "Returns some string." ) );
    ret->add( "length",
              data( &FakeComponent::length,
                    "Returns the length of a given string",
                    "s", "the string" ) );
    return ret;
  }

  std::string someString() const {
    return "appel";
  }

  int length( std::string s ) const {
    return s.length();
  }

  bool returnTrue() const {
    return true;
  }

  bool printString( std::string s ) {
    std::cout << "printString: " << s << std::endl;
    return true;
  }

  bool printDouble( double d ) {
    std::cout << "printDouble: " << d << std::endl;
  }

  bool printInt( int a ) {
    std::cout << "printInt: " << a << std::endl;
    return true;
  }
};
