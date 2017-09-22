#include "AABB.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

AABB::AABB(): min(Vec2()), max(Vec2()) {}
AABB::AABB(double width, double height): min(Vec2(-width/2, -height/2)), max(Vec2(width/2, height/2)) {}
AABB::AABB(double xMin, double yMin, double xMax, double yMax):
    min(Vec2(xMin, yMin)), max(Vec2(xMax, yMax)) {}

AABB& AABB::translate(Vec2 v){
    min += v;
    max += v;
    return *this;
}

AABB& AABB::rotate(double angle){
    min.rotate(angle);
    max.rotate(angle);
    return *this;
}

void AABB::draw() const{
    glBegin(GL_LINE_LOOP);
        glVertex2d(min.getX(), min.getY());
        glVertex2d(min.getX(), max.getY());
        glVertex2d(max.getX(), max.getY());
        glVertex2d(max.getX(), min.getY());
    glEnd();

    std::cout << "AABB draw done" << std::endl;
}