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

void Poly::draw(GLuint textureID, double textureZoom) const{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBegin(GL_POLYGON);
        for(int i = 0; i < vertexs.size(); i++){
            double x = vertexs[i]->getX();
            double y = vertexs[i]->getY();
            glTexCoord2d(x/textureZoom + 0.5, y/textureZoom + 0.5);
            glVertex2d(x, y);
        }
    glEnd();
}