#ifndef TEMPLATE_DATASOURCE_FACTORY_HPP
#define TEMPLATE_DATASOURCE_FACTORY_HPP

#include "DataSourceFactoryInterface.hpp"

#define MEMBER data
#define MEMBER_DS data_ds
#define MEMBER_CONST 
#define TEMPLATE_FACTORY_NAME TemplateDataSourceFactory
#define NEW_MEMBER_FACTORY newDataSourceFactory
#include "TemplateMemberFactory.hpp"
#undef NEW_MEMBER_FACTORY
#undef TEMPLATE_FACTORY_NAME
#undef MEMBER_CONST
#undef MEMBER_DS
#undef MEMBER

#endif
