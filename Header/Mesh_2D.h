#ifndef _MESH_2D_H_
#define _MESH_2D_H_

#include <cmath>

struct Cell_2D;
struct Face_2D;

struct Face_2D{

	size_t bc_type = 2;
	Face_2D *shadow = nullptr;
	double *NodeX_F[2] = {nullptr,nullptr};
	double *NodeY_F[2] = {nullptr,nullptr};
	Cell_2D *lhsCell = nullptr,*rhsCell = nullptr;
	double xc = 0.0  ,yc = 0.0, Area = 0.0;
	double Vx = 0.0,Vy = 0.0;
	inline void SetArea();
	inline void SetNormalV();
};

struct Cell_2D{

	size_t celltype = 0;
	double xc = 0.0, yc = 0.0, volume = 0.0;
	double *NodeX_C[4] = {nullptr,nullptr,nullptr,nullptr};
	double *NodeY_C[4] = {nullptr,nullptr,nullptr,nullptr};
	Face_2D *Face_C[4] = {nullptr,nullptr,nullptr,nullptr};
	inline void SetVolume();
};
inline void Cell_2D::SetVolume()
{
	if(3 == celltype) 
	{
		xc = (*NodeX_C[0] + *NodeX_C[1] + *NodeX_C[2])/3.0;
		yc = (*NodeY_C[0] + *NodeY_C[1] + *NodeY_C[2])/3.0;
		volume = 0.5* ((*NodeX_C[1] - *NodeX_C[0]) * (*NodeY_C[2] - *NodeY_C[0])
				 - (*NodeX_C[2] - *NodeX_C[0]) * (*NodeY_C[1] - *NodeY_C[0]));
	}
	else if(4 == celltype)
	{
		xc = 0.5*((*NodeX_C[0] + *NodeX_C[1] + *NodeX_C[2])/3.0 +
		          (*NodeX_C[0] + *NodeX_C[2] + *NodeX_C[3])/3.0);
		yc = 0.5*((*NodeY_C[0] + *NodeY_C[1] + *NodeY_C[2])/3.0 +
			      (*NodeY_C[0] + *NodeY_C[2] + *NodeY_C[3])/3.0);
		volume = 0.5*((*NodeX_C[2] - *NodeX_C[0]) * (*NodeY_C[3] - *NodeY_C[1])
				   -(*NodeY_C[2] - *NodeY_C[0]) * (*NodeX_C[3] - *NodeX_C[1]));
		if(volume < 0) volume = -volume;
	}
}
inline void Face_2D::SetArea()
{
	
	xc = 0.5*(*NodeX_F[0] + *NodeX_F[1]);
	yc = 0.5*(*NodeY_F[0] + *NodeY_F[1]);
	Area = sqrt(
				(*NodeY_F[1] - *NodeY_F[0]) * (*NodeY_F[1] - *NodeY_F[0]) +
				(*NodeX_F[1] - *NodeX_F[0]) * (*NodeX_F[1] - *NodeX_F[0])
			   );
}
inline void Face_2D::SetNormalV()
{
	double dy = (*NodeY_F[1] - *NodeY_F[0])/Area, 
	 	   dx = (*NodeX_F[1] - *NodeX_F[0])/Area;
	if(dy > -1.0E-12 && dy < 1.0E-12) dy = 0.0; 
	if(dx > -1.0E-12 && dx < 1.0E-12) dx = 0.0;  
	Vx = dy;Vy = -dx;
}
#endif