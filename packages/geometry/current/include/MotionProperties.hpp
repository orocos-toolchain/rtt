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
     * Additional functionality for handling a Property<Double6D>
     */
    void decomposeProperty(PropertyIntrospection *p, const Property<Double6D> &v);

    /**
     * Aim of this kind of functions : given a Property<PropertyBag> with one of the
     * properties being a bag containing a Double6D, fill the given non primitive Property<T>.
     */
    bool composeProperty(const PropertyBag& bag, Property<Double6D> &v);


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
