#include "Polygon.h"

Poly::Poly(): vertexs(), type(_POLYGON) {}
Poly::Poly(const Poly& p): vertexs(p.getVertexs()), type(_POLYGON) {}
Poly::Poly(std::vector<Vec2 *> _vertexs): vertexs(_vertexs), type(_POLYGON) {}

std::vector<Vec2 *> Poly::getVertexs() const{
    return vertexs;
}

int Poly::getType() const{
    return type;
}

void Poly::draw() const{
    glBegin(GL_LINE_STRIP);
    glColor3ub(255,255,255);
    glVertex2d(0,0);
        for(int i = 0; i < vertexs.size(); i++){
            glVertex2d(vertexs[i]->getX(), vertexs[i]->getY());
        }
        glVertex2d(vertexs[0]->getX(), vertexs[0]->getY());
    glEnd();
}