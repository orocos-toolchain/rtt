
#include <control_kernel/DataServer.hpp>
#include <iostream>

using namespace std;
using namespace ORO_ControlKernel;

/*******************************************************************************/
/**
 * Compile using : g++ -I install/include -L install/lib -ltarget -lpthread 
 */

    /**
     * One unit of data
     */
    struct PosXY
    {
        // default constructor is allowed 
        // (and encouraged).
        PosXY() : x(0), y(0) {}
        int x, y;
    };

    struct VelXY
    {
        VelXY() : xdot(0), ydot(0) {}
        int xdot, ydot;
    };

    struct MouseVelocities
    {
        MouseVelocities() 
            : xdot(0), ydot(0), zdot(0), 
              adot(0), bdot(0), gdot(0) {}
        int xdot, ydot, zdot;
        int adot, bdot, gdot;
    };


    /**
     * The not-nameserved input data.
     */
    struct NormalInputs
    {
        VelXY velocity;
    };

    using std::make_pair;

    /**
     * Example : A Container indicating the ServedTypes and the UnServedType of a DataObject.
     * The constructor must contain the names for the ServedTypes.
     */
    struct MyInputTypes 
        : public ServedTypes<PosXY, MouseVelocities>, public UnServedType<NormalInputs>
    {
        /**
         * Obliged giving of names to all ServedTypes
         */
        InputTypes() 
        {
            // two objects of PosXY :
            this->insert( make_pair(0,"Position1"));
            this->insert( make_pair(0,"Position2"));
            // one object of MouseVelocities :
            this->insert( make_pair(1,"MouseVels"));
            // not used, but allowed :
            this->insert( make_pair(2,"Forces"));
            this->insert( make_pair(3,"Torques"));
        }
    };


template <class InputType>
class InputKernel
    :public BaseKernel< ... >
{
    typedef NameServedDataObject< DataObjectContainer<InputType> Inputs;
};

template <class ImplementationKernel>
class PublicKernel
{
    typedef ImplementationKernel::Inputs Inputs;
};    

template <class I, class M, class C, class S, class O, class Ext=DefaultExt>
class DefaultControlKernel
    : public BaseKernel<DataObject<I>, DataObject<M>, DataObjectLocked<C>, DataObject<S>, DataObject<0>, Ext>
{
    using BaseKernel::Inputs;
    using BaseKernel::Models;
    //...
    using BaseKernel::DefaultGenerator;
    //...
};

template <class I, class M, class C, class S, class O, class Ext=DefaultExt>
class HighPriorityControlKernel
    : public BaseKernel<DataObjectPriorityGet<I>, DataObject<M>, DataObjectPriorityGet<C>, DataObject<S>, DataObjectPrioritySet<0>, Ext>
{
    using BaseKernel::Inputs;
    using BaseKernel::Models;
    //...
    using BaseKernel::DefaultGenerator;
    //...
};



int main()
{
    NamesKernel<MyInputTypes, MyExtension> NKernel;

    // Example use in the kernel :
    typedef NameServedDataObject< DataObjectPrioritySetContainer<MyInputTypes> > Inputs;
   
    // old use :
    typedef DataObject< NormalInputs > OldInputs;

    Inputs dObj;

    // reading the UnServedType data.
    Inputs::DataType local_in = dObj.Get();
    MouseVelocities mv;
    PosXY ps;

    Inputs::?<MouseVelocities>::type mvDO;

    dObj.Get("MouseVels", mvDO);
    mv = dObj->Get();

    // testing nameserver :
    cout << "MouseVels : "<<dObj.Get("MouseVels", mv)<<endl;
    cout << "Position1 : "<<dObj.Get("Position1", ps)<<endl;
    cout << "Position2 : "<<dObj.Get("Position2", ps)<<endl;
    cout << "Forces    : "<<dObj.Get("Forces", ps)<<endl;
    cout << "Position3 : "<<dObj.Get("Position3", ps)<<endl;

    // testing data passing :


    mv.xdot = 1; mv.ydot=2;
    cout << "Set Mouse: "<<dObj.Set("MouseVels", mv)<<endl;
    MouseVelocities mv2;
    dObj.Get("MouseVels", mv2);
    cout << "Success : "<< (mv.xdot == mv2.xdot && mv.ydot == mv2.ydot) <<endl;


    
    return 0;
}
