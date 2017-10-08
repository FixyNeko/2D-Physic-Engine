#include "Shape.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

int Shape::getType() const{
    return NULL;
}

void Shape::draw(GLuint textureID, double textureZoom) const{
    GLUquadric* params = gluNewQuadric();
    gluQuadricDrawStyle(params, GLU_LINE);
    gluSphere(params, 150, 5, 5);

    std::cout << "Shape draw done" << std::endl;
}