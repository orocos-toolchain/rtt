/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ComponentTemplates.hpp 

                        ComponentTemplates.hpp -  description
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
 
 

    /**
     * @file ComponentTemplates.hpp
     * @brief Handy copy/paste component templates.
     *
     * This file contains templates of the most common 
     * components of the framework. A more concrete example
     * can be found in the tests/XYTable.hpp file.
     * The Extensions demonstrated are : PropertyExtension
     * and ReportingExtension.
     *
     * You can copy/paste these templates and rename the classes
     * to suit your needs.
     */
    
namespace ORO_ControlKernel
{
    /**
     * A Template Generator
     */
    template <class Base>
    class TemplateGenerator 
         : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::SetPointType SetPointType;
            typedef typename Base::InputType InputType;
            typedef typename Base::CommandType CommandType;
            
            /**
             * Constructor.
             */
            TemplateGenerator() 
                : prop_var("PropertyVariable","One of many variables which can be configured.",0),
                 rep_var("ReportVariable","One of many variables which can be reported.",3.14)
            {}

            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                Input::dObj()->Get(input);
                Command::dObj()->Get(command);
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void calculate() 
            {
                /*
                 * Internal computations.
                 */
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                SetPoints::dObj()->Set(result);
            }
            
            /**
             * @see KernelReportingExtension.hpp class ReportingComponent
             */
            virtual void exportReports(PropertyBag& bag)
            {
                bag.add(&rep_var);
            }

            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual bool updateProperties( const PropertyBag& bag)
            {
                return composeProperty(bag, prop_var );
            }
            
            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual void exportProperties( PropertyBag& bag )
            {
                bag.add(&prop_var);
            }
            
        protected:
            Property<double> prop_var;
            Property<double> rep_var;
            InputType    input;
            CommandType  command;
            SetPointType result;
    };
    
    /**
     * A Template Estimator
     */
    template <class Base>
    class TemplateEstimator
        : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::InputType InputType;
            typedef typename Base::ModelType ModelType;
            
            TemplateEstimator() 
                : prop_var("PropertyVariable","One of many variables which can be configured.",0),
                 rep_var("ReportVariable","One of many variables which can be reported.",3.14)
            {}
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                Input::dObj()->Get(input);
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void calculate() 
            {
                /*
                 * Internal computations.
                 */
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                Model::dObj()->Set(model);
            }
            
            /**
             * @see KernelReportingExtension.hpp class ReportingComponent
             */
            virtual void exportReports(PropertyBag& bag)
            {
                bag.add(&rep_var);
            }

            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual bool updateProperties( const PropertyBag& bag)
            {
                return composeProperty(bag, prop_var );
            }
            
            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual void exportProperties( PropertyBag& bag )
            {
                bag.add(&prop_var);
            }
            
        protected:
            Property<double> prop_var;
            Property<double> rep_var;
            ModelType model;
            InputType input;
    };

    /**
     * A Template Controller
     */
    template <class Base>
    class TemplateController
        : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::SetPointType SetPointType;
            typedef typename Base::InputType InputType;
            typedef typename Base::ModelType ModelType;
            typedef typename Base::OutputType OutputType;
            
            TemplateController() 
                : prop_var("PropertyVariable","One of many variables which can be configured.",0),
                 rep_var("ReportVariable","One of many variables which can be reported.",3.14)
            {}
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                Input::dObj()->Get(input);
                Model::dObj()->Get(model);
                SetPoint::dObj()->Get(setpoint);
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void calculate() 
            {
                /*
                 * Internal computations.
                 */
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                Output::dObj()->Set(output);
            }
            
            /**
             * @see KernelReportingExtension.hpp class ReportingComponent
             */
            virtual void exportReports(PropertyBag& bag)
            {
                bag.add(&rep_var);
            }

            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual bool updateProperties( const PropertyBag& bag)
            {
                return composeProperty(bag, prop_var );
            }
            
            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual void exportProperties( PropertyBag& bag )
            {
                bag.add(&prop_var);
            }
            
        protected:
            Property<double> prop_var;
            Property<double> rep_var;
            ModelType model;
            InputType input;
            SetPointType setpoint;
            OutputType output;
    };

    /**
     * A Template Sensor
     */
    template <class Base>
    class TemplateSensor
        : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::InputType InputType;
            
            TemplateSensor() 
                : prop_var("PropertyVariable","One of many variables which can be configured.",0),
                 rep_var("ReportVariable","One of many variables which can be reported.",3.14)
            {}
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                /*
                 * Gather physical sensor data.
                 */
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void calculate() 
            {
                /*
                 * Internal computations.
                 */
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                Input::dObj()->Set(input);
            }
            
            /**
             * @see KernelReportingExtension.hpp class ReportingComponent
             */
            virtual void exportReports(PropertyBag& bag)
            {
                bag.add(&rep_var);
            }

            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual bool updateProperties( const PropertyBag& bag)
            {
                return composeProperty(bag, prop_var );
            }
            
            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual void exportProperties( PropertyBag& bag )
            {
                bag.add(&prop_var);
            }
            
        protected:
            Property<double> prop_var;
            Property<double> rep_var;
            InputType input;
    };

    /**
     * A Template Effector
     */
    template <class Base>
    class TemplateEffector
        : public Base
    {
        public:
            /**
             * Necessary typedefs.
             */
            typedef typename Base::OutputType OutputType;
            
            TemplateEffector() 
                : prop_var("PropertyVariable","One of many variables which can be configured.",0),
                 rep_var("ReportVariable","One of many variables which can be reported.",3.14)
            {}
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void pull()      
            {
                Output::dObj()->Get(output);
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void calculate() 
            {
                /*
                 * Internal computations.
                 */
            }
            
            /**
             * @see KernelInterfaces.hpp class ModuleControlInterface
             */
            virtual void push()      
            {
                /*
                 * Acces device drivers
                 */
            }
            
            /**
             * @see KernelReportingExtension.hpp class ReportingComponent
             */
            virtual void exportReports(PropertyBag& bag)
            {
                bag.add(&rep_var);
            }

            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual bool updateProperties( const PropertyBag& bag)
            {
                return composeProperty(bag, prop_var );
            }
            
            /**
             * @see KernelPropertyExtension.hpp class PropertyComponentInterface
             */
            virtual void exportProperties( PropertyBag& bag )
            {
                bag.add(&prop_var);
            }
            
        protected:
            Property<double> prop_var;
            Property<double> rep_var;
            OutputType output;
    };
}
