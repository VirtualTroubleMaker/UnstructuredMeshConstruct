vpath %.h ../Header

objects = main.o MeshRead.o PreConstruct.o

pexe : $(objects)
	g++ -o pexe $(objects)

main.o : main.cpp Mesh_2D.h ZeroDebugFlip.h
	g++ -c -O2 main.cpp

MeshRead.o : MeshRead.cpp Mesh_2D.h ZeroDeclaration.h MeshConstructFunc.h ZeroDebugFlip.h
	g++ -c -O2 MeshRead.cpp

PreConstruct.o : PreConstruct.cpp Mesh_2D.h ZeroDeclaration.h ZeroDebugFlip.h
	g++ -c -O2 PreConstruct.cpp

.PHONY : clean
clean :
	del pexe.exe $(objects)