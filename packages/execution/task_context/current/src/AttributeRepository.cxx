
#include "execution/AttributeRepository.hpp"
#include "execution/TaskAttribute.hpp"
#include "execution/DataSource.hpp"

namespace ORO_Execution
{

  AttributeRepository::AttributeRepository()
  {
  }

  AttributeRepository::~AttributeRepository()
  {
      // we do not claim automatically ownership
      // call clear() manually to delete all contents.
      //clear();
  }

    AttributeRepository* AttributeRepository::copy( std::map<const DataSourceBase*, DataSourceBase*>& repl ) const
    {
        AttributeRepository* ar = new AttributeRepository();
        for ( map_t::const_iterator i = values.begin(); i != values.end(); ++i )
            ar->setValue(i->first, i->second->copy( repl ) );
        return ar;
    }


  void AttributeRepository::clear()
  {
    for ( map_t::iterator i = values.begin(); i != values.end(); ++i )
      delete i->second;
    values.clear();
  }

  void AttributeRepository::setValue( const std::string& name,
                                      TaskAttributeBase* value )
  {
    std::auto_ptr<TaskAttributeBase> v( value );
    map_t::iterator i = values.find( name );
    // if the name is already defined, we replace its definition..
    if ( i != values.end() )
      delete i->second;
    values[name] = v.release();
  }

  void AttributeRepository::addConstant( const std::string& name, bool value )
  {
    setValue( name, new TaskAliasAttribute<bool>(
                new VariableDataSource<bool>( value ) ) );
  }

  void AttributeRepository::addConstant( const std::string& name, int value )
  {
    setValue( name, new TaskAliasAttribute<int>(
                new VariableDataSource<int>( value ) ) );
  }

  void AttributeRepository::addConstant( const std::string& name, double value )
  {
    setValue( name, new TaskAliasAttribute<double>(
                new VariableDataSource<double>( value ) ) );
  }

  void AttributeRepository::addConstant( const std::string& name,
                                 const std::string& value )
  {
    setValue( name, new TaskAliasAttribute<std::string>(
                new VariableDataSource<std::string>( value ) ) );
  }

  void AttributeRepository::removeValue( const std::string& name )
  {
    map_t::iterator i = values.find( name );
    if ( i != values.end() )
      delete i->second;
    values.erase( name );
  }

  TaskAttributeBase* AttributeRepository::getValue( const std::string& name )
  {
    map_t::iterator i = values.find( name );
    if ( i == values.end() ) return 0;
    else return i->second;
  }

  bool AttributeRepository::isDefined( const std::string& name ) const
  {
    return values.find( name ) != values.end();
  }

}
