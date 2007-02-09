
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace function_types {
namespace constants
{
  enum generated_tag_constants
  {
    dummy_enumerator_to_avoid_PP_COMMA_IF_logic
  , plain_function
      = xxx_function | static_function | plain
  , plain_non_variadic_function
      = xxx_function | static_function | plain | non_variadic
  , plain_variadic_function
      = xxx_function | static_function | plain | variadic
  , plain_defaultcall_function
      = xxx_function | static_function | plain | (1 << ( n_bits + 0 ))
  , plain_non_variadic_defaultcall_function
      = xxx_function | static_function | plain | (1 << ( n_bits + 0 )) | non_variadic
  , plain_variadic_defaultcall_function
      = xxx_function | static_function | plain | (1 << ( n_bits + 0 )) | variadic
  , function_reference
      = xxx_function | static_function | reference
  , non_variadic_function_reference
      = xxx_function | static_function | reference | non_variadic
  , variadic_function_reference
      = xxx_function | static_function | reference | variadic
  , defaultcall_function_reference
      = xxx_function | static_function | reference | (1 << ( n_bits + 0 ))
  , non_variadic_defaultcall_function_reference
      = xxx_function | static_function | reference | (1 << ( n_bits + 0 )) | non_variadic
  , variadic_defaultcall_function_reference
      = xxx_function | static_function | reference | (1 << ( n_bits + 0 )) | variadic
  , function_pointer
      = xxx_function | static_function | pointer
  , non_variadic_function_pointer
      = xxx_function | static_function | pointer | non_variadic
  , variadic_function_pointer
      = xxx_function | static_function | pointer | variadic
  , defaultcall_function_pointer
      = xxx_function | static_function | pointer | (1 << ( n_bits + 0 ))
  , non_variadic_defaultcall_function_pointer
      = xxx_function | static_function | pointer | (1 << ( n_bits + 0 )) | non_variadic
  , variadic_defaultcall_function_pointer
      = xxx_function | static_function | pointer | (1 << ( n_bits + 0 )) | variadic
  , member_function_pointer
      = xxx_function | member_function | pointer
  , non_variadic_member_function_pointer
      = xxx_function | member_function | pointer | non_variadic
  , variadic_member_function_pointer
      = xxx_function | member_function | pointer | variadic
  , defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | (1 << ( n_bits + 0 ))
  , non_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | (1 << ( n_bits + 0 )) | non_variadic
  , variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | (1 << ( n_bits + 0 )) | variadic
  , const_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified
  , const_non_variadic_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | non_variadic
  , const_variadic_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | variadic
  , const_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | (1 << ( n_bits + 0 ))
  , const_non_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | (1 << ( n_bits + 0 )) | non_variadic
  , const_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | (1 << ( n_bits + 0 )) | variadic
  , volatile_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified
  , volatile_non_variadic_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | non_variadic
  , volatile_variadic_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | variadic
  , volatile_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | (1 << ( n_bits + 0 ))
  , volatile_non_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | (1 << ( n_bits + 0 )) | non_variadic
  , volatile_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | (1 << ( n_bits + 0 )) | variadic
  , const_volatile_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified
  , const_volatile_non_variadic_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | non_variadic
  , const_volatile_variadic_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | variadic
  , const_volatile_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | (1 << ( n_bits + 0 ))
  , const_volatile_non_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | (1 << ( n_bits + 0 )) | non_variadic
  , const_volatile_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | (1 << ( n_bits + 0 )) | variadic
  };
}
namespace tags
{
  typedef mpl::integral_c<long,constants:: plain_function> plain_function;
  typedef mpl::integral_c<long,constants:: plain_non_variadic_function> plain_non_variadic_function;
  typedef mpl::integral_c<long,constants:: plain_variadic_function> plain_variadic_function;
  typedef mpl::integral_c<long,constants:: plain_defaultcall_function> plain_defaultcall_function;
  typedef mpl::integral_c<long,constants:: plain_non_variadic_defaultcall_function> plain_non_variadic_defaultcall_function;
  typedef mpl::integral_c<long,constants:: plain_variadic_defaultcall_function> plain_variadic_defaultcall_function;
  typedef mpl::integral_c<long,constants:: function_reference> function_reference;
  typedef mpl::integral_c<long,constants:: non_variadic_function_reference> non_variadic_function_reference;
  typedef mpl::integral_c<long,constants:: variadic_function_reference> variadic_function_reference;
  typedef mpl::integral_c<long,constants:: defaultcall_function_reference> defaultcall_function_reference;
  typedef mpl::integral_c<long,constants:: non_variadic_defaultcall_function_reference> non_variadic_defaultcall_function_reference;
  typedef mpl::integral_c<long,constants:: variadic_defaultcall_function_reference> variadic_defaultcall_function_reference;
  typedef mpl::integral_c<long,constants:: function_pointer> function_pointer;
  typedef mpl::integral_c<long,constants:: non_variadic_function_pointer> non_variadic_function_pointer;
  typedef mpl::integral_c<long,constants:: variadic_function_pointer> variadic_function_pointer;
  typedef mpl::integral_c<long,constants:: defaultcall_function_pointer> defaultcall_function_pointer;
  typedef mpl::integral_c<long,constants:: non_variadic_defaultcall_function_pointer> non_variadic_defaultcall_function_pointer;
  typedef mpl::integral_c<long,constants:: variadic_defaultcall_function_pointer> variadic_defaultcall_function_pointer;
  typedef mpl::integral_c<long,constants:: member_function_pointer> member_function_pointer;
  typedef mpl::integral_c<long,constants:: non_variadic_member_function_pointer> non_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: variadic_member_function_pointer> variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: defaultcall_member_function_pointer> defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: non_variadic_defaultcall_member_function_pointer> non_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: variadic_defaultcall_member_function_pointer> variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_member_function_pointer> const_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_non_variadic_member_function_pointer> const_non_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_variadic_member_function_pointer> const_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_defaultcall_member_function_pointer> const_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_non_variadic_defaultcall_member_function_pointer> const_non_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_variadic_defaultcall_member_function_pointer> const_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_member_function_pointer> volatile_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_non_variadic_member_function_pointer> volatile_non_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_variadic_member_function_pointer> volatile_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_defaultcall_member_function_pointer> volatile_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_non_variadic_defaultcall_member_function_pointer> volatile_non_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_variadic_defaultcall_member_function_pointer> volatile_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_member_function_pointer> const_volatile_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_non_variadic_member_function_pointer> const_volatile_non_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_variadic_member_function_pointer> const_volatile_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_defaultcall_member_function_pointer> const_volatile_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_non_variadic_defaultcall_member_function_pointer> const_volatile_non_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_variadic_defaultcall_member_function_pointer> const_volatile_variadic_defaultcall_member_function_pointer;
}
using namespace tags;
}
using namespace function_types::tags;
}

// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the boost Software License,
// Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt).

// Preprocessed code, do not edit manually !

namespace boost { namespace function_types {
namespace constants
{
  enum generated_tag_constants
  {
    dummy_enumerator_to_avoid_PP_COMMA_IF_logic
  , plain_function
      = xxx_function | static_function | plain
  , plain_non_variadic_function
      = xxx_function | static_function | plain | non_variadic
  , plain_variadic_function
      = xxx_function | static_function | plain | variadic
  , plain_defaultcall_function
      = xxx_function | static_function | plain | (1 << ( n_bits + 0 ))
  , plain_non_variadic_defaultcall_function
      = xxx_function | static_function | plain | (1 << ( n_bits + 0 )) | non_variadic
  , plain_variadic_defaultcall_function
      = xxx_function | static_function | plain | (1 << ( n_bits + 0 )) | variadic
  , function_reference
      = xxx_function | static_function | reference
  , non_variadic_function_reference
      = xxx_function | static_function | reference | non_variadic
  , variadic_function_reference
      = xxx_function | static_function | reference | variadic
  , defaultcall_function_reference
      = xxx_function | static_function | reference | (1 << ( n_bits + 0 ))
  , non_variadic_defaultcall_function_reference
      = xxx_function | static_function | reference | (1 << ( n_bits + 0 )) | non_variadic
  , variadic_defaultcall_function_reference
      = xxx_function | static_function | reference | (1 << ( n_bits + 0 )) | variadic
  , function_pointer
      = xxx_function | static_function | pointer
  , non_variadic_function_pointer
      = xxx_function | static_function | pointer | non_variadic
  , variadic_function_pointer
      = xxx_function | static_function | pointer | variadic
  , defaultcall_function_pointer
      = xxx_function | static_function | pointer | (1 << ( n_bits + 0 ))
  , non_variadic_defaultcall_function_pointer
      = xxx_function | static_function | pointer | (1 << ( n_bits + 0 )) | non_variadic
  , variadic_defaultcall_function_pointer
      = xxx_function | static_function | pointer | (1 << ( n_bits + 0 )) | variadic
  , member_function_pointer
      = xxx_function | member_function | pointer
  , non_variadic_member_function_pointer
      = xxx_function | member_function | pointer | non_variadic
  , variadic_member_function_pointer
      = xxx_function | member_function | pointer | variadic
  , defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | (1 << ( n_bits + 0 ))
  , non_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | (1 << ( n_bits + 0 )) | non_variadic
  , variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | (1 << ( n_bits + 0 )) | variadic
  , const_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified
  , const_non_variadic_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | non_variadic
  , const_variadic_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | variadic
  , const_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | (1 << ( n_bits + 0 ))
  , const_non_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | (1 << ( n_bits + 0 )) | non_variadic
  , const_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_qualified | (1 << ( n_bits + 0 )) | variadic
  , volatile_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified
  , volatile_non_variadic_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | non_variadic
  , volatile_variadic_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | variadic
  , volatile_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | (1 << ( n_bits + 0 ))
  , volatile_non_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | (1 << ( n_bits + 0 )) | non_variadic
  , volatile_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | volatile_qualified | (1 << ( n_bits + 0 )) | variadic
  , const_volatile_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified
  , const_volatile_non_variadic_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | non_variadic
  , const_volatile_variadic_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | variadic
  , const_volatile_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | (1 << ( n_bits + 0 ))
  , const_volatile_non_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | (1 << ( n_bits + 0 )) | non_variadic
  , const_volatile_variadic_defaultcall_member_function_pointer
      = xxx_function | member_function | pointer | const_volatile_qualified | (1 << ( n_bits + 0 )) | variadic
  };
}
namespace tags
{
  typedef mpl::integral_c<long,constants:: plain_function> plain_function;
  typedef mpl::integral_c<long,constants:: plain_non_variadic_function> plain_non_variadic_function;
  typedef mpl::integral_c<long,constants:: plain_variadic_function> plain_variadic_function;
  typedef mpl::integral_c<long,constants:: plain_defaultcall_function> plain_defaultcall_function;
  typedef mpl::integral_c<long,constants:: plain_non_variadic_defaultcall_function> plain_non_variadic_defaultcall_function;
  typedef mpl::integral_c<long,constants:: plain_variadic_defaultcall_function> plain_variadic_defaultcall_function;
  typedef mpl::integral_c<long,constants:: function_reference> function_reference;
  typedef mpl::integral_c<long,constants:: non_variadic_function_reference> non_variadic_function_reference;
  typedef mpl::integral_c<long,constants:: variadic_function_reference> variadic_function_reference;
  typedef mpl::integral_c<long,constants:: defaultcall_function_reference> defaultcall_function_reference;
  typedef mpl::integral_c<long,constants:: non_variadic_defaultcall_function_reference> non_variadic_defaultcall_function_reference;
  typedef mpl::integral_c<long,constants:: variadic_defaultcall_function_reference> variadic_defaultcall_function_reference;
  typedef mpl::integral_c<long,constants:: function_pointer> function_pointer;
  typedef mpl::integral_c<long,constants:: non_variadic_function_pointer> non_variadic_function_pointer;
  typedef mpl::integral_c<long,constants:: variadic_function_pointer> variadic_function_pointer;
  typedef mpl::integral_c<long,constants:: defaultcall_function_pointer> defaultcall_function_pointer;
  typedef mpl::integral_c<long,constants:: non_variadic_defaultcall_function_pointer> non_variadic_defaultcall_function_pointer;
  typedef mpl::integral_c<long,constants:: variadic_defaultcall_function_pointer> variadic_defaultcall_function_pointer;
  typedef mpl::integral_c<long,constants:: member_function_pointer> member_function_pointer;
  typedef mpl::integral_c<long,constants:: non_variadic_member_function_pointer> non_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: variadic_member_function_pointer> variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: defaultcall_member_function_pointer> defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: non_variadic_defaultcall_member_function_pointer> non_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: variadic_defaultcall_member_function_pointer> variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_member_function_pointer> const_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_non_variadic_member_function_pointer> const_non_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_variadic_member_function_pointer> const_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_defaultcall_member_function_pointer> const_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_non_variadic_defaultcall_member_function_pointer> const_non_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_variadic_defaultcall_member_function_pointer> const_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_member_function_pointer> volatile_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_non_variadic_member_function_pointer> volatile_non_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_variadic_member_function_pointer> volatile_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_defaultcall_member_function_pointer> volatile_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_non_variadic_defaultcall_member_function_pointer> volatile_non_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: volatile_variadic_defaultcall_member_function_pointer> volatile_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_member_function_pointer> const_volatile_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_non_variadic_member_function_pointer> const_volatile_non_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_variadic_member_function_pointer> const_volatile_variadic_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_defaultcall_member_function_pointer> const_volatile_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_non_variadic_defaultcall_member_function_pointer> const_volatile_non_variadic_defaultcall_member_function_pointer;
  typedef mpl::integral_c<long,constants:: const_volatile_variadic_defaultcall_member_function_pointer> const_volatile_variadic_defaultcall_member_function_pointer;
}
using namespace tags;
}
using namespace function_types::tags;
}
