/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ModuleControlInterface.hpp 

                        ModuleControlInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef DATAFLOWINTERFACE_HPP
#define DATAFLOWINTERFACE_HPP


namespace ORO_ControlKernel
{
    using namespace ORO_CoreLib;

    /**
     * @brief This interface defines the methods of the Data Flow
     * for each Component.
     *
     * A DataFlowInterface allows you to control roughly the
     * Data Flow of a Component. These methods can be seen as the
     * external 'triggers' to set a certain behaviour in action.
     * push() and pull() are meant to trigger external communication
     * with other components, while calculate() should perform its
     * operation without external interaction. update() will be called
     * to perform optimized local functioning of the component, under
     * the assumption that communication has no cost.
     *
     */
    struct DataFlowInterface
    {
        virtual ~DataFlowInterface() {}

        /**
         * @brief update() calls pull(), calculate(), push()
         * sequentially and can be overriden by the user to change this behaviour.
         *
         * It will pull
         * new data, do the required calculations and push them
         * to the output data object. 
         */
        virtual void update()
        {
            pull();
            calculate();
            push();
        }

        /**
         * @brief This will refresh the internals of the Component so that the
         * calculation will work with refreshed values.
         */
        virtual void pull() {}

        /**
         * @brief This will push the results of the last calculation to the
         * output data object.
         */
        virtual void push() {}

        /**
         * @brief This will do the calculations on the latest pulled data.
         */
        virtual void calculate() {}
    };
}
#endif
    

