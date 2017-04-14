#include <iostream>
#include <ctime>
#include "./Header/Mesh_2D.h"
using std::cout;
using std::endl;


extern const size_t StepControl = 100;
extern const size_t NumPerCell = 4;
size_t Faces = 0,Nodes = 0,Cells = 0;
double *NodeX, *NodeY, *NodeZ;
Face_2D *FaceArray = nullptr;
Cell_2D *CellArray = nullptr;

extern int MeshConstruct();
extern int MeshCheck();
extern int MeshOutput();
extern int MeshComplementary();

int main()
{
	clock_t c_start,c_end;
	time_t t_start,t_end;
	c_start = clock();
	time(&t_start);
	MeshConstruct();
	MeshCheck();
	MeshOutput();
	MeshComplementary();
	c_end = clock();
	time(&t_end);
	cout << "time used : " << difftime(c_end,c_start)/CLOCKS_PER_SEC << "s";
	cout << "time used : " << t_end - t_start << "s";

}