#ifndef TEMPLATE_METHOD_FACTORY_HPP
#define TEMPLATE_METHOD_FACTORY_HPP

#include "MethodFactoryInterface.hpp"

#define MEMBER method
#define MEMBER_CONST
#define FACTORY_INTERFACE MethodFactoryInterface
#define TEMPLATE_FACTORY_NAME TemplateMethodFactory
#define NEW_MEMBER_FACTORY newMethodFactory
#include "TemplateMemberFactory.hpp"
#undef NEW_MEMBER_FACTORY
#undef TEMPLATE_FACTORY_NAME
#undef FACTORY_INTERFACE
#undef MEMBER_CONST
#undef MEMBER

#endif
