#include "Vec2.h"
#include <cmath>
#include <iostream>

Vec2::Vec2(): x(0), y(0) {}
Vec2::Vec2(double x, double y): x(x), y(y) {}
Vec2::Vec2(const Vec2& v): x(v.getX()), y(v.getY()) {}

double Vec2::getX() const{
    return x;
}

double Vec2::getY() const{
    return y;
}

Vec2& Vec2::setX(double _x){
    x = _x;
    return *this;
}

Vec2& Vec2::setY(double _y){
    y = _y;
    return *this;
}

Vec2& Vec2::operator=(const Vec2& v){
    x = v.getX();
    y = v.getY();
    return *this;
}

Vec2 Vec2::operator+(Vec2& v){
    return Vec2(x + v.getX(), y + v.getY());
}

Vec2 Vec2::operator-(const Vec2& v){
    return Vec2(x - v.getX(), y - v.getY());
}

Vec2& Vec2::operator+=(Vec2& v){
    x += v.getX();
    y += v.getY();
    return *this;
}

Vec2& Vec2::operator-=(Vec2& v){
    x -= v.getX();
    y -= v.getY();
    return *this;
}

Vec2 Vec2::operator+(double m){
    double l = this -> length();
    return Vec2(Vec2(x, y) / l * (l+m));
}
    
Vec2 Vec2::operator-(double m){
    double l = this -> length();
    return Vec2(Vec2(x, y) / l * (l-m));
}

Vec2& Vec2::operator+=(double m){
    double l = this -> length();
    x = x/l * (l+m);
    y = y/l * (l+m);
}

Vec2& Vec2::operator-=(double m){
    double l = this -> length();
    x = x/l * (l-m);
    y = y/l * (l-m);
}
    
Vec2 Vec2::operator*(double m){
    return Vec2(x*m, y*m);
}
        
Vec2 Vec2::operator/(double m){
    return Vec2(x/m, y/m);
}

Vec2& Vec2::operator*=(double m){
    x = x*m;
    y = y*m;
    return *this;
}

Vec2& Vec2::operator/=(double m){
    x = x/m;
    y = y/m;
    return *this;
}

Vec2 Vec2::operator-(){
    return(Vec2(-x, -y));
}

bool Vec2::operator==(Vec2& v){
    return(x == v.getX() && y == v.getY());
}

double Vec2::length(){
    return(sqrt(x*x + y*y));
}

double Vec2::lengthSquared(){
    return(x*x + y*y);
}

Vec2& Vec2::normalize(){
    double l = this -> length();
    if(l == 0)
        return *this;
    *this /= l;
    return *this;
}

Vec2& Vec2::rotate(double deg) {
	double theta = deg / 180.0 * M_PI;
	double c = std::cos(theta);
	double s = std::sin(theta);
	double tx = x * c - y * s;
	double ty = x * s + y * c;
	x = tx;
    y = ty;
    return *this;
}

Vec2 Vec2::ortho() const {
    return Vec2(y, -x);
}

double dot(Vec2& v1, Vec2& v2) {
    return v1.getX() * v2.getX() + v1.getY() * v2.getY();
}
double cross(Vec2& v1, Vec2& v2) {
    return (v1.getX() * v2.getY()) - (v1.getY() * v2.getX());
}
Vec2 cross(Vec2& v, double s) {
    return Vec2(v.getY() * s, v.getX() * (-s));
}
Vec2 cross(double s, Vec2& v) {
    return Vec2(v.getY() * (-s), v.getX() * s);
}

void Vec2::print() const{
    std::cout << "(" << x << ":" << y << ")";
}