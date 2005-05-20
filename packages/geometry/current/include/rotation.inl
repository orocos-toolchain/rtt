

Wrench Rotation::Inverse(const Wrench& arg) const
{
    return Wrench(Inverse(arg.force),Inverse(arg.torque));
}

Twist Rotation::Inverse(const Twist& arg) const
{
    return Twist(Inverse(arg.vel),Inverse(arg.rot));
}

double& Rotation::operator()(int i,int j) {
    FRAMES_CHECKI((0<=i)&&(i<=2)&&(0<=j)&&(j<=2));
    return data[i*3+j];
}

double Rotation::operator()(int i,int j) const {
    FRAMES_CHECKI((0<=i)&&(i<=2)&&(0<=j)&&(j<=2));
    return data[i*3+j];
}

Rotation::Rotation( double Xx,double Yx,double Zx,
                            double Xy,double Yy,double Zy,
                            double Xz,double Yz,double Zz)
{
    data[0] = Xx;data[1]=Yx;data[2]=Zx;
    data[3] = Xy;data[4]=Yy;data[5]=Zy;
    data[6] = Xz;data[7]=Yz;data[8]=Zz;
}


Rotation::Rotation(const Vector& x,const Vector& y,const Vector& z) 
{
    data[0] = x.data[0];data[3] = x.data[1];data[6] = x.data[2];
    data[1] = y.data[0];data[4] = y.data[1];data[7] = y.data[2];
    data[2] = z.data[0];data[5] = z.data[1];data[8] = z.data[2];
}

Rotation& Rotation::operator=(const Rotation& arg) {
    int count=9;
    while (count--) data[count] = arg.data[count];
    return *this;
}

Vector Rotation::operator*(const Vector& v) const {
// Complexity : 9M+6A
    return Vector(
     data[0]*v.data[0] + data[1]*v.data[1] + data[2]*v.data[2],
     data[3]*v.data[0] + data[4]*v.data[1] + data[5]*v.data[2],
     data[6]*v.data[0] + data[7]*v.data[1] + data[8]*v.data[2] 
    );
}

Twist Rotation::operator * (const Twist& arg) const
     // Transformation of the base to which the twist is expressed.
     // look at Frame*Twist for a transformation that also transforms
     // the velocity reference point.
     // Complexity : 18M+12A
{
    return Twist((*this)*arg.vel,(*this)*arg.rot);
}

Wrench Rotation::operator * (const Wrench& arg) const
     // Transformation of the base to which the wrench is expressed.
     // look at Frame*Twist for a transformation that also transforms
     // the force reference point.
{
    return Wrench((*this)*arg.force,(*this)*arg.torque);
}

Rotation Rotation::Identity() {
        return Rotation(1,0,0,0,1,0,0,0,1);
}
// *this = *this * ROT(X,angle)
void Rotation::DoRotX(double angle)
{
    double cs = cos(angle);
    double sn = sin(angle);
    double x1,x2,x3;
    x1  = cs* (*this)(0,1) + sn* (*this)(0,2);
    x2  = cs* (*this)(1,1) + sn* (*this)(1,2);
    x3  = cs* (*this)(2,1) + sn* (*this)(2,2);
    (*this)(0,2) = -sn* (*this)(0,1) + cs* (*this)(0,2);
    (*this)(1,2) = -sn* (*this)(1,1) + cs* (*this)(1,2);
    (*this)(2,2) = -sn* (*this)(2,1) + cs* (*this)(2,2);
    (*this)(0,1) = x1;
    (*this)(1,1) = x2;
    (*this)(2,1) = x3;
}

void Rotation::DoRotY(double angle)
{
    double cs = cos(angle);
    double sn = sin(angle);
    double x1,x2,x3;
    x1  = cs* (*this)(0,0) - sn* (*this)(0,2);
    x2  = cs* (*this)(1,0) - sn* (*this)(1,2);
    x3  = cs* (*this)(2,0) - sn* (*this)(2,2);
    (*this)(0,2) = sn* (*this)(0,0) + cs* (*this)(0,2);
    (*this)(1,2) = sn* (*this)(1,0) + cs* (*this)(1,2);
    (*this)(2,2) = sn* (*this)(2,0) + cs* (*this)(2,2);
    (*this)(0,0) = x1;
    (*this)(1,0) = x2;
    (*this)(2,0) = x3;
}

void Rotation::DoRotZ(double angle)
{
    double cs = cos(angle);
    double sn = sin(angle);
    double x1,x2,x3;
    x1  = cs* (*this)(0,0) + sn* (*this)(0,1);
    x2  = cs* (*this)(1,0) + sn* (*this)(1,1);
    x3  = cs* (*this)(2,0) + sn* (*this)(2,1);
    (*this)(0,1) = -sn* (*this)(0,0) + cs* (*this)(0,1);
    (*this)(1,1) = -sn* (*this)(1,0) + cs* (*this)(1,1);
    (*this)(2,1) = -sn* (*this)(2,0) + cs* (*this)(2,1);
    (*this)(0,0) = x1;
    (*this)(1,0) = x2;
    (*this)(2,0) = x3;
}


Rotation Rotation::RotX(double angle) {
    double cs=cos(angle);
    double sn=sin(angle);
    return Rotation(1,0,0,0,cs,-sn,0,sn,cs);
}
Rotation Rotation::RotY(double angle) {
    double cs=cos(angle);
    double sn=sin(angle);
    return Rotation(cs,0,sn,0,1,0,-sn,0,cs);
}
Rotation Rotation::RotZ(double angle) {
    double cs=cos(angle);
    double sn=sin(angle);
    return Rotation(cs,-sn,0,sn,cs,0,0,0,1);
}

Rotation Rotation::Inverse() const
{
    Rotation tmp(*this);
    tmp.SetInverse();
    return tmp;
}

Vector Rotation::Inverse(const Vector& v) const {
    return Vector(
     data[0]*v.data[0] + data[3]*v.data[1] + data[6]*v.data[2],
     data[1]*v.data[0] + data[4]*v.data[1] + data[7]*v.data[2],
     data[2]*v.data[0] + data[5]*v.data[1] + data[8]*v.data[2] 
    );
}


void Rotation::SetInverse()
{
    double tmp;
    tmp = data[1];data[1]=data[3];data[3]=tmp;
    tmp = data[2];data[2]=data[6];data[6]=tmp;
    tmp = data[5];data[5]=data[7];data[7]=tmp;
}

IMETHOD bool operator!=(const Rotation& a,const Rotation& b) {
	return !operator==(a,b);
}


