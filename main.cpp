#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
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
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

int msaa = 4;

double angleX = 0, angleZ = 0;

unsigned int lastTime, currentTime;
SDL_Window *screen;

Object* addCircle(){
    GLuint textureID = loadTexture("resources/textures/blocks/dirt.png");
    Circle* circles = new Circle(100/2);
    Object* circle = new Object(1000, 0/*NOT USED*/, 0.7, 3000, 1000, circles, true, true, textureID, 150);
    addObject(circle);
    return circle;
}

Object* addPlayer(){
    GLuint textureID = loadTexture("resources/textures/player/left_0.png");
    std::vector<Vec2*> vertexs;
    vertexs.push_back(new Vec2(-16/2, -64/2));
    vertexs.push_back(new Vec2(16/2, -64/2));
    vertexs.push_back(new Vec2(16/2, 64/2));
    vertexs.push_back(new Vec2(-16/2, 64/2));
    Poly* playerShape = new Poly(vertexs);
    Object* player = new Object(75, 1, 0., 20000, 15000, playerShape, true, false, textureID, 64);
    addObject(player);

    return player;
}

Object* addPoly(){
    GLuint textureID = loadTexture("resources/textures/blocks/dirt.png");
    std::vector<Vec2*> vertexs;
    vertexs.push_back(new Vec2(-200, 10));
    vertexs.push_back(new Vec2(-200, -10));
    vertexs.push_back(new Vec2(200, -10));
    vertexs.push_back(new Vec2(200, 10));
    
    Poly* polys = new Poly(vertexs);
    Object* poly = new Object(50000, 0/*NOT USED*/,  0.05, 300., 100., polys, true, true,  textureID, 100);
    addObject(poly);

    return poly;
}

void addFloor(){
    GLuint textureID = loadTexture("textures/blocks/hay_block_top.png");
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
        Object* poly = new Object(0, 0., 1., 1., 1., polys, true, false, textureID, 100);
        poly->move(Vec2(0, -400));
        poly->setStatic(false);
        addObject(poly);
    }
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Event event;

    atexit(SDL_Quit);

    if(msaa){
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa);
    }

    screen = SDL_CreateWindow("SDL GL Application",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                WINDOW_WIDTH, WINDOW_HEIGHT,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_GLContext glcontext = SDL_GL_CreateContext(screen);

    ////////////////////////////////////////////////////////////////////////////////////////////
    //addWindowBoundaries();
    addFloor();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    Object* player = addPlayer();
    float color[] = { 0,0,0,0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    addCircle();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    Object* circle = addCircle();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    ////////////////////////////////////////////////////////////////////////////////////////////

    if(msaa)
        glEnable(GLUT_MULTISAMPLE);

    glMatrixMode( GL_PROJECTION );
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glLoadIdentity();
    gluOrtho2D(-WINDOW_WIDTH/2, WINDOW_WIDTH/2, -WINDOW_HEIGHT/2, WINDOW_HEIGHT/2);

    lastTime = SDL_GetTicks();

    for (;;)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                exit(0);
                break;
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL);
        
        if (state[SDL_SCANCODE_Q]){
            addCircle();
        }
        if (state[SDL_SCANCODE_E]){
            addPoly();
        }
        if (state[SDL_SCANCODE_W])
            circle->addVelocity(Vec2(0,10));
        if (state[SDL_SCANCODE_S])
            circle->addVelocity(Vec2(0,-10));
        if (state[SDL_SCANCODE_A])
            circle->addRotationVelocity(1);
        if (state[SDL_SCANCODE_D])
            circle->addRotationVelocity(-1);


        lastTime = currentTime;
        currentTime = SDL_GetTicks();

        physic::update(std::min(10, (int) (currentTime - lastTime))); // slow down physic to avoid teleports
        draw(&screen);
    }

    return 0;
}

void draw(SDL_Window** screen)
{
    glClear( GL_COLOR_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    physic::draw();

    glFlush();
    SDL_GL_SwapWindow(*screen);
}
/*
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
}*/