/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  BaseComponents.hpp 

                        BaseComponents.hpp -  description
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
 
 
#ifndef BASECOMPONENTS_HPP
#define BASECOMPONENTS_HPP

#include "KernelInterfaces.hpp"
#include "DataObjectInterfaces.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * A SupportComponent does not 'run' actually in the control loop,
     * but it gets the same functionality as any other control component.
     */
    template < class _Aspect = DefaultBase >
    class SupportComponent
        :  public _Aspect,
          public NameServerRegistrator< SupportComponent< _Aspect>* >
    {
    public:
        typedef _Aspect Aspect;

        /**
         * A Default Support Component.
         */
        SupportComponent() : Aspect( "SupportComponent" ) {}
        SupportComponent(const std::string& name ) 
            : Aspect( name ), NameServerRegistrator< SupportComponent<_Aspect>* >(nameserver,name,this)
        {}

        /**
         * The SupportComponent nameserver.
         */
        static NameServer< SupportComponent<_Aspect>* > nameserver;
    };
    
    template < class A >
    NameServer<SupportComponent<A>*> SupportComponent<A>::nameserver;

     /**
     * A controller defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _SetPointType, class _InputType, class _ModelType, class _OutputType, class _Aspect = DefaultBase >
    class Controller
        : public _SetPointType::ReadPort,
          public _ModelType::ReadPort,
          public _InputType::ReadPort,
          public _OutputType::WritePort,
          public _Aspect,
          public NameServerRegistrator< Controller<_SetPointType, _InputType, _ModelType, _OutputType, _Aspect>* >
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

        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Controller() : Aspect( "Controller" ) {}
        Controller(const std::string& name ) 
            : Aspect( name ), NameServerRegistrator< Controller<_SetPointType, _InputType, _ModelType, _OutputType, _Aspect>* >(nameserver,name,this)
        {}
            

        /*
          void addController( Controller* c)
          {
          if ( inKernel() )
          {
          c.readFrom(Input::dObj);
          c.readFrom(Setpoint::dObj);
          c.readFrom(Model::dObj);
          c.writeTo (Output::dObj);
          }
          plugins.push_back(c);
          }

          void readFrom( const InputType* _data)
          {
          for (it
            

          void removeController( Controller* c)
          {
          vector<Controller*>::iterator it = find(plugins.begin(), plugins.end(), c);
          if (it != plugins.end() )
          plugins.erase(it);
          }*/
            
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
            
        /**
         * The Controller nameserver.
         */
        static NameServer< Controller<_SetPointType, _InputType, _ModelType, _OutputType, _Aspect>* > nameserver;
    };
    
    template <class S, class I, class M, class O, class A >
    NameServer<Controller<S,I,M,O,A>*> Controller<S,I,M,O,A>::nameserver;

    /**
     * A generator defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _CommandType, class _InputType, class _ModelType, class _SetPointType, class _Aspect = DefaultBase >
    class Generator
        : public _CommandType::ReadPort,
          public _InputType::ReadPort,
          public _ModelType::ReadPort,
          public _SetPointType::WritePort,
          public _Aspect,
          public NameServerRegistrator< Generator<_CommandType, _InputType, _ModelType, _SetPointType, _Aspect>* >
            
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
            
        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Generator( ) : Aspect( "Generator" ) {}
        Generator(const std::string& name ) 
            : Aspect( name ), 
              NameServerRegistrator< Generator<_CommandType, _InputType, _ModelType, _SetPointType, _Aspect>* >(nameserver,name,this) 
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

        /**
         * The Generator nameserver.
         */
        static NameServer< Generator<_CommandType, _InputType, _ModelType, _SetPointType, _Aspect>* > nameserver;
    };

    template <class C, class I, class M, class S, class A >
    //Generator<C,I,M,S,A>::NameServer<Generator<C,I,M,S,A>* > nameserver;
	NameServer<Generator<C,I,M,S,A>*> Generator<C,I,M,S,A>::nameserver;

    /**
     * An estimator defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _InputType, class _ModelType, class _Aspect = DefaultBase >
    class Estimator
        : public _InputType::ReadPort,
          public _ModelType::WritePort,
          public _Aspect,
          public NameServerRegistrator< Estimator<_InputType, _ModelType, _Aspect>* >
        
    {
    public:
        typedef typename _InputType::DataType InputType;
        typedef typename _ModelType::DataType ModelType;
        typedef _Aspect Aspect;

        typedef typename _InputType::ReadPort Input;
        typedef typename _ModelType::WritePort Model;
            
        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Estimator() : Aspect( "Estimator" ) {}
        Estimator(const std::string& name ) 
            : Aspect( name ), 
              NameServerRegistrator< Estimator<_InputType, _ModelType,  _Aspect>* >(nameserver,name,this) 
        {}
            

        using Input::disconnect;
        using Model::disconnect;
            
        /**
         * The Estimator nameserver.
         */
        static NameServer< Estimator<_InputType, _ModelType, _Aspect>* > nameserver;
    };

    template <class I, class M, class A >
    NameServer<Estimator<I,M,A>*> Estimator<I,M,A>::nameserver;


    /**
     * An effector defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _OutputType, class _Aspect = DefaultBase >
    class Effector
        : public _OutputType::ReadPort,
          public _Aspect,
          public NameServerRegistrator< Effector<_OutputType, _Aspect>* >
    {
    public:
        typedef typename _OutputType::DataType OutputType;
        typedef _Aspect Aspect;

        typedef typename _OutputType::ReadPort Output;

        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Effector() : Aspect( "Effector" ) {}
        Effector(const std::string& name ) 
            : Aspect( name ), 
              NameServerRegistrator< Effector<_OutputType, _Aspect>* >(nameserver,name,this) 
        {}
            
        /**
         * The Effector nameserver.
         */
        static NameServer< Effector<_OutputType, _Aspect>* > nameserver;
            
    };
    template <class O, class A >
    //Effector<O,A>::NameServer<Effector<O,A>* > nameserver;
    NameServer<Effector<O,A>*> Effector<O,A>::nameserver;

    /**
     * A sensor defined by the topological connections to Data Objects and
     * its ports to it.
     */
    template <class _InputType, class _Aspect = DefaultBase >
    class Sensor
        : public _InputType::WritePort,
          public _Aspect,
          public NameServerRegistrator< Sensor<_InputType, _Aspect>* >
        
    {
    public:
        typedef typename _InputType::DataType  InputType;
        typedef _Aspect Aspect;
            
        typedef typename _InputType::WritePort Input;
            
        /**
         * Pas the Kernel id key on to the Aspect Base Class.
         */
        Sensor() : Aspect( "Sensor" ) {}
        Sensor(const std::string& name ) 
            : Aspect( name ), 
              NameServerRegistrator< Sensor<_InputType, _Aspect>* >(nameserver,name,this) 
        {}
            
        /**
         * The Sensor nameserver.
         */
        static NameServer< Sensor<_InputType, _Aspect>* > nameserver;
    };

    template <class I, class A >
	//Sensor<I,A>::NameServer<Sensor<I,A>* > nameserver;
    NameServer<Sensor<I,A>*> Sensor<I,A>::nameserver;
                
}

#endif
