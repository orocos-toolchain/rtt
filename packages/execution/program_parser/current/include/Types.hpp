#ifndef TYPES_HPP
#define TYPES_HPP

#include <map>
#include <string>

namespace ORO_Execution
{
  class DataSourceBase;
  class ParsedValueBase;

  /**
   * \file We need some information on types if we want to make
   * constants, variables or aliases of them, the classes in this file
   * provide that information.
   */
  class TypeInfo
  {
  public:
    virtual ~TypeInfo();
    virtual ParsedValueBase* buildConstant() = 0;
    virtual ParsedValueBase* buildVariable() = 0;
    /**
     * build an alias with b as the value.  If b is of the wrong type,
     * 0 will be returned..
     */
    virtual ParsedValueBase* buildAlias( DataSourceBase* b ) = 0;
  };

  class TypeInfoRepository
  {
    TypeInfoRepository();
    ~TypeInfoRepository();
    typedef std::map<std::string, TypeInfo*> map_t;
    map_t data;
  public:
    static TypeInfoRepository& instance();
    TypeInfo* type( const std::string& name );
  };
}

#endif
