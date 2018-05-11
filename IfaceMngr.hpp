#pragma once

namespace Iface
{
	class IfaceMngr
	{
	public:

		class InterfaceReg
		{
		private:

			using InstantiateInterfaceFn = void*(*)();

		public:

			InstantiateInterfaceFn m_CreateFn;
			const char *m_pName;

			InterfaceReg *m_pNext;
		};

		template<typename T>
		static T *getIface(const char *modName, const char *ifaceName, bool exact = false)
		{
			T *iface = nullptr;
			InterfaceReg *ifaceRegList;
			int partMatchLen = strlen(ifaceName);

			DWORD ifaceFn = reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleA(modName), "CreateInterface"));

			if (!ifaceFn)
				return nullptr;

			unsigned int jmpStart = (unsigned int)(ifaceFn)+4;
			unsigned int jmpTarget = jmpStart + *(unsigned int*)(jmpStart + 1) + 5;

			ifaceRegList = **reinterpret_cast<InterfaceReg***>(jmpTarget + 6);

			for (InterfaceReg *cur = ifaceRegList; cur; cur = cur->m_pNext)
			{
				if (exact == true)
				{
					if (strcmp(cur->m_pName, ifaceName) == 0)
						iface = reinterpret_cast<T*>(cur->m_CreateFn());
				}
				else
				{
					if (!strncmp(cur->m_pName, ifaceName, partMatchLen) && std::atoi(cur->m_pName + partMatchLen) > 0)
						iface = reinterpret_cast<T*>(cur->m_CreateFn());
				}
			}
			return iface;
		}
	};
}