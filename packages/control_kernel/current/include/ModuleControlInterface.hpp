/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ModuleControlInterface.hpp 

                        ModuleControlInterface.hpp -  description
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
 
 
#ifndef MODULECONTROLINTERFACE_HPP
#define MODULECONTROLINTERFACE_HPP


namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    /**
     * @brief This interface defines the methods of the Data Flow
     * for each Component.
     *
     * A ModuleControlInterface allows you to control roughly the
     * Data Flow of a Component. These methods can be seen as the
     * external 'triggers' to set a certain behaviour in action.
     * push() and pull() are meant to trigger external communication
     * with other components, while calculate() should perform its
     * operation without external interaction. update() will be called
     * to perform optimized local functioning of the component, under
     * the assumption that communication has no cost.
     *
     */
    struct ModuleControlInterface
    {
        ModuleControlInterface() {}

        /**
         * This will do a full update of the module. It will pull
         * new data, do the required calculations and push them
         * to the output data object. 
         * You may specialize this function if you have a more
         * efficient behaviour.
         */
        virtual void update()
        {
            pull();
            calculate();
            push();
        }

        /**
         * This will refresh the internals of the module so that the
         * calculation will work with refreshed values.
         */
        virtual void pull() {}

        /**
         * This will push the results of the last calculation to the
         * output data object.
         */
        virtual void push() {}

        /**
         * This will do the calculations on the latest pulled data.
         */
        virtual void calculate() {}
    };
}
#endif
    

