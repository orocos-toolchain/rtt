#ifndef GLOBALDATASOURCEFACTORY_HPP
#define GLOBALDATASOURCEFACTORY_HPP

#include "DataSourceFactoryInterface.hpp"

#define GLOBALMEMBERFACTORY GlobalDataSourceFactory
#define MEMBERFACTORYINTERFACE DataSourceFactoryInterface
#include "GlobalMemberFactory.hpp"
#undef GLOBALMEMBERFACTORY
#undef MEMBERFACTORYINTERFACE

#endif
