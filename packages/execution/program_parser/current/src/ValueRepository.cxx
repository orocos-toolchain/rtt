
#include "execution/ParsedValue.hpp"
#include "execution/ValueRepository.hpp"

namespace ORO_Execution
{

  ValueRepository::ValueRepository()
  {
  }

  ValueRepository::~ValueRepository()
  {
    clear();
  }

  void ValueRepository::clear()
  {
    for ( map_t::iterator i = values.begin(); i != values.end(); ++i )
      delete i->second;
    values.clear();
  }

  void ValueRepository::setValue( const std::string& name,
                              ParsedValueBase* value )
  {
    std::auto_ptr<ParsedValueBase> v( value );
    map_t::iterator i = values.find( name );
    // if the name is already defined, we replace its definition..
    if ( i != values.end() )
      delete i->second;
    values[name] = v.release();
  }

  void ValueRepository::addConstant( const std::string& name, bool value )
  {
    setValue( name, new ParsedAliasValue<bool>(
                new VariableDataSource<bool>( value ) ) );
  }

  void ValueRepository::addConstant( const std::string& name, int value )
  {
    setValue( name, new ParsedAliasValue<int>(
                new VariableDataSource<int>( value ) ) );
  }

  void ValueRepository::addConstant( const std::string& name, double value )
  {
    setValue( name, new ParsedAliasValue<double>(
                new VariableDataSource<double>( value ) ) );
  }

  void ValueRepository::addConstant( const std::string& name,
                                 const std::string& value )
  {
    setValue( name, new ParsedAliasValue<std::string>(
                new VariableDataSource<std::string>( value ) ) );
  }

  void ValueRepository::removeValue( const std::string& name )
  {
    map_t::iterator i = values.find( name );
    if ( i != values.end() )
      delete i->second;
    values.erase( name );
  }

  ParsedValueBase* ValueRepository::getValue( const std::string& name )
  {
    map_t::iterator i = values.find( name );
    if ( i == values.end() ) return 0;
    else return i->second;
  }

  bool ValueRepository::isDefined( const std::string& name ) const
  {
    return values.find( name ) != values.end();
  }

}
