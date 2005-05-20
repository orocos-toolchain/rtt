
Wrench Wrench::Zero()
{
    return Wrench(Vector::Zero(),Vector::Zero());
}


void Wrench::ReverseSign()
{   
    torque.ReverseSign();
    force.ReverseSign();
}

Wrench Wrench::RefPoint(const Vector& v_base_AB)
     // Changes the reference point of the Wrench.
     // The vector v_base_AB is expressed in the same base as the twist
     // The vector v_base_AB is a vector from the old point to
     // the new point.
{
    return Wrench(this->force,
                  this->torque+this->force*v_base_AB
                  );
}


Wrench& Wrench::operator-=(const Wrench& arg)
{
    torque-=arg.torque;
    force -=arg.force;
    return *this;
}

Wrench& Wrench::operator+=(const Wrench& arg)
{
    torque+=arg.torque;
    force +=arg.force;
    return *this;
}

double& Wrench::operator()(int i)
{
    // assert((0<=i)&&(i<6)); done by underlying routines
    if (i<3) 
        return force(i);
    else
        return torque(i-3);
}

double Wrench::operator()(int i) const
{
    // assert((0<=i)&&(i<6)); done by underlying routines
    if (i<3) 
        return force(i);
    else
        return torque(i-3);
}


Wrench operator*(const Wrench& lhs,double rhs)
{
    return Wrench(lhs.force*rhs,lhs.torque*rhs);
}

Wrench operator*(double lhs,const Wrench& rhs)
{
    return Wrench(lhs*rhs.force,lhs*rhs.torque);
}

Wrench operator/(const Wrench& lhs,double rhs)
{
    return Wrench(lhs.force/rhs,lhs.torque/rhs);
}

// addition of Wrench's
Wrench operator+(const Wrench& lhs,const Wrench& rhs)
{
    return Wrench(lhs.force+rhs.force,lhs.torque+rhs.torque);
}

Wrench operator-(const Wrench& lhs,const Wrench& rhs)
{
    return Wrench(lhs.force-rhs.force,lhs.torque-rhs.torque);
}

// unary -
Wrench operator-(const Wrench& arg) 
{
    return Wrench(-arg.force,-arg.torque);
}

IMETHOD double dot(const Wrench& rhs,const Twist& lhs) {
    return dot(lhs.vel,rhs.force)+dot(lhs.rot,rhs.torque);
}


IMETHOD bool Equal(const Wrench& a,const Wrench& b,double eps) {
        return (Equal(a.force,b.force,eps)&&
                Equal(a.torque,b.torque,eps)  );
}

IMETHOD bool operator==(const Wrench& a,const Wrench& b ) {
#ifdef OROPKG_GEOMETRY_USE_EQUAL
    return Equal(a,b);
#else
    return (a.force==b.force &&
            a.torque==b.torque );
#endif
}

IMETHOD bool operator!=(const Wrench& a,const Wrench& b) {
	return !operator==(a,b);
}

IMETHOD void SetToZero(Wrench& v) {
    SetToZero(v.force);
    SetToZero(v.torque);
}
