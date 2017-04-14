#ifndef _MESH_CONSTRUCTFUN_H_
#define _MESH_CONSTRUCTFUN_H_
#define NDEBUG

#include "Mesh_2D.h"
#include "MeshDeclaration.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

using std::ios;
using std::istringstream;
using std::ostringstream;
using std::cout;
using std::endl;

extern const size_t MeshPerLine = 5;

template<typename T>
inline size_t MeshIndex(const T& End,const T& Beg)
{
	return (nullptr == End ? 0 : End - Beg + 1);
}

void Allocate_Mesh(const size_t &index,const size_t &Num)
{
	if(index == 10)
	{		
		Nodes = Num;
		NodeX = new double[Nodes];
		NodeY = new double[Nodes];
	}
	else if(index == 12)
	{
		Cells = Num;
		CellArray = new Cell_2D[Cells];
	}
	else if(index == 13)
	{
		Faces = Num;
		FaceArray = new Face_2D[Faces];
	}
	else
	{
		cout << "File : Mesh_zero.h   Line : 56" << endl;
	}
}

void ConstructNodes(size_t* const &ptrHexLine,std::ifstream &InFile_Mesh)
{
	InFile_Mesh >> std::dec;
	for(int i = 0;i != Nodes;++i)
	{
		InFile_Mesh >> NodeX[i] >> NodeY[i];
		#ifndef NDEBUG
		if(i%StepControl == 0)
		std::cout <<std::setiosflags(ios::scientific)<<std::setprecision(15)
				  << NodeX[i] << "  " << NodeY[i] <<std::setprecision(6)
				  << resetiosflags(ios::scientific) << std::endl;
		#endif
	}
	cout <<"Nodes Constructed" << endl;
}
void ConstructCells(size_t* const &ptrHexLine,const size_t &count)
{
	if(5 != count) 
	{
		std::cerr <<"Error : " <<  __FILE__ <<" : in function "<< __func__
				  <<" at line " <<__LINE__ << endl;
		system("pause");
		return;
	}
	if(3 == ptrHexLine[count - 1] )
	{
		for(int i = 0;i != Cells;++i)
			CellArray[i].celltype = 4;
			
	}
	else if(1 == ptrHexLine[count - 1])
	{
		for(int i = 0;i != Cells;++i)
			CellArray[i].celltype = 3;
	}
	else
	{
		std::cerr << "Error : " <<  __FILE__ <<" : in function "<< __func__
				  <<" at line " <<__LINE__ << endl;
		std::cerr << "wrong element type" << endl;
		system("pause");
	}
	#ifndef NDEBUG
	for(int i = 0;i != Cells;++i)
	{
			if(i%StepControl == 0)
				cout <<i<<"  "<<CellArray[i].celltype <<endl;	
	}
	cout << endl;
	#endif
}
void ConstructCells(size_t* const &ptrHexLine,std::ifstream &InFile_Mesh)
{
	int tmp;
	for(int i = 0;i != Cells;++i)
	{
		InFile_Mesh >> tmp;
		if(tmp == 1)
			CellArray[i].celltype = 3;
		else if(tmp == 3)
			CellArray[i].celltype = 4;
		else
		{
			cout << "wrong element index for 2D domain" << endl;
			system("pause");
		}
	}
	#ifndef NDEBUG
	for(int i = 0;i != Cells;++i)
	{
		if(i%StepControl == 0)
		cout <<i<<"  "<<CellArray[i].celltype <<endl;	
	}
	cout << endl;
	#endif
}
void PushNode(Cell_2D &Cell,const size_t &BegN,const size_t &EndN)
{
	if(nullptr == Cell.NodeX_C[0])
	{
		Cell.NodeX_C[0] = NodeX+BegN;
		Cell.NodeX_C[1] = NodeX+EndN;
		Cell.NodeY_C[0] = NodeY+BegN;
		Cell.NodeY_C[1] = NodeY+EndN;
	}
	else if(nullptr == Cell.NodeX_C[2] || nullptr == Cell.NodeX_C[3])
	{
		if((NodeX + BegN) == Cell.NodeX_C[1]  && (NodeY + BegN) == Cell.NodeY_C[1])
		{
			Cell.NodeX_C[2] = NodeX + EndN;
			Cell.NodeY_C[2] = NodeY + EndN;
		}
		else if((NodeX + EndN) == Cell.NodeX_C[1]  && (NodeY + EndN) == Cell.NodeY_C[1])
		{
			Cell.NodeX_C[2] = NodeX + BegN;
			Cell.NodeY_C[2] = NodeY + BegN;
		}
		else if((NodeX + BegN) == Cell.NodeX_C[0]  && (NodeY + BegN) == Cell.NodeY_C[0])
		{
			Cell.NodeX_C[3] = NodeX + EndN;
			Cell.NodeY_C[3] = NodeY + EndN;
		}
		else if((NodeX + EndN) == Cell.NodeX_C[0]  && (NodeY + EndN) == Cell.NodeY_C[0])
		{
			Cell.NodeX_C[3] = NodeX + BegN;
			Cell.NodeY_C[3] = NodeY + BegN;
		}
	}
}
void PushFace(Cell_2D &Cell,Face_2D *ptrF)
{
	int i;
	for(i = 0;Cell.Face_C[i] != nullptr;++i);
	Cell.Face_C[i] = ptrF;
}
void ConstructFacesCells(const size_t &FC,const size_t (&N_C)[MeshPerLine],const size_t &);
void ConstructFaces(size_t* const &ptrHexLine,std::ifstream &InFile_Mesh)
{
	const size_t &zone = ptrHexLine[0];
	const size_t &Beg_Face = ptrHexLine[1] -1;
	const size_t &End_Face = ptrHexLine[2];
	const size_t &bc_type = ptrHexLine[3];
	const size_t &face_type = ptrHexLine[4];
	size_t N_C[MeshPerLine] = {0,0,0,0,0};//set of node index and cell index 

	if(0 == face_type)
	{		
		InFile_Mesh >> std::hex;
		for(size_t i = Beg_Face;i < End_Face;++i)
		{
			InFile_Mesh >> N_C[0] >> N_C[1] >> N_C[2] >> N_C[3] >> N_C[4];

			ConstructFacesCells(i,N_C,bc_type);
			#ifndef NDEBUG
			if(i%StepControl == 0)
			{
				for(int k = 0;k < 5;++k)
				{
					cout << N_C[k] <<"  ";
				}
				cout << endl;
			}
			#endif

		}
		InFile_Mesh >> std::dec;			
	}
	else if(2 == face_type)
	{
		InFile_Mesh >> std::hex;
		for(size_t i = Beg_Face;i < End_Face;++i)
		{
			InFile_Mesh >> N_C[1] >> N_C[2] >> N_C[3] >> N_C[4];
			#ifndef NDEBUG
			if(i%StepControl == 0)
			{
				for(int k = 1;k < 5;++k)
				{
					cout << N_C[k] <<"  ";					
				}
				cout << endl;
			}
			#endif
			ConstructFacesCells(i,N_C,bc_type);
		}
		InFile_Mesh >> std::dec;
	}
	else
		cout <<__FILE__ <<"  "<<__LINE__<<"  "<<__func__<<endl;
}
void ConstructFacesCells(const size_t &FC,const size_t (&N_C)[MeshPerLine],const size_t &bc_type)
{
	size_t BegN = N_C[1], EndN = N_C[2], lhsC = N_C[3], rhsC = N_C[4];
	--BegN; --EndN; --lhsC; --rhsC;
	FaceArray[FC].bc_type = bc_type;
//
	FaceArray[FC].NodeX_F[0] = NodeX+BegN;
	FaceArray[FC].NodeX_F[1] = NodeX+EndN;
//
	FaceArray[FC].NodeY_F[0] = NodeY+BegN;
	FaceArray[FC].NodeY_F[1] = NodeY+EndN;
	FaceArray[FC].lhsCell = CellArray + lhsC;
//	
	PushNode(CellArray[lhsC],BegN,EndN);
	PushFace(CellArray[lhsC],FaceArray + FC);

	if(bc_type == 2)
	{
		FaceArray[FC].rhsCell = CellArray + rhsC;
		PushNode(CellArray[rhsC],EndN,BegN);
		PushFace(CellArray[rhsC],FaceArray + FC);
	}
}
void PeriodFaces(const size_t &F0,const size_t &F1);
void ConstructPeriodicFaces(size_t* const &ptrHexLine,std::ifstream &InFile_Mesh)
{
	const size_t &Beg = ptrHexLine[0] -1;
	const size_t &End = ptrHexLine[1];
	size_t F0 = 0;
	size_t F1 = 0;
	InFile_Mesh >> std::hex;
	for(size_t i = Beg;i < End;++i)
	{
		InFile_Mesh >> F0 >> F1;
		--F0;--F1;
		PeriodFaces(F0,F1);
	}
	InFile_Mesh >> std::dec;
}
void PeriodFaces(const size_t &F0,const size_t &F1)
{
	if(nullptr == FaceArray[F0].rhsCell && nullptr == FaceArray[F1].rhsCell)
	{
		FaceArray[F0].rhsCell = FaceArray[F1].lhsCell;
		FaceArray[F1].rhsCell = FaceArray[F0].lhsCell;
		FaceArray[F1].shadow = FaceArray + F0; 
	}
	else
	{
		cout << FaceArray[F0].rhsCell <<"  " << FaceArray[F1].rhsCell <<endl;
		cout <<__FILE__ <<"  "<<__LINE__<<"  "<<__func__<<endl;
		system("pause");
	}
}

#endif