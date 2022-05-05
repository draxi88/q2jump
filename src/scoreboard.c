#include "g_local.h"
#include "m_player.h"

void BestTimesScoreboardMessage(edict_t *ent, edict_t *killer)
{
	char	string[1400];
	int i;
	int completions = 0;
	int total_count = 0;
	char chr[2];
	char	colorstring[16];
	*string = 0;
	chr[0] = 13;
	chr[1] = 0;
	//get total completions
	completions = 0;
	for (i = 0; i < MAX_USERS; i++)
	{
		if (!maplist.times[level.mapnum][i].completions)
			continue;
		total_count += maplist.times[level.mapnum][i].completions;
		completions++;
	}

	sprintf(string + strlen(string), "xv 0 yv 0 string2 \"No   Player                    Date \" ");
	for (i = 0; i < MAX_HIGHSCORES; i++)
	{
		sprintf(colorstring, "string");
		//gi.dprintf("i:%i -> name:%s\n", i, maplist.users[maplist.times[level.mapnum][i].uid].name);
		if (strlen(maplist.users[maplist.times[level.mapnum][i].uid].name)>1)
		{
			if (Q_stricmp(maplist.times[level.mapnum][i].date,maplist.today)==0)
			{

				sprintf(string + strlen(string), "yv %d %s \"%2d%s *%-16s%8.3f  %s\" ", i * 10 + 16, colorstring, i + 1, (level_items.recorded_time_frames[i] == 0 ? " " : chr),
					maplist.users[maplist.times[level.mapnum][i].uid].name, maplist.times[level.mapnum][i].time
					, maplist.times[level.mapnum][i].date
				);
			}
			else {
				sprintf(string + strlen(string), "yv %d %s \"%2d%s  %-16s%8.3f  %s\" ", i * 10 + 16, colorstring, i + 1, (level_items.recorded_time_frames[i] == 0 ? " " : chr),
					maplist.users[maplist.times[level.mapnum][i].uid].name, maplist.times[level.mapnum][i].time
					, maplist.times[level.mapnum][i].date
				);
			}
		}
		else {
			sprintf(string + strlen(string), "yv %d string \"%2d \" ", i * 10 + 16, i + 1);
		}
	}
	sprintf(string + strlen(string), "yv %d string \"    %d players completed map %d times\" ", i * 10 + 24, completions, total_count);
	//	gi.bprintf(PRINT_HIGH,"%d\n",strlen(string));
	gi.WriteByte(svc_layout);
	gi.WriteString(string);
}

void JumpModScoreboardMessage(edict_t *ent, edict_t *killer)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j, k, n;
	int		sorted[MAX_CLIENTS];
	float		sortedscores[MAX_CLIENTS];
	float	score;
	int		total;
	int		picnum;
	int		x, y;
	gclient_t	*cl;
	edict_t		*cl_ent;
	char	*tag;
	char status[32];
	int maplist_uid;
	int total_easy;
	int total_specs;
	char teamstring[5];
	qboolean idle;

	idle = false;

	// sort the clients by score
	total = 0;
	for (i = 0; i < maxclients->value; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse)
			continue;
		if (gametype->value == GAME_CTF)
		{
			if (cl_ent->client->resp.ctf_team < CTF_TEAM1)
				continue;
		}
		else
		{
			if (cl_ent->client->resp.ctf_team != CTF_TEAM2)
				continue;
		}
		if (cl_ent->client->resp.uid > 0)
			score = cl_ent->client->resp.suid;//cl_ent->client->resp.best_time;
		else
			score = -1;
		if (score < 0)
			score = 99998;
		score++;
		for (j = 0; j < total; j++)
		{
			if (score < sortedscores[j])
				break;
		}
		for (k = total; k > j; k--)
		{
			sorted[k] = sorted[k - 1];
			sortedscores[k] = sortedscores[k - 1];
		}
		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	// print level name and exit rules
	string[0] = 0;

	stringlength = strlen(string);

	// add the clients in sorted order
	//if (total > 16)
	//	total = 16;


	Com_sprintf(entry, sizeof(entry),
		"xv -16 yv 0 string2 \"Ping Pos Player          Best Comp Maps     %%  Team\" ");
	j = strlen(entry);
	strcpy(string + stringlength, entry);
	stringlength += j;

	for (i = 0; i < total; i++)
	{
		cl = &game.clients[sorted[i]];
		cl_ent = g_edicts + 1 + sorted[i];

		picnum = gi.imageindex("i_fixme");
		y = 16 + 10 * i;

		maplist_uid = -1;
		if (cl->resp.uid > 0)
		{
			maplist_uid = FindMaplistUID(level.mapnum, cl->resp.uid - 1);
		}

		// send the layout
		if (cl->pers.idle_player_state != PLAYERIDLE_STATE_NONE)
		{
			strcpy(teamstring, "Idle");
		}
		else {
			strcpy(teamstring, "Hard");
		}
		if (cl->resp.best_time)
		{
			Com_sprintf(entry, sizeof(entry),
				"ctf %d %d %d %d %d xv 152 string \"%8.3f %4i %4i  %4.1f  %s\"",
				-8, y, sorted[i], cl->ping, cl->resp.suid + 1,
				maplist.times[level.mapnum][maplist_uid].time,
				maplist.times[level.mapnum][maplist_uid].completions,

				maplist.sorted_completions[cl->resp.suid].score,
				(float)maplist.sorted_completions[cl->resp.suid].score / (float)maplist.nummaps * 100,
				teamstring
			);

		}
		else
		{
			if (cl->resp.uid > 0)
			{
				Com_sprintf(entry, sizeof(entry),
					"ctf %d %d %d %d %d xv 152 string \"  ------ ---- %4i  %4.1f  %s\"",
					-8, y, sorted[i], cl->ping, cl->resp.suid + 1,
					maplist.sorted_completions[cl->resp.suid].score,
					(float)maplist.sorted_completions[cl->resp.suid].score / (float)maplist.nummaps * 100,
					teamstring
				);

			}
			else
			{
				Com_sprintf(entry, sizeof(entry),
					"ctf %d %d %d %d %d xv 152 string \"    ------ ----           %s\"",
					-8, y, sorted[i], cl->ping, 1000, teamstring
				);

			}
		}
		j = strlen(entry);
		if (stringlength + j > 1024)
			break;
		strcpy(string + stringlength, entry);
		stringlength += j;


	}

	//easy team
	total_easy = 0;
	total_specs = 0;
	if (gametype->value != GAME_CTF)
		for (i = 0; i < maxclients->value; i++)
		{
			cl = &game.clients[i];
			cl_ent = g_edicts + 1 + i;
			if (!cl_ent->inuse)
				continue;
			if (cl_ent->client->resp.ctf_team == CTF_NOTEAM)
			{
				total_specs++;
				continue;
			}
			if (cl_ent->client->resp.ctf_team != CTF_TEAM1)
				continue;

			if (total)
			{
				//if hard team has players, increase gap
				y = 24 + (10 * (total_easy + total));
			}
			else
			{
				y = 16 + (10 * (total_easy));
			}
			maplist_uid = -1;
			if (cl->resp.uid > 0)
			{
				maplist_uid = FindMaplistUID(level.mapnum, cl->resp.uid - 1);
			}

			if (cl->pers.idle_player_state != PLAYERIDLE_STATE_NONE)
			{
				strcpy(teamstring, "Idle");
			}
			else {
				strcpy(teamstring, "Easy");
			}
			if (cl->resp.best_time)
			{
				Com_sprintf(entry, sizeof(entry),
					"ctf %d %d %d %d %d xv 152 string \"%8.3f %4i %4i  %4.1f  %s\"",
					-8, y, i, cl->ping, cl->resp.suid + 1,
					maplist.times[level.mapnum][maplist_uid].time,
					maplist.times[level.mapnum][maplist_uid].completions,

					maplist.sorted_completions[cl->resp.suid].score,
					(float)maplist.sorted_completions[cl->resp.suid].score / (float)maplist.nummaps * 100,
					teamstring
				);

			}
			else
			{
				if (cl->resp.uid > 0)
				{
					Com_sprintf(entry, sizeof(entry),
						"ctf %d %d %d %d %d xv 152 string \"  ------ ---- %4i  %4.1f  %s\"",
						-8, y, i, cl->ping, cl->resp.suid + 1,
						maplist.sorted_completions[cl->resp.suid].score,
						(float)maplist.sorted_completions[cl->resp.suid].score / (float)maplist.nummaps * 100,
						teamstring
					);

				}
				else
				{
					Com_sprintf(entry, sizeof(entry),
						"ctf %d %d %d %d %d xv 152 string \"    ------ ----           %s\"",
						-8, y, i, cl->ping, 1000, teamstring
					);

				}
			}

			j = strlen(entry);
			if (stringlength + j > 1024)
				break;
			strcpy(string + stringlength, entry);
			stringlength += j;
			total_easy++;
		}

	//spectators

	if ((total) && (total_easy))
	{
		//if we have players on both teams, theres an extra 8 gap
		y = 48 + (8 * (total + total_easy));
	}
	else
	{
		y = 40 + (8 * (total + total_easy));
	}

	if (total_specs) {
		Com_sprintf(entry, sizeof(entry),
			"xv -16 yv %d string2 \"Spectators\" ", y);
		j = strlen(entry);
		strcpy(string + stringlength, entry);
		stringlength += j;
	}

	//any spectators idle, if so, add extra gap for the idle tag...
	for (i = 0; i < maxclients->value; i++) {
		cl = &game.clients[i];
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse)
			continue;
		if (cl_ent->client->resp.ctf_team != CTF_NOTEAM)
			continue;
		if (cl_ent->client->pers.idle_player_state != PLAYERIDLE_STATE_NONE)
			idle = true;
	}
	total_specs = 0;
	for (i = 0; i < maxclients->value; i++)
	{
		cl = &game.clients[i];
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse)
			continue;
		if (cl_ent->client->resp.ctf_team != CTF_NOTEAM)
			continue;
		if ((total) && (total_easy))
		{
			//if we have players on both teams, theres an extra 8 gap
			y = 56 + (8 * (total + total_easy + total_specs));
		}
		else
		{
			y = 48 + (8 * (total + total_easy + total_specs));
		}

		//add idle tag if spectator is idle.
		if (cl->pers.idle_player_state != PLAYERIDLE_STATE_NONE) //add idle tag to chaser
		{
			Com_sprintf(entry, sizeof(entry),
				"xv %d yv %d string \" (idle)\"", 56 + (strlen(cl->pers.netname) * 8), y);
			j = strlen(entry);
			strcpy(string + stringlength, entry);
			stringlength += j;
		}

		if (cl->resp.replaying)
		{
			if (cl->resp.replaying == MAX_HIGHSCORES + 1)
				Com_sprintf(entry, sizeof(entry),
					"ctf %d %d %d %d %d xv %d string \" (Replay now)\"",
					-8, y, i,
					cl->ping,
					0, idle ? 224 : 168
				);
			else
				Com_sprintf(entry, sizeof(entry),
					"ctf %d %d %d %d %d xv %d string \" (Replay %d)\"",
					-8, y, i,
					cl->ping,
					0, idle ? 224 : 168,
					cl->resp.replaying

				);
		}
		else
		{
			Com_sprintf(entry, sizeof(entry),
				"ctf %d %d %d %d %d xv %d string \"%s%s\"",
				-8, y, i,
				cl->ping,
				0, idle ? 224 : 168,
				cl->chase_target ? " -> " : "",
				cl->chase_target ? cl->chase_target->client->pers.netname : ""
			);
		}


		j = strlen(entry);
		if (stringlength + j > 1024)
			break;
		strcpy(string + stringlength, entry);
		stringlength += j;

		total_specs++;
	}

	y += 64;

	Com_sprintf(entry, sizeof(entry),
		"xv -16 yv %d string2 \"Next Maps (type nominate <map> or rand)\" yv %d stat_string 3 yv %d stat_string 5 yv %d stat_string 12 ", y, y + 16, y + 24, y + 32
	);
	j = strlen(entry);
	strcpy(string + stringlength, entry);
	stringlength += j;

	gi.WriteByte(svc_layout);
	gi.WriteString(string);
}
