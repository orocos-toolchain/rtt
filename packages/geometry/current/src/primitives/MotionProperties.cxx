#include "geometry/MotionProperties.hpp"

namespace ORO_Geometry
{

    /**
     * @todo TODO : Put the strings/names in a separate file/struct/...
     */
    
    using namespace ORO_CoreLib;

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

        Double6DDecomposer::Double6DDecomposer( const Double6D& d, const string& name )
            : resultBag( name, string(), PropertyBag("MotCon::Double6D") ), // bag_type
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

        bool Double6DComposer::getResult( Double6D& res, const string& name )
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
                    }
                }
            }
            return false;
        }

    VectorDecomposer::VectorDecomposer( const Property<Vector>& v )
            : resultBag(v.getName(), v.getDescription(), PropertyBag("MotCon::Vector") ), // bag_type
        X("X","X Value", v.get()[0]),
        Y("Y","Y Value", v.get()[1]),
        Z("Z","Z Value", v.get()[2])
        {
            resultBag.value().add(&X);
            resultBag.value().add(&Y);
            resultBag.value().add(&Z);
        }

        VectorDecomposer::VectorDecomposer( const Vector& v, const string& name )
            : resultBag( name, string(), PropertyBag("MotCon::Vector") ), // bag_type
        X("X","X Value", v[0]),
        Y("Y","Y Value", v[1]),
        Z("Z","Z Value", v[2])
        {
            resultBag.value().add(&X);
            resultBag.value().add(&Y);
            resultBag.value().add(&Z);
        }

            bool VectorComposer::getResult( Vector& res, const string& name )
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
                            res = Vector( px->get(),py->get(),pz->get() );
                            return true;
                        }
                    }
                }
                return false;
            }


            RotationDecomposer::RotationDecomposer( const Property<Rotation>& r )
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

        RotationDecomposer::RotationDecomposer( const Rotation& r, const string& name )
            : resultBag(name, string(), PropertyBag("MotCon::Rotation") ),
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

            bool RotationComposer::getResult( Rotation& res, const string& name )
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
                            res = Rotation( 
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

    void decomposeProperty(PropertyIntrospection *p, const Property<Vector> &v)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        VectorDecomposer vco(v);
        p->introspect( vco.result() );
    }

    bool composeProperty(const PropertyBag& bag, Property<Vector> &v)
    {
        VectorComposer vas( bag );
        return vas.getResult(v);
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<Rotation> &b)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        RotationDecomposer rot(b);
        p->introspect( rot.result() );
    }

    bool composeProperty(const PropertyBag& bag, Property<Rotation> &r)
    {
        RotationComposer ras(bag);
        return ras.getResult( r );
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<Twist> &t)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        Property<PropertyBag> result(t.getName(), t.getDescription(), PropertyBag("MotCon::Twist") ); // bag_type

        VectorDecomposer vel( t.get().vel, "Trans_Vel" );
        VectorDecomposer rot( t.get().rot, "Rot_Vel" );

        result.value().add( &vel.result() );
        result.value().add( &rot.result() );
        
        p->introspect(result);
    }

    bool composeProperty(const PropertyBag& bag, Property<Twist> &t)
    {
        // find the Twist with the same name in the bag.
        PropertyBase* t_base = bag.find( t.getName() );
        if ( t_base != 0 )
        {
            Property<PropertyBag>* t_bag = dynamic_cast< Property<PropertyBag>* >( t_base );

            if ( t_bag != 0  && t_bag->get().getType() == string("MotCon::Twist") )
            {
                // pass this bag to the vector Composers
                VectorComposer vas_vel( *t_bag );
                VectorComposer vas_rot( *t_bag );

                return vas_vel.getResult( t.value().vel,"Trans_Vel") && vas_rot.getResult( t.value().rot,"Rot_Vel" );
            }
        }
        return false;
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<Wrench> &b)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        Property<PropertyBag> result(b.getName(), b.getDescription(), PropertyBag("MotCon::Wrench") ); // bag_type

        VectorDecomposer force( b.get().force, "Force" );
        VectorDecomposer torque( b.get().torque, "Torque" );

        result.value().add( &force.result() );
        result.value().add( &torque.result() );
        
        p->introspect(result);
    }

    bool composeProperty(const PropertyBag& bag, Property<Wrench> &w)
    {
        // find the Wrench with the same name in the bag.
        PropertyBase* w_base = bag.find( w.getName() );
        if ( w_base != 0 )
        {
            Property<PropertyBag>* w_bag = dynamic_cast< Property<PropertyBag>* >( w_base );

            if ( w_bag != 0  && w_bag->get().getType() == string("MotCon::Wrench") )
            {
                // pass this bag to the vector Composers
                VectorComposer vas_force( *w_bag );
                VectorComposer vas_torque( *w_bag );

                return vas_force.getResult( w.value().force,"Force") && vas_torque.getResult( w.value().torque, "Torque" );
            }
        }
        return false;
    }

    void decomposeProperty(PropertyIntrospection *p, const Property<Frame> &f)
    {
        // construct a property with same name and description, but containing a typed PropertyBag.
        Property<PropertyBag> result(f.getName(), f.getDescription(), PropertyBag("MotCon::Frame") ); // bag_type

        VectorDecomposer vel( f.get().p, "Position" );
        RotationDecomposer rot( f.get().M, "Rotation" );

        result.value().add( &vel.result() );
        result.value().add( &rot.result() );
        
        p->introspect(result);
    }

    bool composeProperty(const PropertyBag& bag, Property<Frame> &f)
    {
        // find the Frame with the same name in the bag.
        PropertyBase* f_base = bag.find( f.getName() );
        if ( f_base != 0 )
        {
            Property<PropertyBag>* f_bag = dynamic_cast< Property<PropertyBag>* >( f_base );

            if ( f_bag != 0  && f_bag->get().getType() == string("MotCon::Frame") )
            {
                // pass this bag to the vector Composers
                VectorComposer vas_pos( *f_bag );
                RotationComposer vas_rot( *f_bag );

                return vas_pos.getResult( f.value().p,"Position" ) && vas_rot.getResult( f.value().M, "Rotation" );
            }
        }
        return false;
    }
} // MotionControl
