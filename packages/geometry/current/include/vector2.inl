

//============ 2 dimensional version of the frames objects =============
IMETHOD Vector2::Vector2(const Vector2 & arg)
{
    data[0] = arg.data[0];
    data[1] = arg.data[1];
}

IMETHOD Vector2::Vector2(double x,double y)
{
        data[0]=x;data[1]=y;
}


IMETHOD Vector2& Vector2::operator =(const Vector2 & arg)
{
    data[0] = arg.data[0];
    data[1] = arg.data[1];
    return *this;
}


IMETHOD Vector2 operator +(const Vector2 & lhs,const Vector2& rhs)
{
    return Vector2(lhs.data[0]+rhs.data[0],lhs.data[1]+rhs.data[1]);
}

IMETHOD Vector2 operator -(const Vector2 & lhs,const Vector2& rhs)
{
    return Vector2(lhs.data[0]-rhs.data[0],lhs.data[1]-rhs.data[1]);
}

IMETHOD Vector2 operator *(const Vector2& lhs,double rhs) 
{
    return Vector2(lhs.data[0]*rhs,lhs.data[1]*rhs);
}

IMETHOD Vector2 operator *(double lhs,const Vector2& rhs) 
{
    return Vector2(lhs*rhs.data[0],lhs*rhs.data[1]);
}

IMETHOD Vector2 operator /(const Vector2& lhs,double rhs) 
{
    return Vector2(lhs.data[0]/rhs,lhs.data[1]/rhs);
}

IMETHOD Vector2& Vector2::operator +=(const Vector2 & arg)
{
    data[0]+=arg.data[0];
    data[1]+=arg.data[1];
    return *this;
}

IMETHOD Vector2& Vector2::operator -=(const Vector2 & arg)
{
    data[0]-=arg.data[0];
    data[1]-=arg.data[1];
    return *this;
}

IMETHOD Vector2 Vector2::Zero() {
    return Vector2(0,0);
}

IMETHOD double Vector2::operator()(int index) const {
    FRAMES_CHECKI((0<=index)&&(index<=1));
    return data[index];
}

IMETHOD double& Vector2::operator () (int index)
{
    FRAMES_CHECKI((0<=index)&&(index<=1));
    return data[index];
}
IMETHOD void Vector2::ReverseSign()
{
    data[0] = -data[0];
    data[1] = -data[1];
}


IMETHOD Vector2 operator-(const Vector2 & arg)
{
    return Vector2(-arg.data[0],-arg.data[1]);
}


IMETHOD void Vector2::Set3DXY(const Vector& v)
// projects v in its XY plane, and sets *this to these values
{
    data[0]=v(0);
    data[1]=v(1);
}
IMETHOD void Vector2::Set3DYZ(const Vector& v)
// projects v in its XY plane, and sets *this to these values
{
    data[0]=v(1);
    data[1]=v(2);
}
IMETHOD void Vector2::Set3DZX(const Vector& v)
// projects v in its XY plane, and and sets *this to these values
{
    data[0]=v(2);
    data[1]=v(0);
}

IMETHOD void Vector2::Set3DPlane(const Frame& F_someframe_XY,const Vector& v_someframe) 
// projects v in the XY plane of F_someframe_XY, and sets *this to these values
// expressed wrt someframe.
{
    Vector tmp = F_someframe_XY.Inverse(v_someframe);
    data[0]=tmp(0);
    data[1]=tmp(1);
}


IMETHOD bool Equal(const Vector2& a,const Vector2& b,double eps) {
        return (Equal(a.data[0],b.data[0],eps)&&
                Equal(a.data[1],b.data[1],eps)   );
     }

IMETHOD void SetToZero(Vector2& v) {
    v = Vector2::Zero();
}
