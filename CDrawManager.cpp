#include "CDrawManager.h"
//===================================================================================
CDrawManager pDrawing;
CDrawManager_t pDrawing_t;

#define ESP_HEIGHT 14
//===================================================================================
void CDrawManager::Initialize( )
{
	if ( Iface.Surface == NULL )
		return;

	Iface.Engine->GetScreenSize( gScreenSize.iScreenWidth, gScreenSize.iScreenHeight );
	m_Font = Iface.Surface->CreateFont( );	//This is better than Arial
	Iface.Surface->SetFontGlyphSet( m_Font, "System", ESP_HEIGHT, 94, 0, 0, 0x200 );
}
//===================================================================================
void CDrawManager_t::Initialize()
{
	if (Iface.Surface == NULL)
		return;

	Iface.Engine->GetScreenSize(gScreenSize.iScreenWidth, gScreenSize.iScreenHeight);
	m_System = Iface.Surface->CreateFont();
	m_Courier = Iface.Surface->CreateFont();
	Iface.Surface->SetFontGlyphSet(m_System, "System", ESP_HEIGHT, 500, 0, 0, 0x200);
	Iface.Surface->SetFontGlyphSet(m_Courier, "System", ESP_HEIGHT, 300, 0, 0, 0x200);
}
//===================================================================================
Color CDrawManager::GetPlayerColor(C_Entity* pPlayer)
{
	if (pPlayer->GetIndex() == g_Options.iAimbotIndex)
		return Color::Green();
	else if (!g_Options.PlayerMode[pPlayer->GetIndex()])
		return Color(0, 255, 255, 255);
	else if (g_Options.PlayerMode[pPlayer->GetIndex()] == 2)
		return Color::Yellow();
	else
	{

		switch (pPlayer->GetTeamNum())
		{
		case 2: //RED
			return Color(244, 66, 66, 255);
		case 3: //BLU
			return Color::Blue();
		default:
			return Color(0, 0, 0, 0);
		}
	}

	return Color(0, 0, 0, 0); //no reason for this to be here, i just wanna look smart
}
//===================================================================================
void CDrawManager::DrawString( int x, int y, Color clrColor, const wchar_t *pszText)
{
	if( pszText == NULL )
		return;

	Iface.Surface->DrawSetTextPos( x, y );
	Iface.Surface->DrawSetTextFont( m_Font );
	Iface.Surface->DrawSetTextColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	Iface.Surface->DrawPrintText( pszText, wcslen( pszText ) );
}
//===================================================================================
void CDrawManager::DrawString( int x, int y, Color clrColor, const char *pszText, ... )
{
	if( pszText == NULL )
		return;

	va_list va_alist;
	char szBuffer[1024] = { '\0' };
	wchar_t szString[1024] = { '\0' };

	va_start( va_alist, pszText );
	vsprintf_s( szBuffer, pszText, va_alist );
	va_end( va_alist );

	wsprintfW( szString, L"%S", szBuffer );

	Iface.Surface->DrawSetTextPos( x, y );
	Iface.Surface->DrawSetTextFont( m_Font );
	Iface.Surface->DrawSetTextColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	Iface.Surface->DrawPrintText( szString, wcslen( szString ) );
}
//===================================================================================
void CDrawManager_t::DrawString(int font, int x, int y, Color clrColor, const char *pszText, ...)
{
	if (pszText == NULL)
		return;

	va_list va_alist;
	char szBuffer[1024] = { '\0' };
	wchar_t szString[1024] = { '\0' };

	va_start(va_alist, pszText);
	vsprintf_s(szBuffer, pszText, va_alist);
	va_end(va_alist);

	wsprintfW(szString, L"%S", szBuffer);

	Iface.Surface->DrawSetTextPos(x, y);
	Iface.Surface->DrawSetTextFont(font ? m_Courier : m_System);
	Iface.Surface->DrawSetTextColor(clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a());
	Iface.Surface->DrawPrintText(szString, wcslen(szString));
}
//===================================================================================
byte CDrawManager::GetESPHeight( )
{
	return ESP_HEIGHT;
}
//===================================================================================
void CDrawManager::DrawLine(int x, int y, int x1, int y1, Color clrColor)
{
	Iface.Surface->DrawSetColor(clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a());
	Iface.Surface->DrawLine(x, y, x1, y1);
}
//===================================================================================
void CDrawManager::DrawLineEx(int x, int y, int w, int h, Color clrColor)
{
	Iface.Surface->DrawSetColor(clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a());
	Iface.Surface->DrawLine(x, y, x + w, y + h);
}
//===================================================================================
void CDrawManager::DrawRect( int x, int y, int w, int h, Color clrColor)
{
	Iface.Surface->DrawSetColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	Iface.Surface->DrawFilledRect( x, y, x + w, y + h );
}
//===================================================================================
void CDrawManager::OutlineRect( int x, int y, int w, int h, Color clrColor)
{
	Iface.Surface->DrawSetColor( clrColor.r(), clrColor.g(), clrColor.b(), clrColor.a() );
	Iface.Surface->DrawOutlinedRect( x, y, x + w, y + h );
}
//===================================================================================
void CDrawManager::DrawBox( Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius )
{
	Vector vScreen;

	if( !WorldToScreen( vOrigin, vScreen ) )
		return;

	int radius2 = radius<<1;

	OutlineRect( vScreen.x - radius + box_width, vScreen.y - radius + box_width, radius2 - box_width, radius2 - box_width, Color::Black() );
	OutlineRect( vScreen.x - radius - 1, vScreen.y - radius - 1, radius2 + ( box_width + 2 ), radius2 + ( box_width + 2 ), Color::Black() );
	DrawRect( vScreen.x - radius + box_width, vScreen.y - radius, radius2 - box_width, box_width, Color( r, g, b, alpha ));
	DrawRect( vScreen.x - radius, vScreen.y + radius, radius2, box_width, Color( r, g, b, alpha ));
	DrawRect( vScreen.x - radius, vScreen.y - radius, box_width, radius2, Color( r, g, b, alpha ));
	DrawRect( vScreen.x + radius, vScreen.y - radius, box_width, radius2 + box_width, Color( r, g, b, alpha ) );
}
//===================================================================================
bool CDrawManager::WorldToScreen( Vector &vOrigin, Vector &vScreen )
{
	const matrix3x4& worldToScreen = Iface.Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

	float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
	vScreen.z = 0; //Screen doesn't have a 3rd dimension.

	if( w > 0.001 ) //If the object is within view.
	{
		float fl1DBw = 1 / w; //Divide 1 by the angle.
		vScreen.x = (gScreenSize.iScreenWidth / 2) + ( 0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * fl1DBw) * gScreenSize.iScreenWidth + 0.5); //Get the X dimension and push it in to the Vector.
		vScreen.y = (gScreenSize.iScreenHeight / 2) - ( 0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * fl1DBw) * gScreenSize.iScreenHeight + 0.5); //Get the Y dimension and push it in to the Vector.
		return true;
	}

	return false;
}