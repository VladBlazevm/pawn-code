public OnGameModeInit()
{
	new Float:r = 68.383377, Float:x = 2737.723632, Float:y = -1760.278808, Float:z = 44.149105,
		Float:xr, Float:yr, Float:deg_orig = 360/42, Float:deg = deg_orig;
	for(new i; i < 45; ++i) {
		xr = x + r * floatsin(deg, degrees);
		yr = y + r * floatcos(deg, degrees);
		CreateDynamicObject(18842, xr, yr, z, 0.0, 0.0, 0.0);
		deg = deg+deg_orig;
	}

	#if defined gravypushka_OnGameModeInit
		return gravypushka_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit gravypushka_OnGameModeInit
#if defined gravypushka_OnGameModeInit
	forward gravypushka_OnGameModeInit();
#endif

CMD:mdm(playerid) {
	PlayerMpID[playerid] = MP_GRAVYPUSHKA;
	SetPlayerPos(playerid, 2737.723632, -1760.278808, 44.149105);
	return 1;
}
CMD:testmdm(playerid) { // проверить на себе
	CallRemoteFunction("OnPlayerTakeDamage", "ddfdd", 0, playerid, 1.0, 38, 0);
	return 1;
}
public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	if(weaponid == 38 && PlayerMpID[issuerid] == MP_GRAVYPUSHKA) {
		new Float:x, Float:y, Float:z;
		GetPlayerVelocity(playerid, x, y, z);
		SetPlayerVelocity(playerid, 0.0, 0.0, z+2.0);
	}

	#if defined gravypushka_OnPlayerTakeDamage
		return gravypushka_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerTakeDamage
	#undef OnPlayerTakeDamage
#else
	#define _ALS_OnPlayerTakeDamage
#endif

#define OnPlayerTakeDamage gravypushka_OnPlayerTakeDamage
#if defined gravypushka_OnPlayerTakeDamage
	forward gravypushka_OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
#endif