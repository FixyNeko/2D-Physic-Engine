#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <cstdint>
#include "main.h"
#include "physic/utils/Vec2.h"
#include "physic/object/shapes/AABB.h"
#include "physic/object/shapes/Circle.h"
#include "physic/physic.h"
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

int msaa = 16;
double fpsLimit = 60;

double angleX = 0, angleZ = 0;

unsigned int last_time, current_time, ellapsed_time, start_time;
SDL_Window *screen;

Object* addCircle(){
    Circle* circles2 = new Circle(20/2);
    Object* circle2 = new Object(7854, 0.9, circles2, false);
    circle2->move(Vec2(20, 200));
    circle2->push(Vec2(500, 200));
    addObject(circle2);

    return circle2;
}

Object* addPoly(){
    std::vector<Vec2*> vertexs;
    vertexs.push_back(new Vec2(0, 0));
    vertexs.push_back(new Vec2(-100, 0));
    vertexs.push_back(new Vec2(-100, -100));
    vertexs.push_back(new Vec2(100, -100));

    Poly* polys = new Poly(vertexs);
    Object* poly = new Object(7854, 0.95, polys, false);
    poly->move(Vec2(0, 0));
    addObject(poly);

    return poly;
}

Object* addPoly2(){
    std::vector<Vec2*> vertexs;
    vertexs.push_back(new Vec2(-250, 250));
    vertexs.push_back(new Vec2(-250, 100));
    vertexs.push_back(new Vec2(0, 100));

    Poly* polys = new Poly(vertexs);
    Object* poly = new Object(0, 1, polys, false);
    poly->move(Vec2(0, -400));
    poly->setStatic(true);
    addObject(poly);

    vertexs.clear();
    std::cout << vertexs.size() << std::endl;
    vertexs.push_back(new Vec2(250, 100));
    vertexs.push_back(new Vec2(500,250));
    vertexs.push_back(new Vec2(0, 100));
    std::cout << vertexs.size() << std::endl;

    polys = new Poly(vertexs);
    poly = new Object(0, 1, polys, false);
    poly->move(Vec2(0, -400));
    poly->setStatic(true);
    addObject(poly);

    return poly;
}

int main(int argc, char *argv[])
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    //addWindowBoundaries();

    Object* poly = addPoly();
    addPoly2();
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

    physic::update(0);
    draw(&screen);

    last_time = SDL_GetTicks();

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
                    case SDLK_z:
                        poly->push(Vec2(0,50));
                        break;
                    case SDLK_s:
                        poly->push(Vec2(0,-10));
                        break;
                    case SDLK_q:
                        poly->push(Vec2(-10,0));
                        break;
                    case SDLK_d:
                        poly->push(Vec2(10,0));
                        break;
                }
                break;
            case SDL_QUIT:
                exit(0);
                break;
        }

        current_time = SDL_GetTicks();
        ellapsed_time = current_time - last_time;
        last_time = current_time;

        physic::update(std::min(10, (int) ellapsed_time)); // slow down physic to avoid teleports
        draw(&screen);

        fpsInTitle();
        SDL_Delay(std::max(1000./fpsLimit - (SDL_GetTicks()-current_time), 0.));
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

void fpsInTitle(){
    std::string fps = "FPS: " + std::to_string((int)(1000./ellapsed_time));
    std::string time_str = std::to_string((SDL_GetTicks()-current_time));
    std::string *title = &fps;

    SDL_SetWindowTitle(screen, title -> c_str());
}

void addWindowBoundaries(){
    AABB* horizontals = new AABB(WINDOW_WIDTH + 200, 100);
    AABB* verticals = new AABB(100, WINDOW_HEIGHT + 200);

    Object* top = new Object(0, 10000, horizontals, false);
    top->move(Vec2(0, WINDOW_HEIGHT/2 + 0));
    top->setStatic(true);
    Object* bottom = new Object(0, 10000, horizontals, false);
    bottom->move(Vec2(0, -WINDOW_HEIGHT/2 - 0));
    bottom->setStatic(true);
    Object* left = new Object(0, 10000, verticals, false);
    left->move(Vec2(-WINDOW_WIDTH/2 - 0, 0));
    left->setStatic(true);
    Object* right = new Object(0, 10000, verticals, false);
    right->move(Vec2(WINDOW_WIDTH/2 + 0, 0));
    right->setStatic(true);

    addObject(top);
    addObject(bottom);
    addObject(left);
    addObject(right);
}