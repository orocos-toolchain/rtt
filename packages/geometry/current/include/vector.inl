
IMETHOD Vector::Vector(const Vector & arg)
{
    data[0] = arg.data[0];
    data[1] = arg.data[1];
    data[2] = arg.data[2];
}

IMETHOD Vector::Vector(double x,double y, double z)
{
        data[0]=x;data[1]=y;data[2]=z;
}


IMETHOD Vector& Vector::operator =(const Vector & arg)
{
    data[0] = arg.data[0];
    data[1] = arg.data[1];
    data[2] = arg.data[2];
    return *this;
}

IMETHOD Vector operator +(const Vector & lhs,const Vector& rhs)
{
    Vector tmp;
    tmp.data[0] = lhs.data[0]+rhs.data[0];
    tmp.data[1] = lhs.data[1]+rhs.data[1];
    tmp.data[2] = lhs.data[2]+rhs.data[2];
    return tmp;
}

IMETHOD Vector operator -(const Vector & lhs,const Vector& rhs)
{
    Vector tmp;
    tmp.data[0] = lhs.data[0]-rhs.data[0];
    tmp.data[1] = lhs.data[1]-rhs.data[1];
    tmp.data[2] = lhs.data[2]-rhs.data[2];
    return tmp;
}

IMETHOD double Vector::x() const { return data[0]; }
IMETHOD double Vector::y() const { return data[1]; }
IMETHOD double Vector::z() const { return data[2]; }

IMETHOD void Vector::x( double _x ) { data[0] = _x; }
IMETHOD void Vector::y( double _y ) { data[1] = _y; }
IMETHOD void Vector::z( double _z ) { data[2] = _z; }

Vector operator *(const Vector& lhs,double rhs) 
{
    Vector tmp;
    tmp.data[0] = lhs.data[0]*rhs;
    tmp.data[1] = lhs.data[1]*rhs;
    tmp.data[2] = lhs.data[2]*rhs;
    return tmp;
}

Vector operator *(double lhs,const Vector& rhs) 
{
    Vector tmp;
    tmp.data[0] = lhs*rhs.data[0];
    tmp.data[1] = lhs*rhs.data[1];
    tmp.data[2] = lhs*rhs.data[2];
    return tmp;
}

Vector operator /(const Vector& lhs,double rhs) 
{
    Vector tmp;
    tmp.data[0] = lhs.data[0]/rhs;
    tmp.data[1] = lhs.data[1]/rhs;
    tmp.data[2] = lhs.data[2]/rhs;
    return tmp;
}

Vector operator *(const Vector & lhs,const Vector& rhs)
// Complexity : 6M+3A
{
    Vector tmp;
    tmp.data[0] = lhs.data[1]*rhs.data[2]-lhs.data[2]*rhs.data[1];
    tmp.data[1] = lhs.data[2]*rhs.data[0]-lhs.data[0]*rhs.data[2];
    tmp.data[2] = lhs.data[0]*rhs.data[1]-lhs.data[1]*rhs.data[0];
    return tmp;
}

Vector& Vector::operator +=(const Vector & arg)
// Complexity : 3A
{
    data[0]+=arg.data[0];
    data[1]+=arg.data[1];
    data[2]+=arg.data[2];
    return *this;
}

Vector& Vector::operator -=(const Vector & arg)
// Complexity : 3A
{
    data[0]-=arg.data[0];
    data[1]-=arg.data[1];
    data[2]-=arg.data[2];
    return *this;
}

Vector Vector::Zero()
{
    return Vector(0,0,0);
}

double Vector::operator()(int index) const {
    FRAMES_CHECKI((0<=index)&&(index<=2));
    return data[index];
}

double& Vector::operator () (int index)
{
    FRAMES_CHECKI((0<=index)&&(index<=2));
    return data[index];
}

void Vector::Set2DPlane(const Frame& F_someframe_XY,const Vector2& v_XY)
// a 3D vector where the 2D vector v is put in the XY plane of the frame
// F_someframe_XY.
{
    Vector tmp_XY;
    tmp_XY.Set2DXY(v_XY);
    tmp_XY = F_someframe_XY*(tmp_XY);
}


void Vector::ReverseSign()
{
    data[0] = -data[0];
    data[1] = -data[1];
    data[2] = -data[2];
}



Vector operator-(const Vector & arg)
{
    Vector tmp;
    tmp.data[0]=-arg.data[0];
    tmp.data[1]=-arg.data[1];
    tmp.data[2]=-arg.data[2];
    return tmp;
}

void Vector::Set2DXY(const Vector2& v)
// a 3D vector where the 2D vector v is put in the XY plane
{
    data[0]=v(0);
    data[1]=v(1);
    data[2]=0;

}
void Vector::Set2DYZ(const Vector2& v)
// a 3D vector where the 2D vector v is put in the YZ plane
{
    data[1]=v(0);
    data[2]=v(1);
    data[0]=0;

}

void Vector::Set2DZX(const Vector2& v)
// a 3D vector where the 2D vector v is put in the ZX plane
{
    data[2]=v(0);
    data[0]=v(1);
    data[1]=0;

}

IMETHOD double dot(const Vector& lhs,const Vector& rhs) {
    return rhs(0)*lhs(0)+rhs(1)*lhs(1)+rhs(2)*lhs(2);
}

IMETHOD bool Equal(const Vector& a,const Vector& b,double eps) {
        return (Equal(a.data[0],b.data[0],eps)&&
                Equal(a.data[1],b.data[1],eps)&&
                Equal(a.data[2],b.data[2],eps)   );
     }
     
IMETHOD bool operator==(const Vector& a,const Vector& b) {
#ifdef OROPKG_GEOMETRY_USE_EQUAL
    return Equal(a,b);
#else
        return (a.data[0]==b.data[0]&&
                a.data[1]==b.data[1]&&
                a.data[2]==b.data[2] );
#endif
     }

IMETHOD bool operator!=(const Vector& a,const Vector& b) {
	return !operator==(a,b);
}
     
IMETHOD void SetToZero(Vector& v) {
    v=Vector::Zero();
}

