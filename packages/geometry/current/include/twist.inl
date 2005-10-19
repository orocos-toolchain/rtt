
Twist Twist::Zero()
{
    return Twist(Vector::Zero(),Vector::Zero());
}


void Twist::ReverseSign()
{   
    vel.ReverseSign();
    rot.ReverseSign();
}

Twist Twist::RefPoint(const Vector& v_base_AB) const
     // Changes the reference point of the twist.
     // The vector v_base_AB is expressed in the same base as the twist
     // The vector v_base_AB is a vector from the old point to
     // the new point.
     // Complexity : 6M+6A
{
    return Twist(this->vel+this->rot*v_base_AB,this->rot);
}

Twist& Twist::operator-=(const Twist& arg)
{
    vel-=arg.vel;
    rot -=arg.rot;
    return *this;
}

Twist& Twist::operator+=(const Twist& arg)
{
    vel+=arg.vel;
    rot +=arg.rot;
    return *this;
}

double& Twist::operator()(int i)
{
    // assert((0<=i)&&(i<6)); done by underlying routines
    if (i<3) 
        return vel(i);
    else
        return rot(i-3);
}

double Twist::operator()(int i) const
{
    // assert((0<=i)&&(i<6)); done by underlying routines
    if (i<3) 
        return vel(i);
    else
        return rot(i-3);
}


Twist operator*(const Twist& lhs,double rhs)
{
    return Twist(lhs.vel*rhs,lhs.rot*rhs);
}

Twist operator*(double lhs,const Twist& rhs)
{
    return Twist(lhs*rhs.vel,lhs*rhs.rot);
}

Twist operator/(const Twist& lhs,double rhs)
{
    return Twist(lhs.vel/rhs,lhs.rot/rhs);
}

// addition of Twist's
Twist operator+(const Twist& lhs,const Twist& rhs)
{
    return Twist(lhs.vel+rhs.vel,lhs.rot+rhs.rot);
}

Twist operator-(const Twist& lhs,const Twist& rhs)
{
    return Twist(lhs.vel-rhs.vel,lhs.rot-rhs.rot);
}

// unary -
Twist operator-(const Twist& arg) 
{
    return Twist(-arg.vel,-arg.rot);
}

IMETHOD double dot(const Twist& lhs,const Wrench& rhs) {
    return dot(lhs.vel,rhs.force)+dot(lhs.rot,rhs.torque);
}

IMETHOD bool Equal(const Twist& a,const Twist& b,double eps) {
        return (Equal(a.rot,b.rot,eps)&&
                Equal(a.vel,b.vel,eps)  );
}

IMETHOD bool operator==(const Twist& a,const Twist& b) {
#ifdef OROPKG_GEOMETRY_USE_EQUAL
    return Equal(a,b);
#else
        return (a.rot==b.rot &&
                a.vel==b.vel  );
#endif
}

IMETHOD bool operator!=(const Twist& a,const Twist& b) {
	return !operator==(a,b);
}

IMETHOD void SetToZero(Twist& v) {
    SetToZero(v.rot);
    SetToZero(v.vel);
}

