#include "execution/Types.hpp"

#include <pkgconf/system.h>
#ifdef OROPKG_GEOMETRY
// Include geometry support
#include <geometry/frames.h>
#endif

#include "execution/ParsedValue.hpp"

namespace ORO_Execution
{
#ifdef OROPKG_GEOMETRY
  using ORO_Geometry::Frame;
  using ORO_Geometry::Vector;
  using ORO_Geometry::Rotation;
#endif

  template<typename T>
  class TemplateTypeInfo
    : public TypeInfo
  {
    ParsedValueBase* buildConstant()
      {
        return new ParsedConstantValue<T>();
      };
    ParsedValueBase* buildVariable()
      {
        return new ParsedVariableValue<T>();
      };
    ParsedValueBase* buildAlias( DataSourceBase* b )
      {
        DataSource<T>* ds( dynamic_cast<DataSource<T>*>( b ) );
        if ( ! ds )
          return 0;
        return new ParsedAliasValue<T>( ds );
      };
  };

  TypeInfo::~TypeInfo()
  {
  };

  TypeInfoRepository& TypeInfoRepository::instance()
  {
    static TypeInfoRepository s;
    return s;
  };

  TypeInfo* TypeInfoRepository::type( const std::string& name )
  {
    map_t::iterator i = data.find( name );
    if ( i == data.end() )
      return 0;
    else return i->second;
  };

  TypeInfoRepository::~TypeInfoRepository()
  {
  };

  TypeInfoRepository::TypeInfoRepository()
  {
#ifdef OROPKG_GEOMETRY
    data["frame"] = new TemplateTypeInfo<Frame>();
    data["vector"] = new TemplateTypeInfo<Vector>();
    data["rotation"] = new TemplateTypeInfo<Rotation>();
#endif
    data["int"] = new TemplateTypeInfo<int>();
    data["string"] = new TemplateTypeInfo<std::string>();
    data["double"] = new TemplateTypeInfo<double>();
    data["bool"] = new TemplateTypeInfo<bool>();
  };
}
