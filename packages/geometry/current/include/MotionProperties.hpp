/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  MotionProperties.hpp 

                        MotionProperties.hpp -  description
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
 
 
#ifndef MOTIONPROPERTIES_HPP
#define MOTIONPROPERTIES_HPP

#include <corelib/Property.hpp>
#include "frames.h"
#include <corelib/MultiVector.hpp>

/**
 * @file MotionProperties.hpp
 * 
 * This implementation is introduced in the ORO_CoreLib namespace.
 * If not, it would not be found by the partial template 
 * specialisation algorithm of the compiler.
 */

namespace ORO_CoreLib
{
    using std::string;
    using ORO_Geometry::Vector;
    using ORO_Geometry::Frame;
    using ORO_Geometry::Twist;
    using ORO_Geometry::Rotation;
    using ORO_Geometry::Wrench;

    /**
     *
     * MotionControl::Double6D
     *
     */
    
    /**
     * A Decomposer for Decomposing a Double6D or Property<Double6D>
     * into a typed Property<PropertyBag>.
     */
    class Double6DDecomposer
    {
        Property<PropertyBag> resultBag;
        Property<double> D1;
        Property<double> D2;
        Property<double> D3;
        Property<double> D4;
        Property<double> D5;
        Property<double> D6;
        
        public: 
        
        Double6DDecomposer( const Property<Double6D>& v );
        Double6DDecomposer( const Double6D& v, const string& name );
        
        Property<PropertyBag>& result() { return resultBag; }
    };
        
    /**
     * An Composer for constructing a Double6D or Property<Double6D>
     * from a typed PropertyBag.
     */
    class Double6DComposer
    {
        const PropertyBag& bag;
        public:
            Double6DComposer( const PropertyBag& _bag )
                : bag(_bag)
            {}

            bool getResult( Property<Double6D>& res )
            {
                return getResult( res.value(), res.getName() );
            }
                
            bool getResult( Double6D& res, const string& name );
    };
            

    /**
     * Additional functionality for handling a Property<Double6D>
     */
    void decomposeProperty(PropertyIntrospection *p, const Property<Double6D> &v);

    /**
     * Aim of this kind of functions : given a Property<PropertyBag> with one of the
     * properties being a bag containing a Double6D, fill the given non primitive Property<T>.
     */
    bool composeProperty(const PropertyBag& bag, Property<Double6D> &v);

    /**
     *
     *    ORO_Geometry::Vector
     *
     */

    /**
     * A Decomposer for Decomposing a Vector or Property<Vector>
     * into a typed Property<PropertyBag>.
     */
    class VectorDecomposer
    {
        Property<PropertyBag> resultBag;
        Property<double> X;
        Property<double> Y;
        Property<double> Z;
        
        public: 
        
        VectorDecomposer( const Property<Vector>& v );
        VectorDecomposer( const Vector& v, const string& name );
        
        Property<PropertyBag>& result() { return resultBag; }
    };
        
    /**
     * An Composer for constructing a Vector or Property<Vector>
     * from a typed PropertyBag.
     */
    class VectorComposer
    {
        const PropertyBag& bag;
        public:
            VectorComposer( const PropertyBag& _bag )
                : bag(_bag)
            {}

            bool getResult( Property<Vector>& res )
            {
                return getResult( res.value(), res.getName() );
            }
                
            bool getResult( Vector& res, const string& name );
    };
            

    /**
     * Additional functionality for handling a Property<Vector>
     */
    void decomposeProperty(PropertyIntrospection *p, const Property<Vector> &v);

    /**
     * Aim of this kind of functions : given a Property<PropertyBag> with one of the
     * properties being a bag containing a Vector, fill the given non primitive Property<T>.
     */
    bool composeProperty(const PropertyBag& bag, Property<Vector> &v);

    /**
     *
     *    ORO_Geometry::Rotation
     *
     */

    /**
     * A Decomposer for Converting a Rotation or Property<Rotation>
     * into a typed Property<PropertyBag>.
     */
    class RotationDecomposer
    {
        Property<PropertyBag> resultBag;
        Property<double> X_x;
        Property<double> X_y;
        Property<double> X_z;
        Property<double> Y_x;
        Property<double> Y_y;
        Property<double> Y_z;
        Property<double> Z_x;
        Property<double> Z_y;
        Property<double> Z_z;
        
        public: 
        
        RotationDecomposer( const Property<Rotation>& r );
        RotationDecomposer( const Rotation& r, const string& name );
        
        Property<PropertyBag>& result() { return resultBag; }
    };
    
    /**
     * An Composer for constructing a Rotation or Property<Rotation>
     * from a typed PropertyBag.
     */
    class RotationComposer
    {
        const PropertyBag& bag;
        public:
            RotationComposer( const PropertyBag& _bag )
              :  bag(_bag)
            {}

            bool getResult( Property<Rotation>& res )
            {
                return getResult( res.value(), res.getName() );
            }

            bool getResult( Rotation& res, const string& name );
    };
            
        
    /**
     * Additional functionality for handling a Property<Rotation>
     */
    void decomposeProperty(PropertyIntrospection *p, const Property<Rotation> &b);

    /**
     * Aim of this kind of functions : given a Property<PropertyBag> with one of the
     * properties being a bag containing a Rotation, fill the given non primitive Property<T>.
     */
    bool composeProperty(const PropertyBag& bag, Property<Rotation> &r);

    /**
     *
     *    ORO_Geometry::Twist
     *
     */

    /**
     * Additional functionality for handling a Property<Twist>
     */
    void decomposeProperty(PropertyIntrospection *p, const Property<Twist> &t);

    /**
     * Aim of this kind of functions : given a Property<PropertyBag> with one of the
     * properties being a bag containing a Twist, fill the given non primitive Property<T>.
     */
    bool composeProperty(const PropertyBag& bag, Property<Twist> &t);

    /**
     *
     *    ORO_Geometry::Wrench
     *
     */

    /**
     * Additional functionality for handling a Property<Wrench>
     */
    void decomposeProperty(PropertyIntrospection *p, const Property<Wrench> &b);

    /**
     * Aim of this kind of functions : given a Property<PropertyBag> with one of the
     * properties being a bag containing a Wrench, fill the given non primitive Property<T>.
     */
    bool composeProperty(const PropertyBag& bag, Property<Wrench> &w);

    /**
     *
     *    ORO_Geometry::Frame
     *
     */

    /**
     * Additional functionality for handling a Property<Frame>
     */
    void decomposeProperty(PropertyIntrospection *p, const Property<Frame> &f);

    /**
     * Aim of this kind of functions : given a Property<PropertyBag> with one of the
     * properties being a bag containing a Frame, fill the given non primitive Property<T>.
     */
    bool composeProperty(const PropertyBag& bag, Property<Frame> &f);

}

#endif
