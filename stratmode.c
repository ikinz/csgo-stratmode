#include <sourcemod>
#include <sdktools>
#include <clients>
#include <console>

new lastPlayer;

new botQuota;
new bot[MAXPLAYERS+1];
new Float:bot_location[MAXPLAYERS+1][2][3];

new bool:tele_saved[MAXPLAYERS+1];
new Float:checkpoint[MAXPLAYERS+1][2][3];

new Handle:cvar_bot_quota,
	Handle:cvar_bot_stop,
	Handle:cvar_game_mode,
	Handle:cvar_sv_cheats,
	Handle:cvar_sv_infinite_ammo,
	Handle:cvar_sv_showimpacts,
	Handle:cvar_mp_ignore_round_win_conditions,
	Handle:cvar_mp_freezetime,
	Handle:cvar_mp_do_warmup_period,
	Handle:cvar_mp_respawn_on_death_ct,
	Handle:cvar_mp_respawn_on_death_t;

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
	SetConVarInt(cvar_mp_respawn_on_death_ct, 1);
	SetConVarInt(cvar_mp_respawn_on_death_t, 1);
}

public OnClientPutInServer(client) {
	if (IsFakeClient(client)) {
		if (lastPlayer) {
			decl String:buffer[17];
			GetClientName(lastPlayer, buffer, 17);
			SetClientInfo(client, "name", buffer);
			
			bot[lastPlayer] = client;
			lastPlayer = 0;
		}
	} else {
		botQuota = GetClientCount() * 2;
		SetConVarInt(cvar_bot_quota, botQuota);
		lastPlayer = client;
	}
}

/***************************************************
 * INITIALIZERS
 **************************************************/

initMenu() {
	
}

initCvars() {
	cvar_bot_quota = FindConVar("bot_quota");
	cvar_bot_stop = FindConVar("bot_stop");
	cvar_game_mode = FindConVar("game_mode");
	cvar_sv_cheats = FindConVar("sv_cheats");
	cvar_sv_infinite_ammo = FindConVar("sv_infinite_ammo");
	cvar_sv_showimpacts = FindConVar("sv_showimpacts");
	cvar_mp_ignore_round_win_conditions = FindConVar("mp_ignore_round_win_conditions");
	cvar_mp_freezetime = FindConVar("mp_freezetime");
	cvar_mp_do_warmup_period = FindConVar("mp_do_warmup_period");
	cvar_mp_respawn_on_death_ct = FindConVar("mp_respawn_on_death_ct");
	cvar_mp_respawn_on_death_t = FindConVar("mp_respawn_on_death_t");
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
	SetConVarInt(cvar_bot_quota, 0);
	SetConVarInt(cvar_bot_stop, 1);
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
	new attacker = GetEventInt(event, "attacker");
	new clientHurt = GetEventInt(event, "userid");
	new dmg = GetEventInt(event, "dmg_health");
	
	PrintToServer("attacker: %i", attacker);
	PrintToServer("clientHurt: %i", clientHurt);
	PrintToServer("dmg: %i", dmg);
	PrintToServer("Bot for player: %i", bot[attacker]);
	
	if (bot[attacker] == clientHurt) {
		PrintToChat(attacker, "\x01 \x03You dealt \x07%i \x03damage", dmg);
	}
	return Plugin_Handled;
}
 
public Action:Command_Save(client, args) {
	GetClientAbsOrigin(client, checkpoint[client][0]);
	GetClientAbsAngles(client, checkpoint[client][1]);
	tele_saved[client] = true;
	PrintToChat(client, "\x01 \x05[CHECKPOINT SAVED]");
	PrintToChatAll("\x01\x0B\x01 1,\x02 2,\x03 3,\x04 4,\x05 5,\x06 6,\x07 7");
}

public Action:Command_Teleport(client, args) {
	if (tele_saved[client]) {
		TeleportEntity(client, checkpoint[client][0], checkpoint[client][1], NULL_VECTOR);
		PrintToChat(client, "\x01 \x05[TELEPORTED TO CHECKPOINT]");
	} else {
		PrintToChat(client, "\x01 \x02[NO CHECKPOINTS AVAILABLE]");
	}
}

public Action:Command_BotPlace(client, args) {
	GetClientAbsOrigin(client, bot_location[client][0]);
	GetClientAbsAngles(client, bot_location[client][1]);
	
	new Handle:pack;
	CreateDataTimer(1.0, SpawnBot, pack);
	WritePackCell(pack, client);
	WritePackCell(pack, 3);
}

public Action:SpawnBot(Handle:timer, Handle:info) {
	ResetPack(info);
	new client = ReadPackCell(info);
	new timeleft = ReadPackCell(info);
	if (bot[client]) {
		if (timeleft == 0) {
			TeleportEntity(bot[client], bot_location[client][0], bot_location[client][1], NULL_VECTOR);
			PrintToChat(client, "\x01 \x05[BOT TELEPORTED]");
		} else if (timeleft > 0) {
			PrintToChat(client, "[BOT TELEPORTING IN %i seconds]", timeleft);
			new Handle:pack;
			CreateDataTimer(1.0, SpawnBot, pack);
			WritePackCell(pack, client);
			WritePackCell(pack, timeleft - 1);
		}
	}
}