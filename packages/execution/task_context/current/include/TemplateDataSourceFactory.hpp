/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  TemplateDataSourceFactory.hpp 

                        TemplateDataSourceFactory.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
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
