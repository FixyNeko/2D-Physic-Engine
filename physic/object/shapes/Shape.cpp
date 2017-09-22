#include "Shape.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

void Shape::draw() const{
    GLUquadric* params = gluNewQuadric();
    gluQuadricDrawStyle(params, GLU_LINE);
    gluSphere(params, 150, 5, 5);

    std::cout << "Shape draw done" << std::endl;
}