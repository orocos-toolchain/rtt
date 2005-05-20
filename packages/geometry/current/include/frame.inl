
Wrench Frame::operator * (const Wrench& arg) const
// Complexity : 24M+18A
{
    Wrench tmp;
    tmp.force  = M*arg.force;
    tmp.torque = M*arg.torque + p*tmp.force;
    return tmp;
}

Wrench Frame::Inverse(const Wrench& arg) const
{
    Wrench tmp;
    tmp.force =  M.Inverse(arg.force);
    tmp.torque = M.Inverse(arg.torque-p*arg.force);
    return tmp;
}



Twist Frame::operator * (const Twist& arg) const
// Complexity : 24M+18A
{
    Twist tmp;
    tmp.rot = M*arg.rot;
    tmp.vel = M*arg.vel+p*tmp.rot;
    return tmp;
}
Twist Frame::Inverse(const Twist& arg) const
{
    Twist tmp;
    tmp.rot =  M.Inverse(arg.rot);
    tmp.vel = M.Inverse(arg.vel-p*arg.rot);
    return tmp;
}

Frame::Frame(const Rotation & R)
{
    M=R;
    p=Vector::Zero();
}

Frame::Frame(const Vector & V)
{
    M = Rotation::Identity();
    p = V;
}

Frame::Frame(const Rotation & R, const Vector & V)
{
    M = R;
    p = V;
}

 Frame operator *(const Frame& lhs,const Frame& rhs)
// Complexity : 36M+36A
{
    return Frame(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}

Vector Frame::operator *(const Vector & arg) const
{
    return M*arg+p;
}

Vector Frame::Inverse(const Vector& arg) const
{
    return M.Inverse(arg-p);
}

Frame Frame::Inverse() const
{
    return Frame(M.Inverse(),-M.Inverse(p));
}


Frame& Frame::operator =(const Frame & arg)
{ 
    M = arg.M;
    p = arg.p;
    return *this;
}

Frame::Frame(const Frame & arg) :
    p(arg.p),M(arg.M)
{}


void Frame::Integrate(const Twist& t_this,double samplefrequency)
{
    double n = t_this.rot.Norm()/samplefrequency;
    if (n<epsilon) {
        p += M*(t_this.vel/samplefrequency);
    } else {
        (*this) = (*this) * 
            Frame ( Rotation::Rot( t_this.rot, n ),
                    t_this.vel/samplefrequency
                );
    }
}

double Frame::operator()(int i,int j) {
    FRAMES_CHECKI((0<=i)&&(i<=3)&&(0<=j)&&(j<=3));
    if (i==3) {
        if (j==3)
            return 1;
        else
            return 0;
    } else {
        if (j==3) 
            return p(i);
        else
            return M(i,j);

    }
}

double Frame::operator()(int i,int j) const {
    FRAMES_CHECKI((0<=i)&&(i<=3)&&(0<=j)&&(j<=3));
        if (i==3) {
        if (j==3)
            return 1;
        else
            return 0;
    } else {
        if (j==3) 
            return p(i);
        else
            return M(i,j);

    }
}


Frame Frame::Identity() {
    return Frame(Rotation::Identity(),Vector::Zero());
}

IMETHOD bool Equal(const Frame& a,const Frame& b,double eps) {
        return (Equal(a.p,b.p,eps)&&
                Equal(a.M,b.M,eps)   );
}

IMETHOD bool operator==(const Frame& a,const Frame& b ) {
#ifdef OROPKG_GEOMETRY_USE_EQUAL
    return Equal(a,b);
#else
        return (a.p == b.p &&
                a.M == b.M );
#endif
}

IMETHOD bool operator!=(const Frame& a,const Frame& b) {
	return !operator==(a,b);
}


/**
 * axis_a_b is a rotation vector, its norm is a rotation angle
 * axis_a_b rotates the a frame towards the b frame.
 * This routine returns the rotation matrix R_a_b
 */
IMETHOD Rotation Rot(const Vector& axis_a_b) {
    // The formula is 
    // V.(V.tr) + st*[V x] + ct*(I-V.(V.tr))
    // can be found by multiplying it with an arbitrary vector p
    // and noting that this vector is rotated.
 Vector rotvec = axis_a_b;
 double angle = rotvec.Normalize(1E-10);
    double ct = ::cos(angle);
    double st = ::sin(angle);
    double vt = 1-ct;
    return Rotation(
        ct            +  vt*rotvec(0)*rotvec(0), 
        -rotvec(2)*st +  vt*rotvec(0)*rotvec(1), 
        rotvec(1)*st  +  vt*rotvec(0)*rotvec(2),
        rotvec(2)*st  +  vt*rotvec(1)*rotvec(0),
        ct            +  vt*rotvec(1)*rotvec(1),
        -rotvec(0)*st +  vt*rotvec(1)*rotvec(2),
        -rotvec(1)*st +  vt*rotvec(2)*rotvec(0),
        rotvec(0)*st  +  vt*rotvec(2)*rotvec(1),
        ct            +  vt*rotvec(2)*rotvec(2)
        );
    }

IMETHOD Vector diff(const Vector& V_a_p1,const Vector& V_a_p2,double dt=1) {
 return (V_a_p2 - V_a_p1)/dt;
}

IMETHOD Vector diff(const Rotation& R_a_b1,const Rotation& R_a_b2,double dt=1) {
 Rotation R_b1_b2(R_a_b1.Inverse()*R_a_b2);
 return R_a_b1 * R_b1_b2.GetRot() / dt;
}

IMETHOD Twist diff(const Frame& F_a_b1,const Frame& F_a_b2,double dt=1) {
 return Twist(
   diff(F_a_b1.p,F_a_b2.p,dt),
   diff(F_a_b1.M,F_a_b2.M,dt)
   );
}

IMETHOD Twist diff(const Twist& T_a_p1,const Twist& T_a_p2,double dt=1) {
 return Twist(diff(T_a_p1.vel,T_a_p2.vel,dt),diff(T_a_p1.rot,T_a_p2.rot,dt));
}

IMETHOD Wrench diff(const Wrench& W_a_p1,const Wrench& W_a_p2,double dt=1) {
 return Wrench(
   diff(W_a_p1.force, W_a_p2.force,dt),
   diff(W_a_p1.torque,W_a_p2.torque,dt)
   );
}

IMETHOD Vector addDelta(const Vector& V_a_p,const Vector&V_a_delta,double dt=1) {
 return V_a_p+V_a_delta*dt;
}

IMETHOD Rotation addDelta(const Rotation& R_a_b,const Vector&V_a_delta,double dt=1)
{
 return R_a_b*Rot(R_a_b.Inverse(V_a_delta)*dt);
}

IMETHOD Frame addDelta(const Frame& F_a_b,const Twist& T_a_delta,double dt=1) {
 return Frame(
   addDelta(F_a_b.M,T_a_delta.rot,dt),
   addDelta(F_a_b.p,T_a_delta.vel,dt)
     );
}

IMETHOD Twist addDelta(const Twist& T_a_p,const Twist&T_a_delta,double dt=1) {
 return Twist(addDelta(T_a_p.vel,T_a_delta.vel,dt),addDelta(T_a_p.rot,T_a_delta.rot,dt));
}

IMETHOD Wrench addDelta(const Wrench& W_a_p,const Wrench&W_a_delta,double dt=1) {
 return
 Wrench(addDelta(W_a_p.force,W_a_delta.force,dt),addDelta(W_a_p.torque,W_a_delta.torque,dt));
}
