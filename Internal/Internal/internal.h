#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <vector>

//gets the address where a module of our game got loaded into:
uintptr_t GetModuleBaseAddress(const wchar_t* modName);

//find dynamic address a multi level pointer is pointing to:
uintptr_t FindDynamicAddress(uintptr_t ptr, std::vector<unsigned int> offsets);

//write new code to memory
void PatchInternal(BYTE* dst, BYTE* src, size_t size);

//class for replacing code with code that does nothing
class NopInternal
{
private:
	HANDLE hProcess; //holds the handle to our game
	BYTE* dst; //address of the code to patch
	size_t size; //size of the patch
	BYTE* originalCode; //backup of original code
	BYTE* nopCode; //code that does nothing
public:
	NopInternal(BYTE* dst, size_t size); //contructor
	~NopInternal(); //destructor
	void enable(); //enable the patch
	void disable(); //disable the patch
};

