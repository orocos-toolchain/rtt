// ============================================================================
//
// = MODULE
//    MotionControl
//
// = FILENAME
//    DataTypes.hpp
//
// = DESCRIPTION
//    Template classes that represent a standard naming convention for 
//    datattypes connecting Components...
//
// = AUTHOR
//    Rene Waarsing <Rene.Waarsing@mech.kuleuven.ac.be>
//
// ============================================================================
//
// $Id: DataTypes.hpp,v 1.1 2003/08/20 12:05:25 rwaarsin Exp $
// $Log: DataTypes.hpp,v $
// Revision 1.1  2003/08/20 12:05:25  rwaarsin
// First version of standardised datatypes fro communication between components
//
//
// ============================================================================

#ifndef DATATYPES_H
#define DATATYPES_H

//-----------------------------------------------------------------------------
// Includes
#include "MultiVector.hpp"

#if 0
#include <motioncontrol/MotionKernel.hpp>
#include <motioncontrol/MotionKernelExtensions.hpp>
#include <motioncontrol/MotionReportingExtension.hpp>
#include <motioncontrol/MotionPropertyExtension.hpp>
#endif

namespace ORO_ControlKernel {

    using ORO_CoreLib::MultiVector;
//-----------------------------------------------------------------------------
// MotionControl::Kernel definitions
//! SetPoints :
template <class VariableType, unsigned int VariableSize>
class SetPointDataT
{
  public:
    SetPointDataT() {};

    MotionControl::MultiVector<VariableSize, VariableType>& ref()
     { return _setpoint; };
    static const unsigned int size = VariableSize;
  private:
    MotionControl::MultiVector<VariableSize, VariableType> _setpoint;
};

//! EffectorCommands:
template <class VariableType, unsigned int VariableSize>
class EffectorOutputDataT
{
  public:
    EffectorOutputDataT () {};

    MotionControl::MultiVector<VariableSize, VariableType>& actu()
      {return _effectorCmd;};

    static const unsigned int size = VariableSize;
  private:
    MotionControl::MultiVector<VariableSize, VariableType> _effectorCmd;
};

//! Sensor measures positions :
template <class VariableType, unsigned int VariableSize>
class SensorInputDataT
{
  public:
    SensorInputDataT() {};

    MotionControl::MultiVector<VariableSize, VariableType>& meas()
    { return _sensordata;};
    static const unsigned int size = VariableSize;
  private:
    MotionControl::MultiVector<VariableSize, VariableType> _sensordata;
};

};
#endif
