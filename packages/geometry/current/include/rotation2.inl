

//============ 2 dimensional version of the frames objects =============

IMETHOD Rotation2& Rotation2::operator=(const Rotation2& arg) {
    c=arg.c;s=arg.s;
    return *this;
}

IMETHOD Vector2 Rotation2::operator*(const Vector2& v) const {
    return Vector2(v.data[0]*c-v.data[1]*s,v.data[0]*s+v.data[1]*c);
}

IMETHOD double Rotation2::operator()(int i,int j) const {
    FRAMES_CHECKI((0<=i)&&(i<=1)&&(0<=j)&&(j<=1));
    if (i==j) return c;
    if (i==0) 
        return s;
    else
        return -s;
}


IMETHOD Rotation2 operator *(const Rotation2& lhs,const Rotation2& rhs) {
    return Rotation2(lhs.c*rhs.c-lhs.s*rhs.s,lhs.s*rhs.c+lhs.c*rhs.s);
}

IMETHOD void Rotation2::SetInverse() {
    s=-s;
}

IMETHOD Rotation2 Rotation2::Inverse() const {
    return Rotation2(c,-s);
}

IMETHOD Vector2 Rotation2::Inverse(const Vector2& v) const {
    return Vector2(v.data[0]*c+v.data[1]*s,-v.data[0]*s+v.data[1]*c);
}

IMETHOD Rotation2 Rotation2::Identity() {
    return Rotation2(1,0);
}
     
IMETHOD void Rotation2::SetIdentity()
{
	c = 1;
	s = 0;
}

IMETHOD void Rotation2::SetRot(double angle) {
    c=cos(angle);s=sin(angle);
}

IMETHOD Rotation2 Rotation2::Rot(double angle) {
    return Rotation2(cos(angle),sin(angle));
}

IMETHOD double Rotation2::GetRot() const {
    return atan2(s,c);
}

IMETHOD bool Equal(const Rotation2& a,const Rotation2& b,double eps) {
    return ( Equal(a.c,b.c,eps) && Equal(a.s,b.s,eps) );
}
