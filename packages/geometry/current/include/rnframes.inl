/*****************************************************************************
 * \file  
 *      provides inline functions of rnframes.h
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
 *      $Id: rnframes.inl,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/

// Methods and operators related to RNFrame
// They all delegate most of the work to RNRotation and RNVector
RNFrame& RNFrame::operator = (const RNFrame& arg) {
    M=arg.M;
    p=arg.p;
    return *this;
}

RNFrame RNFrame::Identity() {
    return RNFrame(RNRotation::Identity(),RNVector::Zero());
}


RNFrame operator *(const RNFrame& lhs,const RNFrame& rhs)
{
    return RNFrame(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}


RNFrame operator *(const RNFrame& lhs,const Frame& rhs)
{
    return RNFrame(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}

RNFrame operator *(const Frame& lhs,const RNFrame& rhs)
{
    return RNFrame(lhs.M*rhs.M , lhs.M*rhs.p+lhs.p );
}

RNVector RNFrame::operator *(const RNVector & arg) const
{
    return M*arg+p;
}

RNVector RNFrame::operator *(const Vector & arg) const
{
    return M*arg+p;
}

RNVector RNFrame::Inverse(const RNVector& arg) const
{
    return M.Inverse(arg-p);
}

RNVector RNFrame::Inverse(const Vector& arg) const
{
    return M.Inverse(arg-p);
}

RNFrame RNFrame::Inverse() const
{
    return RNFrame(M.Inverse(),-M.Inverse(p));
}

RNFrame& RNFrame::operator = (const Frame& arg) {
    M = arg.M;
    p = arg.p;
    return *this;
}

bool Equal(const RNFrame& r1,const RNFrame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}
bool Equal(const Frame& r1,const RNFrame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}
bool Equal(const RNFrame& r1,const Frame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}

Frame RNFrame::GetFrame() const {
    return Frame(M.R,p.p);
}

Twist RNFrame::GetTwist(int i) const {
    return Twist(p.v[i],M.w[i]);
}


RNRotation operator* (const RNRotation& r1,const RNRotation& r2) {
    RNRotation tmp;
    tmp.R = r1.R*r2.R;
    for (int i=0;i<VSIZE;i++)
        tmp.w[i] = r1.w[i]+r1.R*r2.w[i];
    return tmp;
}

RNRotation operator* (const Rotation& r1,const RNRotation& r2) {
    RNRotation tmp;
    tmp.R = r1*r2.R;
    for (int i=0;i<VSIZE;i++)
        tmp.w[i] = r1*r2.w[i];
    return tmp;
}

RNRotation operator* (const RNRotation& r1,const Rotation& r2) {
    RNRotation tmp;
    tmp.R = r1.R*r2;
    for (int i=0;i<VSIZE;i++)
        tmp.w[i] = r1.w[i];
    return tmp;
}

RNRotation& RNRotation::operator = (const RNRotation& arg) {
        R=arg.R;
        w=arg.w;
        return *this;
    }
RNRotation& RNRotation::operator = (const Rotation& arg) {
    R=arg;
    int i;
    for (i=0;i<VSIZE;i++)
        w[i] = Vector::Zero();
    return *this;
}

RNRotation RNRotation::Identity() {
    return RNRotation(Rotation::Identity(),VectorList::Zero());
}

RNRotation RNRotation::Inverse() const {
    RNRotation tmp;
    tmp.R = R.Inverse();
    int i;
    for (i=0;i<VSIZE;i++)
        tmp.w[i] = -R.Inverse(w[i]);
    return tmp;
}

RNVector RNRotation::Inverse(const RNVector& arg) const {
    RNVector tmp;
    tmp.p = R.Inverse(arg.p);
    for (int i=0;i<VSIZE;i++) {
        tmp.v[i] = R.Inverse(arg.v[i]-w[i]*arg.p);
    }
    return tmp;
}

RNVector RNRotation::Inverse(const Vector& arg) const {
    RNVector tmp;
    tmp.p = R.Inverse(arg);
    for (int i=0;i<VSIZE;i++) {
        tmp.v[i] = R.Inverse(w[i]*arg);
    }
    return tmp;
}

RNVector RNRotation::operator*(const RNVector& arg) const {
    RNVector tmp;
    tmp.p = R*arg.p;
    for (int i=0;i<VSIZE;i++) {
        tmp.v[i] = w[i]*tmp.p + R*arg.v[i];
    }
    return tmp;
}


RNVector RNRotation::operator*(const Vector& arg) const {
    RNVector tmp;
    tmp.p = R*arg;
    for (int i=0;i<VSIZE;i++) {
        tmp.v[i] = w[i]*tmp.p;
    }
    return tmp;
}

RNTwist RNRotation::operator * (const RNTwist& arg) const
     // Transformation of the base to which the twist is expressed.
     // look at Frame*Twist for a transformation that also transforms
     // the velocity reference point.
     // Complexity : 18M+12A
{
    return RNTwist((*this)*arg.vel,(*this)*arg.rot);

}



// = Rotations
// The Rot... static functions give the value of the appropriate rotation matrix back.
// The DoRot... functions apply a rotation R to *this,such that *this = *this * R.

void RNRotation::DoRotX(const RNDouble& angle) {
    for (int i=0;i<VSIZE;i++) {
        w[i] += R*Vector(angle.grad[i],0,0);
    }
    R.DoRotX(angle.t);
}
void RNRotation::DoRotY(const RNDouble& angle) {
    for (int i=0;i<VSIZE;i++) {
        w[i] += R*Vector(0,angle.grad[i],0);
    }
    R.DoRotY(angle.t);
}

void RNRotation::DoRotZ(const RNDouble& angle) {
    for (int i=0;i<VSIZE;i++) {
        w[i] += R*Vector(0,0,angle.grad[i]);
    }
    R.DoRotZ(angle.t);
}


RNRotation RNRotation::RotX(const RNDouble& angle) {
    RNRotation V;
    V.R.DoRotX(angle.t);
    for (int i=0;i<VSIZE;i++) {
        V.w[i] += V.R*Vector(angle.grad[i],0,0);
    }
    return V;
}

RNRotation RNRotation::RotY(const RNDouble& angle) {
    RNRotation V;
    V.R.DoRotY(angle.t);
    for (int i=0;i<VSIZE;i++) {
        V.w[i] += V.R*Vector(0,angle.grad[i],0);
    }
    return V;
}

RNRotation RNRotation::RotZ(const RNDouble& angle) {
    RNRotation V;
    V.R.DoRotZ(angle.t);
    for (int i=0;i<VSIZE;i++) {
        V.w[i] += V.R*Vector(0,0,angle.grad[i]);
    }
    return V;
}


RNRotation RNRotation::Rot(const Vector& rotvec,const RNDouble& angle) 
// rotvec has arbitrary norm
// rotation around a constant vector !
{
    RNRotation tmp;
    Vector v(rotvec);
	v.Normalize();
    tmp.R = Rotation::Rot2(v,angle.t);
    for (int i=0;i<VSIZE;i++) {
        tmp.w[i] = v*angle.grad[i];
    }
    return tmp;
}

RNRotation RNRotation::Rot2(const Vector& rotvec,const RNDouble& angle) 
    // rotvec is normalized.
{
    RNRotation tmp;
    tmp.R = Rotation::Rot2(rotvec,angle.t);
    for (int i=0;i<VSIZE;i++) {
        tmp.w[i] = rotvec*angle.grad[i];
    }
    return tmp;
}















RNVector operator + (const RNVector& r1,const RNVector& r2) {
    return RNVector(r1.p+r2.p,r1.v+r2.v);
}

RNVector operator - (const RNVector& r1,const RNVector& r2) {
    return RNVector(r1.p-r2.p,r1.v-r2.v);
}

RNVector operator + (const RNVector& r1,const Vector& r2) {
    return RNVector(r1.p+r2,r1.v);
}

RNVector operator - (const RNVector& r1,const Vector& r2) {
    return RNVector(r1.p-r2,r1.v);
}

RNVector operator + (const Vector& r1,const RNVector& r2) {
    return RNVector(r1+r2.p,r2.v);
}

RNVector operator - (const Vector& r1,const RNVector& r2) {
    return RNVector(r1-r2.p,-r2.v);
}

// unary -
RNVector operator - (const RNVector& r) {
    return RNVector(-r.p,-r.v);
}

// cross prod.
RNVector operator * (const RNVector& r1,const RNVector& r2) {
    return RNVector(r1.p*r2.p,r1.p*r2.v+r1.v*r2.p);
}

RNVector operator * (const RNVector& r1,const Vector& r2) {
    return RNVector(r1.p*r2,r1.v*r2);
}

RNVector operator * (const Vector& r1,const RNVector& r2) {
    return RNVector(r1*r2.p,r1*r2.v);
}


// scalar mult.
RNVector operator * (double r1,const RNVector& r2) {
    RNVector tmp;
    tmp.p = r1*r2.p;
    int i;
    for (i=0;i<VSIZE;i++) {
        tmp.v[i] = r1*r2.v[i];
    }
    return tmp;
}

RNVector operator * (const RNVector& r1,double r2) {
    RNVector tmp;
    tmp.p = r1.p*r2;
    int i;
    for (i=0;i<VSIZE;i++) {
        tmp.v[i] = r1.v[i]*r2;
    }
    return tmp;
}

RNVector operator / (const RNVector& r1,double r2) {
    RNVector tmp;
    tmp.p = r1.p*r2;
    int i;
    for (i=0;i<VSIZE;i++) {
        tmp.v[i] = r1.v[i]/r2;
    }
    return tmp;
}



RNVector operator * (const RNDouble& r1,const RNVector& r2) {
    RNVector v;
    v.p = r1.t*r2.p;
    for (int i=0;i<VSIZE;i++) {
        v.v[i] = r1.t*r2.v[i] + r1.grad[i]*r2.p;
    }
    return v;
}

RNVector operator * (const RNVector& r2,const RNDouble& r1) {
    RNVector v;
    v.p = r1.t*r2.p;
    for (int i=0;i<VSIZE;i++) {
        v.v[i] = r1.t*r2.v[i] + r1.grad[i]*r2.p;
    }
    return v;
}

RNVector operator*(const Rotation& R,const RNVector& x) {
    RNVector tmp;
    tmp.p=R*x.p;
    for (int i=0;i<VSIZE;i++) {
        tmp.v[i]=R*x.v[i];
    }
    return tmp;
}


RNVector& RNVector::operator = (const RNVector& arg) {
    p=arg.p;
    v=arg.v;
    return *this;
}

RNVector& RNVector::operator = (const Vector& arg) {
    p=arg;
    int i;
    for (i=0;i<VSIZE;i++) {
        v[i] = Vector::Zero();
    }
    return *this;
}

RNVector& RNVector::operator += (const RNVector& arg) {
    p+=arg.p;
    v+=arg.v;
    return *this;
}

RNVector& RNVector::operator -= (const RNVector& arg) {
    p-=arg.p;
    v-=arg.v;
    return *this;
}

RNVector RNVector::Zero() {
    RNVector tmp;
    tmp.p = Vector::Zero();
    int i;
    for (i=0;i<VSIZE;i++) {
        tmp.v[i] = Vector::Zero();
    }
    return tmp;
}

void RNVector::ReverseSign() {
    p.ReverseSign();
    int i;
    for (i=0;i<VSIZE;i++) {
        v[i].ReverseSign();
    }
}

RNDouble RNVector::Norm() {
    RNDouble d;
    d.t = p.Norm();
    for (int i=0;i<VSIZE;i++) {
        d.grad[i] = dot(p,v[i])/d.t;    
    }
    return d;
}


bool Equal(const RNVector& r1,const RNVector& r2,double eps) {
    return (Equal(r1.p,r2.p,eps)&&Equal(r1.v,r2.v,eps));
}
bool Equal(const Vector& r1,const RNVector& r2,double eps) {
    int i;
    if (!Equal(r1,r2.p,eps)) return false;
    for (i=0;i<VSIZE;i++) {
        if (!Equal(Vector::Zero(),r2.v[i],eps)) return false;
    }
    return true;
}
bool Equal(const RNVector& r1,const Vector& r2,double eps) {
        int i;
    if (!Equal(r1.p,r2,eps)) return false;
    for (i=0;i<VSIZE;i++) {
        if (!Equal(r1.v[i],Vector::Zero(),eps)) return false;
    }
    return true;
}









bool Equal(const RNRotation& r1,const RNRotation& r2,double eps) {
    return (Equal(r1.w,r2.w,eps) && Equal(r1.R,r2.R,eps));
}
bool Equal(const Rotation& r1,const RNRotation& r2,double eps) {
    return (Equal(Vector::Zero(),r2.w,eps) && Equal(r1,r2.R,eps));
}
bool Equal(const RNRotation& r1,const Rotation& r2,double eps) {
    return (Equal(r1.w,Vector::Zero(),eps) && Equal(r1.R,r2,eps));
}






IMETHOD RNDouble dot(const RNVector& lhs,const RNVector& rhs) {
    RNDouble d;
    int i;
    d.t = dot(lhs.p,rhs.p);
    for (i=0;i<VSIZE;i++) {
        d.grad[i] = dot(lhs.p,rhs.v[i])+dot(lhs.v[i],rhs.p);
    }
    return d;
}
IMETHOD RNDouble dot(const RNVector& lhs,const Vector& rhs) {
    RNDouble d;
    int i;
    d.t = dot(lhs.p,rhs);
    for (i=0;i<VSIZE;i++) {
        d.grad[i] = dot(lhs.v[i],rhs);
    }
    return d;

}
IMETHOD RNDouble dot(const Vector& lhs,const RNVector& rhs) {
    RNDouble d;
    int i;
    d.t = dot(lhs,rhs.p);
    for (i=0;i<VSIZE;i++) {
        d.grad[i] = dot(lhs,rhs.v[i]);
    }
    return d;
}


















RNTwist RNRotation::Inverse(const RNTwist& arg) const
{
    return RNTwist(Inverse(arg.vel),Inverse(arg.rot));
}


RNTwist RNFrame::operator * (const Twist& arg) const
// Complexity : 24M+18A
{
    RNTwist tmp;
    tmp.rot = M*arg.rot;
    tmp.vel = M*arg.vel+p*tmp.rot;
    return tmp;
}

/*
Twist Frame::Inverse(const Twist& arg) const
{
    Twist tmp;
    tmp.rot =  M.Inverse(arg.rot);
    tmp.vel = M.Inverse(arg.vel-p*arg.rot);
    return tmp;
}
*/
RNTwist RNTwist::Zero()
{
    return RNTwist(RNVector::Zero(),RNVector::Zero());
}


void RNTwist::ReverseSign()
{   
    vel.ReverseSign();
    rot.ReverseSign();
}

RNTwist RNTwist::RefPoint(const Vector& v_base_AB)
     // Changes the reference point of the twist.
     // The vector v_base_AB is expressed in the same base as the twist
     // The vector v_base_AB is a vector from the old point to
     // the new point.
     // Complexity : 6M+6A
{
    return RNTwist(this->vel+this->rot*v_base_AB , this->rot);
}

RNTwist RNTwist::RefPoint(const RNVector& v_base_AB)
     // Changes the reference point of the twist.
     // The vector v_base_AB is expressed in the same base as the twist
     // The vector v_base_AB is a vector from the old point to
     // the new point.
     // Complexity : 6M+6A
{
    return RNTwist(this->vel+this->rot*v_base_AB,this->rot);
}

Twist RNTwist::GetTwist() {
    return Twist(vel.p,rot.p);
}

RNTwist& RNTwist::operator-=(const RNTwist& arg)
{
    vel-=arg.vel;
    rot -=arg.rot;
    return *this;
}

RNTwist& RNTwist::operator+=(const RNTwist& arg)
{
    vel+=arg.vel;
    rot +=arg.rot;
    return *this;
}

RNTwist operator*(const RNTwist& lhs,double rhs)
{
    return RNTwist(lhs.vel*rhs,lhs.rot*rhs);
}

RNTwist operator*(double lhs,const RNTwist& rhs)
{
    return RNTwist(lhs*rhs.vel,lhs*rhs.rot);
}

RNTwist operator/(const RNTwist& lhs,double rhs)
{
    return RNTwist(lhs.vel/rhs,lhs.rot/rhs);
}

// addition of Twist's
RNTwist operator+(const RNTwist& lhs,const RNTwist& rhs)
{
    return RNTwist(lhs.vel+rhs.vel,lhs.rot+rhs.rot);
}

RNTwist operator-(const RNTwist& lhs,const RNTwist& rhs)
{
    return RNTwist(lhs.vel-rhs.vel,lhs.rot-rhs.rot);
}

// unary -
RNTwist operator-(const RNTwist& arg) 
{
    return RNTwist(-arg.vel,-arg.rot);

}


RNTwist RNRotation::operator * (const Twist& arg) const
     // Transformation of the base to which the twist is expressed.
     // look at Frame*Twist for a transformation that also transforms
     // the velocity reference point.
     // Complexity : 18M+12A
{
    return RNTwist((*this)*arg.vel,(*this)*arg.rot);
}


/*
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
*/

bool Equal(const RNTwist& a,const RNTwist& b,double eps) {
        return (  Equal(a.rot,b.rot,eps)  &&
                  Equal(a.vel,b.vel,eps)      );
}

void SetToZero(RNVector& rnv) {
    for (int i=0;i<VSIZE;i++) {
        SetToZero(rnv.v[i]);
    }
    SetToZero(rnv.p);
}

void SetToZero(RNTwist& v) {
    SetToZero(v.rot);
    SetToZero(v.vel);
}




