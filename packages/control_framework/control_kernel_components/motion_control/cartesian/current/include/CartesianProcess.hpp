#ifndef ORO_CARTESIAN_PROCESS_HPP
#define ORO_CARTESIAN_PROCESS_HPP

#include "BaseComponents.hpp"
#include "DataServer.hpp"
#include "ExtensionComposition.hpp"
#include "PropertyExtension.hpp"
#include "ExecutionExtension.hpp"
#include "KernelInterfaces.hpp"
#include <corelib/Property.hpp>
#include <vector>
#include <string>

namespace ORO_ControlKernel
{

    /**
     * A Process Component which describes the correct execution order of
     * the Cartesian Components and Kinematics Component in a Control Kernel.
     * It requires the presence of a Kinematics Process component, a cartesian
     * generator and a cartesian controller and executes them in that order.
     * If a sensor or effector are found, they are executed before and after
     * that sequence respectively.
     * @ingroup kcomps kcomp_process
     */
    class CartesianProcess
        : public Process< Expects<NoCommand>, Expects<NoInput>, Expects<NoModel>, Expects<NoSetPoint>,Expects<NoOutput>,
                          MakeFacet<PropertyExtension,KernelBaseFunction,ExecutionExtension>::Result >
    {
        typedef Process< Expects<NoCommand>, Expects<NoInput>, Expects<NoModel>, Expects<NoSetPoint>,Expects<NoOutput>,
                         MakeFacet<PropertyExtension,KernelBaseFunction,ExecutionExtension>::Result >   This;
        typedef std::vector<ComponentBaseInterface*> CList;
        CList components;
        ORO_CoreLib::Property<std::string> sensor;
        ORO_CoreLib::Property<std::string> kinematics;
        ORO_CoreLib::Property<std::string> generator;
        ORO_CoreLib::Property<std::string> controller;
        ORO_CoreLib::Property<std::string> effector;
    public:
        /**
         * Create a Process which orders component execution.
         * Name it "DefaultProcess" to have it run automatically
         * in a ControlKernel.
         * @param name The name of this process.
         */
        CartesianProcess(const std::string& name);

        bool updateProperties( const PropertyBag& bag );

        void exportProperties( PropertyBag& bag );

        bool componentLoaded();

        bool componentStartup();

        void componentShutdown();

        void update();
    };
}

#endif

