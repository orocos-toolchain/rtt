#include "execution/ConditionParser.hpp"

#include "corelib/ConditionDuration.hpp"
#include "corelib/ConditionTrue.hpp"
#include "corelib/ConditionFalse.hpp"
#include "execution/ConditionBoolDataSource.hpp"
#include "execution/ConditionComposite.hpp"
#include "execution/ConditionCompare.hpp"

#include <boost/bind.hpp>

#include "execution/ParseContext.hpp"

#include "execution/DataSourceFactory.hpp"
#include "execution/TemplateFactory.hpp"

namespace ORO_Execution
{
  using boost::bind;
  using ORO_CoreLib::ConditionDuration;
  using ORO_CoreLib::PropertyBag;

  ConditionParser::ConditionParser( ParseContext& c )
    : ret( 0 ), context( c ), expressionparser( c )
  {
    BOOST_SPIRIT_DEBUG_RULE( condition );

    /**
     * conditions used to be more complex, but nowadays, they're just
     * boolean expressions..
     */
    condition =
      expressionparser.parser() [
        bind( &ConditionParser::seenexpression, this ) ];
  }

  void ConditionParser::reset()
  {
    ret = 0;
  };

  ConditionParser::~ConditionParser()
  {
    // if ret is non-zero, then we delete it..  The parser above us
    // should have called reset to prevent this and the fact that it
    // didn't probably indicates that a parse_exception was thrown
    // somewhere..
    delete ret;
  };

  void ConditionParser::seenexpression()
  {
    // get the datasource parsed by the ExpressionParser..
    DataSourceBase::shared_ptr mcurdata =
      expressionparser.getResult();
    expressionparser.dropResult();

    DataSource<bool>* booldata =
      dynamic_cast<DataSource<bool>*>( mcurdata.get() );
    if ( booldata )
    {
      // wrap the datasource in a ConditionBoolDataSource..
      ret = new ConditionBoolDataSource( booldata );
      mcurdata = 0;
    }
    else
    {
      // we only want boolean expressions..
      throw parse_exception(
        "Attempt to use a non-boolean value as a condition." );
    };
  };
}
