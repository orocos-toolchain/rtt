/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  KinematicProcess.hpp 

                        KinematicProcess.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef ORO_CONTROL_KERNEL_KINEMATIC_PROCESS_HPP
#define ORO_CONTROL_KERNEL_KINEMATIC_PROCESS_HPP


#include <control_kernel/ComponentInterfaces.hpp>
#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <kindyn/KinematicsComponent.hpp>
#include <kindyn/KinematicsFactory.hpp>
#include <corelib/PropertyComposition.hpp>

namespace ORO_ControlKernel
{

    /**
     * A Process Component which offers a kinematics interface to the
     * other control kernel components.
     * Use the properties of this component to configure where the
     * data can be found and under which names, as well as the kinematic
     * algorithm to use.
     *
	 *   In the likely case that your robot type is not supported,
	 *   you can load your own kinematic algorithm implemented
	 *   using the KinematicsInterface, into 
	 *   the framework by invoking:
     * @verbatim
  KinematicProcess kproc("Kinematics");

  ORO_KinDyn::KinematicsInterface* newKine = ... // create your algorithm

// initialise positive joint directions to 1.0
  ORO_KinDyn::JointVelocities joint_signs(newKine->maxNumberOfJoints(), 1.0);

// initialise positive joint offsets to 0.0
  ORO_KinDyn::JointPositions joint_offsets(newKine->maxNumberOfJoints(), 0.0);

// load now algorithm:
  kproc.getKinematics()->setKinematics( newKine, joint_signs, joint_offsets );
       @endverbatim
     * @ingroup kcomps kcomp_process
     */
    class KinematicProcess
        : public Process<Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                         Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                         Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                         Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                         Expects< ServedTypes<ORO_KinDyn::JointPositions> >,
                         MakeFacet<PropertyExtension, KernelBaseFunction, ExecutionExtension>::Result>
    {
        Property<std::string> kinarch;
        Property<std::string> qloc;
        Property<std::string> qname;
        Property<std::string> qdotloc;
        Property<std::string> qdotname;
        ORO_KinDyn::KinematicsInterface* mykin;
        ORO_KinDyn::KinematicsComponent  mykincomp;

        DataObjectInterface<ORO_KinDyn::JointPositions>* qDObj;
        DataObjectInterface<ORO_KinDyn::JointVelocities>* qdotDObj;

        ORO_KinDyn::JointPositions tmppos;
    public:
        /**
         * A Component which reads joint positions and velocities and converts
         * them into an end effector frame and twist using a kinematic algorithm.
         * @param name The name of this component in the control kernel.
         */
        KinematicProcess(const std::string& name);

        /**
         * Returns the name of the current architecture.
         */
        const std::string& getArchitecture() const;

        /**
         * Returns an object which can be used to track and perform
         * kinematic algorithms for other Kernel components.
         * Use also this method if you want to manually load a new
         * kinematics algorithm into the KinematicsComponent.
         */
        ORO_KinDyn::KinematicsComponent* getKinematics();

        /**
         * Set the Kinematic Algorithm to use and optional axis velocity conversions and axis position offsets.
         */
        void setAlgorithm( ORO_KinDyn::KinematicsInterface *kin,
                           const ORO_KinDyn::JointVelocities& _signs = ORO_KinDyn::JointVelocities(), 
                           const ORO_KinDyn::JointPositions&  _offsets = ORO_KinDyn::JointPositions() );

        bool componentStartup();

        void update();

        void exportProperties(ORO_CoreLib::PropertyBag& bag);

        bool updateProperties(const ORO_CoreLib::PropertyBag& bag);

        ORO_Execution::MethodFactoryInterface* createMethodFactory();
    };

}

#endif
