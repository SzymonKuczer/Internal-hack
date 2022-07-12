
#include "internal.h"

uintptr_t GetModuleBaseAddress(const wchar_t* modName)
{
	return (uintptr_t)GetModuleHandleW(modName);
}

uintptr_t FindDynamicAddress(uintptr_t ptr, std::vector<unsigned int> offsets)
{
	//loop over the offsets
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ptr = *(BYTE*)ptr; //dereference pointer
		ptr += offsets[i]; //add the currents offset to the address
	}
	//return the address the multi level pointer is pointing at currently
	return ptr;
}

void PatchInternal(BYTE* dst, BYTE* src, size_t size)
{
	DWORD oldprotect; //variable to hold a backup of our old protection

	//make the memory writable and save the old protection in oldprotect
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size); //write the new opcodes to the target location

	//restore our old protection
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

NopInternal::NopInternal(BYTE* dst, size_t size)
	:hProcess{ hProcess }, dst{ dst }, size{ size }, originalCode{ new BYTE[size] }, nopCode{ new BYTE[size] }
{
	memset(nopCode, 0x90, size); //initalize our nopCode
	memcpy(originalCode, dst, size); //backup our original code
}

void NopInternal::enable() //enable our patch
{
	//write the ocode that does nothing to memory:
	PatchInternal(dst, nopCode, size);
}

void NopInternal::disable()
{
	//write the original code back to memory:
	PatchInternal(dst, originalCode, size);
}

NopInternal::~NopInternal()
{
	delete[] this->originalCode;
	delete[] this->nopCode;
}