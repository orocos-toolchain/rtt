#ifndef ASSIGNVARIABLECOMMAND_HPP
#define ASSIGNVARIABLECOMMAND_HPP

#include <corelib/CommandInterface.hpp>
#include <execution/DataSource.hpp>

namespace ORO_Execution
{
  using ORO_CoreLib::CommandInterface;
  using ORO_Execution::DataSource;
  using ORO_Execution::VariableDataSource;

  /**
   * This is a command that will assign the value of an expression to
   * a variable at runtime.  You pass it the VariableDataSource that
   * you want to assign to, and the DataSource that you want to assign
   * on construction, and it will take care of the assignment.  Note
   * that both DataSource's need to be of the same type, and this
   * class needs that type as its template parameter..
   */
  template<typename T>
  class AssignVariableCommand
    : public CommandInterface
  {
      typedef typename VariableDataSource<T>::shared_ptr LHSSource;
      LHSSource lhs;
      typedef typename DataSource<T>::shared_ptr RHSSource;
      RHSSource rhs;
  public:
      AssignVariableCommand( VariableDataSource<T>* l, DataSource<T>* r )
          : lhs( l ), rhs( r )
      {
      }
      
      void execute()
      {
        lhs->set( rhs->get() );
      }

      std::string toString()
      {
        return "AssignVariableCommand";
      }

      virtual CommandInterface* clone() const
      {
          return new AssignVariableCommand( lhs->duplicate(), rhs->clone() );
      }
  };
}

#endif
