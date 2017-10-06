#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <cstdint>
#include <time.h>
#include "main.h"
#include "physic/utils/Vec2.h"
#include "physic/object/shapes/AABB.h"
#include "physic/object/shapes/Circle.h"
#include "physic/physic.h"
#include "physic/utils/sdlglutils.h"
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

int msaa = 16;

double angleX = 0, angleZ = 0;

unsigned int lastTime, currentTime;
SDL_Window *screen;

Object* addCircle(){
    Circle* circles = new Circle(20/2);
    Object* circle = new Object(7854, 0/*NOT USED*/, 0.9, 3000, 1000, circles, false);
    addObject(circle);

    return circle;
}

Object* addPoly(){
    std::vector<Vec2*> vertexs;
    vertexs.push_back(new Vec2(-200, 10));
    vertexs.push_back(new Vec2(-200, -10));
    vertexs.push_back(new Vec2(200, -10));
    vertexs.push_back(new Vec2(200, 10));

    Poly* polys = new Poly(vertexs);
    Object* poly = new Object(50000, 0/*NOT USED*/,  0.05, 300., 100., polys, false);
    addObject(poly);

    return poly;
}

void addFloor(){
    std::vector<double> heightGen;
    double width = WINDOW_WIDTH / 15;
    srand(time(NULL)); // random initialisation

    for(int i = 0-1; i <= 14+1; i++){
        double random = (i == 0-1 || i == 14+1) ? WINDOW_HEIGHT : std::rand() % 50 + 20;
        heightGen.push_back(random);
    }

    for(int i = 0; i < 15; i++){
        
        std::vector<Vec2*> vertexs;
        vertexs.push_back(new Vec2(-WINDOW_WIDTH/2 + i*width, 0));
        vertexs.push_back(new Vec2(-WINDOW_WIDTH/2 + (1+i)*width,0));
        vertexs.push_back(new Vec2(-WINDOW_WIDTH/2 + (1+i)*width, heightGen[i+1]/2 + heightGen[i+2]/2));
        vertexs.push_back(new Vec2(-WINDOW_WIDTH/2 + i*width, heightGen[i]/2 + heightGen[i+1]/2));

        Shape* polys = new Poly(vertexs);
        Object* poly = new Object(0, 0., 1., 1., 1., polys, false);
        poly->move(Vec2(0, -400));
        poly->setStatic(true);
        addObject(poly);
    }
}

int main(int argc, char *argv[])
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    //addWindowBoundaries();

    addFloor();
    Object* poly = addPoly();
    Object* circle = addCircle();
    ////////////////////////////////////////////////////////////////////////////////////////////

    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    if(msaa){
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa);
    }

    screen = SDL_CreateWindow("SDL GL Application",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                WINDOW_WIDTH, WINDOW_HEIGHT,
                                SDL_WINDOW_OPENGL);
    SDL_GLContext glcontext = SDL_GL_CreateContext(screen);


    if(msaa)
        glEnable(GLUT_MULTISAMPLE);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D(-WINDOW_WIDTH/2, WINDOW_WIDTH/2, -WINDOW_HEIGHT/2, WINDOW_HEIGHT/2);

    lastTime = SDL_GetTicks();

    for (;;)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_e:
                        addPoly();
                        break;
                    case SDLK_a:
                        addCircle();
                        break;
                    case SDLK_z:
                        circle->addVelocity(Vec2(0,50));
                        break;
                    case SDLK_s:
                        circle->addVelocity(Vec2(0,-10));
                        break;
                    case SDLK_q:
                        circle->addRotationVelocity(5);
                        break;
                    case SDLK_d:
                        circle->addRotationVelocity(-5);
                        break;
                }
                break;
            case SDL_QUIT:
                exit(0);
                break;
        }

        lastTime = currentTime;
        currentTime = SDL_GetTicks();

        physic::update(std::min(10, (int) (currentTime - lastTime))); // slow down physic to avoid teleports
        draw(&screen);
    }

    return 0;
}

void draw(SDL_Window** screen)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    physic::draw();

    glFlush();
    SDL_GL_SwapWindow(*screen);
}

void addWindowBoundaries(){
    AABB* horizontals = new AABB(WINDOW_WIDTH + 200, 100);
    AABB* verticals = new AABB(100, WINDOW_HEIGHT + 200);

    Object* top = new Object(0, 0, 10000, 99999., 99999., horizontals, false);
    top->move(Vec2(0, WINDOW_HEIGHT/2 + 0));
    top->setStatic(true);
    Object* bottom = new Object(0, 0, 10000, 99999., 99999., horizontals, false);
    bottom->move(Vec2(0, -WINDOW_HEIGHT/2 - 0));
    bottom->setStatic(true);
    Object* left = new Object(0, 0, 10000, 99999., 99999., verticals, false);
    left->move(Vec2(-WINDOW_WIDTH/2 - 0, 0));
    left->setStatic(true);
    Object* right = new Object(0, 0, 10000, 99999., 99999., verticals, false);
    right->move(Vec2(WINDOW_WIDTH/2 + 0, 0));
    right->setStatic(true);

    addObject(top);
    addObject(bottom);
    addObject(left);
    addObject(right);
}