#include <sourcemod>
#include <sdktools>
#include <clients>
#include <console>

new botQuota;

new bot[MAXPLAYERS+1];
new Float:bot_location[MAXPLAYERS+1][2][3];

new bool:tele_saved[MAXPLAYERS+1];
new Float:checkpoint[MAXPLAYERS+1][2][3];

new Handle:cvar_bot_quota,
	Handle:cvar_bot_zombie,
	Handle:cvar_game_mode,
	Handle:cvar_sv_cheats,
	Handle:cvar_sv_infinite_ammo,
	Handle:cvar_sv_showimpacts,
	Handle:cvar_mp_ignore_round_win_conditions,
	Handle:cvar_mp_freezetime,
	Handle:cvar_mp_do_warmup_period;

public Plugin:myinfo = {
	name = "Strat mode",
	author = "ikinz",
	description = "",
	version = "1.0",
	url = ""
};

public OnPluginStart() {
	initCvars();
	initCommands();
	initMenu();
	initServerSettings();
	
	HookEvent("player_hurt", Event_damage, EventHookMode_Pre);
}

public OnMapStart() {
	//ServerCommand("bot_kick");
}

public OnClientPutInServer(client) {
	if (IsFakeClient(client))
		return;
		
	botQuota = GetClientCount() * 2;
	SetConVarInt(cvar_bot_quota, nbrBots);
}

/***************************************************
 * INITIALIZERS
 **************************************************/

initMenu() {
	
}

initCvars() {
	cvar_bot_quota = FindConVar("bot_quota");
	cvar_bot_zombie = FindConVar("bot_zombie");
	cvar_game_mode = FindConVar("game_mode");
	cvar_sv_cheats = FindConVar("sv_cheats");
	cvar_sv_infinite_ammo = FindConVar("sv_infinite_ammo");
	cvar_sv_showimpacts = FindConVar("sv_showimpacts");
	cvar_mp_ignore_round_win_conditions = FindConVar("mp_ignore_round_win_conditions");
	cvar_mp_freezetime = FindConVar("mp_freezetime");
	cvar_mp_do_warmup_period = FindConVar("mp_do_warmup_period");
}

initCommands() {
	// Save Checkpoint
	RegConsoleCmd("checkpoint", Command_Save);
	RegConsoleCmd("save", Command_Save);
	RegConsoleCmd("s", Command_Save);
	// Teleport to checkpoint
	RegConsoleCmd("teleport", Command_Teleport);
	RegConsoleCmd("t", Command_Teleport);
	// Place a bot on player location after 3 sec
	RegConsoleCmd("place_bot", Command_BotPlace);
}

initServerSettings() {
	SetConVarInt(cvar_bot_quota, MAXPLAYERS);
	SetConVarInt(cvar_bot_zombie, 1);
	SetConVarInt(cvar_game_mode, 0);
	SetConVarInt(cvar_sv_cheats, 1);
	SetConVarInt(cvar_sv_infinite_ammo, 2);
	SetConVarInt(cvar_sv_showimpacts, 1);
	SetConVarInt(cvar_mp_ignore_round_win_conditions, 1);
	SetConVarInt(cvar_mp_freezetime, 0);
	SetConVarInt(cvar_mp_do_warmup_period, 0);
	
	ServerCommand("mp_restartmap");
}

/***************************************************
 * CONSOLE COMMAND LISTENERS
 **************************************************/

/*
 * Prints out the damage dealt to the assigned bot
 */
public Action:Event_damage(Handle:event, const String:Name[], bool:dontBroadcast) {
	new String:buffer[4];
	IntToString(GetEventInt(event, "dmg_health"), buffer, 4);
	PrintToChatAll("You dealt %s damage", buffer);
	return Plugin_Handled;
}
 
public Action:Command_Save(client, args) {
	GetClientAbsOrigin(client, checkpoint[client][0]);
	GetClientAbsAngles(client, checkpoint[client][1]);
	tele_saved[client] = true;
	PrintToChat(client, "[CHECKPOINT SAVED]");
}

public Action:Command_Teleport(client, args) {
	if (tele_saved[client]) {
		TeleportEntity(client, checkpoint[client][0], checkpoint[client][1], NULL_VECTOR);
		PrintToChat(client, "[TELEPORTED TO CHECKPOINT]");
	} else {
		PrintToChat(client, "[NO CHECKPOINTS AVAILABLE]");
	}
}

public Action:Command_BotPlace(client, args) {
	GetClientAbsOrigin(client, bot_location[client][0]);
	GetClientAbsAngles(client, bot_location[client][1]);
	//CreateTimer(3.0, SpawnBot, client);
}

public Action:SpawnBot(Handle:timer, any:client) {
	/*if (bot[client]) {
		TeleportEntity(bot[client], bot_location[client][0], bot_location[client][1], NULL_VECTOR);
		PrintToChat(client, "[BOT TELEPORTED]");
	} else {
		
	}*/
}