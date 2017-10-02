#include "Object.h"
#include <iostream>
#include <GL/gl.h>

Object::Object(double _mass, double _inertia, double _restitution, double _staticFriction, double _dynamicFriction, Shape* _shape, bool _isStatic):
    restitution(_restitution), staticFriction(_staticFriction), dynamicFriction(_dynamicFriction), shape(_shape), isStatic(_isStatic){
        this->setMass(_mass);

        switch(shape->getType()){
            case _POLYGON:
                {
                    Poly* s = (Poly*) _shape;
                    Vec2 vertexsSum;

                    std::vector<Vec2*> vertexs = s->getVertexs();
                    double sum = 0;
                    double divider = 0;
                    for(int i = 0; i < vertexs.size(); i++){
                        Vec2 v1 = *vertexs[i];
                        Vec2 v2 = *vertexs[(i+1) % vertexs.size()];
                        sum += std::abs(cross(v1, v2)) * (dot(v1, v1) + dot(v1, v2) + dot(v2, v2));

                        divider += std::abs(cross(v2, v1));

                        vertexsSum += *vertexs[i];
                    }
                    double i = _mass/6.*sum/divider;
                    this->setInertia(i); // inertia based on mass

                    vertexsSum /= vertexs.size();
                    double farthest = 0;

                    for(int i = 0; i < vertexs.size(); i++){
                        Vec2 point = *vertexs[i] - vertexsSum;
                        if(farthest < point.lengthSquared())
                            farthest = point.lengthSquared();
                    }

                    broadShape = new Circle(vertexsSum, sqrt(farthest));
                }
                break;
            
            default:
                {
                    broadShape = _shape;
                    this->setInertia(_inertia);
                }
                break;
        }

        position = velocity = Vec2();
        rotation = rotationVelocity = 0;


}

Vec2& Object::getVelocity(){
    return velocity;
}

Vec2& Object::getPosition(){
    return position;
}

double Object::getRotation(){
    return rotation;
}

double Object::getRotationVelocity(){
    return rotationVelocity;
}

double Object::getRestitution() const{
    return restitution;
}

double Object::getStaticFriction() const{
    return staticFriction;
}
double Object::getDynamicFriction() const{
    return dynamicFriction;
}

double Object::getMass() const{
    return mass;
}

double Object::getInvMass() const{
    return invMass;
}

double Object::getInvInertia() const{
    return invInertia;
}

Shape* Object::getShape(){
    return shape;
}

Shape* Object::setShape(Shape* s){
    shape = s;
    return shape;
}

Shape* Object::getBroad(){
    return broadShape;
}

double& Object::setMass(double& _mass){
    mass = _mass;
    if( _mass == 0)
        invMass = 0;
    else
        invMass = 1/_mass;
    return mass;
}

double& Object::setInertia(double& _inertia){
    inertia = _inertia;
    if( _inertia == 0)
        invInertia = 0;
    else
        invInertia = 1/_inertia;
    return inertia;
}

bool& Object::setStatic(bool _isStatic){
    isStatic = _isStatic;
    return isStatic;
}

bool& Object::getStatic(){
    return isStatic;
}

void Object::addVelocity(const Vec2& v){
    if(isStatic) return;
    Vec2 w = v;
    velocity += w;
}

void Object::move(const Vec2& v){
    if(isStatic) return;
    Vec2 w = v;
    position += w;
}

void Object::addRotationVelocity(double r){
    if(isStatic) return;
    rotationVelocity += r;
}

void Object::addRotation(double r){
    if(isStatic) return;
    rotation += r;
}

void Object::draw(){
    glPushMatrix();
    glTranslated(position.getX(), position.getY(), 0);
    glRotated(rotation * 180 / M_PI, 0, 0, 1);
    shape->draw();
    glPopMatrix();
}