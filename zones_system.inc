#include <a_samp>
#include <streamer>
#include <Pawn.CMD>
#include <foreach>

#define MAX_CELLCENTER	17

enum _CellCenter
{
	STREAMER_TAG_OBJECT:_CellCenter_objectid,
	STREAMER_TAG_AREA:  _CellCenter_areaid,
	Icon,
	STREAMER_TAG_3D_TEXT_LABEL:_CellCenter_3dtext
}
new CellCenter[MAX_CELLCENTER][_CellCenter];

stock CellCenter_Add(Float:cell_X, Float:cell_Y, Float:cell_Z, Float:Size)
{
	new temp[64];
	for(new i; i < MAX_CELLCENTER; i++)
	{
		if(!IsValidDynamicObject(CellCenter[i][_CellCenter_objectid]))
		{
			CellCenter[i][_CellCenter_objectid] = CreateDynamicObject(3763,cell_X,cell_Y,cell_Z+33.1986,0,0,0);
			CellCenter[i][_CellCenter_areaid] = CreateDynamicSphere(cell_X,cell_Y,cell_Z, Size);
			CellCenter[i][Icon] = CreateDynamicMapIcon(cell_X,cell_Y,cell_Z,19,0, -1, -1 ,-1, 900000000.0);
			format(temp, sizeof temp, "Вышка №%d\nРадиус действия: %.1f метров", i, Size);
			CellCenter[i][_CellCenter_3dtext] = CreateDynamic3DTextLabel(temp,0xFF0000FF,cell_X,cell_Y,cell_Z+66.1986,100.0,INVALID_PLAYER_ID,INVALID_VEHICLE_ID,0);
			return 1;
		}
	}
	return 1;
}

public OnGameModeInit()
{
	print("система вышек загружается");
	CellCenter_Add(942.094, -1516.089, 33.171,800.0);
	CellCenter_Add(2375.633, -1643.686, 12.523,800.0);
	CellCenter_Add(1728.527, -2359.898, 27.651,800.0);
	CellCenter_Add(-315.602, -937.691, 48.962,400);
	CellCenter_Add(-1083.383, -917.138, 128.218,400);
	CellCenter_Add(-1037.177, -1616.988, 75.367,400);
	CellCenter_Add(-2232.124, -2317.402, 29.615,800);
	CellCenter_Add(-2181.459, 406.527, 58.549,1200);
	CellCenter_Add(-1470.266, 2648.456, 54.835,400);
	CellCenter_Add(820.124, -377.294, 25.428,800);
	CellCenter_Add(1452.705, 233.028, 18.450,550);
	CellCenter_Add(-128.906, -19.326, 2.117,600);
	CellCenter_Add(1962.240, 1908.531, 129.937,1300);
	CellCenter_Add(-445.270, 2787.730, 87.030,400);
	CellCenter_Add(-996.307, 1781.557, 66.350,500);
	CellCenter_Add(-149.903, 1030.751, 18.742,700);
	CellCenter_Add(2459.766, -43.717, 25.484,600);
	print("система вышек загружена");

	SetTimer("CellWatch", 3000, true);
	#if defined dz_OnGameModeInit
		return dz_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit dz_OnGameModeInit
#if defined dz_OnGameModeInit
	forward dz_OnGameModeInit();
#endif


forward CellWatch();
public CellWatch()
{
	foreach(new id: Player)
	{
		if(GetPlayerNumberDynamicAreas(id) == 0) return SendClientMessage(id, -1, "вы не в зоне");
		new Float:pos_x, Float:pos_y, Float:pos_z, Float:distance = 6000.0, Float:dis, closest_zoneid, STREAMER_TAG_AREA:zoneid;
		//GetPlayerDynamicAreas(id, zones, 17);
		GetPlayerPos(id, pos_x, pos_y, pos_z);
		for(new a; a < MAX_CELLCENTER; a++) {
			zoneid = CellCenter[a][_CellCenter_areaid];
			if( IsValidDynamicArea(zoneid) && IsPlayerInDynamicArea(id, zoneid) ) { 
				Streamer_GetDistanceToItem(pos_x, pos_y, pos_z, STREAMER_TYPE_AREA, zoneid, dis, 3);
				if( distance > dis ) {
					distance = dis;
					closest_zoneid = a;
				}
			}
		}
		SendClientMessagef(id, -1, "ближайшая зона %d", closest_zoneid);
	}
	return 1;
}
#endinput
public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA: areaid)
{
	SendClientMessage(playerid, -1, "enter");

	#if defined dz_OnPlayerEnterDynamicArea
		return dz_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA: areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerEnterDynamicArea
	#undef OnPlayerEnterDynamicArea
#else
	#define _ALS_OnPlayerEnterDynamicArea
#endif
#define OnPlayerEnterDynamicArea dz_OnPlayerEnterDynamicArea
#if defined dz_OnPlayerEnterDynamicArea
	forward dz_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA: areaid);
#endif

public OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA: areaid)
{
	SendClientMessage(playerid, -1, "leave");

	#if defined dz_OnPlayerLeaveDynamicArea
		return dz_OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA: areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerLeaveDynamicArea
	#undef OnPlayerLeaveDynamicArea
#else
	#define _ALS_OnPlayerLeaveDynamicArea
#endif
#define OnPlayerLeaveDynamicArea dz_OnPlayerLeaveDynamicArea
#if defined dz_OnPlayerLeaveDynamicArea
	forward dz_OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA: areaid);
#endif