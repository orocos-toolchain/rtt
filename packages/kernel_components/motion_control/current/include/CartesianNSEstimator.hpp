/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  CartesianNSEstimator.hpp 

                        CartesianNSEstimator.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef CARTESIAN_NS_ESTIMATOR_HPP
#define CARTESIAN_NS_ESTIMATOR_HPP

#include <geometry/frames.h>

#include <kindyn/KinematicsComponent.hpp>
#include <kindyn/KinematicsFactory.hpp>

#include <corelib/EventInterfaces.hpp>

#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif


/**
 * @file CartesianNSEstimator.hpp
 * Contains an Estimator for calculating the
 * cartesian position from joint positions.
 */
    
namespace ORO_ControlKernel
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;
    using namespace ORO_KinDyn;
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
    using namespace ORO_Execution;
#endif
    
    /**
     * This Estimator calculates an end frame from
     * joint positions.
     */
    template <class Base>
    class CartesianEstimator
        : public Base
    {
    public:
        typedef CartesianNSSensorInput InputDataObject;
        typedef CartesianNSModel ModelDataObject;

        /**
         * Necessary typedefs.
         */
        typedef typename Base::InputType InputType;
        typedef typename Base::ModelType ModelType;
            
        CartesianEstimator() 
            : Base("CartesianEstimator"),
              kineName("Kinematics","The name of the KinematicsStub to use","Kuka361"), kine(0),kineComp(0)
        {
            kine = KinematicsFactory::create( kineName );
            if (kine)
                kineComp = new KinematicsComponent( kine );
        }

        virtual ~CartesianEstimator()
        {
            delete kine;
            delete kineComp;
        }

        virtual bool componentStartup()
        {
            // Get the JointPositions DataObject and the 
            if ( !Base::Input::dObj()->Get( "JointPositions", jpos_DObj ) ||
                 !Base::Model::dObj()->Get( "EndEffPosition", endframe_DObj ) )
                return false;
            pull();
            if ( !kineComp->positionForward( q6, mp_base_frame) )
                return false;
            push();
            return true;
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void pull()      
        {
            jpos_DObj->Get( q6 );
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void calculate() 
        {
            if ( !kineComp->positionForward( q6, mp_base_frame) )
                {
                    EventOperationInterface* es = EventOperationInterface::nameserver.getObjectByName("SingularityDetected");
                    if ( es != 0 )
                        es->fire();
                }
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()      
        {
            endframe_DObj->Set( mp_base_frame );
        }
            
        /**
         * @see KernelReportingExtension.hpp class ReportingComponent
         */
        virtual void exportReports(PropertyBag& bag)
        {
        }

        /**
         * @see KernelPropertyExtension.hpp class PropertyComponentInterface
         */
        virtual bool updateProperties( const PropertyBag& bag)
        {
            if ( composeProperty(bag, kineName ) )
                {
                    return setKinematics( kineName );
                }
            return false;
        }

        bool setKinematics( const std::string& name )
        {
            KinematicsInterface* _k;
            if ( (_k = KinematicsFactory::create(name)) != 0 )
                {
                    delete kine;
                    delete kineComp;
                    kineName =  name;
                    kine = _k;
                    kineComp = new KinematicsComponent( kine );
                    return true;
                }
            return false;
        }
            
    protected:
        Property<string> kineName;
        KinematicsInterface* kine;
        KinematicsComponent* kineComp;

        Double6D q6;
        DataObjectInterface<Double6D>* jpos_DObj;

        Frame mp_base_frame;
        DataObjectInterface<Frame>* endframe_DObj;
    };

}
#endif

