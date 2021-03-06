#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "../Header/MeshConstructFunc.h"

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;

void EraseParenthese(string &string_line,size_t &body)
{
	while(string_line.back() == ' ')
		string_line.pop_back();

	if(string_line.back() == '(') ++body;
	auto It = remove_if(string_line.begin(),string_line.end(),
									[](const char& c){return c == ')' || c == '(';});
	if(It == string_line.end()) body = 99;
	string_line.erase(It,string_line.end());
}
int StringToHex(size_t* const &ptrHexLine, istringstream &iss_line,size_t &count)
{
	string str[MeshPerLine];
	for(;iss_line >> str[count];++count);
	if(0 == count) return 0;
	#ifdef _ZERO_NDEBUG_FLIP
		cout << count << endl;
		for(int i = 0;i != count;++i)
			cout << str[i] << " ";
		cout << endl;
	#endif

	for(int i = 0;i != count; ++i)
		ptrHexLine[i] = stoi(str[i],nullptr,16);
	#ifdef _ZERO_NDEBUG_FLIP
		for(int i = 0;i != count;++i)
			cout << ptrHexLine[i] << " ";
		cout << endl;
	#endif
	return 0;
}
void HeadProcess(const size_t &index,size_t* const &ptrHexLine)
{
	Allocate_Mesh(index,ptrHexLine[2]);
}
void BodyProcess(const size_t &index,size_t &FaceCount,
				istringstream &iss_line,ifstream& InFile_Mesh,size_t* const &ptrHexLine)
{
	if(index == 10)
	{		
		ConstructNodes(ptrHexLine,InFile_Mesh);
	}
	else if(index == 12)
	{
		ConstructCells(ptrHexLine,InFile_Mesh);
	}
	else if(index == 13)
	{
		ConstructFaces(ptrHexLine,InFile_Mesh);
	}
	else if(index == 18)
	{
		ConstructPeriodicFaces(ptrHexLine,InFile_Mesh);
	}
	else
	{
		cout << "Invalid index during body processing" << endl;
		system("pause");
	}
}
int MeshConstruct(const string &s)
{
	size_t *ptrHexLine = new size_t[MeshPerLine];
	size_t line = 0, count = 0, index = 0,body = 0,FaceCount = 0;
	string string_line;
	ifstream InFile_Mesh;
	InFile_Mesh.open("../Mesh/"+ s +".cas");
	if(!InFile_Mesh)
	{
		cout << "file open failed  " <<__FILE__ <<" : "<<__LINE__<<"  "<<__func__<<endl; 
		return -99999;
	}
	while(getline(InFile_Mesh,string_line))
	{
		if(string_line.size() == 0) continue;
		cout << string_line << endl;
		body = 0;index = 0;	count = 0;
		for(int i = 0;i != MeshPerLine;++i)
			ptrHexLine[i] =  0;
		EraseParenthese(string_line,body);
		if(99 == body) continue;		
		istringstream iss_line(string_line);
		if(!(iss_line >> index) || !(index == 10 || index == 12 || index == 13 || index == 18))
			continue;
		StringToHex(ptrHexLine,iss_line,count);
		if(ptrHexLine[0] == 0)
			HeadProcess(index,ptrHexLine);
		else
		{
			if(body)
			{
				BodyProcess(index,FaceCount,iss_line,InFile_Mesh,ptrHexLine);
			}
			else if(12 == index)//
			{
				ConstructCells(ptrHexLine,count);
			}
		}
	}
	cout << "Faces " << Faces << " Nodes " << Nodes << " Cells " << Cells <<endl;
	delete []ptrHexLine;
	return 0;
}

int MeshCheck()
{
	for(size_t i = 0;i != Faces;++i)
	{
		#ifdef _ZERO_NDEBUG_FLIP
		if(i%StepControl == 0)
		if(FaceArray[i].bc_type != 12 &&  FaceArray[i].bc_type != 8)
		cout << "FaceArray : " << i <<"   bc_type : " << FaceArray[i].bc_type <<std::hex
			 <<"  "<< MeshIndex(FaceArray[i].NodeX_F[0] , NodeX)
			 <<"  "<< MeshIndex(FaceArray[i].NodeX_F[1] , NodeX)
			 <<"  "<< MeshIndex(FaceArray[i].lhsCell , CellArray)
			 <<"  "<< MeshIndex(FaceArray[i].rhsCell , CellArray)<< std::dec <<endl;
		else
		cout << "FaceArray : " << i <<"   bc_type : " << FaceArray[i].bc_type <<std::hex
			 <<"  "<< MeshIndex(FaceArray[i].NodeX_F[0] , NodeX)
			 <<"  "<< MeshIndex(FaceArray[i].NodeX_F[1] , NodeX)
			 <<"  "<< MeshIndex(FaceArray[i].lhsCell , CellArray)
			 <<"  "<< MeshIndex(FaceArray[i].rhsCell->ShadowC , CellArray)<< std::dec <<endl;	
		#endif
		#ifdef _QUAD_PERIODIC_FLIP
		if(FaceArray[i].Vx * FaceArray[i].Vy != 0.0)
		{
			cout << "FaceArray : " << i <<" Vx : " << FaceArray[i].Vx
				 <<" Vx : " << FaceArray[i].Vy <<" Area: "
				 <<std::setiosflags(ios::scientific) <<std::setprecision(16)
				 <<FaceArray[i].Area
				 <<std::resetiosflags(ios::scientific) <<std::setprecision(6)
				 << endl;
			system("pause");
		}
		#endif
		if(2 == FaceArray[i].bc_type)
		{
			if(FaceArray[i].lhsCell == nullptr || FaceArray[i].rhsCell == nullptr)
			{
				cout << "FaceArray : " << i <<" bc_type : " << FaceArray[i].bc_type
					 <<"Cell = -1" << endl;
				system("pause");
			}
			if(FaceArray[i].shadowF != nullptr)
			{
				cout<<"construct shadow faces for interior faces"<< endl;
				system("pause");
			}
		}
		else if(3 == FaceArray[i].bc_type)
		{
			if(FaceArray[i].lhsCell == nullptr || FaceArray[i].rhsCell != nullptr)
			{
				cout << "FaceArray : " << i <<" bc_type : " << FaceArray[i].bc_type
					 <<"lhsCell == nullptr || rhsCell != nullptr" << endl;
				system("pause");
			}
			if(FaceArray[i].shadowF != nullptr)
			{
				cout<<"constructed shadow faces for wall boundaries"<< endl;
				system("pause");
			}
		}
		else if(8 == FaceArray[i].bc_type || 12 == FaceArray[i].bc_type)
		{
			if(FaceArray[i].lhsCell == nullptr || FaceArray[i].rhsCell == nullptr)
			{
				cout << "FaceArray : " << i <<" bc_type : " << FaceArray[i].bc_type
					 <<"Cell = -1" << endl;
				system("pause");
			}
			if(FaceArray[i].shadowF == nullptr)
			{
				cout<<"Failed to construct shadowF faces for 8 & 12"<< endl;
				system("pause");
			}
		}
		else
		{
			cout << "FaceArray : " << i <<"unknown bc_type : " << FaceArray[i].bc_type << endl;
			system("pause");
		}
	}
	cout <<"Face Check Done" <<endl;
	for(size_t i = 0;i < Cells;++i)
	{
		if(nullptr != CellArray[i].ShadowC)
		{
			cout <<"CellArray : " << i <<endl;
			cout <<"  "<<__FILE__<<"  "<<__LINE__<<"  "<<__func__<<endl;
			system("pause");
		}
		if(3 == CellArray[i].celltype)
		{
			for(int k = 0;k != NumPerCell;++k)
			if(nullptr == CellArray[i].NodeX_C[k] || nullptr == CellArray[i].NodeY_C[k])
			{
				cout <<"Triangular Cell "<<"CellArray : " << i 
					 <<"  Node Index : " << k <<" = nullptr"<<endl;
				system("pause");
			}

			if((CellArray[i].NodeX_C[2] != CellArray[i].NodeX_C[3]) ||
			   (CellArray[i].NodeY_C[2] != CellArray[i].NodeY_C[3]))
			{
				cout <<"Triangular Cell "<<"CellArray : " << i 
					 <<"  Addrs of last two Nodes don't equal"<<endl;
				system("pause");
			}

			for(int k = 0;k != NumPerCell - 1;++k)
			if(nullptr == CellArray[i].Face_C[k])
			{
				cout <<"Triangular Cell "<<"CellArray : " << i 
					 <<"  Face Index : " << k <<" = nullptr"<<endl;
				system("pause");
			}

			if(nullptr != CellArray[i].Face_C[NumPerCell - 1])
			{
				cout <<"Triangular Cell "<<"CellArray : " << i 
					 <<"  Face Index : " << NumPerCell - 1 <<" != nullptr"<<endl;
				system("pause");
			}
		}
		else if(4 == CellArray[i].celltype)
		{
			for(int k = 0;k != NumPerCell;++k)
			if(nullptr == CellArray[i].NodeX_C[k] || nullptr == CellArray[i].NodeY_C[k])
			{
				cout <<"Quadrilateral Cell "<<"CellArray : " << i 
					 <<"  Node Index : " << k <<" = nullptr"<<endl;
				system("pause");
			}
			for(int k = 0;k != NumPerCell;++k)
			if(nullptr == CellArray[i].Face_C[k])
			{
				cout <<"Quadrilateral Cell "<<"CellArray : " << i 
					 <<"  Face Index : " << k <<" = nullptr"<<endl;
				system("pause");
			}
		}
		else
		{
			cout << "CellArray : " << i <<"unknown celltype : "
			     << CellArray [i].celltype << endl;
			system("pause");
		}
	}
	cout <<"Cell Check Done" <<endl;
	return 0;
}
int MeshOutput(const string& s)
{
	cout << "Mesh output verifing..."<<endl;
	ofstream OutFile_Mesh;
	OutFile_Mesh.open("../MeshOutPut/"+ s +".plt");
	if(!OutFile_Mesh)
	{
		cout << __FILE__ <<"  " << __func__ <<"  " << __LINE__ 
			 <<"  "<<"file open failed" << endl; 
		return -99999;
	}
	ostringstream fname,zonename,dataNE;
	zonename<<"ZONE T = \"Mesh\"\n";
	dataNE<<"Nodes="<<Nodes<<", Elements="<<Cells<<", ZONETYPE=FEQuadrilateral\n";
	string tecformat[5]={"VARIABLES = \"X\",\"Y\"\n",
						zonename.str().c_str(),dataNE.str().c_str(),"DATAPACKING=BLOCK\n",
						"VarLocation=([1-2]=NODAL)\n"};
	OutFile_Mesh << tecformat[0]<<tecformat[1]<<tecformat[2]<<tecformat[3]<<tecformat[4];
	for(size_t i = 0;i != Nodes;++i)
	{
		OutFile_Mesh << NodeX[i] <<"   ";
		if((i+1)%16 == 0)
			OutFile_Mesh << endl;
	}
	OutFile_Mesh << endl;
	for(size_t i = 0;i != Nodes;++i)
	{
		OutFile_Mesh << NodeY[i] <<"   ";
		if((i+1)%16 == 0)
			OutFile_Mesh << endl;
	}
	OutFile_Mesh << endl;
	for(size_t i = 0;i != Cells;++i)
	{
		OutFile_Mesh << MeshIndex(CellArray[i].NodeX_C[0] , NodeX)<< " "
					 << MeshIndex(CellArray[i].NodeX_C[1] , NodeX)<< " "
					 << MeshIndex(CellArray[i].NodeX_C[2] , NodeX)<< " " 
					 << MeshIndex(CellArray[i].NodeX_C[3] , NodeX)<< endl;
	}
	cout <<"Mesh Output Done" <<endl;
	OutFile_Mesh.close();
	cout <<"Face Output verifing..."<<endl;
	ofstream OutFile_Face;
	OutFile_Face.open("../MeshOutPut/13_" + s + ".dat");
	if(!OutFile_Face)
	{
		cout << __FILE__ <<"  " << __func__ <<"  " << __LINE__ 
			 <<"  "<<"file open failed" << endl; 
		system("pause");
	}
	OutFile_Face << std::hex;
	for(size_t i = 0;i != Faces;++i)
	{
		OutFile_Face << MeshIndex(FaceArray[i].NodeY_F[0] , NodeY)<<" "
					 << MeshIndex(FaceArray[i].NodeY_F[1] , NodeY)<<" "
					 << MeshIndex(FaceArray[i].lhsCell , CellArray)<<" "
					 << MeshIndex(FaceArray[i].rhsCell , CellArray)<<endl;
	}
	OutFile_Face.close();
//
	OutFile_Face.open("../MeshOutPut/18_" + s +".dat");
	if(!OutFile_Face)
	{
		cout << __FILE__ <<"  " << __func__ <<"  " << __LINE__ 
			 <<"  "<<"file open failed" << endl; 
		system("pause");
	}
	OutFile_Face << std::hex;
	for(size_t i = 0;i != Faces;++i)
	{
		if(8 == FaceArray[i].bc_type)
		OutFile_Face << MeshIndex(FaceArray[i].shadowF , FaceArray)<<" "
					 << i + 1 <<endl;
	}
	OutFile_Face.close();
	ofstream OutFile_Node;
	OutFile_Node.open("../MeshOutPut/10_" + s + ".dat");
	if(!OutFile_Node)
	{
		cout << __FILE__ <<"  " << __func__ <<"  " << __LINE__ 
			 <<"  "<<"file open failed" << endl; 
		system("pause");
	}
	for(size_t i = 0;i != Nodes;++i)
	{
		OutFile_Node <<std::setiosflags(ios::scientific) <<std::setprecision(16)
					 <<NodeX[i]<<"  "<<NodeY[i] << endl;
	}
	OutFile_Node.close();
	cout <<"Face Output Done"<<endl;
	return 0;
}
