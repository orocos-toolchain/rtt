/*****************************************************************************
 * \file  
 *      provides inline functions of rframes.h
 *       
 *  \author 
 *      Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *      ORO_Geometry V0.2
 *
 *  \par History
 *      - $log$
 *
 *  \par Release
 *      $Id: rframes.inl,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/


// Methods and operators related to RRFrame
// They all delegate most of the work to RRRotation and RRVector
RFrame& RFrame::operator = (const RFrame& arg) {
    M=arg.M;
    p=arg.p;
    return *this;
}

RFrame RFrame::Identity() {
    return RFrame(RRotation::Identity(),RVector::Zero());
}


RFrame operator *(const RFrame& lhs,const RFrame& rhs)
{
    return RFrame(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}
RFrame operator *(const RFrame& lhs,const Frame& rhs)
{
    return RFrame(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}
RFrame operator *(const Frame& lhs,const RFrame& rhs)
{
    return RFrame(lhs.M*rhs.M , lhs.M*rhs.p+lhs.p );
}

RVector RFrame::operator *(const RVector & arg) const
{
    return M*arg+p;
}
RVector RFrame::operator *(const Vector & arg) const
{
    return M*arg+p;
}

RVector RFrame::Inverse(const RVector& arg) const
{
    return M.Inverse(arg-p);
}

RVector RFrame::Inverse(const Vector& arg) const
{
    return M.Inverse(arg-p);
}

RFrame RFrame::Inverse() const
{
    return RFrame(M.Inverse(),-M.Inverse(p));
}

RFrame& RFrame::operator = (const Frame& arg) {
    M = arg.M;
    p = arg.p;
    return *this;
}
bool Equal(const RFrame& r1,const RFrame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}
bool Equal(const Frame& r1,const RFrame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}
bool Equal(const RFrame& r1,const Frame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}

Frame RFrame::GetFrame() const {
    return Frame(M.R,p.p);
}

Twist RFrame::GetTwist() const {
    return Twist(p.v,M.w);
}

















RRotation operator* (const RRotation& r1,const RRotation& r2) {
    return RRotation( r1.R*r2.R, r1.w + r1.R*r2.w );
}

RRotation operator* (const Rotation& r1,const RRotation& r2) {
    return RRotation( r1*r2.R, r1*r2.w );
}

RRotation operator* (const RRotation& r1,const Rotation& r2) {
    return RRotation( r1.R*r2, r1.w );
}

RRotation& RRotation::operator = (const RRotation& arg) {
        R=arg.R;
        w=arg.w;
        return *this;
    }
RRotation& RRotation::operator = (const Rotation& arg) {
    R=arg;
    w=Vector::Zero();
    return *this;
}

RRotation RRotation::Identity() {
    return RRotation(Rotation::Identity(),Vector::Zero());
}

RRotation RRotation::Inverse() const {
    return RRotation(R.Inverse(),-R.Inverse(w));
}

RVector RRotation::Inverse(const RVector& arg) const {
    Vector tmp=R.Inverse(arg.p);
    return RVector(tmp,
                    R.Inverse(arg.v-w*arg.p)
                    );
}

RVector RRotation::Inverse(const Vector& arg) const {
    Vector tmp=R.Inverse(arg);
    return RVector(tmp,
                    R.Inverse(-w*arg)
                    );
}


RVector RRotation::operator*(const RVector& arg) const {
    Vector tmp=R*arg.p;
    return RVector(tmp,w*tmp+R*arg.v);
}

RVector RRotation::operator*(const Vector& arg) const {
    Vector tmp=R*arg;
    return RVector(tmp,w*tmp);
}


// = Rotations
// The Rot... static functions give the value of the appropriate rotation matrix back.
// The DoRot... functions apply a rotation R to *this,such that *this = *this * R.

void RRotation::DoRotX(const RDouble& angle) {
    w+=R*Vector(angle.grad,0,0);
    R.DoRotX(angle.t);
}
RRotation RRotation::RotX(const RDouble& angle) {
    return RRotation(Rotation::RotX(angle.t),Vector(angle.grad,0,0));
}

void RRotation::DoRotY(const RDouble& angle) {
    w+=R*Vector(0,angle.grad,0);
    R.DoRotY(angle.t);
}
RRotation RRotation::RotY(const RDouble& angle) {
    return RRotation(Rotation::RotX(angle.t),Vector(0,angle.grad,0));
}

void RRotation::DoRotZ(const RDouble& angle) {
    w+=R*Vector(0,0,angle.grad);
    R.DoRotZ(angle.t);
}
RRotation RRotation::RotZ(const RDouble& angle) {
    return RRotation(Rotation::RotZ(angle.t),Vector(0,0,angle.grad));
}


RRotation RRotation::Rot(const Vector& rotvec,const RDouble& angle) 
// rotvec has arbitrary norm
// rotation around a constant vector !
{
    Vector v(rotvec);
	v.Normalize();
    return RRotation(Rotation::Rot2(v,angle.t),v*angle.grad);
}

RRotation RRotation::Rot2(const Vector& rotvec,const RDouble& angle) 
    // rotvec is normalized.
{
    return RRotation(Rotation::Rot2(rotvec,angle.t),rotvec*angle.grad);
}


RVector operator + (const RVector& r1,const RVector& r2) {
    return RVector(r1.p+r2.p,r1.v+r2.v);
}

RVector operator - (const RVector& r1,const RVector& r2) {
    return RVector(r1.p-r2.p,r1.v-r2.v);
}

RVector operator + (const RVector& r1,const Vector& r2) {
    return RVector(r1.p+r2,r1.v);
}

RVector operator - (const RVector& r1,const Vector& r2) {
    return RVector(r1.p-r2,r1.v);
}

RVector operator + (const Vector& r1,const RVector& r2) {
    return RVector(r1+r2.p,r2.v);
}

RVector operator - (const Vector& r1,const RVector& r2) {
    return RVector(r1-r2.p,-r2.v);
}

// unary -
RVector operator - (const RVector& r) {
    return RVector(-r.p,-r.v);
}

// cross prod.
RVector operator * (const RVector& r1,const RVector& r2) {
    return RVector(r1.p*r2.p, r1.p*r2.v+r1.v*r2.p);
}

RVector operator * (const RVector& r1,const Vector& r2) {
    return RVector(r1.p*r2, r1.v*r2);
}

RVector operator * (const Vector& r1,const RVector& r2) {
    return RVector(r1*r2.p, r1*r2.v);
}



// scalar mult.
RVector operator * (double r1,const RVector& r2) {
    return RVector(r1*r2.p, r1*r2.v);
}

RVector operator * (const RVector& r1,double r2) {
    return RVector(r1.p*r2, r1.v*r2);
}



RVector operator * (const RDouble& r1,const RVector& r2) {
    return RVector(r1.t*r2.p, r1.t*r2.v + r1.grad*r2.p);
}

RVector operator * (const RVector& r2,const RDouble& r1) {
    return RVector(r1.t*r2.p, r1.t*r2.v + r1.grad*r2.p);
}

RVector operator / (const RVector& r1,double r2) {
    return RVector(r1.p/r2, r1.v/r2);
}

RVector operator / (const RVector& r2,const RDouble& r1) {
    return RVector(r2.p/r1.t, r2.v/r1.t - r2.p*r1.grad/r1.t/r1.t);
}

RVector operator*(const Rotation& R,const RVector& x) {
    return RVector(R*x.p,R*x.v);
}

RVector& RVector::operator = (const RVector& arg) {
    p=arg.p;
    v=arg.v;
    return *this;
}
RVector& RVector::operator = (const Vector& arg) {
    p=arg;
    v=Vector::Zero();
    return *this;
}
RVector& RVector::operator += (const RVector& arg) {
    p+=arg.p;
    v+=arg.v;
    return *this;
}
RVector& RVector::operator -= (const RVector& arg) {
    p-=arg.p;
    v-=arg.v;
    return *this;
}

RVector RVector::Zero() {
    return RVector(Vector::Zero(),Vector::Zero());
}
void RVector::ReverseSign() {
    p.ReverseSign();
    v.ReverseSign();
}
RDouble RVector::Norm() {
    double n = p.Norm();
    return RDouble(n,dot(p,v)/n);
}

bool Equal(const RVector& r1,const RVector& r2,double eps) {
    return (Equal(r1.p,r2.p,eps) && Equal(r1.v,r2.v,eps));
}
bool Equal(const Vector& r1,const RVector& r2,double eps) {
    return (Equal(r1,r2.p,eps) && Equal(Vector::Zero(),r2.v,eps));
}
bool Equal(const RVector& r1,const Vector& r2,double eps) {
    return (Equal(r1.p,r2,eps) && Equal(r1.v,Vector::Zero(),eps));
}

bool Equal(const RRotation& r1,const RRotation& r2,double eps) {
    return (Equal(r1.w,r2.w,eps) && Equal(r1.R,r2.R,eps));
}
bool Equal(const Rotation& r1,const RRotation& r2,double eps) {
    return (Equal(Vector::Zero(),r2.w,eps) && Equal(r1,r2.R,eps));
}
bool Equal(const RRotation& r1,const Rotation& r2,double eps) {
    return (Equal(r1.w,Vector::Zero(),eps) && Equal(r1.R,r2,eps));
}
bool Equal(const RTwist& a,const RTwist& b,double eps) {
        return (Equal(a.rot,b.rot,eps)&&
                Equal(a.vel,b.vel,eps)  );
}
bool Equal(const Twist& a,const RTwist& b,double eps) {
        return (Equal(a.rot,b.rot,eps)&&
                Equal(a.vel,b.vel,eps)  );
}
bool Equal(const RTwist& a,const Twist& b,double eps) {
        return (Equal(a.rot,b.rot,eps)&&
                Equal(a.vel,b.vel,eps)  );
}



IMETHOD RDouble dot(const RVector& lhs,const RVector& rhs) {
    return RDouble(dot(lhs.p,rhs.p),dot(lhs.p,rhs.v)+dot(lhs.v,rhs.p));
}
IMETHOD RDouble dot(const RVector& lhs,const Vector& rhs) {
    return RDouble(dot(lhs.p,rhs),dot(lhs.v,rhs));
}
IMETHOD RDouble dot(const Vector& lhs,const RVector& rhs) {
    return RDouble(dot(lhs,rhs.p),dot(lhs,rhs.v));
}












RTwist RTwist::Zero()
{
    return RTwist(RVector::Zero(),RVector::Zero());
}


void RTwist::ReverseSign()
{   
    vel.ReverseSign();
    rot.ReverseSign();
}

RTwist RTwist::RefPoint(const RVector& v_base_AB)
     // Changes the reference point of the RTwist.
     // The RVector v_base_AB is expressed in the same base as the RTwist
     // The RVector v_base_AB is a RVector from the old point to
     // the new point.
     // Complexity : 6M+6A
{
    return RTwist(this->vel+this->rot*v_base_AB,this->rot);
}

RTwist& RTwist::operator-=(const RTwist& arg)
{
    vel-=arg.vel;
    rot -=arg.rot;
    return *this;
}

RTwist& RTwist::operator+=(const RTwist& arg)
{
    vel+=arg.vel;
    rot +=arg.rot;
    return *this;
}


RTwist operator*(const RTwist& lhs,double rhs)
{
    return RTwist(lhs.vel*rhs,lhs.rot*rhs);
}

RTwist operator*(double lhs,const RTwist& rhs)
{
    return RTwist(lhs*rhs.vel,lhs*rhs.rot);
}

RTwist operator/(const RTwist& lhs,double rhs)
{
    return RTwist(lhs.vel/rhs,lhs.rot/rhs);
}


RTwist operator*(const RTwist& lhs,const RDouble& rhs)
{
    return RTwist(lhs.vel*rhs,lhs.rot*rhs);
}

RTwist operator*(const RDouble& lhs,const RTwist& rhs)
{
    return RTwist(lhs*rhs.vel,lhs*rhs.rot);
}

RTwist operator/(const RTwist& lhs,const RDouble& rhs)
{
    return RTwist(lhs.vel/rhs,lhs.rot/rhs);
}



// addition of RTwist's
RTwist operator+(const RTwist& lhs,const RTwist& rhs)
{
    return RTwist(lhs.vel+rhs.vel,lhs.rot+rhs.rot);
}

RTwist operator-(const RTwist& lhs,const RTwist& rhs)
{
    return RTwist(lhs.vel-rhs.vel,lhs.rot-rhs.rot);
}

// unary -
RTwist operator-(const RTwist& arg) 
{
    return RTwist(-arg.vel,-arg.rot);
}





RTwist RRotation::Inverse(const RTwist& arg) const
{
    return RTwist(Inverse(arg.vel),Inverse(arg.rot));
}

RTwist RRotation::operator * (const RTwist& arg) const
{
    return RTwist((*this)*arg.vel,(*this)*arg.rot);
}

RTwist RRotation::Inverse(const Twist& arg) const
{
    return RTwist(Inverse(arg.vel),Inverse(arg.rot));
}

RTwist RRotation::operator * (const Twist& arg) const
{
    return RTwist((*this)*arg.vel,(*this)*arg.rot);
}


RTwist RFrame::operator * (const RTwist& arg) const
{
    RTwist tmp;
    tmp.rot = M*arg.rot;
    tmp.vel = M*arg.vel+p*tmp.rot;
    return tmp;
}

RTwist RFrame::operator * (const Twist& arg) const
{
    RTwist tmp;
    tmp.rot = M*arg.rot;
    tmp.vel = M*arg.vel+p*tmp.rot;
    return tmp;
}

RTwist RFrame::Inverse(const RTwist& arg) const
{
    RTwist tmp;
    tmp.rot =  M.Inverse(arg.rot);
    tmp.vel = M.Inverse(arg.vel-p*arg.rot);
    return tmp;
}

RTwist RFrame::Inverse(const Twist& arg) const
{
    RTwist tmp;
    tmp.rot =  M.Inverse(arg.rot);
    tmp.vel = M.Inverse(arg.vel-p*arg.rot);
    return tmp;
}

Twist RTwist::GetTwist() const {
    return Twist(vel.p,rot.p);
}

Twist RTwist::GetTwistDot() const {
    return Twist(vel.v,rot.v);
}
