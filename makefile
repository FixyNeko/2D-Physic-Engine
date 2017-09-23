SHELL = powershell

INCD=\
	D:\Documents\C++\Libraries \
	D:\Programs\mingw64\lib\gcc\x86_64-w64-mingw32\7.1.0\include\c++ \
	D:\Programs\mingw64\include
INCD_P=$(foreach d, $(INCD), -I$d)

INC=mingw32 \
	SDL2main SDL2 \
	opengl32 glu32 freeglut
INC_P=$(foreach i, $(INC), -l$i)

CPP=main.cpp physic\physic.cpp \
	physic\object\Object.cpp \
	physic\object\shapes\AABB.cpp \
	physic\object\shapes\Circle.cpp \
	physic\object\shapes\Shape.cpp \
	physic\utils\Vec2.cpp

main: main.cpp
	g++ $(CPP) -o main -LD:\Documents\C++\Libraries\SDL2\lib $(INCD_P) $(INC_P)
	./main.exe
#	PowerShell -Command "(ls -R -Inc *.cpp -Name) -join ' ' | g++ -x c++ -o main -LD:\Documents\C++\Libraries\SDL2\lib $(INCD_P) $(INC_P) -"