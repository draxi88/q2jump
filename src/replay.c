#include "g_local.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Stop_Recording(edict_t *ent)
{
	int index;
	index = ent - g_edicts - 1;
	client_record[index].allow_record = false;
}

void		Save_Recording(edict_t *ent, int uid)//,int uid_1st)
{
	FILE	*f;
	char	name[256];
	int index;
	cvar_t	*tgame;

	index = ent - g_edicts - 1;
	if (!client_record[index].current_frame)
		return;
	client_record[index].allow_record = false;

	tgame = gi.cvar("game", "jump", 0);

#ifdef ANIM_REPLAY
	sprintf(name, "%s/jumpdemo/%s.dj2", tgame->string, level.mapname);
#else
	sprintf(name, "%s/jumpdemo/%s.dj1", tgame->string, level.mapname);
#endif


	f = fopen(name, "wb");

	if (!f)
		return;

	fwrite(client_record[index].data, sizeof(record_data), client_record[index].current_frame, f);
	//now put it in local data
	level_items.recorded_time_frames[0] = client_record[index].current_frame;
	memcpy(level_items.recorded_time_data[0], client_record[index].data, sizeof(client_record[index].data));

	fclose(f);
}

void Save_Individual_Recording(edict_t *ent)
{
	FILE	*f;
	char	name[256];
	int index;
	cvar_t	*tgame;

	index = ent - g_edicts - 1;
	if (!client_record[index].current_frame)
		return;
	client_record[index].allow_record = false;

	tgame = gi.cvar("game", "jump", 0);

	sprintf(name, "%s/jumpdemo/%s_%d.dj3", tgame->string, level.mapname, ent->client->resp.uid - 1);
	f = fopen(name, "wb");
	if (!f)
		return;

	fwrite(client_record[index].data, sizeof(record_data), client_record[index].current_frame, f);
	fclose(f);
}

void Save_Current_Recording(edict_t *ent)
{
	int index;
	index = ent - g_edicts - 1;
	level_items.recorded_time_frames[MAX_HIGHSCORES] = client_record[index].current_frame;
	memcpy(level_items.recorded_time_data[MAX_HIGHSCORES], client_record[index].data, sizeof(client_record[index].data));
}

void Record_Frame(edict_t *ent)
{
	int index;
	int store;
	index = ent - g_edicts - 1;

	if (!ent->client->resp.paused)
		if ((client_record[index].allow_record) && (ent->client->resp.ctf_team == CTF_TEAM2 || (gametype->value == GAME_CTF && ent->client->resp.ctf_team == CTF_TEAM1)))
		{
			if (client_record[index].current_frame < MAX_RECORD_FRAMES)
			{
				VectorCopy(ent->s.origin, client_record[index].data[client_record[index].current_frame].origin);
				VectorCopy(ent->client->v_angle, client_record[index].data[client_record[index].current_frame].angle);
#ifdef ANIM_REPLAY

				store = ent->s.frame | ((ent->client->pers.fps & 255) << RECORD_FPS_SHIFT);
				if (ent->client->resp.key_back)
					store |= RECORD_KEY_BACK;
				else if (ent->client->resp.key_forward)
					store |= RECORD_KEY_FORWARD;
				if (ent->client->resp.key_up)
					store |= RECORD_KEY_UP;
				else if (ent->client->resp.key_down)
					store |= RECORD_KEY_DOWN;
				if (ent->client->resp.key_left)
					store |= RECORD_KEY_LEFT;
				else if (ent->client->resp.key_right)
					store |= RECORD_KEY_RIGHT;
				if (ent->client->buttons & BUTTON_ATTACK)
					store |= RECORD_KEY_ATTACK;


				client_record[index].data[client_record[index].current_frame].frame = store;
#endif
				client_record[index].current_frame++;
			}
		}
}

void Cmd_Replay(edict_t *ent)
{
	int i;
	qboolean done_num = false;
	int num;
	char txt[1024];
	char	temp[128];

	//if (ent->client->resp.replay_speed==0)
	ent->client->resp.replay_speed = REPLAY_SPEED_ONE;

	if (ent->client->chase_target)
	{
		// ===================================================
		// added by lilred
		ent->client->chase_target = NULL;
		ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
		// ===================================================
	}

	strncpy(temp, gi.argv(1), sizeof(temp) - 1);
	if (strcmp(temp, "now") == 0)
	{
		if (level_items.recorded_time_frames[MAX_HIGHSCORES])
		{
			ent->client->resp.replaying = MAX_HIGHSCORES + 1;
			ent->client->resp.replay_frame = 0;
			gi.cprintf(ent, PRINT_HIGH, "Replaying %s who finished in %1.3f seconds.\n", level_items.item_owner, level_items.fastest_time);
		}
		else {
			gi.cprintf(ent, PRINT_HIGH, "No time set this map.\n");
		}
	}
	else
		if (strcmp(temp, "list") == 0)
		{
			Com_sprintf(txt, sizeof(txt), "No. Player               Time");
			gi.cprintf(ent, PRINT_HIGH, "\n%s\n", HighAscii(txt));
			for (i = 0; i < MAX_HIGHSCORES; i++)
			{
				if (level_items.recorded_time_frames[i])
				{
					gi.cprintf(ent, PRINT_HIGH, "%2d. %-16s %8.3f\n", i + 1, maplist.users[maplist.times[level.mapnum][i].uid].name, maplist.times[level.mapnum][i].time);
				}
			}
			return;
		}

		else
		{
			num = atoi(temp);
			num--;
			done_num = false;
			//2 to 15
			if (num >= 1 && num < 15)
			{
				//time set?
				if (maplist.times[level.mapnum][num].time > 0)
				{
					//if uid doestn match that stored load demo again
					if (level_items.recorded_time_uid[num] != maplist.times[level.mapnum][num].uid)
					{
						//					gi.bprintf(PRINT_HIGH,"loading recording %d %d\n",level_items.recorded_time_uid[num],maplist.times[level.mapnum][num].uid);
						Load_Individual_Recording(num, maplist.times[level.mapnum][num].uid);
					}
					if (level_items.recorded_time_frames[num])
					{
						//demo exists?
						done_num = true;
						ent->client->resp.replaying = num + 1;
						ent->client->resp.replay_frame = 0;
						gi.cprintf(ent, PRINT_HIGH, "Replaying %s who finished in %1.3f seconds.\n", maplist.users[maplist.times[level.mapnum][num].uid].name, maplist.times[level.mapnum][num].time);
					}
				}
				if (!done_num)
				{
					gi.cprintf(ent, PRINT_HIGH, "No Demo exists for that position.\n");
					return;
				}
			}
			if (!done_num)
			{
				if (level_items.recorded_time_frames[0])
				{
					ent->client->resp.replaying = 1;
					ent->client->resp.replay_frame = 0;
					gi.cprintf(ent, PRINT_HIGH, "Replaying %s who finished in %1.3f seconds.\n", maplist.users[maplist.times[level.mapnum][0].uid].name, maplist.times[level.mapnum][0].time);
					gi.cprintf(ent, PRINT_HIGH, "Hit forward and back keys to change demo speed, jump to toggle repeating.\n");
					gi.cprintf(ent, PRINT_HIGH, "Type replay list to see all replays available.\n");
				}
				else {
					gi.cprintf(ent, PRINT_HIGH, "No Demo available.\n");
					return;
				}
			}
			//gi.cprintf(ent,PRINT_HIGH,"You can type 'replay now' to see a demo of fastest run this map.\n");
		}
	CTFReplayer(ent);
	ClearPersistants(&ent->client->pers);
	if (ent->client->resp.store->checkpoints > 0) {
		ClearCheckpoints(ent);
	}
	hud_footer(ent);
}

void Load_Recording(void)
{
	//load recording using level.mapname
	FILE	*f;
	char	name[256];
	cvar_t	*tgame;
	int i;
	long lSize;
	qboolean loaded = false;

	tgame = gi.cvar("game", "", 0);

	//multireplay code, look for dj3 first via uid of player at 0
	if (maplist.times[level.mapnum][0].uid >= 0)
	{
		sprintf(name, "%s/jumpdemo/%s_%d.dj3", tgame->string, level.mapname, maplist.times[level.mapnum][0].uid);
		f = fopen(name, "rb");
		loaded = true;
	}
	if (!loaded || !f)
	{
#ifdef ANIM_REPLAY
		sprintf(name, "%s/jumpdemo/%s.dj2", tgame->string, level.mapname);
#else
		sprintf(name, "%s/jumpdemo/%s.dj1", tgame->string, level.mapname);
#endif
		f = fopen(name, "rb");
	}

	for (i = 0; i < MAX_HIGHSCORES + 1; i++)
	{
		level_items.recorded_time_frames[i] = 0;
		level_items.recorded_time_uid[i] = -1;
	}

	if (!f)
	{
		level_items.recorded_time_frames[0] = 0;
		return;
	}

	fseek(f, 0, SEEK_END);
	lSize = ftell(f);
	rewind(f);

	fread(level_items.recorded_time_data[0], 1, lSize, f);
	//now put it in local data
	level_items.recorded_time_frames[0] = lSize / sizeof(record_data);

	fclose(f);

}

void Load_Individual_Recording(int num, int uid)
{
	//load recording using level.mapname
	FILE	*f;
	char	name[256];
	cvar_t	*tgame;
	long lSize;

	if (num < 1 || num >= MAX_HIGHSCORES)
		return;
	if (uid < 0)
		return;
	level_items.recorded_time_frames[num] = 0;
	level_items.recorded_time_uid[num] = -1;
	tgame = gi.cvar("game", "", 0);
	sprintf(name, "%s/jumpdemo/%s_%d.dj3", tgame->string, level.mapname, uid);
	//gi.dprintf("%s\n",name);
	f = fopen(name, "rb");


	if (!f)
	{
		return;
	}

	fseek(f, 0, SEEK_END);
	lSize = ftell(f);
	rewind(f);
	level_items.recorded_time_uid[num] = uid;

	fread(level_items.recorded_time_data[num], 1, lSize, f);
	//now put it in local data
	level_items.recorded_time_frames[num] = lSize / sizeof(record_data);

	fclose(f);

}

void Replay_Recording(edict_t *ent)
{
	int i;
	int temp;
	double frame_fraction;
	double frame_integer;
	double frame_fraction2;
	double frame_integer2;
	vec3_t prev_frame;
	vec3_t prev_angle;
	vec3_t next_frame;
	vec3_t next_angle;
	vec3_t diff_frame;
	vec3_t diff_angle;
	vec3_t rep_speed1;
	vec3_t rep_speed2;
	int rep_speed;

	temp = ent->client->resp.replaying - 1;
	if (temp >= 0)
		if (ent->client->resp.replay_frame < level_items.recorded_time_frames[temp])
		{
			ent->client->ps.pmove.pm_type = PM_FREEZE;
			ent->viewheight = 0;
			ent->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
			// trigger_push or anything that potentially modifies player's velocity
			// will screw up the view on the client.
			VectorClear(ent->velocity);
			VectorClear(ent->client->oldvelocity);

			//need to get fraction and whole value of replay_frame

			frame_fraction2 = modf(replay_speed_modifier[ent->client->resp.replay_speed], &frame_integer2);
			frame_fraction = modf(ent->client->resp.replay_frame, &frame_integer);
			if (frame_fraction2)
			{
				//if we have a fraction, process new origin/angles
				VectorCopy(level_items.recorded_time_data[temp][(int)frame_integer].origin, prev_frame);
				VectorCopy(level_items.recorded_time_data[temp][(int)frame_integer].angle, prev_angle);
				if (frame_integer > 0)
				{
					VectorCopy(level_items.recorded_time_data[temp][(int)frame_integer + 1].origin, next_frame);
					VectorCopy(level_items.recorded_time_data[temp][(int)frame_integer + 1].angle, next_angle);
				}
				else
				{
					VectorCopy(level_items.recorded_time_data[temp][(int)frame_integer - 1].origin, next_frame);
					VectorCopy(level_items.recorded_time_data[temp][(int)frame_integer - 1].angle, next_angle);
				}
				ent->client->resp.replay_data = level_items.recorded_time_data[temp][(int)frame_integer].frame;

				VectorSubtract(next_frame, prev_frame, diff_frame);
				VectorSubtract(next_angle, prev_angle, diff_angle);
				for (i = 0; i < 3; i++)
				{
					diff_frame[i] = diff_frame[i] * frame_fraction;
					if (diff_angle[i] > 180)
					{
						//					gi.cprintf(ent,PRINT_HIGH,">180\n");
						diff_angle[i] = -360.0f + diff_angle[i];
					}
					if (diff_angle[i] < -180)
					{
						//					gi.cprintf(ent,PRINT_HIGH,"<180\n");
						diff_angle[i] = 360.0f + diff_angle[i];
					}
					diff_angle[i] = diff_angle[i] * frame_fraction;
					prev_frame[i] += diff_frame[i];
					prev_angle[i] += diff_angle[i];
				}

				VectorCopy(prev_frame, ent->s.origin);
				VectorCopy(prev_angle, ent->client->v_angle);
				VectorCopy(prev_angle, ent->client->ps.viewangles);
				for (i = 0; i < 3; i++)
					ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(prev_angle[i] - ent->client->resp.cmd_angles[i]);
			}
			else
			{
				if (ent->client->resp.replay_speed != REPLAY_SPEED_ZERO)
					if (frame_fraction)
						ent->client->resp.replay_frame = frame_integer;

				ent->client->resp.replay_data = level_items.recorded_time_data[temp][(int)ent->client->resp.replay_frame].frame;

				VectorCopy(level_items.recorded_time_data[temp][(int)ent->client->resp.replay_frame].origin, ent->s.origin);
				VectorCopy(level_items.recorded_time_data[temp][(int)ent->client->resp.replay_frame].angle, ent->client->v_angle);
				VectorCopy(level_items.recorded_time_data[temp][(int)ent->client->resp.replay_frame].angle, ent->client->ps.viewangles);

				for (i = 0; i < 3; i++)
					ent->client->ps.pmove.delta_angles[i] = ANGLE2SHORT(level_items.recorded_time_data[temp][(int)ent->client->resp.replay_frame].angle[i] - ent->client->resp.cmd_angles[i]);
			}
			if (ent->client->resp.replay_speed != REPLAY_SPEED_ZERO)
				ent->client->resp.replay_frame += replay_speed_modifier[ent->client->resp.replay_speed];
			if (ent->client->resp.replay_frame <= 0)
			{
				if (ent->client->resp.rep_repeat)
				{
					ent->client->resp.replay_frame = level_items.recorded_time_frames[temp] - 1;
				}
				else
				{
					ent->client->resp.replaying = 0;
					ent->client->resp.replay_speed = REPLAY_SPEED_ONE;
				}
			}
			//replay speedometer a la Killa
			if (ent->client->resp.replaying) {
				VectorCopy(level_items.recorded_time_data[temp][(int)ent->client->resp.replay_frame - 10].origin, rep_speed1);
				rep_speed1[2] = 0;
				VectorCopy(level_items.recorded_time_data[temp][(int)ent->client->resp.replay_frame].origin, rep_speed2);
				rep_speed2[2] = 0;
				VectorSubtract(rep_speed1, rep_speed2, rep_speed1);
				rep_speed = (int)fabs(VectorLength(rep_speed1));
				//Don't update rep_speed if it's not 10 ups faster/slower than current rep_speed.
				if (rep_speed > ent->client->resp.rep_speed + 10 || rep_speed < ent->client->resp.rep_speed - 10)
					ent->client->resp.rep_speed = rep_speed;
			}
		}
		else {
			// =========================
			// added by lilred
			if (ent->client->resp.rep_repeat)
			{
				ent->client->resp.replay_frame = 0;
				return;
			}
			// =========================
			ent->client->resp.replaying = 0;
			ent->client->resp.replay_speed = REPLAY_SPEED_ONE;
		}
}
