/*****************************************************************************
 * \file  
 *      provides inline functions of rrframes.h
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
 *      $Id: rrframes.inl,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/




/////////////////// RRVector /////////////////////////////////////

RRVector operator + (const RRVector& r1,const RRVector& r2) {
    return RRVector(r1.p+r2.p,r1.v+r2.v,r1.dv+r2.dv);
}

RRVector operator - (const RRVector& r1,const RRVector& r2) {
    return RRVector(r1.p-r2.p, r1.v-r2.v, r1.dv-r2.dv);
}
RRVector operator + (const Vector& r1,const RRVector& r2) {
    return RRVector(r1+r2.p,r2.v,r2.dv);
}

RRVector operator - (const Vector& r1,const RRVector& r2) {
    return RRVector(r1-r2.p, -r2.v, -r2.dv);
}
RRVector operator + (const RRVector& r1,const Vector& r2) {
    return RRVector(r1.p+r2,r1.v,r1.dv);
}

RRVector operator - (const RRVector& r1,const Vector& r2) {
    return RRVector(r1.p-r2, r1.v, r1.dv);
}

// unary -
RRVector operator - (const RRVector& r) {
    return RRVector(-r.p,-r.v,-r.dv);
}

// cross prod.
RRVector operator * (const RRVector& r1,const RRVector& r2) {
    return RRVector(r1.p*r2.p, 
                    r1.p*r2.v+r1.v*r2.p,
                    r1.dv*r2.p+2*r1.v*r2.v+r1.p*r2.dv
                    );
}

RRVector operator * (const RRVector& r1,const Vector& r2) {
    return RRVector(r1.p*r2, r1.v*r2, r1.dv*r2 );
}

RRVector operator * (const Vector& r1,const RRVector& r2) {
    return RRVector(r1*r2.p, r1*r2.v, r1*r2.dv );
}



// scalar mult.
RRVector operator * (double r1,const RRVector& r2) {
    return RRVector(r1*r2.p, r1*r2.v, r1*r2.dv );
}

RRVector operator * (const RRVector& r1,double r2) {
    return RRVector(r1.p*r2, r1.v*r2, r1.dv*r2 );
}

RRVector operator * (const RRDouble& r1,const RRVector& r2) {
    return RRVector(r1.t*r2.p, 
                   r1.t*r2.v  + r1.d*r2.p,
                   r1.t*r2.dv + 2*r1.d*r2.v + r1.dd*r2.p
                   );
}

RRVector operator * (const RRVector& r2,const RRDouble& r1) {
    return RRVector(r1.t*r2.p, 
               r1.t*r2.v  + r1.d*r2.p,
               r1.t*r2.dv + 2*r1.d*r2.v + r1.dd*r2.p
               );
}

RRVector& RRVector::operator = (const RRVector& arg) {
    p=arg.p;
    v=arg.v;
    dv=arg.dv;
    return *this;
}

RRVector& RRVector::operator = (const Vector& arg) {
    p=arg;
    v=Vector::Zero();
    dv=Vector::Zero();
    return *this;
}

RRVector& RRVector::operator += (const RRVector& arg) {
    p+=arg.p;
    v+=arg.v;
    dv+= arg.dv;
    return *this;
}
RRVector& RRVector::operator -= (const RRVector& arg) {
    p-=arg.p;
    v-=arg.v;
    dv-=arg.dv;
    return *this;
}

RRVector RRVector::Zero() {
    return RRVector(Vector::Zero(),Vector::Zero(),Vector::Zero());
}

void RRVector::ReverseSign() {
    p.ReverseSign();
    v.ReverseSign();
    dv.ReverseSign();
}

RRDouble RRVector::Norm() {
    RRDouble res;
    res.t  = p.Norm();
    res.d  = dot(p,v)/res.t;
    res.dd = (dot(p,dv)+dot(v,v)-res.d*res.d)/res.t;
    return res;
}

RRDouble dot(const RRVector& lhs,const RRVector& rhs) {
    return RRDouble( dot(lhs.p,rhs.p),
                    dot(lhs.p,rhs.v)+dot(lhs.v,rhs.p),
                    dot(lhs.p,rhs.dv)+2*dot(lhs.v,rhs.v)+dot(lhs.dv,rhs.p)
                    );
}

RRDouble dot(const RRVector& lhs,const Vector& rhs) {
    return RRDouble( dot(lhs.p,rhs),
                     dot(lhs.v,rhs),
                     dot(lhs.dv,rhs)
                    );
}

RRDouble dot(const Vector& lhs,const RRVector& rhs) {
    return RRDouble( dot(lhs,rhs.p),
                    dot(lhs,rhs.v),
                    dot(lhs,rhs.dv)
                    );
}


bool Equal(const RRVector& r1,const RRVector& r2,double eps) {
    return (Equal(r1.p,r2.p,eps) 
         && Equal(r1.v,r2.v,eps)
         && Equal(r1.dv,r2.dv,eps)
         );
}

bool Equal(const Vector& r1,const RRVector& r2,double eps) {
    return (Equal(r1,r2.p,eps) 
         && Equal(Vector::Zero(),r2.v,eps)
         && Equal(Vector::Zero(),r2.dv,eps)
         );
}

bool Equal(const RRVector& r1,const Vector& r2,double eps) {
    return (Equal(r1.p,r2,eps) 
         && Equal(r1.v,Vector::Zero(),eps)
         && Equal(r1.dv,Vector::Zero(),eps)
         );
}

RRVector operator / (const RRVector& r1,double r2) {
    return r1*(1.0/r2);
}

RRVector operator / (const RRVector& r2,const RRDouble& r1) {
    return r2*(1.0/r1);
}



/////////////////// RRRotation /////////////////////////////////////

RRRotation operator* (const RRRotation& r1,const RRRotation& r2) {
    return RRRotation( r1.R  * r2.R, 
                      r1.w  + r1.R*r2.w,  
                      r1.dw + r1.w*(r1.R*r2.w) + r1.R*r2.dw 
                      );
}

RRRotation operator* (const Rotation& r1,const RRRotation& r2) {
    return RRRotation( r1*r2.R, r1*r2.w, r1*r2.dw);
}

RRRotation operator* (const RRRotation& r1,const Rotation& r2) {
    return RRRotation( r1.R*r2, r1.w, r1.dw );
}

RRRotation& RRRotation::operator = (const RRRotation& arg) {
    R=arg.R;
    w=arg.w;
    dw=arg.dw;
    return *this;
}
RRRotation& RRRotation::operator = (const Rotation& arg) {
    R = arg;
    w = Vector::Zero();
    dw = Vector::Zero();
    return *this;
}

RRRotation RRRotation::Identity() {
    return RRRotation(Rotation::Identity(),Vector::Zero(),Vector::Zero());
}

RRRotation RRRotation::Inverse() const {
    return RRRotation(R.Inverse(),-R.Inverse(w),-R.Inverse(dw));
}

RRVector RRRotation::Inverse(const RRVector& arg) const {
    RRVector tmp;
    tmp.p  = R.Inverse(arg.p);
    tmp.v  = R.Inverse(arg.v - w * arg.p);
    tmp.dv = R.Inverse(arg.dv - dw*arg.p - w*(arg.v+R*tmp.v));
    return tmp;
}

RRVector RRRotation::Inverse(const Vector& arg) const {
    RRVector tmp;
    tmp.p  = R.Inverse(arg);
    tmp.v  = R.Inverse(-w*arg);
    tmp.dv = R.Inverse(-dw*arg - w*(R*tmp.v));
    return tmp;
}


RRVector RRRotation::operator*(const RRVector& arg) const {
    RRVector tmp;
    tmp.p = R*arg.p;
    tmp.dv = R*arg.v;
    tmp.v = w*tmp.p + tmp.dv;
    tmp.dv = dw*tmp.p + w*(tmp.v + tmp.dv) + R*arg.dv;
    return tmp;
}

RRVector operator*(const Rotation& R,const RRVector& x) {
    return RRVector(R*x.p,R*x.v,R*x.dv);
}

RRVector RRRotation::operator*(const Vector& arg) const {
    RRVector tmp;
    tmp.p = R*arg;
    tmp.v = w*tmp.p;
    tmp.dv = dw*tmp.p + w*tmp.v;
    return tmp;
}

/*
        // = Rotations
        // The Rot... static functions give the value of the appropriate rotation matrix back.
        // The DoRot... functions apply a rotation R to *this,such that *this = *this * R.

        void RRotation::DoRotX(const RDouble& angle) {
            w+=R*Vector(angle.grad,0,0);
            R.DoRotX(angle.t);
        }
RRRotation RRRotation::RotX(const RRDouble& angle) {
    return RRRotation(Rotation::RotX(angle.t),
                      Vector(angle.d,0,0),
                      Vector(angle.dd,0,0)
                      );
}

        void RRotation::DoRotY(const RDouble& angle) {
            w+=R*Vector(0,angle.grad,0);
            R.DoRotY(angle.t);
        }
RRRotation RRRotation::RotY(const RRDouble& angle) {
    return RRRotation(
              Rotation::RotX(angle.t),
              Vector(0,angle.d,0),
              Vector(0,angle.dd,0)
            );
}

        void RRotation::DoRotZ(const RDouble& angle) {
            w+=R*Vector(0,0,angle.grad);
            R.DoRotZ(angle.t);
        }
RRRotation RRRotation::RotZ(const RRDouble& angle) {
    return RRRotation(
                Rotation::RotZ(angle.t),
                Vector(0,0,angle.d),
                Vector(0,0,angle.dd)
            );
}


        RRotation RRotation::Rot(const Vector& rotvec,const RDouble& angle) 
        // rotvec has arbitrary norm
        // rotation around a constant vector !
        {
            Vector v = rotvec.Normalize();
            return RRotation(Rotation::Rot2(v,angle.t),v*angle.grad);
        }

        RRotation RRotation::Rot2(const Vector& rotvec,const RDouble& angle) 
            // rotvec is normalized.
        {
            return RRotation(Rotation::Rot2(rotvec,angle.t),rotvec*angle.grad);
        }

*/

bool Equal(const RRRotation& r1,const RRRotation& r2,double eps) {
    return (Equal(r1.w,r2.w,eps) && Equal(r1.R,r2.R,eps) && Equal(r1.dw,r2.dw,eps) );
}
bool Equal(const Rotation& r1,const RRRotation& r2,double eps) {
    return (Equal(Vector::Zero(),r2.w,eps) && Equal(r1,r2.R,eps) && 
            Equal(Vector::Zero(),r2.dw,eps) );
}
bool Equal(const RRRotation& r1,const Rotation& r2,double eps) {
    return (Equal(r1.w,Vector::Zero(),eps) && Equal(r1.R,r2,eps) && 
            Equal(r1.dw,Vector::Zero(),eps) );
}


// Methods and operators related to RRFrame
// They all delegate most of the work to RRRotation and RRVector
RRFrame& RRFrame::operator = (const RRFrame& arg) {
    M=arg.M;
    p=arg.p;
    return *this;
}

RRFrame RRFrame::Identity() {
    return RRFrame(RRRotation::Identity(),RRVector::Zero());
}


RRFrame operator *(const RRFrame& lhs,const RRFrame& rhs)
{
    return RRFrame(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}
RRFrame operator *(const RRFrame& lhs,const Frame& rhs)
{
    return RRFrame(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}
RRFrame operator *(const Frame& lhs,const RRFrame& rhs)
{
    return RRFrame(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}

RRVector RRFrame::operator *(const RRVector & arg) const
{
    return M*arg+p;
}
RRVector RRFrame::operator *(const Vector & arg) const
{
    return M*arg+p;
}

RRVector RRFrame::Inverse(const RRVector& arg) const
{
    return M.Inverse(arg-p);
}

RRVector RRFrame::Inverse(const Vector& arg) const
{
    return M.Inverse(arg-p);
}

RRFrame RRFrame::Inverse() const
{
    return RRFrame(M.Inverse(),-M.Inverse(p));
}

RRFrame& RRFrame::operator =(const Frame & arg)
{ 
    M = arg.M;
    p = arg.p;
    return *this;
}

bool Equal(const RRFrame& r1,const RRFrame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}
bool Equal(const Frame& r1,const RRFrame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}
bool Equal(const RRFrame& r1,const Frame& r2,double eps) {
    return (Equal(r1.M,r2.M,eps) && Equal(r1.p,r2.p,eps));
}


Frame RRFrame::GetFrame() const {
    return Frame(M.R,p.p);
}


Twist RRFrame::GetTwist() const {
    return Twist(p.v,M.w);
}


Twist RRFrame::GetAccTwist() const {
    return Twist(p.dv,M.dw);
}

















RRTwist RRTwist::Zero()
{
    return RRTwist(RRVector::Zero(),RRVector::Zero());
}


void RRTwist::ReverseSign()
{   
    vel.ReverseSign();
    rot.ReverseSign();
}

RRTwist RRTwist::RefPoint(const RRVector& v_base_AB)
     // Changes the reference point of the RRTwist.
     // The RVector v_base_AB is expressed in the same base as the RRTwist
     // The RVector v_base_AB is a RVector from the old point to
     // the new point.
     // Complexity : 6M+6A
{
    return RRTwist(this->vel+this->rot*v_base_AB,this->rot);
}

RRTwist& RRTwist::operator-=(const RRTwist& arg)
{
    vel-=arg.vel;
    rot -=arg.rot;
    return *this;
}

RRTwist& RRTwist::operator+=(const RRTwist& arg)
{
    vel+=arg.vel;
    rot +=arg.rot;
    return *this;
}


RRTwist operator*(const RRTwist& lhs,double rhs)
{
    return RRTwist(lhs.vel*rhs,lhs.rot*rhs);
}

RRTwist operator*(double lhs,const RRTwist& rhs)
{
    return RRTwist(lhs*rhs.vel,lhs*rhs.rot);
}

RRTwist operator/(const RRTwist& lhs,double rhs)
{
    return RRTwist(lhs.vel/rhs,lhs.rot/rhs);
}


RRTwist operator*(const RRTwist& lhs,const RRDouble& rhs)
{
    return RRTwist(lhs.vel*rhs,lhs.rot*rhs);
}

RRTwist operator*(const RRDouble& lhs,const RRTwist& rhs)
{
    return RRTwist(lhs*rhs.vel,lhs*rhs.rot);
}

RRTwist operator/(const RRTwist& lhs,const RRDouble& rhs)
{
    return RRTwist(lhs.vel/rhs,lhs.rot/rhs);
}



// addition of RRTwist's
RRTwist operator+(const RRTwist& lhs,const RRTwist& rhs)
{
    return RRTwist(lhs.vel+rhs.vel,lhs.rot+rhs.rot);
}

RRTwist operator-(const RRTwist& lhs,const RRTwist& rhs)
{
    return RRTwist(lhs.vel-rhs.vel,lhs.rot-rhs.rot);
}

// unary -
RRTwist operator-(const RRTwist& arg) 
{
    return RRTwist(-arg.vel,-arg.rot);
}





RRTwist RRRotation::Inverse(const RRTwist& arg) const
{
    return RRTwist(Inverse(arg.vel),Inverse(arg.rot));
}

RRTwist RRRotation::operator * (const RRTwist& arg) const
{
    return RRTwist((*this)*arg.vel,(*this)*arg.rot);
}

RRTwist RRRotation::Inverse(const Twist& arg) const
{
    return RRTwist(Inverse(arg.vel),Inverse(arg.rot));
}

RRTwist RRRotation::operator * (const Twist& arg) const
{
    return RRTwist((*this)*arg.vel,(*this)*arg.rot);
}


RRTwist RRFrame::operator * (const RRTwist& arg) const
{
    RRTwist tmp;
    tmp.rot = M*arg.rot;
    tmp.vel = M*arg.vel+p*tmp.rot;
    return tmp;
}

RRTwist RRFrame::operator * (const Twist& arg) const
{
    RRTwist tmp;
    tmp.rot = M*arg.rot;
    tmp.vel = M*arg.vel+p*tmp.rot;
    return tmp;
}

RRTwist RRFrame::Inverse(const RRTwist& arg) const
{
    RRTwist tmp;
    tmp.rot =  M.Inverse(arg.rot);
    tmp.vel = M.Inverse(arg.vel-p*arg.rot);
    return tmp;
}

RRTwist RRFrame::Inverse(const Twist& arg) const
{
    RRTwist tmp;
    tmp.rot =  M.Inverse(arg.rot);
    tmp.vel = M.Inverse(arg.vel-p*arg.rot);
    return tmp;
}

Twist RRTwist::GetTwist() const {
    return Twist(vel.p,rot.p);
}

Twist RRTwist::GetTwistDot() const {
    return Twist(vel.v,rot.v);
}

bool Equal(const RRTwist& a,const RRTwist& b,double eps) {
        return (Equal(a.rot,b.rot,eps)&&
                Equal(a.vel,b.vel,eps)  );
}
bool Equal(const Twist& a,const RRTwist& b,double eps) {
        return (Equal(a.rot,b.rot,eps)&&
                Equal(a.vel,b.vel,eps)  );
}
bool Equal(const RRTwist& a,const Twist& b,double eps) {
        return (Equal(a.rot,b.rot,eps)&&
                Equal(a.vel,b.vel,eps)  );
}

