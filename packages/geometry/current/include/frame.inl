
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

IMETHOD Twist FrameDifference(const Frame& goal,const Frame& current) {
    return Twist(
             goal.p-current.p,
             (goal.M*current.M.Inverse()).GetRot()
        );
}
