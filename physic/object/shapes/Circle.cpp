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

Vec2& Circle::getPosition(){
    return pos;
}

int Circle::getType() const{
    return type;
}

void Circle::draw(GLuint textureID, double textureZoom) const{
    int vertex = 0.75 * radius;//std::min(floor(2*M_PI*radius) / 5, 20.); //lines of 5 length approx.
    glPushMatrix();
    glTranslated(pos.getX(), pos.getY(), 0);
    glBindTexture(GL_TEXTURE_2D, textureID);    
    glBegin(GL_POLYGON);
        for(int i = 0; i < vertex ; i++){
            double x = radius*std::cos(2*M_PI*i/(vertex-1));
            double y = radius*std::sin(2*M_PI*i/(vertex-1));
            glTexCoord2d(x/textureZoom + 0.5, y/textureZoom + 0.5);
            glVertex2d(x, y);
        }
    glEnd();

    glPopMatrix();
}