#pragma once
struct Layout
{
	const char* Name;
	int Index;
	int AttriSize; 
	int Stride; 
	int Offset;
	Layout(const char* name, const int attriSize): 
		Name(name), Index(0), AttriSize(attriSize),Stride(0), Offset(0)
	{

	}
};


