

//============ 2 dimensional version of the frames objects =============

IMETHOD Frame2::Frame2() {
}

IMETHOD Frame2::Frame2(const Rotation2 & R)
{
    M=R;
    p=Vector2::Zero();
}

IMETHOD Frame2::Frame2(const Vector2 & V)
{
    M = Rotation2::Identity();
    p = V;
}

IMETHOD Frame2::Frame2(const Rotation2 & R, const Vector2 & V)
{
    M = R;
    p = V;
}

IMETHOD Frame2 operator *(const Frame2& lhs,const Frame2& rhs)
{
    return Frame2(lhs.M*rhs.M,lhs.M*rhs.p+lhs.p);
}

IMETHOD Vector2 Frame2::operator *(const Vector2 & arg)
{
    return M*arg+p;
}

IMETHOD Vector2 Frame2::Inverse(const Vector2& arg) const
{
    return M.Inverse(arg-p);
}

IMETHOD void Frame2::SetIdentity()
{
	M.SetIdentity();
	p = Vector2::Zero();
}

IMETHOD void Frame2::SetInverse()
{
    M.SetInverse();
    p = M*p;
    p.ReverseSign();
}


IMETHOD Frame2 Frame2::Inverse() const
{
    Frame2 tmp(*this);
    tmp.SetInverse();
    return tmp;
}

IMETHOD Frame2& Frame2::operator =(const Frame2 & arg)
{ 
    M = arg.M;
    p = arg.p;
    return *this;
}

IMETHOD Frame2::Frame2(const Frame2 & arg) :
    p(arg.p), M(arg.M)
{}


IMETHOD double Frame2::operator()(int i,int j) {
    FRAMES_CHECKI((0<=i)&&(i<=2)&&(0<=j)&&(j<=2));
    if (i==2) {
        if (j==2)
            return 1;
        else
            return 0;
    } else {
        if (j==2) 
            return p(i);
        else
            return M(i,j);

    }
}

IMETHOD double Frame2::operator()(int i,int j) const {
    FRAMES_CHECKI((0<=i)&&(i<=2)&&(0<=j)&&(j<=2));
    if (i==2) {
        if (j==2)
            return 1;
        else
            return 0;
    } else {
        if (j==2) 
            return p(i);
        else
            return M(i,j);

    }
}

IMETHOD bool Equal(const Frame2& a,const Frame2& b,double eps) {
        return (Equal(a.p,b.p,eps)&&
                Equal(a.M,b.M,eps)   );
}
