/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  BaseComponents.hpp 

                        BaseComponents.hpp -  description
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
 
#ifndef BASECOMPONENTS_HPP
#define BASECOMPONENTS_HPP

#include "ComponentInterfaces.hpp"
#include "DataObjectInterfaces.hpp"
#include "PortInterfaces.hpp"

namespace ORO_ControlKernel
{
    /**
     * @defgroup kcomps All Available Control Kernel Components.
     */

    /**
     * @defgroup kcomp_support All Available ControlKernel Support Components
     */

    /**
     * @brief A SupportComponent does not 'run' actually in the control loop,
     * but it gets the same functionality as any other control component.
     * @ingroup kcomps kcomp_support
     */
    template < class _Aspect = DefaultBase >
    class SupportComponent
        :  public _Aspect
    {
    public:
        typedef _Aspect Aspect;

        /**
         * A Support Component.
         */
        SupportComponent(const std::string& name ) 
            : Aspect( name )
        {}

    };

    /**
     * @defgroup kcomp_controller All Available ControlKernel Controller Components
     */

    /**
     * @brief A controller defined by the topological connections to Data Objects and
     * its ports to it.
     * @ingroup kcomps kcomp_controller
     */
    template <class _InputType, class _ModelType, class _SetPointType, class _OutputType, class _Aspect = DefaultBase >
    class Controller
        : public _SetPointType::ReadPort,
          public _ModelType::ReadPort,
          public _InputType::ReadPort,
          public _OutputType::WritePort,
          public _Aspect
    {
    public:
        typedef typename _SetPointType::DataType SetPointType;
        typedef typename _InputType::DataType InputType;
        typedef typename _ModelType::DataType ModelType;
        typedef typename _OutputType::DataType OutputType;
        typedef _Aspect Aspect;

        typedef typename _OutputType::WritePort Output;
        typedef typename _SetPointType::ReadPort SetPoint;
        typedef typename _InputType::ReadPort Input;
        typedef typename _ModelType::ReadPort Model;

        Controller(const std::string& name ) 
            : Aspect( name )
        {}
            
        /**
         * Method Overloading is not done across scopes.
         * These lines introduce the methods anyway in this class.
         */
        using Input::readFrom;
        using Model::readFrom;
        using SetPoint::readFrom;
        using Input::disconnect;
        using Model::disconnect;
        using SetPoint::disconnect;
        using Output::disconnect;
            
    };

    /**
     * @defgroup kcomp_generator All Available ControlKernel Generator Components
     */

    /**
     * @brief A generator defined by the topological connections to Data Objects and
     * its ports to it.
     * @ingroup kcomps kcomp_generator
     */
    template <class _InputType, class _ModelType, class _CommandType, class _SetPointType, class _Aspect = DefaultBase >
    class Generator
        : public _CommandType::ReadPort,
          public _InputType::ReadPort,
          public _ModelType::ReadPort,
          public _SetPointType::WritePort,
          public _Aspect
    {
    public:
        typedef typename _CommandType::DataType CommandType;
        typedef typename _SetPointType::DataType SetPointType;
        typedef typename _InputType::DataType InputType;
        typedef typename _ModelType::DataType ModelType;
        typedef _Aspect Aspect;

        typedef typename _CommandType::ReadPort   Command;
        typedef typename _InputType::ReadPort     Input;
        typedef typename _ModelType::ReadPort     Model ;
        typedef typename _SetPointType::WritePort SetPoint;
            
        Generator(const std::string& name ) 
            : Aspect( name )
        {}
            
        /**
         * Method Overloading is not done across scopes.
         * These lines introduce the methods anyway in this class.
         */
        using Input::readFrom;
        using Model::readFrom;
        using Command::readFrom;
        using Input::disconnect;
        using Model::disconnect;
        using Command::disconnect;
        using SetPoint::disconnect;

    };

    /**
     * @defgroup kcomp_estimator All Available ControlKernel Estimator Components
     */

    /**
     * @brief An estimator defined by the topological connections to Data Objects and
     * its ports to it.
     * @ingroup kcomps kcomp_estimator
     */
    template <class _InputType, class _ModelType, class _Aspect = DefaultBase >
    class Estimator
        : public _InputType::ReadPort,
          public _ModelType::WritePort,
          public _Aspect
    {
    public:
        typedef typename _InputType::DataType InputType;
        typedef typename _ModelType::DataType ModelType;
        typedef _Aspect Aspect;

        typedef typename _InputType::ReadPort Input;
        typedef typename _ModelType::WritePort Model;
            
        Estimator(const std::string& name ) 
            : Aspect( name )
        {}
            

        using Input::disconnect;
        using Model::disconnect;
            
    };


    /**
     * @defgroup kcomp_effector All Available ControlKernel Effector Components
     */

    /**
     * @brief An effector defined by the topological connections to Data Objects and
     * its ports to it.
     * @ingroup kcomps kcomp_effector
     */
    template <class _OutputType, class _Aspect = DefaultBase >
    class Effector
        : public _OutputType::ReadPort,
          public _Aspect
    {
    public:
        typedef typename _OutputType::DataType OutputType;
        typedef _Aspect Aspect;

        typedef typename _OutputType::ReadPort Output;

        Effector(const std::string& name ) 
            : Aspect( name )
        {}
            
    };

    /**
     * @defgroup kcomp_sensor All Available ControlKernel Sensor Components
     */

    /**
     * @brief A sensor defined by the topological connections to Data Objects and
     * its ports to it.
     * @ingroup kcomps kcomp_sensor
     */
    template <class _InputType, class _Aspect = DefaultBase >
    class Sensor
        : public _InputType::WritePort,
          public _Aspect
    {
    public:
        typedef typename _InputType::DataType  InputType;
        typedef _Aspect Aspect;
            
        typedef typename _InputType::WritePort Input;
            
        Sensor(const std::string& name ) 
            : Aspect( name )
        {}
            
    };
                
}

#endif
