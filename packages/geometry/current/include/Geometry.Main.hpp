/**  \mainpage ORO_Geometry Library
 *
 *  \section intro Introduction
 *
 *  This documentation page is located in utility.cpp.
 *
 *  The ORO_Geometry library provides for classes to work with kinematics, like
 *  Frame, Vector, Rotation in 2D and 3D.  On top of this classes are provided
 *  to maintain a kinematic chain and provide operations on it (Chain, etc...)
 *  conversions to a whole range of other representations are provided.
 *  For all these classes variants
 *  are provided in the form :
 *  - R... : first order autodifferentiation objects. used to calculate the derivative
 *           towards 1 variable
 *  - RR... : second order autodifferentiation objects. used to calculate the 2nd derivative
 *           towards 1 variable
 *  - RN...  autodifferentiation objects. used to calculate the derivative
 *           towards more variables
 *
 *  For all math. objects in ORO_Geometry the SetToZero, SetToIdentity and Equal functions are provided
 *  This is also done for built-in doubles, floats, etc.  This allows uniform treatment of these
 *  types in template classes.
 *
 *  Classes are provided to do the same thing with scalar numbers.
 *
 *  For the task of redundancy resolution :
 *      - Trajectory specification (VelocityProfile, ... and Trajectory,.. classes)
 *          (The current solution does not scale well to a large number of segments)
 *      - ResolutionTask describes a task for redundancy resolution and provides I/O on it.
 *      - FunctionAccResolve / FunctionVelResolve do the actual work.
 *
 *  Other classes in the library :
 *      - Random provides a good random generator (in contrast to the standard C++ generator)
 *      - Some smart pointer classes (CloneableObjPtr,CopiedObjPtr,
 *              CountedObjPtr, CountedBuiltInPtr, DCPtr)
 *      - An adaptive Runga-Kutta integrator Adaptive_Integrator
 *      - Counting : equivalent to double, but it counts operations.
 *
 *  \section dirstruct Directory structure
 *      - algebra : contains general autodifferentiation operations and classes 
 *          and autodifferentiation on numbers
 *      - frames : basic frame-operations (Frame,Vector,Rotation,...)
 *      - rframes : the R..., RR... and RN... variant of the classes in frames
 *      - integrator : the adaptive integrator
 *      - random : the random number generator
 *      - kinematics : trajectory specification, kinematic chains, and scenes
 *      - pointers : smart pointer classes
 *      - utilities : some general header files that are included by most of the other
 *              library components. Utility.h configures the library.
 *      - lib : project file/make file and lib-files of the library.
 *
 *  \section reading Reading the code
 *      - the source code is written with tabsize=4
 *
 * \section tools Additional Tools
 *      - Documentation generation with Doxygen
 *      - There is a program to view kinematic chains and the results of the
 *        redundancy resolution : SceneViewer
 *      
 * \section rem  Remarks
 *      - Multithreading is not explicitly provided; you should protect yourself the access
 *        to concurrently used objects.  In some places static memory is used (should be
 *        thread-local storage when multithreading).  Mostly, this is indicated in 
 *        the TODO-lists.
 *
 * \section depend Dependencies
 *      - sometimes the standard C++ STL library is used.
 *      - sometimes the publically available matrix library newmat10 is used.
 *      - The following dependencies exist between the subdirectories :
 *         - utilities : none
 *         - algebra   :   utilities
 *         - frames    :   utilities
 *         - rframes   :   algebra, frames, utilities
 *         - integrator:   utilities
 *         - chain     :   algebra, frames, rframes, utilities, newmat
 *         - motion    :   frames, utilities
 *         - redundancy:   algebra, frames, rframes, utilities, integrator, newmat, chain
 *      - The examples directory contains examples and tests
 */
 
