#ifndef GLOBALMETHODFACTORY_HPP
#define GLOBALMETHODFACTORY_HPP

#include "MethodFactoryInterface.hpp"

#define GLOBALMEMBERFACTORY GlobalMethodFactory
#define MEMBERFACTORYINTERFACE MethodFactoryInterface
#include "GlobalMemberFactory.hpp"
#undef GLOBALMEMBERFACTORY
#undef MEMBERFACTORYINTERFACE

#endif
