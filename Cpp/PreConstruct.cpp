#include "../Header/ZeroDeclaration.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

int ShadowCellConstruct()
{
	CellShadowA = new Cell_2D[ShadowCellNum];
	size_t k = 0;
	for(size_t i = 0;i != Faces;++i)
	{
		if(12 == FaceArray[i].bc_type || 8 == FaceArray[i].bc_type)
		{
			FaceArray[i].rhsCell = CellShadowA + k; 
			CellShadowA[k].xc = FaceArray[i].shadowF->lhsCell->xc;
			CellShadowA[k].yc = FaceArray[i].shadowF->lhsCell->yc;
			CellShadowA[k].volume = FaceArray[i].shadowF->lhsCell->volume;
			CellShadowA[k].ShadowC = FaceArray[i].shadowF->lhsCell;
			if(X_Beg == FaceArray[i].xf)
			{
				CellShadowA[k].xc -= Lx;
			}
			else if(X_End == FaceArray[i].xf)
			{
				CellShadowA[k].xc += Lx;
			}
			else if(Y_Beg == FaceArray[i].yf)
			{
				CellShadowA[k].yc -= Ly;
			}
			else if(Y_End == FaceArray[i].yf)
			{
				CellShadowA[k].yc += Ly;
			}
			else
			{
				cout <<"Construct Shadow Cell Failed"<<endl;
				cout <<__FILE__ <<" : "<<__LINE__<<"  "<<__func__<<endl;
				system("pause");
			}
			++k;
		}
	}
	return 0;
}

void LSCellMatrix(Cell_2D* const &Center,size_t k,
					const double &neighbour_xc, double const &neighbour_yc)
{
	double dx = neighbour_xc - Center->xc, dy = neighbour_yc - Center->yc;
	if(dx < 1.0E-15 && dx > -1.0E-15) dx = 0.0;
	if(dy < 1.0E-15 && dy > -1.0E-15) dy = 0.0;
	double DistanceP2P = dx*dx + dy*dy;
	Center->LS_M[0][0] += dx*dx/DistanceP2P;
	Center->LS_M[0][1] += dx*dy/DistanceP2P;
	Center->LS_M[1][0] += dx*dy/DistanceP2P;
	Center->LS_M[1][1] += dy*dy/DistanceP2P;
	Center->dfdx_C[k] = dx/DistanceP2P;
	Center->dfdy_C[k] = dy/DistanceP2P;
	#ifdef _QUAD_PERIODIC_FLIP
		if(Center->dfdx_C[k] * Center->dfdy_C[k] != 0.0)
		{
			cout << Center->dfdx_C[k] <<"  " << Center->dfdy_C[k] <<"  "
				 << dx <<"  "<< dy <<"  "<<DistanceP2P<<endl;
			system("pause");
		}
	#endif
}

void InverseLS_M(double (&LS_M)[2][2])
{
	double a[2][2],A;
	A = LS_M[0][0] * LS_M[1][1] - LS_M[0][1] * LS_M[1][0];
	if(0.0 == A)
	{
		cout <<"Singular Maxtrix : " <<__FILE__<<"  "<<__LINE__<<"  "<<__func__<<endl;
		system("pause");
		return;
	}
	a[0][0] = LS_M[1][1]/A;
	a[1][1] = LS_M[0][0]/A;
	a[0][1] = -LS_M[0][1]/A;
	a[1][0] = -LS_M[1][0]/A;
	for(size_t i = 0;i < 2;++i)
		for(size_t j = 0;j < 2;++j)
			LS_M[i][j] = a[i][j];
}

int fx_fy_LS_M()
{
	Cell_2D* neighbour = nullptr;
	Cell_2D* center = nullptr;
	for(size_t i = 0;i != Cells;++i)
	{
		center = CellArray + i;
		for(size_t k = 0;k != center->celltype;++k)
		{
			if(2 == center->Face_C[k]->bc_type || 12 == center->Face_C[k]->bc_type
				|| 8 == center->Face_C[k]->bc_type)
			{
				neighbour = (center->Face_C[k]->lhsCell == center) ? (center->Face_C[k]->rhsCell) :
																 (center->Face_C[k]->lhsCell);
				LSCellMatrix(center,k,neighbour->xc,neighbour->yc);
			}
			else if(3 == center->Face_C[k]->bc_type){
				LSCellMatrix(center,k,center->Face_C[k]->xf,center->Face_C[k]->yf);
			}
			else{
				cout << "FaceArray : " << i <<"unknown bc_type : "
				<< center->Face_C[k]->bc_type << endl;
				system("pause");
			}
		}
		#ifdef _QUAD_PERIODIC_FLIP
		if(center->LS_M[0][0] != 2.0 || center->LS_M[0][1] != 0.0 ||
		   center->LS_M[1][0] != 0.0 || center->LS_M[1][1] != 2.0){
			cout <<"cell : "<<i<<endl;
			cout <<"Original Matrix : "<<endl;
			cout <<	center->LS_M[0][0] <<"  "<< center->LS_M[0][1] << endl;
			cout <<	center->LS_M[1][0] <<"  "<< center->LS_M[1][1] << endl;
			system("pause");
		}
		#endif
		InverseLS_M(center->LS_M);
		#ifdef _QUAD_PERIODIC_FLIP		
		if(center->LS_M[0][0] != 0.5 || center->LS_M[0][1] != 0.0 ||
		   center->LS_M[1][0] != 0.0 || center->LS_M[1][1] != 0.5){
			cout <<"Inversed Matrix : "<<endl;
			cout <<	center->LS_M[0][0] <<"  "<< center->LS_M[0][1] << endl;
			cout <<	center->LS_M[1][0] <<"  "<< center->LS_M[1][1] << endl;
			system("pause");
		}
		#endif
	}
	cout <<"LeastSquare Matrix Construction Done" << endl;
	return 0;
}

void FacesClassify()
{
	vector<Face_2D*> Interior,Wall;
	for(size_t i = 0;i != Faces; ++i)
	{
		if(2 == FaceArray[i].bc_type || 12 == FaceArray[i].bc_type || 8 == FaceArray[i].bc_type)
			Interior.push_back(&FaceArray[i]);
		else if(3 == FaceArray[i].bc_type)
			Wall.push_back(&FaceArray[i]);
		else
		{
			cout << "FaceArray : " << i <<"unknown bc_type : " << FaceArray[i].bc_type
			<<"  "<<__FILE__<<"  "<<__LINE__<<"  "<<__func__<< endl;
			system("pause");
		}
	}
	InteriorFaceNum = Interior.size();
	WallFaceNum = Wall.size();
	#ifdef _ZERO_NDEBUG_FLIP
	cout <<"Faces : "<<Faces<<"  InFaceNum : "<<InteriorFaceNum
		 <<"  WallFaceNum : "<<WallFaceNum<<endl;
	if(Faces != (InteriorFaceNum + WallFaceNum))
	{
		cout << "FaceArray : " << Faces <<" Sum not equal : "
		<<"  "<<__FILE__<<"  "<<__LINE__<<"  "<<__func__<< endl;
		system("pause");
	}	
	#endif
	InteriorFaceA = new Face_2D* [InteriorFaceNum];
	for(size_t i = 0;i != InteriorFaceNum; ++i)
		InteriorFaceA[i] = Interior[i];

	WallFaceA = new Face_2D* [WallFaceNum];
	for(size_t i = 0;i != WallFaceNum; ++i)
		WallFaceA[i] = Wall[i];
}