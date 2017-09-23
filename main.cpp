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
#define WINDOW_HEIGHT 900

int msaa = 8;
double fpsLimit = 10000000000000;

double angleX = 0, angleZ = 0;

unsigned int last_time, current_time, ellapsed_time, start_time;
SDL_Window *screen;

int main(int argc, char *argv[])
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    addWindowBoundaries();

    AABB* box1 = new AABB(100,70);
    AABB* box2 = new AABB(30, 80);
    Circle* circles1 = new Circle(10/2);
    Circle* circles2 = new Circle(100/2);

    Object* aabb1 = new Object(0, 1, box1, false);
    Object* aabb2 = new Object(50, 0.5, box2, false);
    Object* circle1 = new Object(78.54, 0.9, circles1, false);
    Object* circle2 = new Object(7854, 1, circles2, false);
    aabb1->move(Vec2(0, -WINDOW_HEIGHT/2 + 120));
    circle2->move(Vec2(20, 200));
    circle2->push(Vec2(0, -10));
    circle1->move(Vec2(1, 300));

    aabb1->setStatic(true);

    //addObject(circle1);
    //addObject(aabb1);
    addObject(circle2);
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
                        circle2->push(Vec2(50,150));
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
        SDL_Delay(std::max((double)1000./fpsLimit - (SDL_GetTicks()-current_time), 0.));
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
    AABB* horizontals = new AABB(WINDOW_WIDTH + 20, 10);
    AABB* verticals = new AABB(10, WINDOW_HEIGHT + 20);

    Object* top = new Object(0, 1, horizontals, false);
    top->move(Vec2(0, WINDOW_HEIGHT/2 + 5));
    top->setStatic(true);
    Object* bottom = new Object(0, 1, horizontals, false);
    bottom->move(Vec2(0, -WINDOW_HEIGHT/2 - 5));
    bottom->setStatic(true);
    Object* left = new Object(0, 1, verticals, false);
    left->move(Vec2(-WINDOW_WIDTH/2 - 5, 0));
    left->setStatic(true);
    Object* right = new Object(0, 1, verticals, false);
    right->move(Vec2(WINDOW_WIDTH/2 + 5, 0));
    right->setStatic(true);

    addObject(top);
    addObject(bottom);
    addObject(left);
    addObject(right);
}