
#include <geometry/GeometryToolkit.hpp>
#include <corelib/Types.hpp>
#include <corelib/TemplateTypeInfo.hpp>
#include <corelib/Operators.hpp>
#include <corelib/OperatorTypes.hpp>
#include <corelib/mystd.hpp>
#include <geometry/frames_io.h>
#include <geometry/MotionProperties.hpp>

namespace ORO_Geometry
{
    using namespace ORO_CoreLib;
    using namespace ORO_std;
    using namespace ORO_CoreLib::detail;

    GeometryToolkitPlugin GeometryToolkit;

    std::string GeometryToolkitPlugin::getName()
    {
        return "Geometry";
    }

    /**
     * Used for Frame and Rotation.
     */
    template<class GeomType>
    struct GeomTypeInfo
        : public TemplateTypeInfo<GeomType,true>
    {
        GeomTypeInfo(std::string name) : TemplateTypeInfo<GeomType,true>(name) {}

        virtual bool decomposeTypeImpl(const GeomType& source, PropertyBag& targetbag ) const {
            decomposeProperty( source, targetbag );
            return true;
        }

        virtual bool composeTypeImpl(const PropertyBag& source, GeomType& result) const {
            return composeProperty( source, result );
        }
    };

    /**
     * Used for Vector, Twist and Wrench.
     */
    template<class GeomType, int R>
    struct GeomIndexTypeInfo
        : public TemplateIndexTypeInfo<GeomType, int, double, RangeIndexChecker<GeomType,R>, AlwaysAssignChecker<GeomType>, true>
    {
        GeomIndexTypeInfo(std::string name) 
            : TemplateIndexTypeInfo<GeomType, int, double, RangeIndexChecker<GeomType,R>, AlwaysAssignChecker<GeomType>, true>(name) {}

        virtual bool decomposeTypeImpl(const GeomType& source, PropertyBag& targetbag ) const {
            decomposeProperty( source, targetbag );
            return true;
        }

        virtual bool composeTypeImpl(const PropertyBag& source, GeomType& result) const {
            return composeProperty( source, result );
        }
    };


    bool GeometryToolkitPlugin::loadTypes() 
    {
        TypeInfoRepository::shared_ptr ti = TypeInfoRepository::Instance();

        ti->addType( new GeomIndexTypeInfo<Double6D,6>("double6d") );

        ti->addType( new GeomTypeInfo<Frame>("frame") );
        ti->addType( new GeomTypeInfo<Rotation>("rotation") );
        ti->addType( new GeomIndexTypeInfo<Vector,3>("vector") );
        ti->addType( new GeomIndexTypeInfo<Twist,6>("twist") );
        ti->addType( new GeomIndexTypeInfo<Wrench,6>("wrench") );

//         ti->addType( new TemplateIndexTypeInfo<Wrench,int, double, RangeIndexChecker<Wrench,6>, AlwaysAssignChecker<Wrench>,true >("wrench") );
//         ti->addType( new TemplateIndexTypeInfo<Twist,int, double, RangeIndexChecker<Twist,6>, AlwaysAssignChecker<Twist>,true >("twist") );
//         ti->addType( new TemplateIndexTypeInfo<Vector,int, double, RangeIndexChecker<Vector,3>, AlwaysAssignChecker<Vector>,true >("vector") );
        return true;
    }


    namespace {
        // CONSTRUCTORS
        Double6D double6Dd( double d )
        {
            Double6D d6d;
            for (int i=0; i<6; i++)
                d6d[i] = d;
            return d6d;
        }

        Double6D double6D6d( double a, double b, double c, double d, double e, double f )
        {
            Double6D d6d;
            d6d[0] = a;
            d6d[1] = b;
            d6d[2] = c;
            d6d[3] = d;
            d6d[4] = e;
            d6d[5] = f;
            return d6d;
        }

        struct framevr
            : public std::binary_function<Vector,Rotation, Frame>
        {
            Frame operator()( const Vector& v, const Rotation& r ) const
            {
                return Frame( r, v );
            }
        };

        struct wrenchft
            : public std::binary_function<Vector,Vector,Wrench>
        {
            Wrench operator()( const Vector& force, const Vector& torque ) const
            {
                return Wrench( force, torque );
            }
        };

        struct twistvw
            : public std::binary_function<Vector,Vector,Twist>
        {
            Twist operator()( const Vector& trans, const Vector& rot ) const
            {
                return Twist( trans, rot );
            }
        };

        struct vectorxyz
            : public std::ternary_function<double,double,double,Vector>
        {
            Vector operator()( double a, double b, double c ) const
            {
                return Vector( a, b, c );
            }
        };


        // INDEXING
        template<class WT>
        struct wrenchtwist_index
            : public std::binary_function<WT,int,double>
        {
            double operator()( WT& w, int index) const
            {
                if (index > 5 || index <0)
                    return 0.0;
                else
                    return w[index];
            }
        };
        /*
          double twist_index( Twist& t, int index)
          {
          if (index > 5 || index <0)
          return 0.0;
          else
          return t[index];
          }
        */

        double double6D_index( Double6D d6,  int index )
        {
            if ( index > 5 || index < 0 )
                return 0.0;
            return d6[index];
        }


        struct vector_index
            : public std::binary_function<Vector, int, double>
        {
            double operator()( Vector& v, int index ) const
                {
                    if (index > 2 || index <0)
                        return 0.0;
                    else
                        return v[index];
                }
        };

        // DOTTING
        template< int I>
        struct vector_dot
            : public std::unary_function<Vector, double>
        {
            double operator()(const Vector& v ) const
            {
                return v[I];
            }
        };

        struct twist_rot
            : public std::unary_function<Twist, Vector>
        {
            Vector operator()(const Twist& t ) const
            {
                return t.rot;
            }
        };

        struct twist_vel
            : public std::unary_function<Twist, Vector>
        {
            Vector operator()(const Twist& t ) const
            {
                return t.vel;
            }
        };

        struct wrench_torque
            : public std::unary_function<Wrench, Vector>
        {
            Vector operator()(const Wrench& t ) const
            {
                return t.torque;
            }
        };

        struct wrench_force
            : public std::unary_function<Wrench, Vector>
        {
            Vector operator()(const Wrench& t ) const
            {
                return t.force;
            }
        };

        struct frame_pos
            : public std::unary_function<Frame, Vector>
        {
            Vector operator()(const Frame& f ) const
            {
                return f.p;
            }
        };

        struct frame_rot
            : public std::unary_function<Frame, Rotation>
        {
            Rotation operator()(const Frame& f ) const
            {
                return f.M;
            }
        };

        struct frame_inv
            : public std::unary_function<Frame, Frame>
        {
            Frame operator()(const Frame& f ) const
            {
                return f.Inverse();
            }
        };

        struct rotation_roll
            : public std::unary_function<Rotation, double>
        {
            double operator()(const Rotation& rot ) const
            {
                double r,p,y;
                rot.GetRPY(r,p,y);
                return r;
            }
        };

        struct rotation_pitch
            : public std::unary_function<Rotation, double>
        {
            double operator()(const Rotation& rot ) const
            {
                double r,p,y;
                rot.GetRPY(r,p,y);
                return p;
            }
        };

        struct rotation_yaw
            : public std::unary_function<Rotation, double>
        {
            double operator()(const Rotation& rot ) const
            {
                double r,p,y;
                rot.GetRPY(r,p,y);
                return y;
            }
        };

        struct rotation_inv
            : public std::unary_function<Rotation, Rotation>
        {
            Rotation operator()(const Rotation& rot ) const
            {
                return rot.Inverse();
            }
        };
    }

    bool GeometryToolkitPlugin::loadOperators()
    {

        OperatorRepository::shared_ptr oreg = OperatorRepository::Instance();

        oreg->add( newUnaryOperator( "double6Dd", std::ptr_fun( &double6Dd ) ) );
        oreg->add( newSixaryOperator( "double6D6d", ptr_fun( &double6D6d ) ) );

        oreg->add( newBinaryOperator( "==", std::equal_to<Double6D>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<Double6D>() ) );
        oreg->add( newUnaryOperator( "-", std::negate<Double6D>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<Double6D>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<Double6D>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<Double6D>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Double6D, double, Double6D>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Double6D, Double6D, double>() ) );
        oreg->add( newBinaryOperator( "/", divides3<Double6D, Double6D, double>() ) );
        oreg->add( newBinaryOperator( "[]", std::ptr_fun( &double6D_index ) ) );


        oreg->add( newBinaryOperator( "==", std::equal_to<Frame>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<Frame>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<Vector>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<Vector>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<Rotation>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<Rotation>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<Wrench>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<Wrench>() ) );
        oreg->add( newBinaryOperator( "==", std::equal_to<Twist>() ) );
        oreg->add( newBinaryOperator( "!=", std::not_equal_to<Twist>() ) );
        oreg->add( newUnaryOperator( "-", std::negate<Vector>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<Vector>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<Frame>() ) );
        oreg->add( newBinaryOperator( "*", std::multiplies<Rotation>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<Vector>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<Vector>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<Wrench>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<Wrench>() ) );
        oreg->add( newBinaryOperator( "+", std::plus<Twist>() ) );
        oreg->add( newBinaryOperator( "-", std::minus<Twist>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Vector,int, Vector>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Vector,Vector, int>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Vector,double, Vector>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Vector,Vector, double>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Wrench, Frame, Wrench>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Twist, Frame, Twist>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Vector, Frame, Vector>() ) );
        oreg->add( newBinaryOperator( "*", multiplies3<Vector, Rotation, Vector>() ) );

        oreg->add( newBinaryOperator( "[]", vector_index() ) );
        oreg->add( newBinaryOperator( "[]", wrenchtwist_index<Wrench>() ) );
        oreg->add( newBinaryOperator( "[]", wrenchtwist_index<Twist>() ) );

        // access to members of composite types, use the 'dot' operator
        oreg->add( newDotOperator( "x", vector_dot<0>() ) );
        oreg->add( newDotOperator( "y", vector_dot<1>() ) );
        oreg->add( newDotOperator( "z", vector_dot<2>() ) );
        oreg->add( newDotOperator( "rot", twist_rot() ) );
        oreg->add( newDotOperator( "vel", twist_vel() ) );
        oreg->add( newDotOperator( "force", wrench_torque() ) );
        oreg->add( newDotOperator( "torque", wrench_force() ) );
        oreg->add( newDotOperator( "roll", rotation_roll() ) );
        oreg->add( newDotOperator( "pitch", rotation_pitch() ) );
        oreg->add( newDotOperator( "yaw", rotation_yaw() ) );
        oreg->add( newDotOperator( "inv", rotation_inv() ) );
        oreg->add( newDotOperator( "p", frame_pos() ) );
        oreg->add( newDotOperator( "M", frame_rot() ) );
        oreg->add( newDotOperator( "R", frame_rot() ) );
        oreg->add( newDotOperator( "inv", frame_inv() ) );

        oreg->add( newBinaryOperator( "[]", wrenchtwist_index<Wrench>() ) );
        oreg->add( newBinaryOperator( "[]", wrenchtwist_index<Twist>() ) );

        // constructors:
        oreg->add( newTernaryOperator( "vectorxyz", vectorxyz() ));
        oreg->add( newTernaryOperator( "rotationRPY", ptr_fun( Rotation::RPY ) ) );
        oreg->add( newBinaryOperator( "framevr", framevr() ) );
        oreg->add( newBinaryOperator( "wrenchft", wrenchft( ) ) );
        oreg->add( newBinaryOperator( "twistvw", twistvw () ) );

        return true;
    }

}
