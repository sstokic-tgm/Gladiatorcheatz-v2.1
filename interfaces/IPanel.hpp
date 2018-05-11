#pragma once

class IPanel
{
public:
    const char *GetName(unsigned int vguiPanel)
    {
        typedef const char *(__thiscall* tGetName)(void*, unsigned int);
        return VT::vfunc<tGetName>(this, 36)(this, vguiPanel);
    }
#ifdef GetClassName
#undef GetClassName
#endif
    const char *GetClassName(unsigned int vguiPanel)
    {
        typedef const char *(__thiscall* tGetClassName)(void*, unsigned int);
        return VT::vfunc<tGetClassName>(this, 37)(this, vguiPanel);
    }
};
