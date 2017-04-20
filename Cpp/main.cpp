#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include "../Header/Mesh_2D.h"
#include "../Header/ZeroDebugFlip.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;


extern const size_t StepControl = 1;
extern const double X_Beg = 0.0,X_End = 1.0,Y_Beg = 0.0,Y_End = 1.0,
					Lx = X_End - X_Beg,Ly = Y_End - Y_Beg;

size_t Faces = 0,Nodes = 0,Cells = 0,ShadowCellNum = 0;
size_t InteriorFaceNum = 0, WallFaceNum = 0;
double *NodeX, *NodeY, *NodeZ;

Face_2D *FaceArray = nullptr;
Cell_2D *CellArray = nullptr, *CellShadowA = nullptr;

Face_2D **InteriorFaceA = nullptr;
Face_2D **WallFaceA = nullptr;

extern int MeshConstruct(const string& s);
extern int MeshCheck();
extern int MeshOutput(const string& s);
extern int MeshArea();
extern int ShadowCellConstruct();
extern void FacesClassify();
extern int fx_fy_LS_M();

int main()
{
	string s("Cartesian_square_periodic_BCs_5_5");
	
	clock_t c_start,c_end;
	time_t t_start,t_end;
	c_start = clock();
	time(&t_start);
	MeshConstruct(s);
	MeshArea();
	ShadowCellConstruct();
	MeshCheck();
	#ifdef _ZERO_NDEBUG_FLIP
	MeshOutput(s);
	#endif
	FacesClassify();
	fx_fy_LS_M();

	c_end = clock();
	time(&t_end);
	//cout << "time used : " << difftime(c_end,c_start)/CLOCKS_PER_SEC << "s";
	//cout << "time used : " << t_end - t_start << "s";

}

