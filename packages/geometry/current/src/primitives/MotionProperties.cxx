/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  MotionProperties.cxx 

                        MotionProperties.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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

#include <corelib/Property.hpp>
#include <corelib/PropertyIntrospection.hpp>
#include "geometry/MotionProperties.hpp"
#include <pkgconf/geometry.h>
#include <corelib/Logger.hpp>

namespace ORO_CoreLib
{

    template class Property<ORO_Geometry::Frame>;
    template class Property<Double6D>;
    template class Property<ORO_Geometry::Wrench>;
    template class Property<ORO_Geometry::Twist>;
    template class Property<ORO_Geometry::Rotation>;
    template class Property<ORO_Geometry::Vector>;

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
        Double6DDecomposer( const Double6D& v, const std::string& name );
        
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
                
        bool getResult( Double6D& res, const std::string& name );
    };
            
    /**
     * @todo TODO : Put the strings/names in a separate file/struct/...
     */
    
    Double6DDecomposer::Double6DDecomposer( const Property<Double6D>& d )
        : resultBag(d.getName(), d.getDescription(), PropertyBag("MotCon::Double6D") ), // bag_type
          D1("D1","D1 Value", d.get()[0]),
          D2("D2","D2 Value", d.get()[1]),
          D3("D3","D3 Value", d.get()[2]),
          D4("D4","D4 Value", d.get()[3]),
          D5("D5","D5 Value", d.get()[4]),
          D6("D6","D6 Value", d.get()[5])
    {
        resultBag.value().add(&D1);
        resultBag.value().add(&D2);
        resultBag.value().add(&D3);
        resultBag.value().add(&D4);
        resultBag.value().add(&D5);
        resultBag.value().add(&D6);
    }

    Double6DDecomposer::Double6DDecomposer( const Double6D& d, const std::string& name )
        : resultBag( name, std::string(), PropertyBag("MotCon::Double6D") ), // bag_type
          D1("D1","D1 Value", d[0]),
          D2("D2","D2 Value", d[1]),
          D3("D3","D3 Value", d[2]),
          D4("D4","D4 Value", d[3]),
          D5("D5","D5 Value", d[4]),
          D6("D6","D6 Value", d[5])
    {
        resultBag.value().add(&D1);
        resultBag.value().add(&D2);
        resultBag.value().add(&D3);
        resultBag.value().add(&D4);
        resultBag.value().add(&D5);
        resultBag.value().add(&D6);
    }

    bool Double6DComposer::getResult( Double6D& res, const std::string& name )
    {
        // find the Double6D with the same name in the bag.
        PropertyBase* v_base = bag.find( name );
        if ( v_base != 0 )
            {
                Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

                if ( v_bag != 0  && v_bag->get().getType() == "MotCon::Double6D" )
                    {
                        Property<double>* d1 = dynamic_cast<Property<double>*>( v_bag->get().find("D1") );
                        Property<double>* d2 = dynamic_cast<Property<double>*>( v_bag->get().find("D2") );
                        Property<double>* d3 = dynamic_cast<Property<double>*>( v_bag->get().find("D3") );
                        Property<double>* d4 = dynamic_cast<Property<double>*>( v_bag->get().find("D4") );
                        Property<double>* d5 = dynamic_cast<Property<double>*>( v_bag->get().find("D5") );
                        Property<double>* d6 = dynamic_cast<Property<double>*>( v_bag->get().find("D6") );
                        // found it.
                        if ( d1 != 0 && d2 != 0  && d3 != 0 && d4 != 0 && d5 != 0  && d6 != 0)
                            {
                                res[0] = d1->get();
                                res[1] = d2->get();
                                res[2] = d3->get();
                                res[3] = d4->get();
                                res[4] = d5->get();
                                res[5] = d6->get();
                                return true;
                            } else {
                                std::string element = !d1 ? "D1" : !d2 ? "D2" : !d3 ? "D3" : !d4 ? "D4" : !d5 ? "D5" :  "D6";
                                Logger::log() << Logger::Error << "Aborting composition of Property< Double6D > "<<v_bag->getName()
                                              << ": Missing element '" <<element<<"'." <<Logger::endl;
                                return false;
                            }
                    } else {
                        if ( v_bag == 0 ) {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Double6D > "<< name
                                          << ": not a PropertyBag." <<Logger::endl;
                        } else {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Double6D > "<<v_bag->getName()
                                          << ": Expected type 'MotCon::Double6D', got type '"<< v_bag->get().getType() <<"'."
                                          <<Logger::endl;
                        }
                        return false;
                    }

            }
        return false;
    }

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
        
        VectorDecomposer( const Property<ORO_Geometry::Vector>& v );
        VectorDecomposer( const ORO_Geometry::Vector& v, const std::string& name );
        
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

        bool getResult( Property<ORO_Geometry::Vector>& res )
        {
            return getResult( res.value(), res.getName() );
        }
                
        bool getResult( ORO_Geometry::Vector& res, const std::string& name );
    };
            
    VectorDecomposer::VectorDecomposer( const Property<ORO_Geometry::Vector>& v )
        : resultBag(v.getName(), v.getDescription(), PropertyBag("MotCon::Vector") ), // bag_type
          X("X","X Value", v.get()[0]),
          Y("Y","Y Value", v.get()[1]),
          Z("Z","Z Value", v.get()[2])
    {
        resultBag.value().add(&X);
        resultBag.value().add(&Y);
        resultBag.value().add(&Z);
    }

    VectorDecomposer::VectorDecomposer( const ORO_Geometry::Vector& v, const std::string& name )
        : resultBag( name, std::string(), PropertyBag("MotCon::Vector") ), // bag_type
          X("X","X Value", v[0]),
          Y("Y","Y Value", v[1]),
          Z("Z","Z Value", v[2])
    {
        resultBag.value().add(&X);
        resultBag.value().add(&Y);
        resultBag.value().add(&Z);
    }

    bool VectorComposer::getResult( ORO_Geometry::Vector& res, const std::string& name )
    {
        // find the Vector with the same name in the bag.
        PropertyBase* v_base = bag.find( name );
        if ( v_base != 0 )
            {
                Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

                if ( v_bag != 0  && v_bag->get().getType() == "MotCon::Vector" )
                    {
                        Property<double>* px = dynamic_cast<Property<double>*>( v_bag->get().find("X") );
                        Property<double>* py = dynamic_cast<Property<double>*>( v_bag->get().find("Y") );
                        Property<double>* pz = dynamic_cast<Property<double>*>( v_bag->get().find("Z") );
                        // found it.
                        if ( px != 0 && py != 0  && pz != 0)
                            {
                                res = ORO_Geometry::Vector( px->get(),py->get(),pz->get() );
                                return true;
                            } else {
                                std::string element = !px ? "X" : !py ? "Y" : "Z";
                                Logger::log() << Logger::Error << "Aborting composition of Property< Vector > "<<v_bag->getName()
                                              << ": Missing element '" <<element<<"'." <<Logger::endl;
                                return false;
                            }
                    } else {
                        if ( v_bag == 0 ) {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Vector > "<< name
                                          << ": not a PropertyBag." <<Logger::endl;
                        } else {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Vector > "<<v_bag->getName()
                                          << ": Expected type 'MotCon::Vector', got type '"<< v_bag->get().getType() <<"'."
                                          <<Logger::endl;
                        }
                        return false;
                    }
            }
        return false;
    }


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
        
        RotationDecomposer( const Property<ORO_Geometry::Rotation>& r );
        RotationDecomposer( const ORO_Geometry::Rotation& r, const std::string& name );
        
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

        bool getResult( Property<ORO_Geometry::Rotation>& res )
        {
            return getResult( res.value(), res.getName() );
        }

        bool getResult( ORO_Geometry::Rotation& res, const std::string& name );
    };
            
    RotationDecomposer::RotationDecomposer( const Property<ORO_Geometry::Rotation>& r )
        : resultBag(r.getName(), r.getDescription(), PropertyBag("MotCon::Rotation") ),
          X_x("X_x","", r.get()(0,0) ),
          X_y("X_y","", r.get()(0,1) ),
          X_z("X_z","", r.get()(0,2) ),
          Y_x("Y_x","", r.get()(1,0) ),
          Y_y("Y_y","", r.get()(1,1) ),
          Y_z("Y_z","", r.get()(1,2) ),
          Z_x("Z_x","", r.get()(2,0) ),
          Z_y("Z_y","", r.get()(2,1) ),
          Z_z("Z_z","", r.get()(2,2) )
    {
        resultBag.value().add(&X_x);
        resultBag.value().add(&X_y);
        resultBag.value().add(&X_z);
        resultBag.value().add(&Y_x);
        resultBag.value().add(&Y_y);
        resultBag.value().add(&Y_z);
        resultBag.value().add(&Z_x);
        resultBag.value().add(&Z_y);
        resultBag.value().add(&Z_z);
    }

    RotationDecomposer::RotationDecomposer( const ORO_Geometry::Rotation& r, const std::string& name )
        : resultBag(name, std::string(), PropertyBag("MotCon::Rotation") ),
          X_x("X_x","", r(0,0) ),
          X_y("X_y","", r(0,1) ),
          X_z("X_z","", r(0,2) ),
          Y_x("Y_x","", r(1,0) ),
          Y_y("Y_y","", r(1,1) ),
          Y_z("Y_z","", r(1,2) ),
          Z_x("Z_x","", r(2,0) ),
          Z_y("Z_y","", r(2,1) ),
          Z_z("Z_z","", r(2,2) )
    {
        resultBag.value().add(&X_x);
        resultBag.value().add(&X_y);
        resultBag.value().add(&X_z);
        resultBag.value().add(&Y_x);
        resultBag.value().add(&Y_y);
        resultBag.value().add(&Y_z);
        resultBag.value().add(&Z_x);
        resultBag.value().add(&Z_y);
        resultBag.value().add(&Z_z);
    }

    bool RotationComposer::getResult( ORO_Geometry::Rotation& res, const std::string& name )
    {
        // find the Rotation with the same name in the bag.
        PropertyBase* v_base = bag.find( name );
        if ( v_base != 0 )
            {
                Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

                if ( v_bag != 0  && v_bag->get().getType() == "MotCon::Rotation" )
                    {
                        Property<double>* X_x = dynamic_cast<Property<double>*>( v_bag->get().find("X_x") );
                        Property<double>* X_y = dynamic_cast<Property<double>*>( v_bag->get().find("X_y") );
                        Property<double>* X_z = dynamic_cast<Property<double>*>( v_bag->get().find("X_z") );
                        Property<double>* Y_x = dynamic_cast<Property<double>*>( v_bag->get().find("Y_x") );
                        Property<double>* Y_y = dynamic_cast<Property<double>*>( v_bag->get().find("Y_y") );
                        Property<double>* Y_z = dynamic_cast<Property<double>*>( v_bag->get().find("Y_z") );
                        Property<double>* Z_x = dynamic_cast<Property<double>*>( v_bag->get().find("Z_x") );
                        Property<double>* Z_y = dynamic_cast<Property<double>*>( v_bag->get().find("Z_y") );
                        Property<double>* Z_z = dynamic_cast<Property<double>*>( v_bag->get().find("Z_z") );
                        // found it.
                        if (  X_x != 0 && X_y != 0  && X_z != 0 &&
                              Y_x != 0 && Y_y != 0  && Y_z != 0 &&
                              Z_x != 0 && Z_y != 0  && Z_z != 0 )
                            {
                                res = ORO_Geometry::Rotation( 
                                               X_x->get(), Y_x->get(),Z_x->get(),
                                               X_y->get(),Y_y->get(),Z_y->get(),
                                               X_z->get(),Y_z->get(),Z_z->get() 
                                               );
                                return true;
                            }
                    }
            }
        return false;
    }

    /**
     * A Decomposer for Converting a Rotation or Property<Rotation>
     * into a typed Property<PropertyBag> with the EulerZYX convention.
     */
    class EulerZYXDecomposer
    {
        Property<PropertyBag> resultBag;
        Property<double> _a;
        Property<double> _b;
        Property<double> _g;

    public: 
        
        EulerZYXDecomposer( const Property<ORO_Geometry::Rotation>& r );
        EulerZYXDecomposer( const ORO_Geometry::Rotation& r, const std::string& name );
        
        Property<PropertyBag>& result() { return resultBag; }
    };
    
    /**
     * An Composer for constructing an EulerZYX Rotation or Property<Rotation>
     * from a typed PropertyBag.
     */
    class EulerZYXComposer
    {
        const PropertyBag& bag;
    public:
        EulerZYXComposer( const PropertyBag& _bag )
            :  bag(_bag)
        {}

        bool getResult( Property<ORO_Geometry::Rotation>& res )
        {
            return getResult( res.value(), res.getName() );
        }

        bool getResult( ORO_Geometry::Rotation& res, const std::string& name );
    };
            
    EulerZYXDecomposer::EulerZYXDecomposer( const Property<ORO_Geometry::Rotation>& r )
        : resultBag(r.getName(), r.getDescription(), PropertyBag("MotCon::EulerZYX") ),
          _a("alpha","First Rotate around the Z axis with alpha in radians" ),
          _b("beta","Then Rotate around the new Y axis with beta in radians" ),
          _g("gamma","Then Rotation around the new X axis with gamma in radians" )
    {
        r.get().GetEulerZYX(_a.set(), _b.set(), _g.set());
        resultBag.value().add(&_a);
        resultBag.value().add(&_b);
        resultBag.value().add(&_g);
    }

    EulerZYXDecomposer::EulerZYXDecomposer( const ORO_Geometry::Rotation& r, const std::string& name )
        : resultBag(name, std::string(), PropertyBag("MotCon::EulerZYX") ),
          _a("alpha","First Rotate around the Z axis with alpha in radians" ),
          _b("beta","Then Rotate around the new Y axis with beta in radians" ),
          _g("gamma","Then Rotation around the new X axis with gamma in radians" )
    {
        r.GetEulerZYX(_a.set(), _b.set(), _g.set());
        resultBag.value().add(&_a);
        resultBag.value().add(&_b);
        resultBag.value().add(&_g);
    }

    bool EulerZYXComposer::getResult( ORO_Geometry::Rotation& res, const std::string& name )
    {
        // find the Rotation with the same name in the bag.
        PropertyBase* v_base = bag.find( name );
        if ( v_base != 0 )
            {
                Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

                if ( v_bag != 0  && v_bag->get().getType() == "MotCon::EulerZYX" )
                    {
                        // ZYX is deprecated, use alpha, beta, gamma. also alpha maps to Z and gamma to X !
                        Property<double>* _a = dynamic_cast<Property<double>*>( v_bag->get().find("alpha") );
                        if ( !_a)
                            _a = dynamic_cast<Property<double>*>( v_bag->get().find("Z") );
                        Property<double>* _b = dynamic_cast<Property<double>*>( v_bag->get().find("beta") );
                        if ( !_b)
                            _b = dynamic_cast<Property<double>*>( v_bag->get().find("Y") );
                        Property<double>* _g = dynamic_cast<Property<double>*>( v_bag->get().find("gamma") );
                        if ( !_g)
                            _g = dynamic_cast<Property<double>*>( v_bag->get().find("X") );

                        // found it.
                        if (  _a != 0 && _b != 0  && _g != 0 )
                            {
                                res = ORO_Geometry::Rotation::EulerZYX(_a->get(), _b->get(), _g->get() );
                                return true;
                            } else {
                                std::string element = !_a ? "alpha" : !_b ? "beta" : "gamma";
                                Logger::log() << Logger::Error << "Aborting composition of (EulerZYX) Property< Rotation > "<<v_bag->getName()
                                              << ": Missing element '" <<element<<"'." <<Logger::endl;
                                return false;
                            }
                    }
            }
        return false;
    }

    /**
     * A Decomposer for Converting a Rotation or Property<Rotation>
     * into a typed Property<PropertyBag> with the RPY convention.
     */
    class RPYDecomposer
    {
        Property<PropertyBag> resultBag;
        Property<double> _r;
        Property<double> _p;
        Property<double> _y;

    public: 
        
        RPYDecomposer( const Property<ORO_Geometry::Rotation>& r );
        RPYDecomposer( const ORO_Geometry::Rotation& r, const std::string& name );
        
        Property<PropertyBag>& result() { return resultBag; }
    };
    
    /**
     * An Composer for constructing an RPY Rotation or Property<Rotation>
     * from a typed PropertyBag.
     */
    class RPYComposer
    {
        const PropertyBag& bag;
    public:
        RPYComposer( const PropertyBag& _bag )
            :  bag(_bag)
        {}

        bool getResult( Property<ORO_Geometry::Rotation>& res )
        {
            return getResult( res.value(), res.getName() );
        }

        bool getResult( ORO_Geometry::Rotation& res, const std::string& name );
    };
            
    RPYDecomposer::RPYDecomposer( const Property<ORO_Geometry::Rotation>& r )
        : resultBag(r.getName(), r.getDescription(), PropertyBag("MotCon::RPY") ),
          _r("R","First rotate around X with R(oll) in radians" ),
          _p("P","Next rotate around old Y with P(itch) in radians" ),
          _y("Y","Next rotate around old Z with Y(aw) in radians" )
    {
        r.get().GetRPY(_r.set(), _p.set(), _y.set());
        resultBag.value().add(&_r);
        resultBag.value().add(&_p);
        resultBag.value().add(&_y);
    }

    RPYDecomposer::RPYDecomposer( const ORO_Geometry::Rotation& r, const std::string& name )
        : resultBag(name, std::string(), PropertyBag("MotCon::RPY") ),
          _r("R","First rotate around X with R(oll) in radians" ),
          _p("P","Next rotate around old Y with P(itch) in radians" ),
          _y("Y","Next rotate around old Z with Y(aw) in radians" )
    {
        r.GetRPY(_r.set(), _p.set(), _y.set());
        resultBag.value().add(&_r);
        resultBag.value().add(&_p);
        resultBag.value().add(&_y);
    }

    bool RPYComposer::getResult( ORO_Geometry::Rotation& res, const std::string& name )
    {
        // find the Rotation with the same name in the bag.
        PropertyBase* v_base = bag.find( name );
        if ( v_base != 0 )
            {
                Property<PropertyBag>* v_bag = dynamic_cast< Property<PropertyBag>* >( v_base );

                if ( v_bag != 0  && v_bag->get().getType() == "MotCon::RPY" )
                    {
                        Property<double>* _r = dynamic_cast<Property<double>*>( v_bag->get().find("R") );
                        Property<double>* _p = dynamic_cast<Property<double>*>( v_bag->get().find("P") );
                        Property<double>* _y = dynamic_cast<Property<double>*>( v_bag->get().find("Y") );

                        // found it.
                        if (  _r != 0 && _p != 0  && _y != 0 )
                            {
                                res = ORO_Geometry::Rotation::RPY(_r->get(), _p->get(), _y->get() );
                                return true;
                            } else {
                                std::string element = !_r ? "R" : !_p ? "P" : "Y";
                                Logger::log() << Logger::Error << "Aborting composition of (RPY) Property< Rotation > "<<v_bag->getName()
                                              << ": Missing element '" <<element<<"'." <<Logger::endl;
                                return false;
                            }
                    }
            }
        return false;
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<Double6D> &v)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        Double6DDecomposer vco(v);
        p->introspect( vco.result() );
    }

    bool composeProperty(const PropertyBag& bag, Property<Double6D> &v)
    {
        Double6DComposer vas( bag );
        return vas.getResult(v);
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<ORO_Geometry::Vector> &v)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        VectorDecomposer vco(v);
        p->introspect( vco.result() );
    }

    bool composeProperty(const PropertyBag& bag, Property<ORO_Geometry::Vector> &v)
    {
        VectorComposer vas( bag );
        return vas.getResult(v);
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<ORO_Geometry::Rotation> &b)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
#ifdef OROSEM_GEOMETRY_ROTATION_PROPERTIES_EULER
        EulerZYXDecomposer rot(b);
#else
# ifdef OROSEM_GEOMETRY_ROTATION_PROPERTIES_RPY
        RPYDecomposer rot(b);
# else
        RotationDecomposer rot(b);
# endif
#endif
        p->introspect( rot.result() );
    }

    bool composeProperty(const PropertyBag& bag, Property<ORO_Geometry::Rotation> &r)
    {
        // try all three, see which one works, that one will fill in r.
        RPYComposer      rpyc(bag);
        EulerZYXComposer eulc(bag);
        RotationComposer rotc(bag);

        if ( rpyc.getResult( r ) || eulc.getResult( r ) || rotc.getResult( r ) )
            return true;
        else {
            Property<PropertyBag>* b = bag.getProperty<PropertyBag>( r.getName() );
            if ( b == 0 ) {
                Logger::log() << Logger::Error << "Aborting composition of Property< Rotation > "<< r.getName()
                              << ": element not found." <<Logger::endl;
            } else {
                Logger::log() << Logger::Error << "Aborting composition of Property< Rotation > "<< r.getName()
                              << ": Expected type 'MotCon::Rotation','MotCon::EulerZYX' or 'MotCon::RPY', got type '"<< b->get().getType() <<"'."
                              <<Logger::endl;
            }
            return false;
        }

    }

    void decomposeProperty(PropertyIntrospection *p, const Property<ORO_Geometry::Twist> &t)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        Property<PropertyBag> result(t.getName(), t.getDescription(), PropertyBag("MotCon::Twist") ); // bag_type

        VectorDecomposer vel( t.get().vel, "Trans_Vel" );
        VectorDecomposer rot( t.get().rot, "Rot_Vel" );

        result.value().add( &vel.result() );
        result.value().add( &rot.result() );
        
        p->introspect(result);
    }

    bool composeProperty(const PropertyBag& bag, Property<ORO_Geometry::Twist> &t)
    {
        // find the Twist with the same name in the bag.
        PropertyBase* t_base = bag.find( t.getName() );
        if ( t_base != 0 )
            {
                Property<PropertyBag>* t_bag = dynamic_cast< Property<PropertyBag>* >( t_base );

                if ( t_bag != 0  && t_bag->get().getType() == std::string("MotCon::Twist") )
                    {
                        // pass this bag to the vector Composers
                        VectorComposer vas_vel( t_bag->get() );
                        VectorComposer vas_rot( t_bag->get() );

                        return vas_vel.getResult( t.value().vel,"Trans_Vel") && vas_rot.getResult( t.value().rot,"Rot_Vel" );
                    } else {
                        if ( t_bag == 0 ) {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Twist > "<< t.getName()
                                          << ": not a PropertyBag." <<Logger::endl;
                        } else {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Twist > "<<t_bag->getName()
                                          << ": Expected type 'MotCon::Twist', got type '"<< t_bag->get().getType() <<"'."
                                          <<Logger::endl;
                        }
                        return false;
                    }
            }
        return false;
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<ORO_Geometry::Wrench> &b)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        Property<PropertyBag> result(b.getName(), b.getDescription(), PropertyBag("MotCon::Wrench") ); // bag_type

        VectorDecomposer force( b.get().force, "Force" );
        VectorDecomposer torque( b.get().torque, "Torque" );

        result.value().add( &force.result() );
        result.value().add( &torque.result() );
        
        p->introspect(result);
    }

    bool composeProperty(const PropertyBag& bag, Property<ORO_Geometry::Wrench> &w)
    {
        // find the Wrench with the same name in the bag.
        PropertyBase* w_base = bag.find( w.getName() );
        if ( w_base != 0 )
            {
                Property<PropertyBag>* w_bag = dynamic_cast< Property<PropertyBag>* >( w_base );

                if ( w_bag != 0  && w_bag->get().getType() == std::string("MotCon::Wrench") )
                    {
                        // pass this bag to the vector Composers
                        VectorComposer vas_force( w_bag->get() );
                        VectorComposer vas_torque( w_bag->get() );

                        return vas_force.getResult( w.value().force,"Force") && vas_torque.getResult( w.value().torque, "Torque" );
                    } else {
                        if ( w_bag == 0 ) {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Wrench > "<< w.getName()
                                          << ": not a PropertyBag." <<Logger::endl;
                        } else {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Wrench > "<<w_bag->getName()
                                          << ": Expected type 'MotCon::Wrench', got type '"<< w_bag->get().getType() <<"'."
                                          <<Logger::endl;
                        }
                        return false;
                    }
            }
        return false;
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<ORO_Geometry::Frame> &f)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        Property<PropertyBag> result(f.getName(), f.getDescription(), PropertyBag("MotCon::Frame") ); // bag_type

        VectorDecomposer vel( f.get().p, "Position" );
#ifdef OROSEM_GEOMETRY_ROTATION_PROPERTIES_EULER
        EulerZYXDecomposer rot( f.get().M, "Rotation" );
#else
# ifdef OROSEM_GEOMETRY_ROTATION_PROPERTIES_RPY
        RPYDecomposer rot( f.get().M, "Rotation");
# else
        RotationDecomposer rot( f.get().M, "Rotation" );
# endif
#endif

        result.value().add( &vel.result() );
        result.value().add( &rot.result() );
        
        p->introspect(result);
    }

    bool composeProperty(const PropertyBag& bag, Property<ORO_Geometry::Frame> &f)
    {
        // find the Frame with the same name in the bag.
        PropertyBase* f_base = bag.find( f.getName() );
        if ( f_base != 0 )
            {
                Property<PropertyBag>* f_bag = dynamic_cast< Property<PropertyBag>* >( f_base );

                if ( f_bag != 0  && f_bag->get().getType() == std::string("MotCon::Frame") )
                    {
                        // pass this bag to the vector Composers
                        VectorComposer vas_pos( f_bag->get() );
                        RPYComposer vas_rpy( f_bag->get() );
                        EulerZYXComposer vas_eul( f_bag->get() );
                        RotationComposer vas_rot( f_bag->get() );
                        bool result = vas_pos.getResult( f.value().p,"Position" );
                        if (!result )
                            {
                                Property<PropertyBag>* b = f_bag->get().getProperty<PropertyBag>( "Rotation" );
                                if ( b == 0 ) {
                                    Logger::log() << Logger::Error << "Aborting composition of Property< Frame > "<< f.getName()
                                                  << ": element 'Position' not found." <<Logger::endl;
                                } else {
                                    Logger::log() << Logger::Error << "Aborting composition of Property< Frame > "<< f.getName()
                                                  << ": element 'Position' has wrong format." <<Logger::endl;
                                }
                                return false;
                            }
                        result = vas_rpy.getResult( f.value().M, "Rotation" ) ||
                            vas_eul.getResult( f.value().M, "Rotation" ) ||
                            vas_rot.getResult( f.value().M, "Rotation" );
                        if (!result )
                            {
                                Property<PropertyBag>* b = f_bag->get().getProperty<PropertyBag>( "Rotation" );
                                if ( b == 0 ) {
                                    Logger::log() << Logger::Error << "Aborting composition of Property< Frame > "<< f.getName()
                                                  << ": element 'Rotation' not found." <<Logger::endl;
                                } else {
                                    Logger::log()
                                        << Logger::Error << "Aborting composition of Property< Frame > "<< f.getName()
                                        << ": Could not compose 'Rotation' type 'MotCon::Rotation','MotCon::EulerZYX' or 'MotCon::RPY', got type '"
                                        << b->get().getType() <<"'."<<Logger::endl;
                                }
                                return false;
                            }
                        return true;
                    } else {
                        if ( f_bag == 0 ) {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Frame > "<< f.getName()
                                          << ": not a PropertyBag." <<Logger::endl;
                        } else {
                            Logger::log() << Logger::Error << "Aborting composition of Property< Frame > "<<f_bag->getName()
                                          << ": Expected type 'MotCon::Frame', got type '"<< f_bag->get().getType() <<"'."
                                          <<Logger::endl;
                        }
                        return false;
                    }
            }
        return false;
    }
} // MotionControl
