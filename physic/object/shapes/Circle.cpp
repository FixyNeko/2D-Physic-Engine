#include "Circle.h"

Circle::Circle(): pos(Vec2()), radius(0), type(_CIRCLE) {}
Circle::Circle(double r): pos(Vec2()), radius(r), type(_CIRCLE) {}
Circle::Circle(Vec2 p, double r): pos(p), radius(r), type(_CIRCLE) {}

Circle& Circle::translate(Vec2 v){
    pos += v;
    return *this;
}

double Circle::getRadius() const{
    return radius;
}

int Circle::getType() const{
    return type;
}

void Circle::draw() const{
    int vertex = floor(2*M_PI*radius) / 5; //lines of 5 length approx.
    glPushMatrix();
    glTranslated(pos.getX(), pos.getY(), 0);
    glBegin(GL_LINE_LOOP);
        for(int i =0; i< vertex; i++){
            glVertex2d(radius*std::cos(2*M_PI*i/vertex), radius*std::sin(2*M_PI*i/vertex));
        }
    glEnd();

    glPopMatrix();
    
}