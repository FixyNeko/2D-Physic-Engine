#ifndef DEF_VEC2
#define DEF_VEC2

class Vec2
{
private:
    double x, y;

public:
    Vec2();
    Vec2(double x, double y);
    Vec2(const Vec2& v);
    double getX() const;
    double getY() const;
    Vec2& setX(double x);
    Vec2& setY(double y);
    Vec2& operator=(const Vec2& v);
    Vec2 operator+(Vec2& v);
    Vec2 operator-(Vec2& v);
    Vec2& operator+=(Vec2& v);
    Vec2& operator-=(Vec2& v);
    Vec2 operator+(double m);
    Vec2 operator-(double m);
    Vec2& operator+=(double m);
    Vec2& operator-=(double m);
    Vec2 operator*(double m);
    Vec2 operator/(double m);
    Vec2& operator*=(double m);
    Vec2& operator/=(double m);
    Vec2 operator-();
    bool operator==(Vec2& v);
    double length();
    double lengthSquared();
    Vec2& normalize();
    Vec2& rotate(double angle);
    Vec2 ortho() const;
};

double dot(Vec2& v1, Vec2& v2);
double cross(Vec2& v1, Vec2& v2);

#endif