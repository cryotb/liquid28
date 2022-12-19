#pragma once
#include "SDK.h"

class C_Cast
{
public:
	template< class CCastBreaker>

	CCastBreaker Cast(C_Entity* entity, DWORD &static_castPtr)
	{
		auto pThis = reinterpret_cast<CCastBreaker*>(reinterpret_cast<DWORD>(entity) + static_castPtr);
		return *pThis;
	}

	template< class CCastBreaker_t>

	CCastBreaker_t Cast_t(C_Entity* entity, DWORD &static_castPtr, int pIncrement)
	{
		auto pThis = reinterpret_cast<CCastBreaker_t*>(reinterpret_cast<DWORD>(entity) + static_castPtr + pIncrement);
		return *pThis;
	}
};

extern C_Cast pCast;