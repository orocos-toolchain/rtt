/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  KernelConfig.hpp 

                        KernelConfig.hpp -  description
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
 
 
#ifndef KERNEL_CONFIG_HPP
#define KERNEL_CONFIG_HPP

#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include "KernelInterfaces.hpp"

namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;
    using detail::ExtensionInterface;

    /**
     * @brief A class for managing the initial kernel configuration.
     * It parses an xml kernel config file with the data for this kernel
     */
    class KernelConfig
    {
    public:
            /**
             * Create a kernel configurator which reads XML data
             * from a file.
             *
             * @param _k The kernel to configure.
             * @param _filename The filename of the XML data in cpf format.
             */
        KernelConfig( KernelBaseFunction& _k, const std::string& _filename);

        ~KernelConfig() ;

        /**
         * Configure the given kernel. 
         *
         * @return false If configuration failed.
         */
        bool configure();
    protected:
        std::string filename;

        /**
         * A bag containing all extensions properties.
         */
        PropertyBag            config;
        Property<PropertyBag>* baseBag;
        Property<PropertyBag>* extensionBag;
        Property<PropertyBag>* selectBag;
        KernelBaseFunction* kernel;
    };

}



#endif
