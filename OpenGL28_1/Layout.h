#pragma once
struct Layout
{
	const char* Name;
	int Index;
	int AttriSize; 
	int Stride; 
	int Offset;
	int UnitSize;
	Layout(const char* name, const int attriSize, const int unitSize): 
		Name(name), Index(0), AttriSize(attriSize),Stride(0), Offset(0), UnitSize(unitSize)
	{

	}
};


