#include <a_samp>
#include <streamer>
#include <Pawn.CMD>
#include <a_mysql>

#define business_system

static MySQL:DB_LocalHandle;


#define BUSINESS_TYPE_247 0
#define BUSINESS_TYPE_GAS_STATION 1
#define BUSINESS_TYPE_HOUSE 2

#define MAX_BUSINESSES_247 26
#define MAX_GAS_STATIONS 14
#define MAX_BUSINESSES_HOUSES 63
//new bizid = 0, fuelid = 0;

new houses_values;

enum b247_Info
{ // No comment..
	bDB_ID,         // Номер в БД.
	bName[30], // название бизнеса
	bVladel[MAX_PLAYER_NAME], // ник владельца
	bool:bBuy_status, // куплен ли бизнес
	bool:bOpen, // Открыт/Закрыт бизнес - 1 открыт, 2 закрыт
	bMoney, // кол-во денег на счету бизнеса
	STREAMER_TAG_3D_TEXT_LABEL:b3DText, // текст у бизнеса
	STREAMER_TAG_MAP_ICON:bMapIcon, // иконка на карте
	STREAMER_TAG_PICKUP:bPickup, // пикап бизнеса
	Float:bExit_Pos_X,
	Float:bExit_Pos_Y,
	Float:bExit_Pos_Z, // координаты выхода из бизнеса
	bBuy_price, // цена покупки
	bWeapons[43]
}
enum bFuel_Info {
	bDB_ID,         // Номер в БД.
	bName[30], // название бизнеса
	bVladel[MAX_PLAYER_NAME], // ник владельца
	bool:bBuy_status, // куплен ли бизнес
	bool:bOpen, // Открыт/Закрыт бизнес - 1 открыт, 2 закрыт
	bMoney, // кол-во денег на счету бизнеса
	STREAMER_TAG_3D_TEXT_LABEL:b3DText,
	STREAMER_TAG_MAP_ICON:bMapIcon, 
	STREAMER_TAG_PICKUP:bPickup, 
	STREAMER_TAG_AREA:bFuelArea, 
	Float:bExit_Pos_X,
	Float:bExit_Pos_Y,
	Float:bExit_Pos_Z, 
	bBuy_price 
}
enum bHouse_Info {
	bDB_ID,  
	bName[30],
	bVladel[MAX_PLAYER_NAME],
	bool:bBuy_status,
	bool:bOpen,
	bMoney,
	bHouse_Class,
	STREAMER_TAG_3D_TEXT_LABEL:b3DText,
	STREAMER_TAG_MAP_ICON:bMapIcon,
	STREAMER_TAG_PICKUP:bPickup,
	Float:bExit_From_Int_Pos_X,
	Float:bExit_From_Int_Pos_Y,
	Float:bExit_From_Int_Pos_Z,
	Float:bExit_Pos_X,
	Float:bExit_Pos_Y,
	Float:bExit_Pos_Z,
	bBuy_price, // цена покупки
	Float:bHouse_Interioir_X,
	Float:bHouse_Interioir_Y,
	Float:bHouse_Interioir_Z,
	bHouse_Interioir_ID,
	Float:bHouse_CameraPos_X, // interpolate
	Float:bHouse_CameraPos_Y,
	Float:bHouse_CameraPos_Z,
	Float:bHouse_CameraLookAt_X,
	Float:bHouse_CameraLookAt_Y,
	Float:bHouse_CameraLookAt_Z,
	Float:bVeh_Pos_X,
	Float:bVeh_Pos_Y,
	Float:bVeh_Pos_Z,
	Float:bVeh_Pos_Angle,
	bVeh_Slot,
	bWeapons[43],
	Float:bSafe_Pos_X,
	Float:bSafe_Pos_Y,
	Float:bSafe_Pos_Z
}

new const weapon_names[43][26] = {
	"Кастет",
	"Клюшка для гольфа",
	"Полицейская дубинка",
	"Нож",
	"Бейсбольная бита",
	"Лопата",
	"Кий",
	"Катана",
	"Бензопила",
	"Двухсторонний дилдо",
	"Дилдо",
	"Вибратор",
	"Серебряный вибратор",
	"Букет цветов",
	"Трость",
	"Граната",
	"Слезоточивый газ",
	"Коктейль Молотова",
	"Пистолет 9мм",
	"Пистолет 9мм с глушителем",
	"Пистолет Дезерт Игл",
	"Обычный дробовик",
	"Обрез",
	"Скорострельный дробовик",
	"Узи",
	"MP5",
	"Автомат Калашникова",
	"Винтовка M4",
	"Tec-9",
	"Охотничье ружье",
	"Снайперская винтовка",
	"РПГ",
	"Самонаводящиеся ракеты HS",
	"Огнемет",
	"Миниган",
	"Сумка с тротилом",
	"Детонатор к сумке",
	"Баллончик с краской",
	"Огнетушитель",
	"Фотоаппарат",
	"Прибор ночного видения",
	"Тепловизор",
	"Парашют"
};

new Gas_stations[MAX_GAS_STATIONS][bFuel_Info];
new Businesses_247[MAX_BUSINESSES_247][b247_Info];
new Businesses_Houses[MAX_BUSINESSES_HOUSES][bHouse_Info];

new exitdoor;
new zakuppickup;

stock Player_Buy_Business_247_ID[MAX_PLAYERS] = {-1, ...};
stock Player_Buy_Gas_stations_ID[MAX_PLAYERS] = {-1, ...};

stock Player_Entered_247_Index[MAX_PLAYERS] = {-1, ...};

stock Player_Business_247_DB_ID[MAX_PLAYERS] = {-1, ...};
stock Player_Gas_stations_DB_ID[MAX_PLAYERS] = {-1, ...};
stock Player_Business_House_DB_ID[MAX_PLAYERS] = {-1, ...};
stock Guest_House_ID[MAX_PLAYERS] = {-1, ...};
stock bool:Guest_House_Safe_Access[MAX_PLAYERS] = {false, ...};

new Player_Change_Business_Type[MAX_PLAYERS] = {-1, ...},
	Player_Change_Business_ID[MAX_PLAYERS] = {-1, ...};
new Player_Pickup_Sale_House_ID[MAX_PLAYERS] = {-1, ...},
	bool:Player_Is_Viewed_Interior[MAX_PLAYERS] = {false, ...},
	bool:Player_Enter_House[MAX_PLAYERS] = {false, ...},
	Player_Entered_House_DB_ID[MAX_PLAYERS] = {-1, ...},
	Player_Viewed_Interior_Second[MAX_PLAYERS] = {0, ...},
	Player_Take_House_Weapon_ID[MAX_PLAYERS];

stock Business_FindBy_DBUID(const business_uid) {   // (Добавил. Возвращает номер бизнеса в массиве по его номеру в БД).
	for(new businessid = 0; businessid < MAX_BUSINESSES_247; ++businessid) {
		if( Businesses_247[businessid][bDB_ID] == business_uid ) {
			return businessid;
		}
	}
	return -1;
}
stock Gas_FindBy_DBUID(const gas_uid) {
	for(new gasid = 0; gasid < MAX_BUSINESSES_247; ++gasid) {
		if( Gas_stations[gasid][bDB_ID] == gas_uid ) {
			return gasid;
		}
	}
	return -1;
}
stock House_FindBy_DBUID(const house_uid) {
	for(new houseid = 0; houseid < MAX_BUSINESSES_HOUSES; ++houseid) {
		if( Businesses_Houses[houseid][bDB_ID] == house_uid ) {
			return houseid;
		}
	}
	return -1;
}
stock Business_ResetAll() {
	for(new businessid = 0; businessid < MAX_BUSINESSES_247; ++businessid) {
		Businesses_247[businessid][bDB_ID] = 0;
		//Gas_stations[businessid][bDB_ID] = 0;
	}
}
stock Business_LoadAll_DB(const MySQL:db_handle) {
	Business_ResetAll();
	mysql_set_charset("cp1251");
	DB_LocalHandle = db_handle; // В дальнейшем обращаться в этом файле только через DB_LocalHandle
	Set_LocalHandle_Business_Dialog(DB_LocalHandle);
	new Cache:mysql_cache = mysql_query(DB_LocalHandle, "SELECT * FROM `business` WHERE `Type` != '2'", true);

	new test_start = GetTickCount(), test_end;
	new rows,
		Float:x, Float:y, Float:z,
		//#pragma warning disable 203
		type, id_biz, id_gas, id_house,
		bizstock, gasstock, housestock
	;
	cache_get_row_count(rows);
	if(!rows) return 0;

	exitdoor = CreateDynamicPickup(19130, 2, -25.2042,-141.4801,1003.5469);
	zakuppickup = CreateDynamicPickup(1550, 2, -22.1465,-138.6281,1003.5469);
	for( new row_id; row_id < rows; row_id++ ) {
		cache_get_value_name_int(row_id, "Type", type);
		if( type == BUSINESS_TYPE_247 ) {
			cache_get_value_name_int(row_id, "ID", id_biz);
			bizstock = Business_FindBy_DBUID(0);
			if( bizstock != -1 ) {
				Businesses_247[bizstock][bDB_ID] = id_biz;
				cache_get_value_name(row_id, "Name", Businesses_247[bizstock][bName], 30);
				cache_get_value_name(row_id, "Vladel", Businesses_247[bizstock][bVladel], MAX_PLAYER_NAME);
				cache_get_value_name_bool(row_id, "Buy_status", Businesses_247[bizstock][bBuy_status]);
				cache_get_value_name_bool(row_id, "Open", Businesses_247[bizstock][bOpen]);
				cache_get_value_name_int(row_id, "Money", Businesses_247[bizstock][bMoney]);
				cache_get_value_name_float(row_id, "X", x);
				cache_get_value_name_float(row_id, "Y", y); 
				cache_get_value_name_float(row_id, "Z", z);
				cache_get_value_name_float(row_id, "exitX", Businesses_247[bizstock][bExit_Pos_X]);
				cache_get_value_name_float(row_id, "exitY", Businesses_247[bizstock][bExit_Pos_Y]);
				cache_get_value_name_float(row_id, "exitZ", Businesses_247[bizstock][bExit_Pos_Z]);
				cache_get_value_name_int(row_id, "Buy_price", Businesses_247[bizstock][bBuy_price]);
				new string[100];
				printf("vladel name == %s", Businesses_247[bizstock][bVladel]);
				if(Businesses_247[bizstock][bBuy_status]) {
					format(string, sizeof string, "№ %d\n%s\nВладелец: {B22222}%s\n%s", id_biz, Businesses_247[bizstock][bName], Businesses_247[bizstock][bVladel], (Businesses_247[bizstock][bOpen]) ? ("{FF00CC}Открыт") : ("{B22222}Закрыт"));
				} else format(string, sizeof string, "%s\nСвободен", Businesses_247[bizstock][bName]);
				Business_Create_Components(x, y, z, string, bizstock, false);
			}
		}
		else if(type == BUSINESS_TYPE_GAS_STATION) {
			cache_get_value_name_int(row_id, "ID", id_gas);
			gasstock = Gas_FindBy_DBUID(0);
			if( gasstock != -1 ) {
				Gas_stations[gasstock][bDB_ID] = id_gas;
				cache_get_value_name(row_id, "Name", Gas_stations[gasstock][bName], 30);
				cache_get_value_name(row_id, "Vladel", Gas_stations[gasstock][bVladel], MAX_PLAYER_NAME);
				cache_get_value_name_bool(row_id, "Buy_status", Gas_stations[gasstock][bBuy_status]);
				cache_get_value_name_bool(row_id, "Open", Gas_stations[gasstock][bOpen]);
				cache_get_value_name_int(row_id, "Money", Gas_stations[gasstock][bMoney]);
				cache_get_value_name_float(row_id, "X", x); 
				cache_get_value_name_float(row_id, "Y", y); 
				cache_get_value_name_float(row_id, "Z", z);
				new string[100];
				if(Gas_stations[gasstock][bBuy_status]) {
					format(string, sizeof string, "№ %d\n%s\nВладелец: {B22222}%s\n%s", id_gas, Gas_stations[gasstock][bName], Gas_stations[gasstock][bVladel], (Gas_stations[gasstock][bOpen]) ? ("{FF00CC}Открыт") : ("{B22222}Закрыт"));
				} else format(string, sizeof string, "%s\nСвободен", Gas_stations[gasstock][bName]);
				Business_Create_Components(x, y, z, string, gasstock, true);
			}
		}
	}

	cache_delete(mysql_cache);
	mysql_cache = mysql_query(DB_LocalHandle, "SELECT * FROM `business` INNER JOIN `houses_info` USING (ID)", true);
	cache_get_row_count(rows);
	if(!rows) return 0;

	for( new row_id; row_id < rows; row_id++ ) {
		cache_get_value_name_int(row_id, "ID", id_house);
		housestock = House_FindBy_DBUID(0);
		if( housestock != -1 ) {
			Businesses_Houses[housestock][bDB_ID] = id_house;
			cache_get_value_name(row_id, "Name", Businesses_Houses[housestock][bName], 30);
			cache_get_value_name(row_id, "Vladel", Businesses_Houses[housestock][bVladel], MAX_PLAYER_NAME);
			cache_get_value_name_bool(row_id, "Buy_status", Businesses_Houses[housestock][bBuy_status]);
			cache_get_value_name_bool(row_id, "Open", Businesses_Houses[housestock][bOpen]);
			cache_get_value_name_int(row_id, "Money", Businesses_Houses[housestock][bMoney]);
			cache_get_value_name_float(row_id, "X", x);
			cache_get_value_name_float(row_id, "Y", y);
			cache_get_value_name_float(row_id, "Z", z);
	
			printf("%s, db id = %d, row_id = %d", Businesses_Houses[housestock][bName], id_house, row_id);
			new string[128], string_class[8];
			cache_get_value_name_int(row_id, "Class", Businesses_Houses[housestock][bHouse_Class]);
			cache_get_value_name_float(row_id, "CameraPos_X", Businesses_Houses[housestock][bHouse_CameraPos_X]);
			cache_get_value_name_float(row_id, "CameraPos_Y", Businesses_Houses[housestock][bHouse_CameraPos_Y]);
			cache_get_value_name_float(row_id, "CameraPos_Z", Businesses_Houses[housestock][bHouse_CameraPos_Z]);
			
			cache_get_value_name_float(row_id, "CameraLookAt_X", Businesses_Houses[housestock][bHouse_CameraLookAt_X]);
			cache_get_value_name_float(row_id, "CameraLookAt_Y", Businesses_Houses[housestock][bHouse_CameraLookAt_Y]);
			cache_get_value_name_float(row_id, "CameraLookAt_Z", Businesses_Houses[housestock][bHouse_CameraLookAt_Z]);
			cache_get_value_name_float(row_id, "Veh_Pos_X", Businesses_Houses[housestock][bVeh_Pos_X]);
			cache_get_value_name_float(row_id, "Veh_Pos_Y", Businesses_Houses[housestock][bVeh_Pos_Y]);
			cache_get_value_name_float(row_id, "Veh_Pos_Z", Businesses_Houses[housestock][bVeh_Pos_Z]);
			cache_get_value_name_float(row_id, "Veh_Pos_Angle", Businesses_Houses[housestock][bVeh_Pos_Angle]);

			cache_get_value_name_float(row_id, "House_Interior_X", Businesses_Houses[housestock][bHouse_Interioir_X]);
			cache_get_value_name_float(row_id, "House_Interior_Y", Businesses_Houses[housestock][bHouse_Interioir_Y]);
			cache_get_value_name_float(row_id, "House_Interior_Z", Businesses_Houses[housestock][bHouse_Interioir_Z]);
			cache_get_value_name_int(row_id, "House_Interior_ID", Businesses_Houses[housestock][bHouse_Interioir_ID]);

			cache_get_value_name_float(row_id, "Exit_Pos_X", Businesses_Houses[housestock][bExit_From_Int_Pos_X]);
			cache_get_value_name_float(row_id, "Exit_Pos_Y", Businesses_Houses[housestock][bExit_From_Int_Pos_Y]);
			cache_get_value_name_float(row_id, "Exit_Pos_Z", Businesses_Houses[housestock][bExit_From_Int_Pos_Z]);

			cache_get_value_name_float(row_id, "exitX", Businesses_Houses[housestock][bExit_Pos_X]);
			cache_get_value_name_float(row_id, "exitY", Businesses_Houses[housestock][bExit_Pos_Y]);
			cache_get_value_name_float(row_id, "exitZ", Businesses_Houses[housestock][bExit_Pos_Z]);

			cache_get_value_name_float(row_id, "Safe_Pos_X", Businesses_Houses[housestock][bSafe_Pos_X]);
			cache_get_value_name_float(row_id, "Safe_Pos_Y", Businesses_Houses[housestock][bSafe_Pos_Y]);
			cache_get_value_name_float(row_id, "Safe_Pos_Z", Businesses_Houses[housestock][bSafe_Pos_Z]);
			if(Businesses_Houses[housestock][bDB_ID] == 41) {
				printf("tp pos x = %f, y = %f, z = %f", Businesses_Houses[housestock][bExit_Pos_X], Businesses_Houses[housestock][bExit_Pos_Y], Businesses_Houses[housestock][bExit_Pos_Z]);
			}
			switch(Businesses_Houses[housestock][bHouse_Class]) {
				case 0: string_class = "VIP"; //  0 vip, 1 A, 2 B, 3 C
				case 1: string_class = "A";
				case 2: string_class = "B";
				case 3: string_class = "C";
			}
	
			if(Businesses_Houses[housestock][bBuy_status]) {
				format(string, sizeof string, "№ %d\n%s\n%s класс\nВладелец: {B22222}%s\n%s", id_house, Businesses_Houses[housestock][bName], string_class, Businesses_Houses[housestock][bVladel], (Businesses_Houses[housestock][bOpen]) ? ("{FF00CC}Открыт") : ("{B22222}Закрыт"));
			} else format(string, sizeof string, "№ %d\n%s\n%s класс\nСвободен", id_house, Businesses_Houses[housestock][bName], string_class);
			Business_Create_Components(x, y, z, string, housestock, BUSINESS_TYPE_HOUSE);
		}



		houses_values++;
	}
	cache_delete(mysql_cache);

	for(new house_id, string[128], modelid, color1, color2; house_id < MAX_BUSINESSES_HOUSES; house_id++) {
		mysql_format(DB_LocalHandle, string, sizeof string, "SELECT * FROM `vehicles` WHERE `Business_ID` = '%d' AND `Slot` = '%d'", Businesses_Houses[house_id][bDB_ID], Businesses_Houses[house_id][bVeh_Slot]);
		mysql_cache = mysql_query(DB_LocalHandle, string, true);
		cache_get_row_count(rows);
		if(rows) {
			cache_get_value_name_int(0, "Modelid", modelid);
			cache_get_value_name_int(0, "Color1", color1);
			cache_get_value_name_int(0, "Color2", color2);
			CreateVehicle(modelid, Businesses_Houses[house_id][bVeh_Pos_X], Businesses_Houses[house_id][bVeh_Pos_Y], Businesses_Houses[house_id][bVeh_Pos_Z], Businesses_Houses[house_id][bVeh_Pos_Angle], color1, color2, 200, 0);
			cache_delete(mysql_cache);
		}
	}
	mysql_cache = mysql_query(DB_LocalHandle, "SELECT * FROM `weapons`", true);
	cache_get_row_count(rows);
	for(new row_id, value, string[32], string_weapon_id; row_id < rows; row_id++) {
		cache_get_value_name_int(row_id, "Business_ID", id_biz);
		cache_get_value_name_int(row_id, "type", type);
		if(type == BUSINESS_TYPE_247) {
			bizstock = Business_FindBy_DBUID(id_biz);
			if(bizstock != -1) {
				for(; string_weapon_id < 43; string_weapon_id++) {
					format(string, sizeof string, "weapon_%d", string_weapon_id);
					cache_get_value_name_int(row_id, string, value);
					Businesses_247[bizstock][bWeapons][string_weapon_id] = value;
					string_weapon_id++;
				}
				string_weapon_id = 0;
				string[0] = EOS;
			}
		}
		else if(type == BUSINESS_TYPE_HOUSE) {
			housestock = House_FindBy_DBUID(id_biz);
			if(housestock != -1) {
				for(; string_weapon_id < 43; string_weapon_id++) {
					format(string, sizeof string, "weapon_%d", string_weapon_id);
					cache_get_value_name_int(row_id, string, value);
					Businesses_Houses[housestock][bWeapons][string_weapon_id] = value;
					//string_weapon_id++;
				}
				string_weapon_id = 0;
				string[0] = EOS;
			}
		}
	}
	cache_delete(mysql_cache);
	//mysql_cache = mysql_query(DB_LocalHandle, "SELECT `ID` FROM `houses_info`", true);
	//cache_get_row_count(rows);
	//for(new index, weap_value = 44, house_id, Cache:query1; index < rows; index++, weap_value--) {
	//	cache_get_value_name_int(index, "ID", house_id);
	//	new insert[1024];
	//	mysql_format(DB_LocalHandle, insert, 5000, "INSERT INTO `weapons` (`Business_ID`, `type`, `weapon_0`, `weapon_1`, `weapon_2`, `weapon_3`, `weapon_4`, `weapon_5`, `weapon_6`, `weapon_7`, `weapon_8`, `weapon_9`, `weapon_10`, `weapon_11`, `weapon_12`, `weapon_13`, `weapon_14`, `weapon_15`, `weapon_16`, `weapon_17`, `weapon_18`, `weapon_19`, `weapon_20`, `weapon_21`, `weapon_22`, `weapon_23`, `weapon_24`, `weapon_25`, `weapon_26`, `weapon_27`, `weapon_28`, `weapon_29`, `weapon_30`, `weapon_31`, `weapon_32`, `weapon_33`, `weapon_34`, `weapon_35`, `weapon_36`, `weapon_37`, `weapon_38`, `weapon_39`, `weapon_40`, `weapon_41`, `weapon_42`) VALUES ('%d', '2', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '43')", house_id);
	//	query1 = mysql_query(DB_LocalHandle, insert, false);
	//	cache_delete(query1);
	//}


	test_end = GetTickCount();
	printf("ms load business == %d", test_end - test_start);
	return 1;
}

stock Player_Buy_Business(const playerid, const type) {
	if(type == BUSINESS_TYPE_247) {
		new index = Player_Buy_Business_247_ID[playerid], player_name[MAX_PLAYER_NAME], string[128];
		strcat(player_name, PlayerInfo_Name[playerid], MAX_PLAYER_NAME);
		Businesses_247[index][bBuy_status] = true;
		Businesses_247[index][bVladel][0] = '\0';
		Player_Business_247_DB_ID[playerid] = Businesses_247[index][bDB_ID];
		Player_Buy_Business_247_ID[playerid] = -1;
		strcat(Businesses_247[index][bVladel], player_name, MAX_PLAYER_NAME);
		mysql_format(DB_LocalHandle, string, sizeof string, "UPDATE `business` SET `Buy_status` = '1', `Vladel` = '%s' WHERE `ID` = '%d'", player_name, Businesses_247[index][bDB_ID]);
		mysql_tquery(DB_LocalHandle, string, "", "");
		string[0] = EOS;
		format(string, sizeof string, "%s\nВладелец: {B22222}%s\n%s", Businesses_247[index][bName], Businesses_247[index][bVladel], (Businesses_247[index][bOpen]) ? ("{FF00CC}Открыт") : ("{B22222}Закрыт"));
		UpdateDynamic3DTextLabelText(Businesses_247[index][b3DText], -1, string);
	}
	else if(type == BUSINESS_TYPE_GAS_STATION) {
		new index = Player_Buy_Gas_stations_ID[playerid], player_name[MAX_PLAYER_NAME], string[128];
		strcat(player_name, PlayerInfo_Name[playerid], MAX_PLAYER_NAME);
		Gas_stations[index][bBuy_status] = true;
		Gas_stations[index][bVladel][0] = '\0';
		Player_Gas_stations_DB_ID[playerid] = Gas_stations[index][bDB_ID];
		strcat(Gas_stations[index][bVladel], player_name, MAX_PLAYER_NAME);
		SendClientMessagef(playerid, -1, "ID = %d", Gas_stations[index][bDB_ID]);
		mysql_format(DB_LocalHandle, string, sizeof string, "UPDATE `business` SET `Buy_status` = '1', `Vladel` = '%s' WHERE `ID` = '%d'", player_name, Gas_stations[index][bDB_ID]);
		mysql_tquery(DB_LocalHandle, string, "", "");
		format(string, sizeof string, "%s\nВладелец: {B22222}%s\n%s", Gas_stations[index][bName], Gas_stations[index][bVladel], (Gas_stations[index][bOpen]) ? ("{FF00CC}Открыт") : ("{B22222}Закрыт"));
		UpdateDynamic3DTextLabelText(Gas_stations[index][b3DText], -1, string);
	}
}

stock Player_Enter_Business(const playerid, const business_type, const business_array_index) {
	/*
	#define BUSINESS_TYPE_247 0
	#define BUSINESS_TYPE_GAS_STATION 1
	#define BUSINESS_TYPE_HOUSE 2*/
	if(business_type == BUSINESS_TYPE_247) {
		SetPlayerPos(playerid, -25.132598,-139.066986,1003.546875); // телепортируем игрока в интерьер 24/7
		SetPlayerInterior(playerid, 16);
		SendClientMessagef(playerid, -1, "vv %d", business_array_index);
		if(business_array_index != 0) SetPlayerVirtualWorld(playerid, business_array_index); // чтоб игроки не появлялись в 1 инте, если зашли в разные здания
		Player_Entered_247_Index[playerid] = business_array_index;

	}
	else if(business_type == BUSINESS_TYPE_HOUSE) {
		Player_Is_Viewed_Interior[playerid] = true;
		Player_Enter_House[playerid] = true;
		Player_Entered_House_DB_ID[playerid] = Businesses_Houses[business_array_index][bDB_ID];
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);
		TogglePlayerControllable(playerid, 0);
		SendClientMessagef(playerid, -1, "%s", Businesses_Houses[business_array_index][bName]);
		InterpolateCameraPos(playerid, x, y, z, Businesses_Houses[business_array_index][bHouse_CameraPos_X], Businesses_Houses[business_array_index][bHouse_CameraPos_Y], Businesses_Houses[business_array_index][bHouse_CameraPos_Z], 2000, CAMERA_CUT);
		InterpolateCameraLookAt(playerid, x, y, z, Businesses_Houses[business_array_index][bHouse_CameraLookAt_X], Businesses_Houses[business_array_index][bHouse_CameraLookAt_Y], Businesses_Houses[business_array_index][bHouse_CameraLookAt_Z], 2000, CAMERA_CUT);
		SetPlayerPos(playerid, Businesses_Houses[business_array_index][bHouse_Interioir_X], Businesses_Houses[business_array_index][bHouse_Interioir_Y], Businesses_Houses[business_array_index][bHouse_Interioir_Z]);
		SetPlayerInterior(playerid, Businesses_Houses[business_array_index][bHouse_Interioir_ID]);
	}
}

stock Business_Create_Components(const Float:x, const Float:y, const Float:z, const text[], const bizid, const type) {
	printf("X= %f, Y= %f, Z= %f, text = %s, type = %d", x,y,z,text,type);
	if(type == BUSINESS_TYPE_247) {
		Businesses_247[bizid][b3DText] = CreateDynamic3DTextLabel(text, -1, x, y, z, 20.0);
		Businesses_247[bizid][bMapIcon] = CreateDynamicMapIcon(x, y, z, 19, 0xFF00CC, -1, -1, -1, 150.0);
		Businesses_247[bizid][bPickup] = CreateDynamicPickup(19130, 2, x, y, z);
	}
	else if(type == BUSINESS_TYPE_GAS_STATION) {
		Gas_stations[bizid][b3DText] = CreateDynamic3DTextLabel(text, -1, x, y, z, 20.0);
		Gas_stations[bizid][bMapIcon] = CreateDynamicMapIcon(x, y, z, 19, 0xFF00CC, -1, -1, -1, 150.0);
		Gas_stations[bizid][bFuelArea] = CreateDynamicSphere(x, y, z, 15.0);
		if(!Gas_stations[bizid][bBuy_status]) {
			Gas_stations[bizid][bPickup] = CreateDynamicPickup(19607, 2, x, y, z);
		}
	}
	else if(type == BUSINESS_TYPE_HOUSE) {
		Businesses_Houses[bizid][b3DText] = CreateDynamic3DTextLabel(text, -1, x, y, z, 20.0);
		if(!Businesses_Houses[bizid][bBuy_status]) {
			Businesses_Houses[bizid][bMapIcon] = CreateDynamicMapIcon(x, y, z, 31, 0x00FF00, -1, -1, -1, 150.0);
		}
		else {
			Businesses_Houses[bizid][bMapIcon] = CreateDynamicMapIcon(x, y, z, 31, 0xFF0000, -1, -1, -1, 150.0);
		}
		Businesses_Houses[bizid][bPickup] = CreateDynamicPickup(19130, 2, x, y, z, -1, -1, -1, 150.0);
	}
}
stock GetDynamicPickupPos(pickupid, &Float:x, &Float:y, &Float:z) {
    //В первый аргумент передали тип элемента - STREAMER_TYPE_PICKUP
    //Во второй аргумент передали сам ID элемента - pickupid
    //В третий аргумент тип данных, который хотим узнать - E_STREAMER_X
    //В четвёртый аргумент саму координату x - x
    Streamer_GetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_X, x);
    //В третий аргумент тип данных, который хотим узнать - E_STREAMER_Y
    //В четвёртый аргумент саму координату y - y
    Streamer_GetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_Y, y);
    //В третий аргумент тип данных, который хотим узнать - E_STREAMER_Z
    //В четвёртый аргумент саму координату z - z
    //По итогу: узнаём позицию динамического пикапа и возвращаем результат выполнения функции
    return Streamer_GetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_Z, z);
}

/*public OnPlayerShootDynamicObject(playerid, weaponid, STREAMER_TAG_OBJECT:objectid, Float:x, Float:y, Float:z) {
	new modelid = Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_MODEL_ID);
	SendClientMessagef(playerid, -1, "modelid == %d, objectid == %d", modelid, objectid);
}*/

#include "C:\Users\user\Desktop\mod\pawno\include\systems\Business_system\Callbacks\OnDialogResponse.inc"
#include "C:\Users\user\Desktop\mod\pawno\include\systems\Business_system\Callbacks\OnPlayerKeyStateChange.inc"
#include "C:\Users\user\Desktop\mod\pawno\include\systems\Business_system\Cmds\CMD_hmenu.inc"
#include "C:\Users\user\Desktop\mod\pawno\include\systems\Business_system\house0_functional.inc"
#include "C:\Users\user\Desktop\mod\pawno\include\systems\Business_system\guests_system.inc"


public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA: areaid)
{
	SendClientMessagef(playerid, -1, "enter bs zone %d", areaid);

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


public OnPlayerConnect(playerid)
{
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, MAX_PLAYER_NAME);
	for(new i; i < MAX_BUSINESSES_HOUSES; ++i) {
		if(!strcmp(Businesses_Houses[i][bVladel], name, false, MAX_PLAYER_NAME)) {
			Player_Business_House_DB_ID[playerid] = Businesses_Houses[i][bDB_ID];
			break;
		}
	}
	if(Player_Business_House_DB_ID[playerid] == -1) {
		new string[128];
		mysql_format(DB_LocalHandle, string, sizeof string, "SELECT * FROM `guests_houses` WHERE `Name` = '%s'", name);
		mysql_tquery(DB_LocalHandle, string, "business_PlayerConnect_callback", "i", playerid);
	}

	#if defined business_OnPlayerConnect
		return business_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect business_OnPlayerConnect
#if defined business_OnPlayerConnect
	forward business_OnPlayerConnect(playerid);
#endif

forward business_PlayerConnect_callback(playerid);
public business_PlayerConnect_callback(playerid) {
	cache_get_value_name_int(0, "House_ID", Guest_House_ID[playerid]);
	cache_get_value_name_bool(0, "Safe_Access", Guest_House_Safe_Access[playerid]);
	SendClientMessagef(playerid, -1, "guest house = %d", Guest_House_ID[playerid]);
	return 1;
}