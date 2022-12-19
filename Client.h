#pragma once

#include "SDK.h"
#include <d3d9.h>

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, C_UserCmd* pCommand);
int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding);
void __fastcall Hooked_FSN(PVOID CHLClient, void *_this, ClientFrameStage_t stage);
bool __fastcall Hooked_NetMsg(PVOID CHLClient, INetChannel* pNetChan, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice);
void __fastcall Hooked_OverrideView(void* _this, void* _edx, C_SetupV* pSetup);
void __stdcall hkReset(PVOID ptr, IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* pPresentationParameters);
void __stdcall hkEndScene(PVOID ptr, IDirect3DDevice9* thisptr);