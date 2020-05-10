#include "g_local.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int FindMaplistUID(int uid)
{
	int i;
	int ret_this = -1;
	if (uid == -1)
		return -1;

	for (i = 0; i < MAX_USERS; i++)
	{
		if (maplist.times[level.mapnum][i].uid == uid)
		{
			//found a current record, update
			ret_this = i;
			break;
		}
	}
	return ret_this;
}

qboolean maplist_log(edict_t *ent, int uid, float time, char *date)
{
	int i;
	int maplist_uid;
	float oldtime;
	edict_t *cl_ent;

	//find user in maplist.times
	maplist_uid = FindMaplistUID(uid);

	//record old time
	oldtime = 0;
	if (maplist.times[level.mapnum][maplist_uid].time > 0) {
		oldtime = maplist.times[level.mapnum][maplist_uid].time;
	}
	gi.dprintf("maplist_log TIME: %f\n", time);
	// split checking for final gun grab
	int my_time;
	float my_time_float;
	float my_split;

	my_time = Sys_Milliseconds() - ent->client->resp.client_think_begin;
	my_time_float = (float)my_time / 1000.0f;
	my_split = my_time_float - ent->client->pers.cp_split;

	// this player already has a time, we can just update their old one
	if (maplist_uid >= 0) {
		if (time < maplist.times[level.mapnum][maplist_uid].time) {
			maplist.times[level.mapnum][maplist_uid].time = time;
			strcpy(maplist.times[level.mapnum][maplist_uid].date, date);
			ent->client->resp.maplist_uid = maplist_uid;
			ent->client->resp.best_time = time;
			maplist.times[level.mapnum][maplist_uid].fresh = true;
		}

		if (!maplist.times[level.mapnum][maplist_uid].completions) {
			maplist.users[uid].completions++;
			append_uid_file(uid, level.mapname);
		}

		maplist.times[level.mapnum][maplist_uid].completions++;
		

		//setting a first
		if (time < maplist.times[level.mapnum][0].time) {
			gi.bprintf(PRINT_HIGH, "%s finished in %1.3f seconds (PB ", ent->client->pers.netname, time);
			gi.bprintf(PRINT_CHAT, "%1.3f ", time - oldtime);
			gi.bprintf(PRINT_HIGH, "| 1st ");
			gi.bprintf(PRINT_CHAT, "%1.3f", time - maplist.times[level.mapnum][0].time);
			if (ent->client->pers.cp_split > 0)
				gi.cprintf(ent, PRINT_HIGH, " | split: %1.3f", my_split);
			gi.bprintf(PRINT_HIGH, ")\n");
			return false;
		}

		// beat pb, show to server
		if (time < oldtime) {
			gi.bprintf(PRINT_HIGH, "%s finished in %1.3f seconds (PB ", ent->client->pers.netname, time);
			gi.bprintf(PRINT_CHAT, "%1.3f ", time - oldtime);
			gi.bprintf(PRINT_HIGH, "| 1st +%1.3f", time - maplist.times[level.mapnum][0].time);
			if (ent->client->pers.cp_split > 0)
				gi.cprintf(ent, PRINT_HIGH, " | split: %1.3f", my_split);
			gi.bprintf(PRINT_HIGH, ")\n");
			return false;
		}

		// didn't beat pb/1st, only show to players that wants it, or players who are chasing you! :D //no splits atm though :thinking: :linux: :thing:
		for (i = 0; i < maxclients->value; i++) {
			cl_ent = g_edicts + 1 + i;
			if (!cl_ent->inuse)
				continue;

			if (cl_ent->client->resp.showtimes || cl_ent->client->chase_target && Q_stricmp(cl_ent->client->chase_target->client->pers.netname, ent->client->pers.netname) == 0)
				gi.cprintf(cl_ent, PRINT_HIGH, "%s finished in %1.3f seconds (PB +%1.3f | 1st +%1.3f)\n",
					ent->client->pers.netname, time, time - oldtime, time - maplist.times[level.mapnum][0].time);
		}

		// even with showtimes off, you should still see your own time
		if (time >= oldtime && !ent->client->resp.showtimes) {
			if (ent->client->pers.cp_split > 0)
				gi.cprintf(ent, PRINT_HIGH, "You finished in %1.3f seconds (PB +%1.3f | 1st +%1.3f | split: %1.3f)\n",
					time, time - oldtime, time - maplist.times[level.mapnum][0].time, my_split);
			else
				gi.cprintf(ent, PRINT_HIGH, "You finished in %1.3f seconds (PB +%1.3f | 1st +%1.3f)\n",
					time, time - oldtime, time - maplist.times[level.mapnum][0].time);
			return false;
		}

		// something is very wrong...
		return false;
	}

	// these players don't have a time on the map yet
	else {
		if (time > 0)
			for (i = 0; i < MAX_USERS; i++)
			{
				if (maplist.times[level.mapnum][i].completions <= 0) {
					//found a spare record, use it
					if (maplist.times[level.mapnum][i].completions == -1) {
						maplist.times[level.mapnum][i].completions = 0;
					}
					maplist.times[level.mapnum][i].uid = uid;
					maplist.times[level.mapnum][i].time = time;
					strcpy(maplist.times[level.mapnum][i].date, date);
					if (!maplist.times[level.mapnum][i].completions)
					{
						maplist.users[uid].completions++;
						append_uid_file(uid, level.mapname);
					}
					maplist.times[level.mapnum][i].completions++;
					maplist.times[level.mapnum][i].fresh = true;
					ent->client->resp.maplist_uid = i;
					ent->client->resp.best_time = time;

					// new map, so don't show comparison
					if (maplist.times[level.mapnum][0].time == 0) {
						gi.bprintf(PRINT_HIGH, "%s finished in %1.3f seconds (", ent->client->pers.netname, time);
						if (ent->client->pers.cp_split > 0)
							gi.cprintf(ent, PRINT_HIGH, "split: %1.3f | ", my_split);
						gi.bprintf(PRINT_HIGH, "1st completion on the map)\n");
						return false;
					}

					// 1st comp AND 1st place
					if (time < maplist.times[level.mapnum][0].time) {
						gi.bprintf(PRINT_HIGH, "%s finished in %1.3f seconds (1st ", ent->client->pers.netname, time);
						gi.bprintf(PRINT_CHAT, "%1.3f ", time - maplist.times[level.mapnum][0].time);
						gi.bprintf(PRINT_HIGH, "| ");
						if (ent->client->pers.cp_split > 0)
							gi.cprintf(ent, PRINT_HIGH, "split: %1.3f | ", my_split);
						gi.bprintf(PRINT_HIGH, "1st completion)\n");
						return false;
					}

					// always display someone's first completion
					gi.bprintf(PRINT_HIGH, "%s finished in %1.3f seconds (1st +%1.3f | ",
						ent->client->pers.netname, time, time - maplist.times[level.mapnum][0].time);
					if (ent->client->pers.cp_split > 0)
						gi.cprintf(ent, PRINT_HIGH, "split: %1.3f | ", my_split);
					gi.bprintf(PRINT_HIGH, "1st completion)\n");

					if (gset_vars->playsound)
						if (time > gset_vars->playsound)
							return true;
					return false;
				}
			}
	}
	return false;
}

void sort_maplist_times() {
	int i, j, swap;
	times_record   temp;
	swap = 0;

	for (i = 1; i < MAX_USERS; i++) {
		if (strlen(maplist.times[level.mapnum][i].date) < 2) {
			break;
		}
		swap = 0;
		for (j = 0; j < (MAX_USERS - i); j++) {
			if (strlen(maplist.times[level.mapnum][j].date) < 2 || strlen(maplist.times[level.mapnum][j + 1].date) < 2) {
				break;
			}
			if (maplist.times[level.mapnum][j].time == maplist.times[level.mapnum][j + 1].time) {
				continue;
			}
			if (maplist.times[level.mapnum][j].time > maplist.times[level.mapnum][j + 1].time) {
				//gi.dprintf("Swap: %d  <->  %d\n",maplist.times[level.mapnum][j].uid,maplist.times[level.mapnum][j+1].uid);
				temp = maplist.times[level.mapnum][j];
				maplist.times[level.mapnum][j] = maplist.times[level.mapnum][j + 1];
				maplist.times[level.mapnum][j + 1] = temp;
				//gi.dprintf("Swapped: %d  <->  %d\n",maplist.times[level.mapnum][j].uid,maplist.times[level.mapnum][j+1].uid);
				swap = 1;
			}
		}
		if (!swap) {
			break;
		}
	}
}

//update_maplist..
/*
void update_maplist_times(char *filename) {
	FILE	*f;
	int i, i2;
	char	name[128];
	cvar_t	*port;
	cvar_t	*tgame;
	char temp[1024];
	times_record tempuser;
	qboolean founduser;

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

	if (!*tgame->string)
	{
		sprintf(name, "jump/%s/%s.t", port->string, filename);
	}
	else
	{
		sprintf(name, "%s/%s/%s.t", tgame->string, port->string, filename);
	}

	f = fopen(name, "rb");
	if (!f)
	{
		return;
	}


	for (i2 = 0; i2 < MAX_USERS; i2++) {
		tempuser.uid = -1; // so it'll go away if it's not set to anything else.
		fscanf(f, "%s", tempuser.date);
		fscanf(f, "%f", &tempuser.time);
		fscanf(f, "%i", &tempuser.uid);
		fscanf(f, "%i", &tempuser.completions);
		if (tempuser.uid == -1) {
			break;
		}
		founduser = false;
		for (i = 0; i < MAX_USERS; i++) {
			if (tempuser.uid == maplist.times[level.mapnum][i].uid) {
				founduser = true; //not a new user..
				if (maplist.times[level.mapnum][i].completions == -1) { //if remtime
					gi.dprintf("remove time: %f\n", maplist.times[level.mapnum][i].time);
					maplist.times[level.mapnum][i].fresh = false;
					maplist.times[level.mapnum][i].time = 0;
					maplist.times[level.mapnum][i].uid = -1;
					maplist.times[level.mapnum][i].completions = 0;
					break;
				}
				if (maplist.times[level.mapnum][i].time > tempuser.time) { //check if user has a better time..
					maplist.times[level.mapnum][i].time = tempuser.time;
					sprintf(maplist.times[level.mapnum][i].date, "%s", tempuser.date);
				}
				if (tempuser.completions > maplist.times[level.mapnum][i].completions) {
					maplist.times[level.mapnum][i].completions = tempuser.completions;
				}
				break;
			}
		}
		if (!founduser) { //new user!
			for (i = 0; i < MAX_USERS; i++) {
				if (!maplist.times[level.mapnum][i].completions) {
					maplist.times[level.mapnum][i] = tempuser;
					gi.dprintf("remove time: %f\n", maplist.times[level.mapnum][i].time);
					break;
				}
			}
		}
		if (feof(f)) {
			break;
		}
	}
	for (i = 0; i < MAX_USERS; i++) { //check if there are any removed times left, and reset them.
		if (maplist.times[level.mapnum][i].completions == -1) {
			maplist.times[level.mapnum][i].fresh = false;
			maplist.times[level.mapnum][i].time = 0;
			maplist.times[level.mapnum][i].uid = -1;
			maplist.times[level.mapnum][i].completions = 0;
		}
	}
	fclose(f);
}
*/

void open_map_file(char *filename, qboolean apply)
{
	FILE	*f;
	int i, i2;
	char	name[128];
	cvar_t	*port;
	cvar_t	*tgame;
	char temp[1024];
	int uid;
	int maplisted;
	struct stat filestat;

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

	if (!*tgame->string)
	{
		sprintf(name, "jump/%s/%s.t", port->string, filename);
	}
	else
	{
		sprintf(name, "%s/%s/%s.t", tgame->string, port->string, filename);
	}

	maplisted = -1;
	stat(name, &filestat);
	for (i2 = 0; i2 < maplist.nummaps; i2++)
	{
		if (strcmp(maplist.mapnames[i2], filename) == 0)
		{
			maplisted = i2;
			break;
		}
	}
	//clear maplist.times for this map.
	for (i = 0; i < MAX_USERS; i++)
	{
		maplist.times[maplisted][i].fresh = false;
		maplist.times[maplisted][i].uid = -1;
		maplist.times[maplisted][i].time = 0;
		maplist.times[maplisted][i].completions = 0;
	}
	i = 0;

	f = fopen(name, "rb");

	if (!f)
	{
		return;
	}
	fseek(f, 0, SEEK_END);
	if (ftell(f) == 0) { //if file is empty.
		return;
	}
	else {
		rewind(f);
	}
	
	for (i = 0; i < MAX_USERS; i++) {
		fscanf(f, "%s", &maplist.times[maplisted][i].date);
		if (strlen(maplist.times[maplisted][i].date) < 2)
			break; //ugly hack to stop it from loading 1 more uid, even though it has reached the end of the file..
		fscanf(f, "%f", &maplist.times[maplisted][i].time);
		fscanf(f, "%i", &uid);
		maplist.times[maplisted][i].uid = uid;
		fscanf(f, "%i", &maplist.times[maplisted][i].completions);
		//gi.dprintf("NR: %d Uid: %d date: %s time: %f\n", i, uid, maplist.times[maplisted][i].date, maplist.times[maplisted][i].time);
		if ((maplisted != -1) && apply && maplist.times[level.mapnum][i].completions)
		{
			append_uid_file(uid, filename);
		}
		if (feof(f)) {
			break;
		}
	}
	fclose(f);
}

void write_map_file(char *filename, int mapnum)
{
	char	buffer[1024];
	FILE	*f;
	char	name[256];
	int i, i2;
	char port_d[32];
	cvar_t	*port;
	cvar_t	*tgame;

	sort_maplist_times();
	tgame = gi.cvar("game", "jump", 0);
	port = gi.cvar("port", "27910", 0);
	strcpy(port_d, port->string);
	if (!port_d[0])
		strcpy(port_d, "27910");
	if (!*tgame->string)
	{
		sprintf(name, "jump/%s/%s.t", port_d, filename);
	}
	else
	{
		sprintf(name, "%s/%s/%s.t", tgame->string, port_d, filename);
	}

	f = fopen(name, "wb");

	if (!f)
		return;

	for (i = 0; i < MAX_USERS; i++)
	{
		if (maplist.times[level.mapnum][i].completions != -1) {
			if (maplist.times[level.mapnum][i].uid > -1 && maplist.times[level.mapnum][i].time > 0) {
				Com_sprintf(buffer, sizeof(buffer), "%s %f %i %i", maplist.times[level.mapnum][i].date, maplist.times[level.mapnum][i].time, maplist.times[level.mapnum][i].uid, maplist.times[level.mapnum][i].completions);
				fprintf(f, "%s\n", buffer);
				//gi.dprintf("ID=%d\n",i);
			}
		}
	}
	fclose(f);
}

void update_users_file()
{
	int throwaway;
	FILE	*f;
	char	name[128];
	cvar_t	*port;
	cvar_t	*tgame;
	int uid;
	int score;
	int completions;
	qboolean newusers;

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

	if (!*tgame->string)
	{
		sprintf(name, "jump/%s/users.t", port->string);
	}
	else
	{
		sprintf(name, "%s/%s/users.t", tgame->string, port->string);
	}

	f = fopen(name, "rb");
	if (!f)
	{
		return;
	}
	maplist.num_users = 0;
	while (!feof(f))
	{
		fscanf(f, "%i", &uid);
		fscanf(f, "%i", &completions);
		fscanf(f, "%i", &score);
		fscanf(f, "%s", &name);
		if ((uid >= MAX_USERS) || (uid < 0))
			continue;
		if (Q_stricmp(maplist.users[uid].name, name) != 0) { //new player frome elsewhere
			strcpy(maplist.users[uid].name, name);
			maplist.users[uid].score = score;
			maplist.users[uid].completions = completions;
		}
		//gi.bprintf(PRINT_HIGH, "%s updated at %i\n", name, uid);
		maplist.num_users++;
	}
	maplist.sort_num_users = maplist.num_users;

	fclose(f);
}
void open_users_file()
{
	int throwaway;
	FILE	*f;
	int i, j;
	char	name[128];
	cvar_t	*port;
	cvar_t	*tgame;
	int uid;
	int score;
	int completions;

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

	if (!*tgame->string)
	{
		sprintf(name, "jump/%s/users.t", port->string);
	}
	else
	{
		sprintf(name, "%s/%s/users.t", tgame->string, port->string);
	}

	f = fopen(name, "rb");
	if (!f)
	{
		return;
	}
	maplist.num_users = 0;
	//clear users file
	for (i = 0; i < MAX_USERS; i++)
	{
		maplist.users[i].name[0] = 0;
		maplist.users[i].score = 0;
		maplist.users[i].lastseen = 0;
		for (j = 0; j < 10; j++)
			maplist.users[i].points[j] = 0;
	}

	i = 0;
	while (!feof(f))
	{
		fscanf(f, "%i", &uid);
		fscanf(f, "%i", &completions);
		fscanf(f, "%i", &score);
		fscanf(f, "%s", &name);
		if ((uid >= MAX_USERS) || (uid < 0))
			continue;
		strcpy(maplist.users[uid].name, name);
		//gi.dprintf("Load users: uid=%i name=%s\n", uid, maplist.users[uid].name);
		maplist.users[uid].completions = completions;
		maplist.num_users++;
	}
	maplist.sort_num_users = maplist.num_users;

	fclose(f);
}

void write_users_file(void)
{
	char	buffer[1024];
	FILE	*f;
	char	name[256];
	int i;
	char port_d[32];
	cvar_t	*port;
	cvar_t	*tgame;

	tgame = gi.cvar("game", "jump", 0);
	port = gi.cvar("port", "27910", 0);

	strcpy(port_d, port->string);
	if (!port_d[0])
		strcpy(port_d, "27910");
	if (!*tgame->string)
	{
		sprintf(name, "jump/%s/users.t", port_d);
	}
	else
	{
		sprintf(name, "%s/%s/users.t", tgame->string, port_d);
	}

	f = fopen(name, "wb");
	if (!f)
		return;
	for (i = 0; i < MAX_USERS; i++)
	{
		if (maplist.users[i].name[0]) {
			Com_sprintf(buffer, sizeof(buffer), " %i %i %i %s", i, maplist.users[i].completions, maplist.users[i].score, maplist.users[i].name);
			fprintf(f, "%s", buffer);
		}
	}
	fclose(f);
}

void write_uid_file(int uid, edict_t *ent)
{
	int index;
	FILE	*f;
	int i;
	cvar_t	*port;
	cvar_t	*tgame;
	char	name[256];

	index = ent - g_edicts - 1;

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

	if (!*tgame->string)
	{
		sprintf(name, "jump/%s/%i.u", port->string, uid);
	}
	else
	{
		sprintf(name, "%s/%s/%i.u", tgame->string, port->string, uid);
	}

	f = fopen(name, "wb");

	if (!f)
		return;

	for (i = 0; i < MAX_MAPS; i++)
	{
		if (overall_completions[index].maps[i])
		{
			fprintf(f, " %s", maplist.mapnames[i]);
		}
	}
	fclose(f);
}

float add_item_to_queue(edict_t *ent, float item_time)//, char *owner, char *name)
{
	int m_current;
	int m_time;
	float item_time_new;
	int i;
	struct	tm *current_date;
	time_t	time_date;
	int		month, day, year;
	int i2;
	int placement;
	char temp_owner[128], temp_name[128];
	qboolean fresh;
	char temp_date[128];
	qboolean played_wav = false;
	float diff;
	qboolean save_demo = true;
	char temp_stamp[32];
	int uid;
	int maplist_uid;
	int uid_1st = -1;

	//dont add if name = zero length
	if (strcmp(ent->client->pers.netname, "") == 0)
		return item_time;

	//item_time = 3.7f;
	strcpy(temp_owner, ent->client->pers.netname);
	//strcpy(temp_name, name);

	for (i2 = 0; i2 < strlen(temp_owner); i2++)
	{
		if (temp_owner[i2] == ' ')
			temp_owner[i2] = '_';
	}

	/*for (i2 = 0; i2 < strlen(temp_name); i2++)
	{
		if (temp_name[i2] == ' ')
			temp_name[i2] = '_';
	}*/


	//set the date
	time_date = time(NULL);                // DOS system call
	current_date = localtime(&time_date);  // DOS system call
	month = current_date->tm_mon + 1;
	day = current_date->tm_mday;
	year = current_date->tm_year;

	sprintf(temp_stamp, "1%02d%02d%02d", year - 100, month, day);

	uid = GetPlayerUid(temp_owner);
	if (uid == -1)
		//no slots or somin fucked
		return item_time;
	maplist_uid = FindMaplistUID(uid);

	//ent->client->resp.uid = uid + 1;
	UpdateThisUsersSortedUid(ent);
	//log off everyones time
	sprintf(temp_date, "%02d/%02d/%02d", day, month, year - 100);
	if (ent->client->resp.client_think_begin)
	{

		m_current = Sys_Milliseconds();
		m_time = m_current - ent->client->resp.client_think_begin;
		item_time_new = (float)m_time / 1000.0f;
		//0.5 limit difference, just to be sure nothing breaks
		if (fabs(item_time - item_time_new) < 0.5)
			item_time = item_time_new;
		//add 0.1 otherwise due to replay starting early
		else
			item_time += 0.1f;
		if (gset_vars->time_adjust)
			item_time += ((double)gset_vars->time_adjust*0.001);
	}
	else
	{
		//due to paused start we now need to add 0.1
		item_time += 0.1f;
	}

	played_wav = maplist_log(ent, uid, item_time, temp_date);
	sort_maplist_times();

	//placement = level_items.stored_item_times_count;
	//if (level_items.stored_item_times_count)
	//{
	//uid_1st = maplist.times[level.mapnum][0].uid;
	save_demo = false;
	placement = -1;
	for (i = 0; i < MAX_HIGHSCORES; i++)
	{
		if (maplist.times[level.mapnum][i].uid == uid)
		{
			//0.001 is used due to gay floating points. this hack should prevent same times being added with a new date
			//if ((item_time + 0.0001) < maplist.times[level.mapnum][i].time)
			if (item_time == maplist.times[level.mapnum][i].time)
			{
				placement = i;
				save_demo = true;
			}
			break;
		}
	}
	//}

	fresh = false;

	if (placement >= 0)
	{
		maplist.times[level.mapnum][placement].uid = uid;
		//maplist.times[level.mapnum][placement].timestamp = 0;
		//maplist.times[level.mapnum][placement].timeint = 0;
		maplist.times[level.mapnum][placement].time = item_time;
		//strcpy(maplist.times[level.mapnum][placement].owner, temp_owner);
		//strcpy(maplist.times[level.mapnum][placement].name, temp_name);
		maplist.times[level.mapnum][placement].fresh = true;
		fresh = true;
		sprintf(maplist.times[level.mapnum][placement].date, "%02d/%02d/%02d", day, month, year - 100);

		//sort_queue(level_items.stored_item_times_count);
	}
	if (save_demo)
	{
		//only save if its now on the board
		for (i = 0; i < MAX_HIGHSCORES; i++)
			if (maplist.times[level.mapnum][i].uid == uid)
			{
				Save_Individual_Recording(ent);
				break;
			}
		//load the recordings from where this went in to end of stored item list
		for (i2 = i; i2 < MAX_HIGHSCORES; i2++)
		{
			Load_Individual_Recording(i2, maplist.times[level.mapnum][i2].uid);
		}
	}

	if (fresh)
	{
		//need to sort the demo recording code completely
		//if (strcmp(temp_name, maplist.times[level.mapnum][0].name) == 0)
		if (strcmp(temp_owner, maplist.users[maplist.times[level.mapnum][0].uid].name) == 0)
			gi.dprintf("1st place? name:%s time:%f  item_time:%f\n", maplist.users[maplist.times[level.mapnum][0].uid].name, maplist.times[level.mapnum][0].time,item_time);
			if (maplist.times[level.mapnum][0].time == item_time)
			{
				gi.dprintf("1st place set!!!!!\n");
				Save_Recording(ent, uid);// , uid_1st);
				if (gset_vars->playsound && song_timer(gset_vars->numberone_length)) // change the # to length of your 1st place song
				{
					gi.positioned_sound(world->s.origin, world, CHAN_AUTO | CHAN_RELIABLE, gi.soundindex(gset_vars->numberone_wav), 1, ATTN_NONE, 0);
					played_wav = false;
				}
			}

	}
	if (played_wav)
	{
		if (gset_vars->numsoundwavs > 1)
		{
			gi.positioned_sound(world->s.origin, world, CHAN_AUTO | CHAN_RELIABLE, gi.soundindex(va("jump%d.wav", rand() % gset_vars->numsoundwavs)), 1, ATTN_NONE, 0);
		}
		else
			gi.positioned_sound(world->s.origin, world, CHAN_AUTO | CHAN_RELIABLE, gi.soundindex("jump.wav"), 1, ATTN_NONE, 0);
	}
	return item_time;
}

void UpdateThisUsersSortedUid(edict_t *ent)
{
	int i;
	int uid;
	uid = ent->client->resp.uid - 1;
	for (i = 0; i < maplist.sort_num_users; i++)
	{
		if (maplist.sorted_completions[i].uid == uid)
		{
			ent->client->resp.suid = i;
			break;
		}
	}
}

void UpdateThisUsersUID(edict_t *ent, char *name)
{
	int uid;
	int maplist_uid;
	uid = GetPlayerUid_NoAdd(name);
	if (uid >= 0)
	{
		ent->client->resp.uid = uid + 1;
		UpdateThisUsersSortedUid(ent);
		maplist_uid = FindMaplistUID(uid);
		ent->client->resp.maplist_uid = maplist_uid; //maplist_uidbug - need to sort first?
		if (maplist_uid >= 0)
		{
			ent->client->resp.best_time = maplist.times[level.mapnum][maplist_uid].time;
		}
		else
		{
			ent->client->resp.best_time = 0;
		}
	}
	else
	{
		ent->client->resp.uid = 0;
		ent->client->resp.suid = -1;
		ent->client->resp.maplist_uid = -1;
		ent->client->resp.best_time = 0;
	}
}


void remtimes(edict_t *ent)
{
	int i;
	char	name[256];
	cvar_t	*tgame;
	edict_t	*e2;

	if (ent->client->resp.admin < aset_vars->ADMIN_REMTIMES_LEVEL)
		return;

	tgame = gi.cvar("game", "", 0);


	//set all tourney times to some high value
	//remove top 10 times
	for (i = 0; i < MAX_USERS; i++)
	{
		if (maplist.times[level.mapnum][i].uid >= 0)
		{
			sprintf(name, "%s/jumpdemo/%s_%d.dj3", tgame->string, level.mapname, maplist.times[level.mapnum][i].uid);
			remove(name);
		}
		maplist.times[level.mapnum][i].uid = -1;
		maplist.times[level.mapnum][i].fresh = false;
		maplist.times[level.mapnum][i].time = 0;
		maplist.times[level.mapnum][i].completions = 0;
	}
	for (i = 1; i <= maxclients->value; i++)
	{
		e2 = g_edicts + i;
		if (!e2->inuse)
			continue;
		UpdateThisUsersUID(e2, e2->client->pers.netname);

	}

	for (i = 0; i < MAX_HIGHSCORES + 1; i++)
	{
		level_items.recorded_time_frames[i] = 0;
		level_items.recorded_time_uid[i] = -1;
	}

	EmptyTimes(level.mapnum);
	UpdateScores();
	sort_users();
#ifdef ANIM_REPLAY
	sprintf(name, "%s/jumpdemo/%s.dj2", tgame->string, level.mapname);
#else
	sprintf(name, "%s/jumpdemo/%s.dj1", tgame->string, level.mapname);
#endif
	remove(name);

	write_map_file(level.mapname, level.mapnum);


	gi.cprintf(ent, PRINT_HIGH, "Times removed.\n");
}


void apply_time(edict_t *other, edict_t *ent)
{
	char		item_name[128];

	Stop_Recording(other);
	if (((other->client->resp.item_timer_allow) || (other->client->resp.ctf_team == CTF_TEAM2)) || (gametype->value == GAME_CTF && other->client->resp.ctf_team == CTF_TEAM1))
	{
		if (!other->client->resp.finished)
		{
			//stop recording, setup for new one
			if (other->client->resp.auto_record_on)
			{
				autorecord_newtime(other);
				if (other->client->resp.auto_recording)
					autorecord_stop(other);
			}


			other->client->resp.finished = true;
			other->client->resp.score++;
			other->client->resp.got_time = true;


			//other->client->resp.item_timer = 
			other->client->resp.item_timer = add_item_to_queue(other, other->client->resp.item_timer);//, other->client->pers.netname, ent->item->pickup_name);

			ClearPersistants(&other->client->pers);
			ClearCheckpoints(other);
			hud_footer(other);

			if (((other->client->resp.item_timer + 0.0001) < level_items.fastest_time) || (level_items.fastest_time == 0))
			{
				level_items.jumps = other->client->resp.jumps;
				level_items.fastest_time = other->client->resp.item_timer;
				strcpy(level_items.item_owner, other->client->pers.netname);
				strcpy(level_items.item_name, ent->item->pickup_name);
				level_items.fastest_player = other;
				Save_Current_Recording(other);

				//give them admin
				if ((other->client->resp.item_timer == maplist.times[level.mapnum][0].time) || (!maplist.times[level.mapnum][0].time))
				{
					if (!Neuro_RedKey_Overide && map_added_time < 5)
					{
						gi.bprintf(PRINT_CHAT, "%s has set a 1st place, adding 5 minutes extra time.\n", other->client->pers.netname);
						map_added_time += 5;
						Update_Added_Time();
					}
					else
					{
						gi.bprintf(PRINT_CHAT, "%s has set a 1st place.\n", other->client->pers.netname);
					}
				}

			}
		}
		if (!other->client->resp.playtag)
		{
			if (!Neuro_RedKey_Overide)
				if ((gset_vars->jetpack))
				{
					strcpy(item_name, "jetpack");
					give_item(other, item_name);
				}
		}
	}
}
