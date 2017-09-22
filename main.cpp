#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include "main.h"
#include "physic/utils/Vec2.h"
#include "physic/object/shapes/AABB.h"
#include "physic/object/shapes/Circle.h"
#include "physic/physic.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700

int msaa = 0;
double fpsLimit = 120;

double angleX = 0, angleZ = 0;

Uint32 last_time, current_time, ellapsed_time, start_time;
SDL_Window *screen;

int main(int argc, char *argv[])
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    AABB* aabb = new AABB(100,70);
    Circle* circle = new Circle(30);
    Object* aabb1 = new Object(20, 0.8, aabb, true);
    Object* circle1= new Object(10, 0.5, circle, false);
    addObject(aabb1);
    addObject(circle1);
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
        current_time = SDL_GetTicks();
        ellapsed_time = current_time - last_time;
        last_time = current_time;

        SDL_WaitEvent(&event);

        switch(event.type)
        {
            case SDL_QUIT:
            exit(0);
            break;
        }

        draw(&screen);

        fpsInTitle();
        SDL_Delay(max((double)1000./fpsLimit - (SDL_GetTicks()-current_time), 0));
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

double max(double a, double b)
{
    if(a > b)
        return a;
    else
        return b;
}