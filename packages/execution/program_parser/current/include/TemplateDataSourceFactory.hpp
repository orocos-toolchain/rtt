#ifndef TEMPLATE_DATASOURCE_FACTORY_HPP
#define TEMPLATE_DATASOURCE_FACTORY_HPP

#include "DataSourceFactoryInterface.hpp"

#define MEMBER data
#define MEMBER_CONST const
#define FACTORY_INTERFACE DataSourceFactoryInterface
#define TEMPLATE_FACTORY_NAME TemplateDataSourceFactory
#define NEW_MEMBER_FACTORY newDataSourceFactory
#include "TemplateMemberFactory.hpp"
#undef NEW_MEMBER_FACTORY
#undef TEMPLATE_FACTORY_NAME
#undef FACTORY_INTERFACE
#undef MEMBER_CONST
#undef MEMBER

#endif
