#pragma once

namespace VT
{
	template<typename T>
	__forceinline static T vfunc(void *base, int index)
	{
		DWORD *vTabella = *(DWORD**)base;
		return (T)vTabella[index];
	}
}