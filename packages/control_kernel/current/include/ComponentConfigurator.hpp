/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ComponentConfigurator.hpp 

                        ComponentConfigurator.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef COMPONENTCONFIGURATOR_HPP
#define COMPONENTCONFIGURATOR_HPP
#include <string>

namespace ORO_ControlKernel
{
    class PropertyComponentInterface;
    
    /**
     * @brief A class for reading component property files (cpf)
     * and configuring a component with these properties.
     */
    class ComponentConfigurator
    {
        public:
            ComponentConfigurator()
            {}

            /**
             * Read the XML cpf files and try to configure the given component.
             */
            virtual bool configure(const std::string& filename, PropertyComponentInterface* target) const;
    };
}
#endif
