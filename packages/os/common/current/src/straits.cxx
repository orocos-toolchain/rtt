#pragma implementation "std/straits.h"

#include "corelib/rtstl.hpp"
#include <string>

typedef char c;

//template class string_char_traits <c>;

// This will kill us sooner or later
// it should point to an array. see ctype.h

#error This code will crash in userspace programs

const unsigned short int *__ctype_b;
