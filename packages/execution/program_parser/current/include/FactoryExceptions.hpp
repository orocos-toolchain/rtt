#ifndef FACTORYEXCEPTIONS_HPP
#define FACTORYEXCEPTIONS_HPP

/**
 * \file This files contains some structs that can be thrown by both
 * DataSourceFactory's and CommandFactory's.
 */
namespace ORO_Execution
{
  struct name_not_found_exception
  {
  };

  struct wrong_number_of_args_exception
  {
    int wanted;
    int received;
    wrong_number_of_args_exception( int w, int r )
      : wanted( w ), received( r )
      {
      };
  };

  struct wrong_types_of_args_exception
  {
    // this contains the number of the argument that had the wrong
    // type.  The first argument has number 1.
    int whicharg;
    wrong_types_of_args_exception( int w )
      : whicharg( w )
      {
      };
  };
}

#endif
