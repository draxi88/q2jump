#include "g_local.h"
#include "g_wireplay.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int curclients = 0;
int activeclients = 0;
int map_added_time = 0;
qboolean map_allow_voting = true;
/* Help Data */

char map_skill[10][10];
char map_skill2[10][10];

static char *help_main[] = {
	"\n--------------------------------------------------\n",
	"\xd6\xef\xf4\xe9\xee\xe7 \xc3\xef\xed\xed\xe1\xee\xe4\xf3\n", // Voting Commands
	"maplist - view all maps on the server\n",
	"mapvote - vote a specific map, type mapvote for more details\n",
	"timevote - vote more time to play\n",
	"rand - randomize the votemaps\n",
	"boot - vote to kick a player\n",
	"silence - vote to silence a player\n",
	"\n\xc7\xe5\xee\xe5\xf2\xe1\xec \xc3\xef\xed\xed\xe1\xee\xe4\xf3\n", // General Commands
	"hook - bind a key to +hook in order to use\n",
	"cmsg - enable/disable messages triggered in the map\n",
	"replay - replay # to view replays 1-15\n",
	"jumpers - turn on or off player models\n",
	"cpsound - turn on or off checkpoint sounds\n",
	"showtimes - turn on or off displaying all times\n",
	"mute_cprep - turn on or off displaying replays cp-time\n",
	"store - place a marker that stores your location\n",
	"recall / kill - return to your store location\n",
	"reset - removes your store location\n",
	"velstore - toggles velocity storing for your store markers\n", //velocity store feature
	"playerlist - list the players in game\n",
	"\n\xd3\xf4\xe1\xf4\xe9\xf3\xf4\xe9\xe3\xf3\n", // Statistics
	"maptimes - view best times on a map\n",
	"playertimes - view overall points in the server\n",
	"playerscores - view best points per map players\n",
	"playermaps - view the players who have done the most maps\n",
	"mapsdone - the maps on the server you have done\n",
	"mapsleft - the maps on the server you haven't done\n",
	"!stats - view individual stats for players\n",
	"compare - compare yourself to another player\n",
	"1st - view first places set in the last 24 hours\n",
	"!seen - view when a player was last in the server\n",
	"--------------------------------------------------\n\n",
	NULL
};


char moddir[256];
cvar_t		*allow_admin_log;

//vars
cvar_t		*time_remaining;
cvar_t			*jumpmod_version;
cvar_t			*enable_autokick;
cvar_t			*autokick_time;
int server_time;
int				num_admins = 0;
int				num_bans = 0;
maplist_t		maplist; 
manual_t		manual; 

cvar_t			*gametype;
FILE			*admin_file;
FILE			*debug_file;
FILE			*tourney_file;

admin_type		admin_pass[MAX_ADMINS];
qboolean		jump_show_stored_ent;
level_items_t	level_items;
rpos			client_record[16];
ban_t			bans[MAX_BANS];
	qboolean new_maplist = false;

int LoadMapList(char *filename) 
{ 
	struct	tm *current_date;
	time_t	time_date;
	int		month,day,year;

	
	FILE *fp; 
   int  i=0; 
   int  i2=0; 
   char szLineIn[80];
   char date_marker[80];
   int	ourgtype = 0;
	cvar_t	*port;
	cvar_t	*tgame;
	char	name[256];
	qboolean convert;
	FILE *f_demo;
   qboolean done_a_convert = false;
	int duplicate;

   fp = OpenFile2(filename); 

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

   //reset users
   maplist.num_users = 0;
   maplist.sort_num_users = 0;
   //read in users file
   open_users_file();

	strcpy(maplist.path,filename);
	
	//set the date
		time_date = time(NULL);                // DOS system call
		current_date = localtime(&time_date);  // DOS system call
		month = current_date->tm_mon + 1;
		day = current_date->tm_mday;
		year = current_date->tm_year;

   if (fp)  // opened successfully? 
   { 
      // scan for [maplist] section 
      do 
      { 
         fscanf(fp, "%s", szLineIn); 
		 /*if (Q_stricmp(szLineIn, "[JumpMod2020]") == 0)
		 {
			 new_maplist = true;
		 }*/

      } while (!feof(fp) && (Q_stricmp(szLineIn, "[maplist]") != 0)); 

      if (feof(fp)) 
      { 
         // no [maplist] section 
         gi.dprintf ("-------------------------------------\n"); 
         gi.dprintf ("ERROR - No [maplist] section in \"%s\".\n", filename); 
         gi.dprintf ("-------------------------------------\n"); 
      } 
      else 
      { 
         gi.dprintf ("-------------------------------------\n"); 
  
         // read map names into array 
         while ((!feof(fp)) && (i<MAX_MAPS)) 
         { 
            fscanf(fp, "%s", szLineIn); 

            if (Q_stricmp(szLineIn, "###") == 0)  // terminator is "###" 
               break; 

			duplicate = -1;
			//locate duplicates
			for (i2=0;i2<i;i2++) 
			{  
					if (Q_stricmp(szLineIn,maplist.mapnames[i2])==0) 
					{  
						duplicate  = i2;
						break;;
					} 
			} 

			if (duplicate==-1)
				strncpy(maplist.mapnames[i], szLineIn, MAX_MAPNAME_LEN); 

			if (duplicate!=-1)				
			{
				gi.dprintf("DUPLICATE REMOVED: %s\n",szLineIn);
				continue;
			}

			maplist.demoavail[i] = false;
            i++; 
         } 
		 maplist.nummaps = i;
		 gi.dprintf("%i map(s) loaded.\n", i);
		 gi.dprintf("-------------------------------------\n");
      }
	  resync(true);
	  sort_users();
      CloseFile(fp); 
      if (i == 0) 
      { 
         gi.dprintf ("No maps listed in [maplist] section of %s\n", filename); 
         gi.dprintf ("-------------------------------------\n"); 
         return 0;  // abnormal exit -- no maps in file 
      } 
  
	  WriteMapList();
      return 1; // normal exit 
   } 
   else
   {
		sprintf (maplist.mapnames[0],"q2dm1");
		sprintf (maplist.mapnames[1],"q2dm2");
		sprintf (maplist.mapnames[2],"q2dm3");
		maplist.nummaps = 3;
   }

	return 0;
} 
  
int LoadManualList(char *filename) 
{ 
	
   FILE *fp; 
   int  i=0; 
   char szLineIn[80]; 

   fp = OpenFile2(filename); 

   if (fp)  // opened successfully? 
   { 
      // scan for [manual] section 
      do 
      { 
         fscanf(fp, "%s", szLineIn); 
      } while (!feof(fp) && (Q_stricmp(szLineIn, "[manual]") != 0)); 

      if (feof(fp)) 
      { 
         // no [manual] section 
         gi.dprintf ("-------------------------------------\n"); 
         gi.dprintf ("ERROR - No [manual] section in \"%s\".\n", filename); 
         gi.dprintf ("-------------------------------------\n"); 
      } 
      else 
      { 
         gi.dprintf ("-------------------------------------\n"); 
  
         // read map names into array 
         while ((!feof(fp)) && (i<MAX_MANUAL)) 
         { 
            fscanf(fp, "%s", szLineIn); 

            if (Q_stricmp(szLineIn, "###") == 0)  // terminator is "###" 
               break; 

            // TODO: check that maps exist before adding to list 
            //       (might be difficult to search a .pak file for these) 

            strncpy(manual.manual[i], szLineIn, MAX_MANUAL_LEN); 
            gi.dprintf("...%s\n", manual.manual[i]); 
            i++; 
         } 

         strncpy(manual.filename, filename, 20); 
      } 

      CloseFile(fp); 

      if (i == 0) 
      { 
         gi.dprintf ("No manual found in [manual] section of %s\n", filename); 
         gi.dprintf ("-------------------------------------\n"); 
         return 0;  // abnormal exit -- no maps in file 
      } 
  
      gi.dprintf ("%i manaul line(s) loaded.\n", i); 
      gi.dprintf ("-------------------------------------\n"); 
      manual.numlines = i; 
      return 1; // normal exit 
   } 
  
   return 0;  // abnormal exit -- couldn't open file 
} 

void ShowCurrentManual(edict_t *ent) 
{ 
   int i; 

	if (manual.numlines > 0)  // does a maplist exist? 
	{	
		gi.cprintf (ent, PRINT_HIGH, "-------------------------------------\n"); 

		if (ent==NULL)     // only show filename to server 
			gi.dprintf ("FILENAME: %s\n", manual.filename); 

		for (i=0; (i<manual.numlines) ; i++) 
		{ 
			gi.cprintf (ent, PRINT_HIGH, "#%2d \"%s\"\n", i+1, manual.manual[i]);
		} 

		gi.cprintf (ent, PRINT_HIGH, "-------------------------------------\n"); 
	}
} 

void ClearMapList() 
{ 
   maplist.nummaps = 0; 
   dmflags->value = (int) dmflags->value & ~DF_MAP_LIST; 
   gi.dprintf ("Maplist cleared/disabled.\n"); 
} 
 
void ShowCurrentMaplist(edict_t *ent,int offset) 
{ 
   int i; 
	char name[64];
   offset--;
   if (offset<0)
	   offset = 0;
	
   gi.cprintf (ent, PRINT_HIGH, "--------------------------------------------------------\n"); 

   for (i=(20*offset); (i<maplist.nummaps) && (i<(20*offset)+20); i++) 
   { 	   
	   if (maplist.times[i][0].time>0)
	   {
		  Com_sprintf(name,sizeof(name),maplist.users[maplist.times[i][0].uid].name);
		  Highlight_Name(name);
  		  gi.cprintf (ent, PRINT_HIGH, "%-3d %5s %-20s %-16s %8.3f\n", i+1, map_skill[maplist.skill[i]],maplist.mapnames[i],name,maplist.times[i][0].time);
	   }
	   else
			gi.cprintf (ent, PRINT_HIGH, "%-3d %5s %-20s %-16s %-6s\n", i+1, map_skill[maplist.skill[i]],maplist.mapnames[i],"","");
   } 

   gi.cprintf (ent, PRINT_HIGH, "Page %d/%1.0f (%i maps). Use maplist <page> or <letter>\n",(offset+1),ceil(maplist.nummaps/20.0),maplist.nummaps); 

   gi.cprintf (ent, PRINT_HIGH, "--------------------------------------------------------\n"); 
} 
  
void Cmd_Maplist_f (edict_t *ent) 
{ 
 int offset;
 int i;
 char mapname[255];
   switch (gi.argc()) 
   { 
   case 1:  // display current maplist 
      if (maplist.nummaps > 0)  // does a maplist exist? 
      { 
         ShowCurrentMaplist(ent,0); 
      } 
      else       // no maplist 
      { 
         gi.cprintf (ent, PRINT_HIGH, "*** No MAPLIST active ***\n"); 
      } 

      break; 

   case 2: 
	   {
		   //todo pooy
		if (maplist.nummaps > 0)  // does a maplist exist?
		{
			offset = atoi(gi.argv(1));
			if (!offset)
			{
				strncpy(mapname,gi.argv(1),sizeof(mapname)-1);
				
				//assume its a name search
				for (i=0;i<maplist.nummaps;i++)
				{
					if (maplist.mapnames[i][0]==mapname[0])
					{
						offset = (int)(floor(i / 20)) + 1;
						break;
					}
				}
			}
		    ShowCurrentMaplist(ent,offset); 
	    } 
	   }
      break; 

   default: 
      break;
   }  // end switch 
} 

void Cmd_Votelist_f (edict_t *ent) 
{ 
 int offset;
 int i;
   switch (gi.argc()) 
   { 
   case 1:  // display current maplist 
      if (maplist.nummaps > 0)  // does a maplist exist? 
      { 
         ShowCurrentVotelist(ent,0); 
      } 
      else       // no maplist 
      { 
         gi.cprintf (ent, PRINT_HIGH, "*** No MAPLIST active ***\n"); 
      } 

      break; 

   case 2: 
	   {
		   //todo pooy
		if (maplist.nummaps > 0)  // does a maplist exist?
		{
			offset = atoi(gi.argv(1));
		    ShowCurrentVotelist(ent,offset); 
	    } 
	   }
      break; 

   default: 
      break;
   }  // end switch 
}

typedef struct
{
	char name[64];
} highlight_list_t;
highlight_list_t highlight_list[32];

void Generate_Highlight_List(edict_t *ent)
{
	edict_t *e2;
	int i,i2;
	for (i=0;i<32;i++)
		memset(highlight_list[i].name,0,sizeof(highlight_list[i].name));
	i2 = 0;
	for (i = 1; i <= maxclients->value; i++) 
	{
		e2 = g_edicts + i;
		if (!e2->inuse)
			continue;
		strcpy(highlight_list[i2].name,e2->client->pers.netname);
		i2++;
	}
}

void Highlight_Name(char *name)
{
	int ni,li,len; 
				for (ni=0;ni<32;ni++)
				{
					if (!highlight_list[ni].name[0])
						break;
					if (!strcmp(name,highlight_list[ni].name))
					{
						len = strlen(name);
						for (li=0;li<len;li++)
						{
							name[li] += 128;
						}
						break;
					}
				}
}

qboolean Can_highlight_Name(char *name)
{
	int ni; 

	for (ni=0;ni<32;ni++)
	{
		if (!highlight_list[ni].name[0])
			break;
		if (!strcmp(name,highlight_list[ni].name))
		{
			return true;
		}
	}
	return false;
}

void Cmd_Show_Maptimes_Wireplay(edict_t* ent)
{
    if (gi.argc() < 2)
    {
		print_wireplay_time(ent, level.mapname);
		return;
    }
    else
    {
        print_wireplay_time(ent, gi.argv(1));
        return;
    }
}

void ShowMapTimes(edict_t *ent) 
{ 
	int i; 
	int mapnum;
	char	temp[128];
	char name[32];
	int index;
	float time;
	mapnum = -1;

	//if no args, show current map
	if (gi.argc() < 2) {
		if ((level.mapnum>=0) && (level.mapnum<maplist.nummaps))
			mapnum = level.mapnum;
	goto def;
	}

	strncpy(temp,gi.argv(1),sizeof(temp)-1);
	for (i=0;i<maplist.nummaps;i++)
	{
		if (strcmp(maplist.mapnames[i],temp)==0)
		{
			mapnum = i;
            gi.dprintf("Map: %s  id:%d\n",maplist.mapnames[i],i);
			break;
		}
	}

	if (mapnum<0)
	{	
		mapnum = atoi(temp);
		if (mapnum==0)
			mapnum=-1;

		if (mapnum>0 && mapnum<=maplist.nummaps)
			mapnum--;
	}

def:
	//if still no match, report error
	if ((mapnum<0) || (mapnum>=maplist.nummaps))
	{
		gi.cprintf (ent, PRINT_HIGH, "Invalid map.\n"); 
		return;
	}
	index = ent-g_edicts-1;
	if (!ent->client->resp.uid)
	{
		UpdateThisUsersUID(ent,ent->client->pers.netname);
	}
	/*if (ent->client->resp.uid && !overall_completions[index].loaded)
	{
		write_map_file(level.mapname,level.mapnum);   // 084_h3
		//open their file
		open_uid_file(ent->client->resp.uid-1,ent);
	}*/
	if (maplist.times[mapnum][0].time!=0)
	{
		gi.cprintf (ent, PRINT_HIGH, "-----------------------------------------\n"); 
		gi.cprintf (ent, PRINT_HIGH, "Best Times for %s\n",maplist.mapnames[mapnum]); 
		gi.cprintf (ent, PRINT_HIGH, "\xce\xef\xae \xce\xe1\xed\xe5                 \xc4\xe1\xf4\xe5                    \xd4\xe9\xed\xe5\n"); // No. Name Date Time

		for (i=0;i<MAX_HIGHSCORES;i++)
		{
			if (maplist.times[mapnum][i].time==0)
				break;
			if (maplist.times[mapnum][i].uid>=0)
			{
				if (i)
					time = maplist.times[mapnum][0].time - maplist.times[mapnum][i].time;
				else
					time = maplist.times[mapnum][i].time;				
			  Com_sprintf(name,sizeof(name),maplist.users[maplist.times[mapnum][i].uid].name);
			  Highlight_Name(name);
              gi.cprintf (ent, PRINT_HIGH, "%-3d %-18s   %s  %8.3f %9.3f\n",i+1,name,maplist.times[mapnum][i].date,time,maplist.times[mapnum][i].time);
			}
		} 
		if (maplist.users[ent->client->resp.uid-1].maps_done[mapnum]==1)
			gi.cprintf(ent,PRINT_HIGH,"You have completed this map\n");
		else
			gi.cprintf(ent,PRINT_HIGH,"You have NOT completed this map\n");
		gi.cprintf (ent, PRINT_HIGH, "-----------------------------------------\n"); 
	} else {
		gi.cprintf (ent, PRINT_HIGH, "No Times for %s\n",maplist.mapnames[mapnum]); 
	}
} 

void ShowPlayerTimes(edict_t *ent) 
{ 
   int i; 
	int offset;
	int temp;
	char * pEnd;
	char name[64];
	char txt[1024];
	offset = strtol(gi.argv(1),&pEnd,0);

	offset--;
   if (offset<0)
	   offset = 0;
	
   gi.cprintf (ent, PRINT_HIGH, "\n-----------------------------------------\n\n"); 
   gi.cprintf (ent, PRINT_HIGH, "Point Values: 1-15: 25,20,16,13,11,10,9,8,7,6,5,4,3,2,1 \n"); 
   gi.cprintf (ent, PRINT_HIGH, "\n-----------------------------------------\n\n"); 
   // No. Name 1st 2nd 3rd 4th 5th 6th 7th 8th 9th 10th 11th 12th 13th 14th 15th Score
   gi.cprintf (ent, PRINT_HIGH, "\xce\xef\xae \xce\xe1\xed\xe5             \xb1\xf3\xf4 \xb2\xee\xe4 \xb3\xf2\xe4 \xb4\xf4\xe8 \xb5\xf4\xe8 \xb6\xf4\xe8 \xb7\xf4\xe8 \xb8\xf4\xe8 \xb9\xf4\xe8 \xb1\xb0\xf4\xe8 \xb1\xb1\xf4\xe8 \xb1\xb2\xf4\xe8 \xb1\xb3\xf4\xe8 \xb1\xb4\xf4\xe8 \xb1\xb5\xf4\xe8 \xd3\xe3\xef\xf2\xe5\n"); 
   for (i=(20*offset); (i<maplist.sort_num_users) && (i<(20*offset)+20); i++) 
   { 
	  temp = maplist.sorted_users[i].uid;
	  if (temp>=0)
	  {
		  Com_sprintf(name,sizeof(name),maplist.users[temp].name);
		  Com_sprintf(txt,sizeof(txt),"%-3d %-16s %3d %3d %3d %3d %3d %3d %3d %3d %3d  %3d  %3d  %3d  %3d  %3d  %3d %5d", i+1, name,maplist.users[temp].points[0],maplist.users[temp].points[1],maplist.users[temp].points[2],maplist.users[temp].points[3],
			  maplist.users[temp].points[4],maplist.users[temp].points[5],maplist.users[temp].points[6],maplist.users[temp].points[7],maplist.users[temp].points[8],maplist.users[temp].points[9],maplist.users[temp].points[10],maplist.users[temp].points[11],maplist.users[temp].points[12],maplist.users[temp].points[13],maplist.users[temp].points[14],maplist.users[temp].score);
///		  Highlight_Name(name);
		  if (Can_highlight_Name(name))
			  gi.cprintf (ent, PRINT_HIGH,"%s\n",HighAscii(txt));
		  else	
			  gi.cprintf (ent, PRINT_HIGH,"%s\n",txt);
	      
	  }
   } 
   gi.cprintf (ent, PRINT_HIGH, "Page %d/%1.0f (%i users). Use playertimes <page>\n",(offset+1),ceil(maplist.sort_num_users/20.0),maplist.sort_num_users); 
   gi.cprintf (ent, PRINT_HIGH, "-----------------------------------------\n\n"); 

} 

void ShowPlayerScores(edict_t *ent) 
{ 
   int i; 
   char txt[1024];
	int offset;
	int temp;
	char * pEnd;
	char name[64];
	offset = strtol(gi.argv(1),&pEnd,0);

	offset--;
   if (offset<0)
	   offset = 0;
	
   gi.cprintf (ent, PRINT_HIGH, "\n-----------------------------------------\n\n"); 
   gi.cprintf (ent, PRINT_HIGH, "Score = (Your Score) / (Potential Score if 1st on all Your Completed Maps)\n\n"); 
   gi.cprintf (ent, PRINT_HIGH, "Ex: 5 Maps Completed || 3 1st's, 2 3rd's = 107 pts || 5 1st's = 125pts || 107/125 = 85.6 Percent\n");    
   gi.cprintf (ent, PRINT_HIGH, "\n-----------------------------------------\n\n"); 
   Com_sprintf(txt,sizeof(txt), "No. Name             1st 2nd 3rd 4th 5th 6th 7th 8th 9th 10th 11th 12th 13th 14th 15th Score"); 
	gi.cprintf (ent, PRINT_HIGH,"%s\n",HighAscii(txt));

   for (i=(20*offset); (i<maplist.sort_num_users_israfel) && (i<(20*offset)+20); i++) 
   { 
	  temp = maplist.sorted_israfel[i].uid;
	  if (temp>=0)
	  {
		  Com_sprintf(name,sizeof(name),maplist.users[temp].name);
		  //Highlight_Name(name);
		  Com_sprintf(txt,sizeof(txt),"%-3d %-16s %3d %3d %3d %3d %3d %3d %3d %3d %3d  %3d  %3d  %3d  %3d  %3d  %3d %4.1f%%", i+1, name,maplist.users[temp].points[0],maplist.users[temp].points[1],maplist.users[temp].points[2],maplist.users[temp].points[3],
			  maplist.users[temp].points[4],maplist.users[temp].points[5],maplist.users[temp].points[6],maplist.users[temp].points[7],maplist.users[temp].points[8],maplist.users[temp].points[9],maplist.users[temp].points[10],maplist.users[temp].points[11],maplist.users[temp].points[12],maplist.users[temp].points[13],maplist.users[temp].points[14],
			  maplist.users[temp].israfel
			  );
		  if (Can_highlight_Name(name))
			  gi.cprintf (ent, PRINT_HIGH,"%s\n",HighAscii(txt));
		  else	
			  gi.cprintf (ent, PRINT_HIGH,"%s\n",txt);
	  }
   } 

   gi.cprintf (ent, PRINT_HIGH, "Page %d/%1.0f (%i users). Use playerscores <page>\n",(offset+1),ceil(maplist.sort_num_users_israfel/20.0),maplist.sort_num_users_israfel); 

   gi.cprintf (ent, PRINT_HIGH, "\n-----------------------------------------\n\n"); 

}  

void ShowPlayerMaps(edict_t *ent) 
{ 
   int i; 
	int offset;
	int temp;
	char txt[1024];
	char name[64];
	offset = atoi(gi.argv(1));

	offset--;
   if (offset<0)
	   offset = 0;
	
   gi.cprintf (ent, PRINT_HIGH, "-----------------------------------------\n"); 
	gi.cprintf (ent, PRINT_HIGH, "\xce\xef\xae \xce\xe1\xed\xe5               Maps     %%\n"); // No. Name
   for (i=(20*offset); (i<maplist.sort_num_users) && (i<(20*offset)+20); i++) 
   { 
	  temp = maplist.sorted_completions[i].uid;
	  if (temp>=0)
	  {
		  Com_sprintf(name,sizeof(name),maplist.users[temp].name);
//		  Highlight_Name(name);
		Com_sprintf(txt,sizeof(txt),"%-3d %-16s   %4d  %3.1f", i+1, name,maplist.users[temp].completions,(float)maplist.users[temp].completions / (float)maplist.nummaps * 100);
		  if (Can_highlight_Name(name))
			  gi.cprintf (ent, PRINT_HIGH,"%s\n",HighAscii(txt));
		  else	
			  gi.cprintf (ent, PRINT_HIGH,"%s\n",txt);
	  }
   } 

   gi.cprintf (ent, PRINT_HIGH, "Page %d/%1.0f (%i users). Use playermaps <page>\n",(offset+1),ceil(maplist.sort_num_users/20.0),maplist.sort_num_users); 

   gi.cprintf (ent, PRINT_HIGH, "-----------------------------------------\n"); 
} 

int closest_ent(edict_t *ent)
{
   int i; 
   vec3_t closest;
   int closest_num = -1;
	int offset;
	vec3_t v1;

   offset = 0;
	
   gi.cprintf (ent, PRINT_HIGH, "----------------------------\n"); 

   closest[0] = closest[1] = closest[2] = 0;
   for (i=(20*offset); (i<MAX_ENTS) && (i<(20*offset)+20); i++) 
   { 
		if (level_items.ents[i])
		{
			VectorSubtract(ent->s.origin, level_items.ents[i]->s.origin, v1);
			if (!VectorLength(closest))
			{
				VectorCopy(v1,closest);
				closest_num = i;;
			}
			else if (VectorLength(v1)<VectorLength(closest))
			{
				VectorCopy(v1,closest);
				closest_num = i;
			}
		}
   } 

   if (closest_num>=0)
   {
	closest_num++;
   }
   return closest_num;
}
void show_ent_list(edict_t *ent,int page)
{
   int i; 
   vec3_t closest;
   int closest_num = 0;
	int offset;
	vec3_t v1;

	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	offset = page;//strtol(page,&pEnd,0);

	offset--;
   if (offset<0)
	   offset = 0;
	
   gi.cprintf (ent, PRINT_HIGH, "----------------------------\n"); 

   closest[0] = closest[1] = closest[2] = 0;
   for (i=(20*offset); (i<MAX_ENTS) && (i<(20*offset)+20); i++) 
   { 
		if (level_items.ents[i])
		{
			VectorSubtract(ent->s.origin, level_items.ents[i]->s.origin, v1);
			if (!VectorLength(closest))
			{
				VectorCopy(v1,closest);
				closest_num = i;;
			}
			else if (VectorLength(v1)<VectorLength(closest))
			{
				VectorCopy(v1,closest);
				closest_num = i;
			}
            if (strstr(level_items.ents[i]->classname,"cpbox_")){
                gi.cprintf(ent,PRINT_HIGH,"%-2d %-20s ID:%d \"%-3.3f %-3.3f %-3.3f\"\n",i+1,level_items.ents[i]->classname,(level_items.ents[i]->count+1),level_items.ents[i]->s.origin[0],level_items.ents[i]->s.origin[1],level_items.ents[i]->s.origin[2]);
            } else if(level_items.ents[i]->message && strstr(level_items.ents[i]->message,"checkpoint")) {
				gi.cprintf(ent,PRINT_HIGH,"%-2d %-20s ID:%d \"%-3.3f %-3.3f %-3.3f\"\n",i+1,level_items.ents[i]->classname,(level_items.ents[i]->count+1),level_items.ents[i]->s.origin[0],level_items.ents[i]->s.origin[1],level_items.ents[i]->s.origin[2]);
			} else {
                gi.cprintf(ent,PRINT_HIGH,"%-2d %-20s \"%-3.3f %-3.3f %-3.3f\"\n",i+1,level_items.ents[i]->classname,level_items.ents[i]->s.origin[0],level_items.ents[i]->s.origin[1],level_items.ents[i]->s.origin[2]);
            }
        }
   } 

   gi.cprintf (ent, PRINT_HIGH, "Page %d. Use listents <page>\n",(offset+1)); 

   gi.cprintf (ent, PRINT_HIGH, "----------------------------\n"); 
   gi.cprintf (ent, PRINT_HIGH, "Closest Entity : %d\n",closest_num+1); 
   gi.cprintf (ent, PRINT_HIGH, "----------------------------\n"); 
}

qboolean AddNewEnt(void)
{
	int i;
	for (i=0;i<MAX_ENTS;i++)
	{
		if (!level_items.ents[i])
		{
			level_items.ents[i] = level_items.newent;
			level_items.newent = NULL;
			if (strstr(level_items.ents[i]->classname,"jumpbox_") || strstr(level_items.ents[i]->classname,"cpbox_"))
			{
			}
			else
			{
				ED_CallSpawn (level_items.ents[i]);
			}

			return true;
		}
	}
	return false;
}

void ClearNewEnt(void)
{
	if (level_items.newent)
	{
		G_FreeEdict(level_items.newent);
		level_items.newent = NULL;
	}
}

void WriteEnts(void)
{
	FILE	*f;
	qboolean wrote;
	char	name[256];
	int i;
	qboolean first_line;
	cvar_t	*tgame;

	tgame = gi.cvar("game", "", 0);

	sprintf (name, "%s/ent/%s.add", tgame->string,level.mapname);

	f = fopen (name, "wb");
	
	wrote = false;
	first_line = true;
	for (i=0;i<MAX_ENTS;i++)
	if (level_items.ents[i])
	{
		wrote = true;
		if (!first_line)
		{
			fprintf(f,"{\n");
		}
		else
		{
			fprintf(f,"{\n");
			first_line = false;
		}
		
		fprintf (f, "\"classname\" \"%s\"\n",level_items.ents[i]->classname);
		if (strstr(level_items.ents[i]->classname,"jumpmod_effect"))
		{
			fprintf (f, "\"effect\" \"%d\"\n",level_items.ents[i]->s.effects);
		}
		if (strstr(level_items.ents[i]->classname,"jump_clip"))
		{
			if(level_items.ents[i]->message && strstr(level_items.ents[i]->message,"checkpoint")){
				fprintf (f, "\"count\" \"%d\"\n",level_items.ents[i]->count);
				fprintf (f, "\"message\" \"%s\"\n",level_items.ents[i]->message);
			}
			fprintf (f, "\"mins\" \"%f %f %f\"\n",level_items.ents[i]->mins[0],level_items.ents[i]->mins[1],level_items.ents[i]->mins[2]);
			fprintf (f, "\"maxs\" \"%f %f %f\"\n",level_items.ents[i]->maxs[0],level_items.ents[i]->maxs[1],level_items.ents[i]->maxs[2]);
		}

		fprintf (f, "\"origin\" \"%f %f %f\"\n",level_items.ents[i]->s.origin[0],level_items.ents[i]->s.origin[1],level_items.ents[i]->s.origin[2]);
        if (level_items.ents[i]->s.angles[0]){
		    fprintf (f, "\"angles\" \"%f %f %f\"\n",level_items.ents[i]->s.angles[0],level_items.ents[i]->s.angles[1],level_items.ents[i]->s.angles[2]);
        }
        if (level_items.ents[i]->target)
		{
//			temp_e = level_items.ents[i]->target;
			fprintf (f, "\"target\" \"%s\"\n",level_items.ents[i]->target);
		}
		if (level_items.ents[i]->s.skinnum)
		{
			fprintf (f, "\"skinnum\" \"%d\"\n",level_items.ents[i]->s.skinnum);
		}
        if (strstr(level_items.ents[i]->classname,"cpbox_"))
        {
            fprintf (f, "\"count\" \"%d\"\n",level_items.ents[i]->count);
        }
		if (level_items.ents[i]->targetname)
		{
			fprintf (f, "\"targetname\" \"%s\"\n",level_items.ents[i]->targetname);
		}

		fprintf (f, "}\n");
	}
	fprintf (f, "\n\n");
	fclose(f);
	if (!wrote)
		RemoveAllEnts(level.mapname);
}

void add_ent(edict_t *ent) 
{
	char	temp[256];
	char	action[256];
	char	keyn[128],valn[128];

	cvar_t	*game_dir;
	game_dir = gi.cvar("game", "", 0);

	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	if (gametype->value!=GAME_CTF)
	if (ent->client->resp.ctf_team==CTF_TEAM2)
		return;

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}
	//no args, show ent list
	if (gi.argc() < 2) {
		show_ent_list(ent,0);
		return;
	}

	strcpy(action,gi.argv(1));
	if (strcmp(action,"clear")==0)
	{
		ClearNewEnt();
	}
	else if (strcmp(action,"create")==0)
	{
		if (!level_items.newent)
		{
			gi.cprintf(ent,PRINT_HIGH,"No entity to create.\n");
			return;
		}
		if (AddNewEnt())
		{
			strcpy(mset_vars->edited_by,ent->client->resp.admin_name);
			sprintf(temp,"%s/ent/%s.cfg",game_dir->string,level.mapname);
			writeMapCfgFile(temp);
			ClearNewEnt();
			WriteEnts();
			gi.cprintf(ent,PRINT_HIGH,"Entity added.\n");
			return;
		} else {
			gi.cprintf(ent,PRINT_HIGH,"No free slots, please remove an entity.\n");
			return;
		}
	} 
	else if (gi.argc() < 3)
	{
		gi.cprintf(ent,PRINT_HIGH,"Command format : addent <key> <value>.\n");
		return;
	} else {
		if (strcmp(action,"classname")==0)
		{
			if (level_items.newent)
				G_FreeEdict(level_items.newent);
			level_items.newent = G_Spawn();

			sprintf(keyn,gi.argv(1));
			sprintf(valn,gi.argv(2));
			ED_ParseField (keyn, valn, level_items.newent, 0);
//				strcpy(level_items.newent->classname,gi.argv(2));
				VectorCopy(ent->s.origin,level_items.newent->s.origin);
				VectorCopy(ent->s.angles,level_items.newent->s.angles);
//				level_items.newent->s.angles[1] = ent->s.angles[1];
		}
		else 
		{
			if (!level_items.newent)
			{
				gi.cprintf(ent,PRINT_HIGH,"Please use addent classname to initialize the entity\n");
				return;
			}
			sprintf(keyn,gi.argv(1));
			sprintf(valn,gi.argv(2));
			ED_ParseField (keyn, valn, level_items.newent, 0);
		}
	}
}

void ClearEnt(int remnum)
{
	edict_t *ent;
	if (level_items.ents[remnum])
	{
		ent = NULL;
		if (level_items.ents[remnum]->classname)
		{
			if (strcmp(level_items.ents[remnum]->classname,"misc_teleporter")==0)
			{
				while ((ent = G_Find (ent, FOFS(target), level_items.ents[remnum]->target)) != NULL) {
					if (ent->mins[0] == -8)
					{
						G_FreeEdict(ent);
						break;
					}
				}
			}
			else if (strcmp(level_items.ents[remnum]->classname,"jump_score")==0)
			{
				ent = NULL;
				while ((ent = G_Find_contains (ent, FOFS(classname), "jump_score_digit")) != NULL) {
					if (ent->owner==level_items.ents[remnum])
						G_FreeEdict(ent);
				}
			}
			else if (strcmp(level_items.ents[remnum]->classname,"jump_time")==0)
			{
				ent = NULL;
				while ((ent = G_Find_contains (ent, FOFS(classname), "jump_time_digit")) != NULL) {
					if (ent->owner==level_items.ents[remnum])
						G_FreeEdict(ent);
				}
			}
		}

		//add code to prevent spawn pad crash


		G_FreeEdict(level_items.ents[remnum]);
		level_items.ents[remnum] = NULL;
	}
}

void RemoveEnt(int remnum)
{
	ClearEnt(remnum);
	WriteEnts();
}

void RemoveAllEnts(char *fname)
{
	char	name[256];
	cvar_t	*tgame;
	int i;
	tgame = gi.cvar("game", "", 0);

	for (i=0;i<MAX_ENTS;i++)
	{
		if (level_items.ents[i])
		{
			G_FreeEdict(level_items.ents[i]);
			level_items.ents[i] = NULL;
		}

	}

	sprintf (name, "%s/ent/%s.add", tgame->string,fname);
	remove(name);	
	sprintf (name, "%s/ent/%s.rem", tgame->string,fname);
	remove(name);	
}

void remove_ent(edict_t *ent) 
{
	int remnum;
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;

	if (gi.argc() < 2) {
		show_ent_list(ent,0);
		return;
	}

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}

	strcpy(mset_vars->edited_by,ent->client->resp.admin_name);

	if (strcmp(gi.argv(1),"all")==0)
	{
		RemoveAllEnts(level.mapname);
		gi.cprintf(ent,PRINT_HIGH,"All Entities removed.\n");
		return;
	}

	remnum = atoi(gi.argv(1));
	if ((remnum<1) || (remnum>50))
	{
		RemoveAllEnts(gi.argv(1));
		gi.cprintf(ent,PRINT_HIGH,"%s ents removed.\n",gi.argv(1));
	} else {
		RemoveEnt(remnum-1);
		gi.cprintf(ent,PRINT_HIGH,"Entity removed.\n");
	}
}

void Cmd_Chaseme(edict_t *ent)
{
	int i;
	edict_t	*e2;
	qboolean first_name;
	int chase_count;

	gi.cprintf(ent,PRINT_HIGH,"You are being chased by : ");
	first_name = true;
	chase_count = 0;
	for (i = 1; i <= maxclients->value; i++) 
	{
		e2 = g_edicts + i;
		if (!e2->inuse)
			continue;

		if (e2->client->chase_target==ent)
		{
			if (first_name)
			{
				gi.cprintf(ent,PRINT_HIGH,"%s",e2->client->pers.netname);
				first_name = false;
			}
			else 
			{
				gi.cprintf(ent,PRINT_HIGH,", %s",e2->client->pers.netname);
			}
			chase_count++;
		}
	}
	if (!chase_count)
	{
		gi.cprintf(ent,PRINT_HIGH,"no one.\n");
	}
	else
	{
		gi.cprintf(ent,PRINT_HIGH,".\n");
	}
}

void Cmd_Coord_f(edict_t *ent)
{
	gi.cprintf(ent,PRINT_HIGH,"%1.0f %1.0f %1.0f\n",ent->s.origin[0],ent->s.origin[1],ent->s.origin[2]);
}

void Read_Admin_cfg(void)
{
	FILE	*f;
	char	name[256];
	char	temp[256];
	int i,i2;
	cvar_t	*tgame;

	tgame = gi.cvar("game", "", 0);

	sprintf (name, "%s/admin.cfg", tgame->string);

	f = fopen (name, "rb");
	if (!f)
	{
		return;
	}	


	fscanf(f,"%s",&temp);
	if (!strstr(temp,"Jump039"))
	{
		//invalid admin config (old version, we cant use it)
        fclose(f);
		return;
	} 
	
	i = 0;
	while ((!feof(f)) && (i<MAX_ADMINS))
	{
		fscanf(f,"%s %s %d",&admin_pass[i].name,&admin_pass[i].password,&admin_pass[i].level);
		i++;
	}
	if (!admin_pass[i].level)
		i--;
	num_admins = i;
	if (i<MAX_ADMINS)
		for (i2=i;i2<MAX_ADMINS;i2++)
			admin_pass[i2].level = 0;

	//read em in
	fclose(f);
}

void Write_Admin_cfg(void)
{
	FILE	*f;
	char	name[256];
	int i;
	cvar_t	*tgame;

	tgame = gi.cvar("game", "", 0);

	sprintf (name, "%s/admin.cfg", tgame->string);

	f = fopen (name, "wb");
	if (!f)
	{
		return;
	}	

	fprintf (f, "Jump039\n");

	
	for (i=0;i<MAX_ADMINS;i++)
	{
		if (admin_pass[i].level)
		{
			fprintf(f,"%s ",admin_pass[i].name);
			fprintf(f,"%s ",admin_pass[i].password);
			fprintf(f,"%d\n",admin_pass[i].level);
		}
	}

	fclose(f);
}

void add_admin(edict_t *ent)
{
	int placement = -1;
	int i;
	char *name;
	char *pass;
	int alevel;

	if (ent->client->resp.admin<aset_vars->ADMIN_ADMINEDIT_LEVEL)
		return;

	if (gi.argc() != 4)
	{
		list_admins(ent); // list current admins
		gi.cprintf(ent,PRINT_HIGH,"Format : addadmin user pass level\n");
		return;
	}

	name = gi.argv(1);
	pass = gi.argv(2);
	alevel = atoi(gi.argv(3));

	if (ent->client->resp.admin<aset_vars->ADMIN_ADMINEDIT_LEVEL)
		return;

	if ((alevel<0) || (alevel>aset_vars->ADMIN_MAX_LEVEL))
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid admin level\n");
		return;
	}

	//check to see if username exists
	for (i=0;i<num_admins;i++)
	{
		if (strcmp(name,admin_pass[i].name) == 0)
		{
			gi.cprintf(ent,PRINT_HIGH,"That admin already exists.\n");
			return;
		}
	}

	//find a spot for it
	for (i=0;i<MAX_ADMINS;i++)
	{
		if (!admin_pass[i].level)
		{
			placement = i;
			break;
		}
	}
	if (placement==-1)
	{
		gi.cprintf(ent,PRINT_HIGH,"MAX number of admins in use\n");
		return;
	}

	admin_pass[placement].level = alevel;
	strcpy(admin_pass[placement].name,name);
	strcpy(admin_pass[placement].password,pass);

	Write_Admin_cfg();
	Read_Admin_cfg();
	
	gi.cprintf(ent,PRINT_HIGH,"Admin %s password %s with level %d added at position %d\n",name,pass,alevel,placement+1);
}

void change_admin(edict_t *ent)
{
	int admin;
	int alevel;

	if (ent->client->resp.admin<aset_vars->ADMIN_ADMINEDIT_LEVEL)
		return;

	if (gi.argc() != 3)
	{
		list_admins(ent);
		gi.cprintf(ent,PRINT_HIGH,"Format : changeadmin admin_id new_level\n");
		return;
	}

	admin = atoi(gi.argv(1));
	alevel = atoi(gi.argv(2));

	if ((alevel<=0) || (alevel>aset_vars->ADMIN_MAX_LEVEL))
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid admin level\n");
		return;
	}
	if ((admin<=0) || (admin>num_admins))
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid admin\n");
		return;
	}
	admin--;
	admin_pass[admin].level = alevel;

	Write_Admin_cfg();
	Read_Admin_cfg();
	
	gi.cprintf(ent,PRINT_HIGH,"Admin %s's level has been changed to %i\n",admin_pass[admin].name,alevel);
}

void rem_admin(edict_t *ent)
{
	int num;

	if (ent->client->resp.admin<aset_vars->ADMIN_ADMINEDIT_LEVEL)
		return;

	if (gi.argc() != 2)
	{
		list_admins(ent);
		gi.cprintf(ent,PRINT_HIGH,"Format : remadmin number\n");
		return;
	}

	num = atoi(gi.argv(1));

	if ((num<=0) || (num>num_admins))
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid number\n");
		return;
	}
	num--;
	if (admin_pass[num].level)
	{
		admin_pass[num].level = 0;
	}
	Write_Admin_cfg();
	Read_Admin_cfg();

	gi.cprintf(ent,PRINT_HIGH,"Admin %i has been removed.\n",num+1);
}

void list_admins(edict_t *ent)
{
   int i = 0;
   int offset;
   char name[64];
	
	if (ent->client->resp.admin<aset_vars->ADMIN_ADMINEDIT_LEVEL)
		return;

	if (num_admins<=0)
	{
		gi.cprintf(ent,PRINT_HIGH,"No admins to list\n");
		return;
	}

	offset = 1;
	if (gi.argc() == 2)
		offset = atoi(gi.argv(1));

	offset--;
    if (offset<0)
		offset = 0;

   gi.cprintf (ent, PRINT_HIGH,  "-------------------------------------------\n"); 
	gi.cprintf (ent, PRINT_HIGH, "No. Name             Password         Level\n"); 

   for (i=(20*offset); (i<num_admins) && (i<(20*offset)+20); i++) 
   { 
	   if (admin_pass[i].level > ent->client->resp.admin)
	   {
		   gi.cprintf (ent, PRINT_HIGH, "%-3d %-16s\n", i+1, va("ACCESS DENIED"));
		   continue;
	   }
	  Com_sprintf(name,sizeof(name),admin_pass[i].name);
	  Highlight_Name(name);
      gi.cprintf (ent, PRINT_HIGH, "%-3d %-16s %-16s %-1d \n", i+1, name,admin_pass[i].password,admin_pass[i].level);
	  //num++;
   } 

   gi.cprintf (ent, PRINT_HIGH, "Page %d/%1.0f (%i admins).\n",(offset+1),ceil(num_admins/20.0),num_admins); 

   gi.cprintf (ent, PRINT_HIGH,  "-------------------------------------------\n"); 

}

//pooy
void Cmd_Commands_f (edict_t *ent)
{
	//pooy add info to all clients
	ShowCurrentManual(ent);

}

void Cmd_Store_f (edict_t *ent) {
	Store_StoreLocation(ent);
}

void Cmd_Time_f (edict_t *ent)
{
	switch (level.status)
	{
	case 0 :
		gi.cprintf (ent, PRINT_HIGH, "Time remaining %02d:%02d.\n",(int)((int)(((mset_vars->timelimit*60)+(map_added_time*60))-level.time)/60),(int)((int)(((mset_vars->timelimit*60)+(map_added_time*60))-level.time)%60));
	break;
	case LEVEL_STATUS_OVERTIME :
			gi.cprintf (ent, PRINT_HIGH, "Time remaining %02d:%02d.\n",(int)((int)((gset_vars->overtimelimit*60)+gset_vars->overtimewait-level.time)/60),(int)((int)((gset_vars->overtimelimit*60)+gset_vars->overtimewait-level.time)%60));
	break;
	case LEVEL_STATUS_VOTING :
			gi.cprintf (ent, PRINT_HIGH, "Time remaining %02d.\n",(int)(gset_vars->votingtime-level.time));
	break;
	}
}

void Cmd_Reset_f (edict_t *ent)
{
	ent->client->resp.can_store = false;
	ent->client->resp.item_timer_allow = true;
	if (jump_show_stored_ent)
	{
		if (ent->client->resp.stored_ent)	
			G_FreeEdict(ent->client->resp.stored_ent);
	}	
}

void Cmd_Unadmin(edict_t *ent)
{
	char text[1024];
	if (ent->client->resp.admin)
	{
			sprintf(text,"set admin_user none\n");
			stuffcmd(ent, text);
			sprintf(text,"set admin_pass none\n");
			stuffcmd(ent, text);
		ent->client->resp.admin = 0;
		gi.cprintf(ent,PRINT_HIGH,"You are no longer an admin.\n");
	}
}

void CancelElection(edict_t *ent)
{
	if (1 == gset_vars->cvote_announce)
	{
		if (ctfgame.etarget!=NULL)
		{
			if (ctfgame.etarget->client->resp.admin<aset_vars->ADMIN_NOMAXVOTES_LEVEL)
				ctfgame.etarget->client->resp.num_votes++;
		}
		ctfgame.election = ELECT_NONE;
		gi.bprintf (PRINT_HIGH, "Vote failed, %s canceled the vote.\n", ent->client->pers.netname);
	}
	else
	{
		ctfgame.electtime = level.time;
		ctfgame.electframe = level.framenum;
	}
}

void CTFApplyRegeneration2(edict_t *ent)
{
	gclient_t *client;
	int index;

	client = ent->client;
	if (!client)
		return;

	if (ent->health>0)
	if (client->ctf_regentime < level.time) {
		client->ctf_regentime = level.time;
		if (ent->health < mset_vars->health) {
			ent->health += mset_vars->regen;
			if (ent->health > mset_vars->health)
				ent->health = mset_vars->health;
			client->ctf_regentime += 0.5;
		}
	}

	index = ArmorIndex (ent);
	if (index && client->pers.inventory[index] < 100) {
		client->pers.inventory[index] += mset_vars->regen;
		if (client->pers.inventory[index] > 100)
			client->pers.inventory[index] = 100;
		client->ctf_regentime += 0.5;
	}
	
}

size_t q2a_strlen( const char *string )
{
	size_t len = 0;

	while(*string)
	{
		len++;
		string++;
	}

	return len;
}

char* FindIpAddressInUserInfo(char* userinfo)
{
  char *ip = Info_ValueForKey(userinfo, "ip");

  if ( *ip == 0 ) {
     char* ipuserinfo = userinfo + q2a_strlen(userinfo);

     // find the last '\\'
     while(ipuserinfo > userinfo && *ipuserinfo != '\\') {
        ipuserinfo--;
     }

     if ( ipuserinfo - 3 >= userinfo && 
        *(ipuserinfo - 3) == '\\' && 
        *(ipuserinfo - 2) == 'i' &&
        *(ipuserinfo - 1) == 'p') {

        return ipuserinfo + 1;
     }
  }

  return ip;
}

void cvote(edict_t *ent)
{
	if (ent->client->resp.admin<aset_vars->ADMIN_VOTE_LEVEL)
		return;

	if (ctfgame.election == ELECT_NONE) {
		gi.cprintf(ent, PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	admin_log(ent,"cancelled an election.");
	CancelElection(ent);
}

void pvote(edict_t *ent)
{
	int pvote = 0;
	if (ent->client->resp.admin<aset_vars->ADMIN_VOTE_LEVEL)
		return;

	if (ctfgame.election == ELECT_NONE) {
		gi.cprintf(ent, PRINT_HIGH, "No election is in progress.\n");
		return;
	}
	if (1 == gset_vars->pvote_announce)
		pvote = 1;
	ctfgame.evotes = ctfgame.needvotes;
	admin_log(ent,"forced a vote thru.");
	CTFWinElection(pvote, ent);
}


/* Vanilla CTF Grappling Hook */

//#include "g_local.h"
//#include "m_player.h"
//#include "p_hook.h"

void hook_laser_think (edict_t *self)
{
	vec3_t	forward, right, offset, start;
	
	if (!self->owner || !self->owner->owner || !self->owner->owner->client)
	{
		G_FreeEdict(self);
		return;	
	}

	AngleVectors (self->owner->owner->client->v_angle, forward, right, NULL);
	VectorSet(offset, 24, -8, self->owner->owner->viewheight-8);
	P_ProjectSource (self->owner->owner->client, self->owner->owner->s.origin, offset, forward, right, start);

	VectorCopy (start, self->s.origin);
	VectorCopy (self->owner->s.origin, self->s.old_origin);
	gi.linkentity(self);

	self->nextthink = level.time + FRAMETIME;
	return;
}

edict_t *hook_laser_start (edict_t *ent)
{
	edict_t *self;

	self = G_Spawn();
	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_NOT;
	self->s.renderfx |= RF_BEAM|RF_TRANSLUCENT;
	self->s.modelindex = 1;
	self->owner = ent;

	self->s.frame = 4;

	// set the color
    
	if ((ent->owner->client->resp.got_time) && (level_items.fastest_player!=ent->owner))
	{
		self->s.skinnum = 0xdcdddedf;// yellow
	}

	if (level_items.fastest_player==ent->owner)
	{
		self->s.skinnum = 0xd0d1d2d3;		// blue
	}

	if (ent->owner->client->resp.admin)
	{
		self->s.skinnum = 0xf3f3f1f1;		// red+blue = purple
	}


	self->think = hook_laser_think;

	VectorSet (self->mins, -8, -8, -8);
	VectorSet (self->maxs, 8, 8, 8);
	gi.linkentity (self);

	self->spawnflags |= 0x80000001;
	self->svflags &= ~SVF_NOCLIENT;
	hook_laser_think (self);
	return(self);
}

void hook_reset (edict_t *rhook)
{
	if (!rhook) return;
	if (rhook->owner)
	{
		if (rhook->owner->client)
		{
			rhook->owner->client->hook_state = HOOK_READY;
			rhook->owner->client->hook = NULL;
		}
	}
	if (rhook->laser) G_FreeEdict(rhook->laser);
		G_FreeEdict(rhook);
};

qboolean hook_cond_reset(edict_t *self)
 {
		if (!self->owner || (!self->enemy && self->owner->client && self->owner->client->hook_state == HOOK_ON)) {
                hook_reset (self);
                return (true);
        }
	
        if ((self->enemy && !self->enemy->inuse) || (!self->owner->inuse) ||
			(self->enemy && self->enemy->client && self->enemy->health <= 0) || 
			(self->owner->health <= 0))
        {
                hook_reset (self);
                return (true);
        }

        if (!((self->owner->client->latched_buttons|self->owner->client->buttons) & BUTTON_ATTACK)
			&& (strcmp(self->owner->client->pers.weapon->pickup_name, "Grapple") == 0))
        {
                hook_reset (self);
				return (true);
        }

		return(false);
}

void hook_cond_reset_think(edict_t *hook)
{
	if (hook_cond_reset(hook))
		return;
	hook->nextthink = level.time + FRAMETIME;
}

void hook_service (edict_t *self)
 {
        vec3_t	hook_dir;
		if (hook_cond_reset(self)) return;

		if (self->enemy->client)
			VectorSubtract(self->enemy->s.origin, self->owner->s.origin, hook_dir);
		else
			VectorSubtract(self->s.origin, self->owner->s.origin, hook_dir);
        VectorNormalize(hook_dir);
		VectorScale(hook_dir, gset_vars->hookpull, self->owner->velocity);
}

void hook_track (edict_t *self)
 {
		vec3_t	normal;

		if (hook_cond_reset(self))
			return;

        if (self->enemy->client)
        {
			VectorCopy(self->enemy->s.origin, self->s.origin);
			
			VectorSubtract(self->owner->s.origin, self->enemy->s.origin, normal);

			T_Damage (self->enemy, self, self->owner, vec3_origin, self->enemy->s.origin, normal, 1, 0, DAMAGE_NO_KNOCKBACK, MOD_GRAPPLE);
        }
		else
		{
            VectorCopy(self->enemy->velocity, self->velocity);
		}

		gi.linkentity(self);
        self->nextthink = level.time + 0.1;
};

void hook_touch (edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	if (other == self->owner)
		return;
	
	if (other->solid == SOLID_NOT || other->solid == SOLID_TRIGGER || other->movetype == MOVETYPE_FLYMISSILE)
		return;
	
/*	if (other->client)
	{
		VectorSubtract(other->s.origin, self->owner->s.origin, dir);
		VectorSubtract(self->owner->s.origin, other->s.origin, normal);
		T_Damage(other, self, self->owner, dir, self->s.origin, normal, 10, 10, 0, MOD_GRAPPLE);
		hook_reset(self);
		return;
	}
	else
	{
		if (other->takedamage) {
			VectorSubtract(other->s.origin, self->owner->s.origin, dir);
			VectorSubtract(self->owner->s.origin, other->s.origin, normal);
			T_Damage(other, self, self->owner, dir, self->s.origin, normal, 1, 1, 0, MOD_GRAPPLE);
		}
*/
		//gi.positioned_sound(self->s.origin, self, CHAN_WEAPON, gi.soundindex("flyer/Flyatck2.wav"), 1, ATTN_NORM, 0);
		jumpmod_pos_sound(self->s.origin, self, gi.soundindex("flyer/Flyatck2.wav"), CHAN_WEAPON, 1, ATTN_NORM); //hook hit wall


//	}
	
	VectorClear(self->velocity);

	self->enemy = other;
	self->owner->client->hook_state = HOOK_ON;
	
	self->think = hook_track;
	self->nextthink = level.time + 0.1;
	
	self->solid = SOLID_NOT;
}

void fire_hook (edict_t *owner, vec3_t start, vec3_t forward) {
		edict_t	*hook;
		trace_t tr;
        hook = G_Spawn();
        hook->movetype = MOVETYPE_FLYMISSILE;
        hook->solid = SOLID_TRIGGER;
		hook->clipmask = MASK_SHOT;
        hook->owner = owner;
		owner->client->hook = hook;
        hook->classname = "hook";
 
		vectoangles (forward, hook->s.angles);
		VectorScale(forward, gset_vars->hookspeed, hook->velocity);

        hook->touch = hook_touch;

		hook->think = hook_cond_reset_think;
		hook->nextthink = level.time + FRAMETIME;

		gi.setmodel(hook, "");

        VectorCopy(start, hook->s.origin);
		VectorCopy(hook->s.origin, hook->s.old_origin);

		VectorClear(hook->mins);
		VectorClear(hook->maxs);

		hook->laser = hook_laser_start(hook);

		gi.linkentity(hook);

		tr = gi.trace (owner->s.origin, NULL, NULL, hook->s.origin, hook, MASK_SHOT);
		if (tr.fraction < 1.0)
		{
			VectorMA (hook->s.origin, -10, forward, hook->s.origin);
			hook->touch (hook, tr.ent, NULL, NULL);
		}

}

void hook_fire (edict_t *ent) {
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	offset;
	edict_t *cl_ent;
	int		sendchan;
	int		numEnt;
	int		i;

	// ctf check
	if (gametype->value==GAME_CTF)
		return;

	// hook check
	if (!gset_vars->hook)
		return;

	// state check
	if (ent->client->hook_state)
		return;

	// use hook if team easy, or dev gset for mapping
	if (ent->client->resp.ctf_team == CTF_TEAM1 || gset_vars->dev) {

		if (gset_vars->dev && ent->client->resp.ctf_team == CTF_TEAM2)
			gi.cprintf(ent, PRINT_HIGH, "Dev: Hook\n");

		ent->client->hook_state = HOOK_OUT;

		AngleVectors(ent->client->v_angle, forward, right, NULL);
		VectorSet(offset, 8, -8, ent->viewheight - 8);
		P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

		VectorScale(forward, -2, ent->client->kick_origin);
		ent->client->kick_angles[0] = -1;

		fire_hook(ent, start, forward);
		jumpmod_sound(ent, false, gi.soundindex("flyer/Flyatck3.wav"), CHAN_WEAPON, 1, ATTN_NORM);
	}
}

void CTFSilence(edict_t *ent)
{
	int i;
	edict_t *targ;
	char text[1024];
	


	if ((!map_allow_voting) && (ent->client->resp.admin<aset_vars->ADMIN_SILENCE_LEVEL))
		return;

	if (ent->client->resp.silence)
		return;

	if (ClientIsBanned(ent,BAN_SILENCEVOTE))
	{
		gi.cprintf(ent,PRINT_HIGH,"You are not allowed to call silence votes.\n");
		return;
	}

	if (ent->client->resp.admin < aset_vars->ADMIN_VOTE_LEVEL)
	if ((mset_vars->timelimit*60)+(map_added_time*60)-level.time<120)
	{
		gi.cprintf(ent,PRINT_HIGH,"You cannot initiate a vote of this kind when timeleft is under 2 minutes\n");
		return;
	}

	if ((level.time<20) && (ent->client->resp.admin<aset_vars->ADMIN_SILENCE_LEVEL))
	{
		gi.cprintf(ent,PRINT_HIGH,"Please wait %2.1f seconds before calling a vote\n",20.0-level.time);
		return;
	}

	if (gi.argc() < 2) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Who do you want to silence?\n");
		return;
	}

	if (*gi.argv(1) < '0' && *gi.argv(1) > '9') {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Specify the player number to silence.\n");
		return;
	}

	i = atoi(gi.argv(1));
	if (i < 1 || i > maxclients->value) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	targ = g_edicts + i;
	if (!targ->inuse) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	if (ent->client->resp.admin>1)
	{
		if (targ->client->resp.admin>=ent->client->resp.admin)
		{
			gi.cprintf(ent,PRINT_HIGH,"You may not silence an admin with a level higher or equal to yours.\n");
			return;
		}
	}
	else
	{
		if (targ->client->resp.admin>1)
		{
			gi.cprintf(ent,PRINT_HIGH,"You may not silence an admin.\n");
			return;
		}
	}

	if ((ent->client->resp.num_votes>= gset_vars->max_votes) && (ent->client->resp.admin < aset_vars->ADMIN_NOMAXVOTES_LEVEL))   // _h2
	{
		gi.cprintf(ent,PRINT_HIGH,"You had %d elections fail and cannot call anymore.\n",gset_vars->max_votes);   // _h2
		return;
	}

	if ((ClientIsBanned(ent,BAN_SILENCEVOTE)) && (ent->client->resp.admin < aset_vars->ADMIN_SILENCE_LEVEL))
	{
		gi.cprintf(ent,PRINT_HIGH,"You are not allowed to vote to silence people.\n");
		return;
	}


	if (ent->client->resp.admin>=aset_vars->ADMIN_SILENCE_LEVEL) {
		//admin silence
		targ->client->resp.silence = true;
		targ->client->resp.silence_until = 0;
		sprintf(text,"silenced %s.",targ->client->pers.netname);
		admin_log(ent,text);

		gi.bprintf(PRINT_HIGH, "%s was silenced by %s.\n",                     // 084_h3
			targ->client->pers.netname,ent->client->pers.netname);         // 084_h3
/*		for (i = 1; i <= maxclients->value; i++) 
		{
			e2 = g_edicts + i;
			if (!e2->inuse)
				continue;
			if (e2!=targ)
			{
//				gi.cprintf(e2,PRINT_HIGH, "%s was silenced by %s.\n",                  // 084_h3
//					targ->client->pers.netname,ent->client->pers.netname);         // 084_h3
				gi.bprintf(PRINT_HIGH, "%s was silenced by %s.\n",                     // 084_h3
					targ->client->pers.netname,ent->client->pers.netname);         // 084_h3
			}
		}*/
		return;
	}

	if ((ent->client->resp.num_votes==gset_vars->max_votes) && (ent->client->resp.admin < aset_vars->ADMIN_NOMAXVOTES_LEVEL))
	{
		//Last of their 3 votes, ban them from voting for incase they try again.
		AddTempBan(ent,BAN_MAPVOTE | BAN_SILENCEVOTE | BAN_VOTETIME | BAN_BOOT);
	}

	sprintf(text, "%s has requested silencing %s.", 
			ent->client->pers.netname, targ->client->pers.netname);
	if (CTFBeginElection(ent, ELECT_SILENCE, text,false))
	{		
		gi.configstring (CONFIG_JUMP_VOTE_INITIATED,HighAscii(va("Vote by %s",ent->client->pers.netname)));
		gi.configstring (CONFIG_JUMP_VOTE_TYPE,va("Silence: %s",targ->client->pers.netname));
		ctfgame.ekicknum = i-1;
		ctfgame.ekick = targ;
		if (ctfgame.needvotes==0)
			CTFWinElection(0, NULL);
	}
}

void CTFRand(edict_t *ent)
{
	char text[1024];
	if (!map_allow_voting)
		return;

	if (ent->client->resp.silence)
		return;

	if (admin_overide_vote_maps)
	{
		gi.cprintf(ent,PRINT_HIGH,"You cannot randomize the map choices as an admin has set the next 3 maps\n");
		return;
	}

	if (ent->client->resp.admin < aset_vars->ADMIN_VOTE_LEVEL)
	if ((mset_vars->timelimit*60)+(map_added_time*60)-level.time<120)
	{
		if (Get_Voting_Clients()>1)
		{
			gi.cprintf(ent,PRINT_HIGH,"You cannot initiate a vote of this kind when timeleft is under 2 minutes\n");
			return;
		}
	}

	if ((gset_vars->nomapvotetime >= level.time) && (ent->client->resp.admin<aset_vars->ADMIN_VOTE_LEVEL) && curclients > 2) // 0.84wp_h1
	{
		gi.cprintf(ent,PRINT_HIGH,"Votes have been disabled for the first %d seconds of a map.\n",gset_vars->nomapvotetime);
		return;
	}

	if ((ent->client->resp.num_votes>=gset_vars->max_votes) && (ent->client->resp.admin < aset_vars->ADMIN_NOMAXVOTES_LEVEL)) // _h2
	{
		gi.cprintf(ent,PRINT_HIGH,"You had %d elections fail and cannot call anymore.\n",gset_vars->max_votes); // _h2
		return;
	}

	if ((ClientIsBanned(ent,BAN_MAPVOTE)) && (ent->client->resp.admin < aset_vars->ADMIN_VOTE_LEVEL))
	{
		gi.cprintf(ent,PRINT_HIGH,"You are not allowed to call a vote of this kind.\n");
		return;
	}

	if ((ent->client->resp.num_votes==gset_vars->max_votes) && (ent->client->resp.admin < aset_vars->ADMIN_NOMAXVOTES_LEVEL))
	{
		//Last of their 3 votes, ban them from voting for incase they try again.
		AddTempBan(ent,30);
	}

	if (maplist.nummaps<=0)
	{
		gi.cprintf(ent, PRINT_HIGH, "No maps in maplist\n");
		return;
	}


	sprintf(text, "%s has requested randomizing the vote maps.", 
				ent->client->pers.netname);
	if (CTFBeginElection(ent, ELECT_RAND, text,false))
	{
		gi.configstring (CONFIG_JUMP_VOTE_INITIATED,HighAscii(va("Vote by %s",ent->client->pers.netname)));
		gi.configstring (CONFIG_JUMP_VOTE_TYPE,"Randomize Vote Maps");		
		if (ctfgame.needvotes==0)
			CTFWinElection(0, NULL);
	}


}

void CTFNominate(edict_t *ent)
{
	char text[1024];
	char temp[128];
	int i,map;
	int temp_num;
	int index;
	if (!map_allow_voting)
		return;

	if (ent->client->resp.silence)
		return;

	if (nominated_map)
	{
		gi.cprintf(ent,PRINT_HIGH,"A map nomination has already been passed.\n");
		return;
	}
	if (ent->client->resp.admin < aset_vars->ADMIN_VOTE_LEVEL)
	if ((mset_vars->timelimit*60)+(map_added_time*60)-level.time<120)
	{
		if (Get_Voting_Clients()>1)
		{
			gi.cprintf(ent,PRINT_HIGH,"You cannot initiate a vote of this kind when timeleft is under 2 minutes\n");
			return;
		}
	}

	if (admin_overide_vote_maps)
	{
		gi.cprintf(ent,PRINT_HIGH,"You cannot nominate a map as an admin has set the next 3 maps\n");
		return;
	}

	index = ent-g_edicts-1;

	if ((gset_vars->nomapvotetime >= level.time) && (ent->client->resp.admin<aset_vars->ADMIN_VOTE_LEVEL) && curclients > 2) // 0.84wp_h1
	{
		gi.cprintf(ent,PRINT_HIGH,"Votes have been disabled for the first %d seconds of a map.\n",gset_vars->nomapvotetime);
		return;
	}

	if ((ent->client->resp.num_votes>=gset_vars->max_votes) && (ent->client->resp.admin < aset_vars->ADMIN_NOMAXVOTES_LEVEL)) // _h2
	{
		gi.cprintf(ent,PRINT_HIGH,"You had %d elections fail and cannot call anymore.\n",gset_vars->max_votes); // _h2
		return;
	}

	if ((ClientIsBanned(ent,BAN_MAPVOTE)) && (ent->client->resp.admin < aset_vars->ADMIN_VOTE_LEVEL))
	{
		gi.cprintf(ent,PRINT_HIGH,"You are not allowed to call a vote of this kind.\n");
		return;
	}

	if ((ent->client->resp.num_votes==gset_vars->max_votes) && (ent->client->resp.admin < aset_vars->ADMIN_NOMAXVOTES_LEVEL))
	{
		//Last of their 3 votes, ban them from voting for incase they try again.
		AddTempBan(ent,30);
	}

	if (gi.argc() < 2) {
		gi.cprintf(ent, PRINT_HIGH, "--------------------------------\n");
		gi.cprintf(ent, PRINT_HIGH, "Type nominate <mapname>\n");
		gi.cprintf(ent, PRINT_HIGH, "--------------------------------\n");
		return;
	}

	if (maplist.nummaps<=0)
	{
		gi.cprintf(ent, PRINT_HIGH, "No maps in maplist\n");
		return;
	}

	map = -1;

	strncpy(temp,gi.argv(1),sizeof(temp)-1);
	temp_num = atoi(gi.argv(1));
	if ((temp_num>=1) && (temp_num<maplist.nummaps) && (false == IsString(va("%s", temp))))
	{
		//mapvote by number
		strcpy(temp,maplist.mapnames[temp_num-1]);
	}

	for (i=0;i<maplist.nummaps;i++)
	{
		if (Q_stricmp(maplist.mapnames[i],temp)==0)
		{
			map = i;
			break;
		}
	}
	

	if (map==-1) {
		gi.cprintf(ent, PRINT_HIGH, "Unknown Jump map.\n");
		gi.cprintf(ent, PRINT_HIGH, "For available levels type maplist.\n");
		return;
	}


	sprintf(text, "%s has nominated %s to be in the next vote menu", 
			ent->client->pers.netname, temp);
	if (CTFBeginElection(ent, ELECT_NOMINATE, text,false))
	{
		gi.configstring (CONFIG_JUMP_VOTE_INITIATED,HighAscii(va("Vote by %s",ent->client->pers.netname)));
		gi.configstring (CONFIG_JUMP_VOTE_TYPE,va("Nominate: %s",temp));
		strncpy(ctfgame.elevel, maplist.mapnames[map], sizeof(ctfgame.elevel) - 1);
		if (ctfgame.needvotes==0)
			CTFWinElection(0, NULL);
	}

}

int get_admin_level(char *givenpass,char *givenname)
{
	int got_level = 0;
	int i;

	for (i=0;i<num_admins;i++)
	{
		if (!admin_pass[i].level)
			break;
		if ((strcmp(givenpass,admin_pass[i].password) == 0) && (strcmp(givenname,admin_pass[i].name) == 0))
		{
			got_level = admin_pass[i].level;
			break;
		}
	}
	return got_level;
}

void admin_log(edict_t *ent,char *log_this)
{
	struct	tm *current_date;
	time_t	time_date;
	char	tdate[256];
	if ((admin_file) && (allow_admin_log->value))
	{		
	//set the date
		time_date = time(NULL);                // DOS system call
		current_date = localtime(&time_date);  // DOS system call
		sprintf(tdate, "%02d/%02d/%02d %02d:%02d:%02d    %s %s",
			current_date->tm_mday,
			current_date->tm_mon + 1
			,current_date->tm_year-100,
			current_date->tm_hour,
			current_date->tm_min,
			current_date->tm_sec,
			ent->client->pers.netname,log_this
			);
		fprintf (admin_file, "%s\n",tdate);

		fprintf (admin_file, "                     IP : %s\n",FindIpAddressInUserInfo(ent->client->pers.userinfo));
		fflush(admin_file);
	}
}

void open_admin_file(void)
{
	char	name[256];
	cvar_t	*tgame;

	tgame = gi.cvar("game", "", 0);

	sprintf (name, "%s/admin.log", tgame->string);

	admin_file = fopen (name, "a");
}

void close_admin_file(void)
{
	if (admin_file)
	fclose(admin_file);
}

// FUNCTIONS //////////////////////////////////////////////// 

// 
// OpenFile 
// 
// Opens a file for reading.  This function will probably need 
// a major overhaul in future versions so that it will handle 
// writing, appending, etc. 
// 
// Args: 
//   filename - name of file to open. 
// 
// Return: file handle of open file stream. 
//         Returns NULL if file could not be opened. 
// 
FILE *OpenFile2(char *filename) 
{ 
   FILE *fp = NULL; 

   if ((fp = fopen(filename, "r")) == NULL)       // test to see if file opened 
   { 
      // file did not load 
      gi.dprintf ("Could not open file \"%s\".\n", filename); 
      return NULL; 
   } 

   return fp; 
} 
  

// 
// CloseFile 
// 
// Closes a file that was previously opened with OpenFile(). 
// 
// Args: 
//   fp  - file handle of file stream to close. 
// 
// Return: (none) 
// 
void CloseFile(FILE *fp) 
{ 
   if (fp)        // if the file is open 
   { 
      fclose(fp); 
   } 
   else    // no file is opened 
      gi.dprintf ("ERROR -- CloseFile() exception.\n"); 
}

void AddUser(char *name,int i)
{
	int j;
	strcpy(maplist.users[i].name,name);
	for (j=0;j<MAX_HIGHSCORES;j++)
		maplist.users[i].points[j] = 0;
	maplist.users[i].score = 0;
	maplist.users[i].lastseen = Get_Timestamp();
	maplist.num_users++;
	
}

int GetPlayerUid(char *name)
{
	int i;

	for (i=0;i<MAX_USERS;i++)
	{
		if (!maplist.users[i].name[0])
		{
			AddUser(name,i);
			return i;
		}
		if (Q_stricmp(name,maplist.users[i].name)==0)
		{
			return i;
		}
	}
	return -1;
}

int GetPlayerUid_NoAdd(char *name)
{
	int i;
	int len;
	char temp_name[255];
	Com_sprintf(temp_name,sizeof(temp_name),name);
	len = strlen(temp_name);
	for (i=0;i<len;i++)
	{
		if (temp_name[i] == ' ')
			temp_name[i] = '_';
	}
	for (i=0;i<MAX_USERS;i++)
	{
		if (Q_stricmp(temp_name,maplist.users[i].name)==0)
		{
			return i;
		}
	}
	return -1;
}

void sort_users(void)
{
	int i;
	if (maplist.sort_num_users)
	{
		for (i=0;i<maplist.sort_num_users;i++)
		{
			maplist.sorted_users[i].score = maplist.users[i].score;
			maplist.sorted_users[i].uid = i;
			maplist.sorted_users[i].pos = i;

			maplist.sorted_israfel[i].score = maplist.users[i].score;
			maplist.sorted_israfel[i].israfel = maplist.users[i].israfel;
			maplist.sorted_israfel[i].uid = i;

			maplist.sorted_completions[i].score = maplist.users[i].completions;
			maplist.sorted_completions[i].uid = i;
		}
		sort_users_score(maplist.sort_num_users);
		sort_users_completions(maplist.sort_num_users);
		sort_users_israfel(maplist.sort_num_users);
	}	
}

void sort_users_score( int n )
{
	int t_score;
	int t_uid;
	int i;
	int j;
  for ( i = 0; i < n-1; ++i )
    for ( j = 1; j < n-i; ++j )
      if (maplist.sorted_users[j-1].score < maplist.sorted_users[j].score
		   )
	  {
		t_score = maplist.sorted_users[j-1].score;
		t_uid = maplist.sorted_users[j-1].uid;


        maplist.sorted_users[j-1].uid = maplist.sorted_users[j].uid;
        maplist.sorted_users[j-1].score = maplist.sorted_users[j].score;

        maplist.sorted_users[j].uid = t_uid;
        maplist.sorted_users[j].score = t_score;
		maplist.sorted_users[j].pos = i;
	  }
}

void sort_users_israfel( int n )
{
	int t_score;
	int t_uid;
	float t_israfel;
	int i;
	int j;
  for ( i = 0; i < n-1; ++i )
    for ( j = 1; j < n-i; ++j )
      if (maplist.sorted_israfel[j-1].israfel < maplist.sorted_israfel[j].israfel
		   )
	  {
		t_score = maplist.sorted_israfel[j-1].score;
		t_israfel = maplist.sorted_israfel[j-1].israfel;
		t_uid = maplist.sorted_israfel[j-1].uid;

        maplist.sorted_israfel[j-1].uid = maplist.sorted_israfel[j].uid;
        maplist.sorted_israfel[j-1].score = maplist.sorted_israfel[j].score;
        maplist.sorted_israfel[j-1].israfel = maplist.sorted_israfel[j].israfel;

        maplist.sorted_israfel[j].uid = t_uid;
        maplist.sorted_israfel[j].score = t_score;
        maplist.sorted_israfel[j].israfel = t_israfel;
	  }
	for (i=0;i<n;i++)
	{
		if (!maplist.sorted_israfel[i].israfel)
			break;
	}
	maplist.sort_num_users_israfel = i;
}

void sort_users_completions( int n )
{
	int t_score;
	int t_uid;

	int i;
	int j;
  for ( i = 0; i < n-1; ++i )
    for ( j = 1; j < n-i; ++j )
      if (maplist.sorted_completions[j-1].score < maplist.sorted_completions[j].score
		   )
	  {
		t_score = maplist.sorted_completions[j-1].score;
		t_uid = maplist.sorted_completions[j-1].uid;


        maplist.sorted_completions[j-1].uid = maplist.sorted_completions[j].uid;
        maplist.sorted_completions[j-1].score = maplist.sorted_completions[j].score;

        maplist.sorted_completions[j].uid = t_uid;
        maplist.sorted_completions[j].score = t_score;
	  }
}

void Start_Recording(edict_t *ent)
{
	int index;

	index = ent-g_edicts-1;
	if (gametype->value==GAME_CTF)
	{
		client_record[index].allow_record = true;
		client_record[index].current_frame = 0;
	}
	else
	if (ent->client->resp.ctf_team!=CTF_TEAM2)
	{
		client_record[index].allow_record = false;
		client_record[index].current_frame = 0;
	} else
	{
		client_record[index].allow_record = true;
		client_record[index].current_frame = 0;
	}
}

/*
===========
Client_SwitchToWeaponImmediately

Switches the player's weapon immediately. No transitions.
Returns true on success, false otherwise.
============
*/
qboolean Client_SwitchToWeaponImmediately(edict_t *ent, int item_index)
{
	// First and last index is always null.
	assert(item_index > 0 && item_index < MAX_ITEMS-1);
	assert(ent && ent->client);

	gclient_t		*client = ent->client;
	gitem_t			*item = &(itemlist[item_index]);
	gitem_t			*ammo_item;

	// Doesn't exist.
	if (item == NULL)
		return false;

	// Don't have this weapon in inventory!
	if (client->pers.inventory[item_index] == 0)
		return false;

	// Not a weapon.
	if (!(item->flags & IT_WEAPON))
		return false;


	client->pers.weapon = item;
	if (item->ammo && (ammo_item = FindItem(item->ammo)) != NULL) {
		client->ammo_index = ITEM_INDEX(ammo_item);
	} else {
		client->ammo_index = 0;
	}

	// Now, we probably should change the state to ready since we don't know what the prev weapon was doing.
	// But this will allow players to spam fire, so fuck it.
	//client->weaponstate = WEAPON_READY;

	// Still has the old gun model.
	client->ps.gunindex = gi.modelindex(client->pers.weapon->view_model);

	client->machinegun_shots = 0;
	client->grenade_time = 0;
	client->weapon_sound = 0; // Stop weapon sounds

	return true;
}

/*
===========
Store_CreateShowEnt

Creates the little entity that shows where you stored.
============
*/
void Store_CreateShowEnt(edict_t *ent)
{
	assert(ent && ent->client);

	edict_t *store_ent;


	// Remove the old one.
	if (ent->client->resp.stored_ent != NULL)
		G_FreeEdict(ent->client->resp.stored_ent);

	store_ent = G_Spawn();
	VectorCopy(ent->client->resp.store[1].store_pos, store_ent->s.origin);
	VectorCopy(ent->client->resp.store[1].store_pos, store_ent->s.old_origin);
	store_ent->s.old_origin[2] -=10;
	store_ent->s.origin[2] -=10;
	store_ent->svflags = SVF_PROJECTILE;
	VectorCopy(ent->client->resp.store[1].store_angles, store_ent->s.angles);
	store_ent->movetype = MOVETYPE_NONE;
	store_ent->clipmask = MASK_PLAYERSOLID;
	store_ent->solid = SOLID_NOT;
	store_ent->s.renderfx = RF_TRANSLUCENT;
	VectorClear(store_ent->mins);
	VectorClear(store_ent->maxs);
	store_ent->s.modelindex = gi.modelindex (gset_vars->model_store);
	store_ent->dmg = 0;
	store_ent->classname = "stored_ent";
	gi.linkentity (store_ent);

	ent->client->resp.stored_ent = store_ent;
}

/*
===========
Store_StoreLocation

When player wants to save a location.
Returns true on success, false otherwise.
============
*/
qboolean Store_StoreLocation(edict_t *ent)
{
	assert(ent && ent->client);

	int				i;
	gclient_t		*client = ent->client;
	store_struct	*store = &(client->resp.store[STORE_LAST_STORE_POS]);

	if (gametype->value!=GAME_JUMP)
		return false;

	// Spectating
	if (ent->client->resp.ctf_team == CTF_NOTEAM)
		return false;

	// Can we store in the air?
	if (gset_vars->store_safe && !(ent->client->ps.pmove.pm_flags & PMF_ON_GROUND)) {
		gi.cprintf(ent, PRINT_HIGH, "Can only store on ground\n");
		return false;
	}

	for (i = MAX_STORES-1; i >= 1; i--) {
		ent->client->resp.store[i] = ent->client->resp.store[i-1]; //move old stores +1
	}
	store->stored_item_timer = ent->client->resp.item_timer;
	store->stored_finished = ent->client->resp.finished;
	VectorCopy(ent->s.origin, store->store_pos);
	VectorCopy(ent->s.angles, store->store_angles);
	store->store_angles[2] = 0;
	store->stored = true;
	ent->client->resp.can_store = true;
	//velocity store feature 
	//we always store velocity so it can be ready when toggled on
	VectorCopy(ent->velocity, store->stored_velocity);

	// Save inventory
	assert(sizeof(client->pers.inventory) == sizeof(store->store_inventory));
	memcpy(store->store_inventory, client->pers.inventory, sizeof(store->store_inventory));
	// Save current weapon
	store->store_active_weapon_index = ITEM_INDEX(client->pers.weapon);

	if (jump_show_stored_ent) {
		Store_CreateShowEnt(ent);
	}

	return true;
}

/*
===========
Store_Recall

When player wants to recall to a previous location.
Returns true on success, false otherwise.
============
*/
qboolean Store_Recall(edict_t *ent, int store_index)
{
	assert(store_index >= 1 && store_index < MAX_STORES);
	assert(ent && ent->client);

	gclient_t		*client = ent->client;
	store_struct	*store = &(client->resp.store[store_index]);
	vec3_t			spawn_origin, spawn_angles;
	int				i;

	// Nothing stored here.
	if (!(store->stored))
		return false;

	// Spectating.
	if (ent->client->resp.ctf_team == CTF_NOTEAM)
		return false;

	if (client->chase_target)
		return false;

	if (client->resp.replaying)
		return false;

	if (gametype->value!=GAME_JUMP)
		return false;

	if (!client->resp.can_store)
		return false;

	// Must be on team easy
	if (ent->client->resp.ctf_team != CTF_TEAM1)
		return false;

	ClearPersistants(&client->pers);
	ClearCheckpoints(ent);

	// Respawn
	if (ent->deadflag)
		respawn(ent);

	client->resp.item_timer = store->stored_item_timer; // Copy run time
	client->resp.finished = store->stored_finished; // Did we finish?
	client->resp.recalls--;
	
	VectorCopy(store->store_pos,spawn_origin);
	VectorCopy(store->store_angles,spawn_angles);
	client->resp.store[0] = *store;
		
	//velocity store feature - kill player's velocity if the toggle isn't on
	if (client->pers.store_velocity) {
		VectorCopy(store->stored_velocity, ent->velocity); //velocity store feature	
	} else {
		VectorClear(ent->velocity);
	}

	for (i = 0; i < 3; i++)
		client->ps.pmove.origin[i] = spawn_origin[i]*8;
	//ZOID
	client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
	//ZOID
	VectorCopy (spawn_origin, ent->s.origin);
	ent->s.origin[2] += 1;	// make sure off ground
	VectorCopy (ent->s.origin, ent->s.old_origin);

	// set the delta angle
	for (i = 0; i < 2; i++)
		client->ps.pmove.delta_angles[i] = ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	VectorCopy (ent->s.angles, client->ps.viewangles);
	VectorCopy (ent->s.angles, client->v_angle);

	// Copy inventory
	assert(sizeof(client->pers.inventory) == sizeof(store->store_inventory));
	memcpy(client->pers.inventory, store->store_inventory, sizeof(store->store_inventory));

	// Set current weapon.
	if (store->store_active_weapon_index > 0) {
		Client_SwitchToWeaponImmediately(ent, store->store_active_weapon_index);
	}
	
	
	hud_footer(ent);
	return true;
}

void Cmd_Recall(edict_t *ent) {

	// check if they can store
	if (!ent->client->resp.can_store) {
		Cmd_Kill_f(ent);
		return;
	}

	// check for team easy
	if (ent->client->resp.ctf_team != CTF_TEAM1) {
		Cmd_Kill_f(ent);
		return;
	}

	int store_index = 1;

	// They want a specific store?
	if (gi.argc() >= 2) { //recall + number
		store_index = atoi(gi.argv(1));

		if (store_index < 1 || store_index >= MAX_STORES) {
			gi.cprintf(ent, PRINT_HIGH, "Invalid store number. Must be between (1-%i)\n", MAX_STORES-1);
			return;
		}

		if (!(ent->client->resp.store[store_index].stored)) { //don't use it if it isn't set..
			gi.cprintf(ent, PRINT_HIGH, "No recall for number: %i\n", store_index);
			return;
		}
	} else {
		// No argument given.
		// We MUST have something stored here.
		if (!(ent->client->resp.store[store_index].stored)) {
			Cmd_Kill_f(ent);
			return;
		}
	}

	Store_Recall(ent, store_index);
}

void List_Admin_Commands(edict_t *ent)
{
	int i;
	int j;

	gi.cprintf(ent, PRINT_HIGH, "\n-----------------------------------------------------------------------------------");

	for (i = 1;i <= aset_vars->ADMIN_MAX_LEVEL;i++)
	{
		if (ent->client->resp.admin < i)
			break;

		gi.cprintf(ent,PRINT_HIGH,"\nLevel %d - ", i);
		if (i == aset_vars->ADMIN_STUFF_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "stuff ");
		if (i == aset_vars->ADMIN_ADDMAP_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "addmap remmap ");
		if (i == aset_vars->ADMIN_SILENCE_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"silence unsilence ");
		if (i == aset_vars->ADMIN_BOOT_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"boot ");
		if (i == aset_vars->ADMIN_VOTE_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"mvote pvote cvote dvotes ");
		if (i == aset_vars->ADMIN_BRING_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"bring ");
		if (i == aset_vars->ADMIN_ADDTIME_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"addtime ");
		if (i == aset_vars->ADMIN_ADDENT_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"addent rement moveent alignent shiftent listents addbox movebox skinbox lock ");
		if (i == aset_vars->ADMIN_MSET_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"mset ");
		if (i == aset_vars->ADMIN_GSET_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"gset ");
		if (i == aset_vars->ADMIN_IP_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"whois ");
		if (i == aset_vars->ADMIN_BAN_LEVEL)
			gi.cprintf(ent,PRINT_HIGH,"addban remban listbans ");
		if (i == aset_vars->ADMIN_REMTIMES_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "remtime remalltimes ");
		if (i == aset_vars->ADMIN_UPDATESCORES_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "updatescores ");
		if (i == aset_vars->ADMIN_ADMINEDIT_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "listadmin addadmin remadmin changeadmin ");
		if (i == aset_vars->ADMIN_TOGGLEHUD_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "togglehud ");
		if (i == aset_vars->ADMIN_NEXTMAPS_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "nextmaps ");
		if (i == aset_vars->ADMIN_CHANGENAME_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "changename ");
		if (i == aset_vars->ADMIN_RATERESET_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "ratereset ");
		if (i == aset_vars->ADMIN_SORTMAPS_LEVEL)
			gi.cprintf(ent, PRINT_HIGH, "sortmaps ");
		gi.cprintf(ent,PRINT_HIGH,"\n");
	}
	gi.cprintf(ent, PRINT_HIGH, "-----------------------------------------------------------------------------------\n\n");
}
void mvote(edict_t *ent)
{
	char text[1024];
	char temp[128];
	int i,i2,map;
	int	notimes[MAX_MAPS];

	if (ent->client->resp.admin<aset_vars->ADMIN_VOTE_LEVEL) {
		gi.cprintf(ent, PRINT_HIGH, "Please login to use mvote.\n");
		return;
	}

	if (gi.argc() < 2) {
		gi.cprintf(ent, PRINT_HIGH, "--------------------------------\n");
		gi.cprintf(ent, PRINT_HIGH, "Type maplist for a list of maps.\n");
		gi.cprintf(ent, PRINT_HIGH, "mvote [mapname] - a specific map.\n");
		gi.cprintf(ent, PRINT_HIGH, "mvote random - a random map.\n");
		gi.cprintf(ent, PRINT_HIGH, "mvote notime - a random map with no times.\n");
		gi.cprintf(ent, PRINT_HIGH, "mvote next - the next map.\n");		
		gi.cprintf(ent, PRINT_HIGH, "mvote prev - the previous map.\n");
		gi.cprintf(ent, PRINT_HIGH, "mvote new - the newest map.\n");
		gi.cprintf(ent, PRINT_HIGH, "--------------------------------\n");
		return;
	}

	// check for empty maplist
	if (maplist.nummaps<=0) {
		gi.cprintf(ent, PRINT_HIGH, "No maps in maplist\n");
		return;
	}
	map = -1;

	strncpy(temp,gi.argv(1),sizeof(temp)-1);

	if ((strcmp(temp,"RANDOM")==0) || (strcmp(temp,"random")==0)) {
		map = rand() % maplist.nummaps;	
		if (ent->client->resp.admin>=aset_vars->ADMIN_VOTE_LEVEL) {
			sprintf(text,"changed level to %s.",maplist.mapnames[map]);
			admin_log(ent,text);
			gi.bprintf(PRINT_HIGH, "%s's mvote(random) has passed. Map changing to %s.\n", 
				ent->client->pers.netname, maplist.mapnames[map]);
			strncpy(level.forcemap, maplist.mapnames[map], sizeof(level.forcemap) - 1);
			EndDMLevel();
			return;
		}
	}
	else
	if ((strcmp(temp,"NEXT")==0) || (strcmp(temp,"next")==0)) {
		map = level.mapnum;
		map++;
		if (map>=maplist.nummaps)
			map = 0;
		
		if (ent->client->resp.admin>=aset_vars->ADMIN_VOTE_LEVEL) {
			sprintf(text,"changed level to %s.",maplist.mapnames[map]);
			admin_log(ent,text);
			gi.bprintf(PRINT_HIGH, "%s's mvote(next) has passed. Map changing to %s.\n", 
				ent->client->pers.netname, maplist.mapnames[map]);
			strncpy(level.forcemap, maplist.mapnames[map], sizeof(level.forcemap) - 1);
			EndDMLevel();
			return;
		}
	}
	else
	if ((strcmp(temp,"notime")==0) || (strcmp(temp,"NOTIME")==0))
	{
		i2 = 0;
		for (i=0;i<maplist.nummaps;i++)
		{
			if (!maplist.times[i][0].time)
			{
				notimes[i2] = i;
				i2++;
			}
		}
		if (!i2)
		{
			gi.cprintf(ent,PRINT_HIGH,"All maps have a time set\n");
			return;
		}
		map = rand() % i2;

		if (ent->client->resp.admin>=aset_vars->ADMIN_VOTE_LEVEL) {
			sprintf(text,"changed level to %s (no time set).",maplist.mapnames[notimes[map]]);
			admin_log(ent,text);
			gi.bprintf(PRINT_HIGH, "%s's mvote(notime) has passed. Map changing to %s.\n", 
				ent->client->pers.netname, maplist.mapnames[notimes[map]]);
			strncpy(level.forcemap, maplist.mapnames[notimes[map]], sizeof(level.forcemap) - 1);
			EndDMLevel();
			return;
		}
	}
	else
	if ((strcmp(temp,"PREV")==0) || (strcmp(temp,"prev")==0))
	{
		map = level.mapnum;
		map--;
		if (map<0)
			map = maplist.nummaps-1;
		
		if (ent->client->resp.admin>=aset_vars->ADMIN_VOTE_LEVEL) {
			sprintf(text,"changed level to %s.",maplist.mapnames[map]);
			admin_log(ent,text);
			gi.bprintf(PRINT_HIGH, "%s's mvote(prev) has passed. Map changing to %s.\n", 
				ent->client->pers.netname, maplist.mapnames[map]);
			strncpy(level.forcemap, maplist.mapnames[map], sizeof(level.forcemap) - 1);
			EndDMLevel();
			return;
		}
	}
	else
	if ((strcmp(temp,"NEW")==0) || (strcmp(temp,"new")==0))
	{
		map = maplist.nummaps - 1;

		if (ent->client->resp.admin>=aset_vars->ADMIN_VOTE_LEVEL) {
			sprintf(text,"changed level to %s.",maplist.mapnames[map]);
			admin_log(ent,text);
			gi.bprintf(PRINT_HIGH, "%s's mvote(new) has passed. Map changing to %s.\n", 
				ent->client->pers.netname, maplist.mapnames[map]);
			strncpy(level.forcemap, maplist.mapnames[map], sizeof(level.forcemap) - 1);
			EndDMLevel();
			return;
		}
	}
	else
	{
		for (i=0;i<maplist.nummaps;i++)
		{
			if (Q_stricmp(maplist.mapnames[i],gi.argv(1))==0)
			{
				map = i;
				break;
			}
		}
	

		if (map==-1) {
			gi.cprintf(ent, PRINT_HIGH, "Unknown Jump map.\n");
			gi.cprintf(ent, PRINT_HIGH, "For available levels type maplist.\n");
			return;
		}

		if (ent->client->resp.admin>=aset_vars->ADMIN_VOTE_LEVEL) {
			sprintf(text,"changed level to %s.",gi.argv(1));
			admin_log(ent,text);
			gi.bprintf(PRINT_HIGH, "%s's mvote has passed. Map changing to %s.\n", 
				ent->client->pers.netname, gi.argv(1));
			strncpy(level.forcemap, maplist.mapnames[map], sizeof(level.forcemap) - 1);
			EndDMLevel();
			return;
		}
	}
}

void delete_all_times(void)
{
	char	name[256];
	cvar_t	*port;
	cvar_t	*tgame;
	int i;
	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

	for (i=0;i<maplist.nummaps;i++)
	{
		sprintf (name, "%s/%s/%s.t", tgame->string,port->string,maplist.mapnames[i]);
		remove(name);	
	}
}

void delete_all_demos(void)
{
	char	name[256];
	cvar_t	*tgame;
	int i;
	tgame = gi.cvar("game", "", 0);

	for (i=0;i<maplist.nummaps;i++)
	{
#ifdef ANIM_REPLAY
		sprintf (name, "%s/jumpdemo/%s.dj2", tgame->string,maplist.mapnames[i]);
#else
		sprintf (name, "%s/jumpdemo/%s.dj1", tgame->string,maplist.mapnames[i]);
#endif
		remove(name);	
	}
}

void List_Box_Types(edict_t *ent)
{
	gi.cprintf(ent,PRINT_HIGH,"1. Small Box\n");
	gi.cprintf(ent,PRINT_HIGH,"2. Medium Box\n");
	gi.cprintf(ent,PRINT_HIGH,"3. Large Box\n");
	gi.cprintf(ent,PRINT_HIGH,"4. Small Checkpoint\n");
	gi.cprintf(ent,PRINT_HIGH,"5. Medium Checkpoint\n");
	gi.cprintf(ent,PRINT_HIGH,"6. Large Checkpoint\n");
}

void Add_Box(edict_t *ent)
{
	int box_num;
    int cp;
    int cpsize;

	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	if (ent->client->resp.ctf_team==CTF_TEAM2 && gametype->value!=GAME_CTF)
		return;

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}

	if (gi.argc() < 2)
	{
		List_Box_Types(ent);
		return;
	}
		ClearNewEnt();

		level_items.newent = G_Spawn();

		VectorCopy (ent->s.origin, level_items.newent->s.origin);
		VectorCopy (ent->s.origin, level_items.newent->s.old_origin);
		box_num = atoi(gi.argv(1));
        cp = atoi(gi.argv(2));
        cpsize = sizeof(ent->client->resp.store[0].cpbox_checkpoint)/sizeof(int);
         if ((box_num>3) && (box_num<7)){
            if (gi.argc() > 2){
                if ((cp<0) || (cp>cpsize-1)) {
                    gi.cprintf(ent,PRINT_HIGH,"Give the cpbox an ID from 0 to %d (Ex: Addbox 4 1).\n",cpsize-1);
                    return;
                } else {
                    cp = atoi(gi.argv(2)) - 1;
                }
            } else {
                gi.cprintf(ent,PRINT_HIGH,"Give the cpbox an ID from 0 to %d (Ex: Addbox 4 1).\n",cpsize-1);
                return;
            }
        }
		if ((box_num<1) || (box_num>6))
			box_num = 1;
		switch (box_num)
		{

		case 1 : SP_jumpbox_small (level_items.newent);
				level_items.newent->s.origin[2] -=8;
				level_items.newent->s.old_origin[2] -=8;
				break;
		case 2 : SP_jumpbox_medium (level_items.newent);
				level_items.newent->s.origin[2] -=8;
				level_items.newent->s.old_origin[2] -=8;
				break;
		case 3 : SP_jumpbox_large (level_items.newent);
				level_items.newent->s.origin[2] -=8;
				level_items.newent->s.old_origin[2] -=8;
				break;
        case 4 : SP_cpbox_small (level_items.newent);
				level_items.newent->s.origin[2] -=8;
				level_items.newent->s.old_origin[2] -=8;
                level_items.newent->count = cp;
				break;
        case 5 : SP_cpbox_medium (level_items.newent);
				level_items.newent->s.origin[2] -=8;
				level_items.newent->s.old_origin[2] -=8;
                level_items.newent->count = cp;
				break;
        case 6 : SP_cpbox_large (level_items.newent);
				level_items.newent->s.origin[2] -=8;
				level_items.newent->s.old_origin[2] -=8;
                level_items.newent->count = cp;
				break;
		}

		gi.cprintf(ent,PRINT_HIGH,"Displaying box %s\n",gi.argv(1));
		if (ent->client->resp.ctf_team!=CTF_NOTEAM)
		{
			ent->s.origin[2] += 100;
			ent->s.old_origin[2] += 100;
		}
}

void Box_Skin(edict_t *ent)
{
	int snum;
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	if (ent->client->resp.ctf_team==CTF_TEAM2 && gametype->value!=GAME_CTF)
		return;
	if (gi.argc() < 2)
	{
		gi.cprintf(ent,PRINT_HIGH,"Please provide a valid skin number between 1 and 10\n");
		return;
	}
	snum = atoi(gi.argv(1));
	if ((snum<1) || (snum>10))
	{
		gi.cprintf(ent,PRINT_HIGH,"Please provide a valid skin number between 1 and 10\n");
		return;
	}
	snum--;

	if (level_items.newent)
	{
		gi.unlinkentity(level_items.newent);
		level_items.newent->s.skinnum = snum;
		gi.linkentity(level_items.newent);
	}
}

void Move_Box(edict_t *ent)
{
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	if (ent->client->resp.ctf_team==CTF_TEAM2 && gametype->value!=GAME_CTF)
		return;

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}
	if (level_items.newent)
	{
		//we have a box stored, move it
		gi.unlinkentity(level_items.newent);
		VectorCopy (ent->s.origin, level_items.newent->s.origin);
		VectorCopy (ent->s.origin, level_items.newent->s.old_origin);

		//level_items.newent->s.origin[2] -=abs(level_items.newent->mins[2])/2;
		
		//level_items.newent->s.old_origin[2] -=abs(level_items.newent->mins[2])/2;

		level_items.newent->s.origin[2] -=8;
		
		level_items.newent->s.old_origin[2] -=8;

		gi.linkentity(level_items.newent);

	}

}

void Move_Ent(edict_t *ent)
{
	int i;
	edict_t *ent_find;
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	if (ent->client->resp.ctf_team==CTF_TEAM2 && gametype->value!=GAME_CTF)
		return;

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}
	if (gi.argc() < 2)
	{
		show_ent_list(ent,0);
		return;
	}

	i = atoi(gi.argv(1));
	if ((i<1) || (i>50))
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid Entity\n");
		return;
	}
	i--;

	if (level_items.ents[i])
	{
		if (!strstr(level_items.ents[i]->classname,"jumpbox_") || !strstr(level_items.ents[i]->classname,"cpbox_"))
		{
			gi.unlinkentity(level_items.ents[i]);
			VectorCopy (ent->s.origin, level_items.ents[i]->s.origin);
			VectorCopy (ent->s.origin, level_items.ents[i]->s.old_origin);
			gi.linkentity(level_items.ents[i]);
			WriteEnts();
		}
		else
		{
			gi.unlinkentity(level_items.ents[i]);
			VectorCopy (ent->s.origin, level_items.ents[i]->s.origin);
			VectorCopy (ent->s.origin, level_items.ents[i]->s.old_origin);
			
			level_items.ents[i]->s.origin[2] -=8;
			level_items.ents[i]->s.old_origin[2] -=8;
			gi.linkentity(level_items.ents[i]);

			WriteEnts();
		}
		ent_find = NULL;
		if (strcmp(level_items.ents[i]->classname,"misc_teleporter")==0)
		{
			while ((ent_find = G_Find (ent_find, FOFS(target), level_items.ents[i]->target)) != NULL) {
				if (ent_find->mins[0] == -8)
				{
					gi.unlinkentity(ent_find);
					VectorCopy (level_items.ents[i]->s.origin, ent_find->s.origin);
					gi.linkentity(ent_find);
					break;
				}
			}
		}
	}
	else
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid Entity\n");
		return;
	}
}

void Skin_Ent(edict_t *ent)
{
	int i;
	int snum;
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}
	if (gi.argc() < 3)
	{
		show_ent_list(ent,0);
		return;
	}

	i = atoi(gi.argv(1));
	if ((i<1) || (i>50))
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid Entity\n");
		return;
	}
	i--;

	snum = atoi(gi.argv(2));
	snum--;
	if (level_items.ents[i])
	{
		gi.unlinkentity(level_items.ents[i]);
		level_items.ents[i]->s.skinnum = snum;
		gi.linkentity(level_items.ents[i]);
		WriteEnts();
		gi.cprintf(ent,PRINT_HIGH,"%i skinned with value %i\n",i,snum + 1);
	}
	else
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid Entity\n");
		return;
	}
}

void Move_Client(edict_t *ent, edict_t *targ)
{
		gclient_t	*client;
		vec3_t	spawn_origin;

		client = ent->client;
		VectorCopy(targ->s.origin,spawn_origin);
		VectorClear (ent->velocity);

		client->ps.pmove.origin[0] = spawn_origin[0]*8;
		client->ps.pmove.origin[1] = spawn_origin[1]*8;
		client->ps.pmove.origin[2] = spawn_origin[2]*8;
		//ZOID
		client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
		//ZOID
		VectorCopy (spawn_origin, ent->s.origin);
		ent->s.origin[2] += 1;	// make sure off ground
		VectorCopy (ent->s.origin, ent->s.old_origin);

		// set the delta angle
/*		for (i=0 ; i<3 ; i++)
			client->ps.pmove.delta_angles[i] = ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);
*/
//		ent->s.angles[PITCH] = 0;
//		ent->s.angles[YAW] = spawn_angles[YAW];
//		ent->s.angles[ROLL] = 0;
//		VectorCopy (ent->s.angles, client->ps.viewangles);
//		VectorCopy (ent->s.angles, client->v_angle);
}

void BringClient(edict_t *ent)
{
	int i;
	edict_t *targ;
	
	if (ent->client->resp.admin<aset_vars->ADMIN_BRING_LEVEL) {
		return;
	}
	if (gi.argc() < 2) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Who do you want to bring?\n");
		return;
	}

	if (*gi.argv(1) < '0' && *gi.argv(1) > '9') {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Specify the player number to bring.\n");
		return;
	}

	i = atoi(gi.argv(1));
	if (i < 1 || i > maxclients->value) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	targ = g_edicts + i;
	if (!targ->inuse) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	if (targ->client->resp.ctf_team==CTF_TEAM2 || gametype->value==GAME_CTF)
	{
		gi.cprintf(ent, PRINT_HIGH, "You may not transport players on Hard Team.\n");
		return;
	}

	if (ent->client->resp.admin>=aset_vars->ADMIN_BRING_LEVEL) {
		Move_Client(targ,ent);
		gi.cprintf(targ,PRINT_HIGH, "%s has transported you.\n", 
			ent->client->pers.netname);
		return;
	}
}

void GotoClient(edict_t *ent)
{
	int i;
	edict_t *targ;
	
	if (gi.argc() < 2) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Who do you want to goto?\n");
		return;
	}

	if (*gi.argv(1) < '0' && *gi.argv(1) > '9') {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Specify the player number to goto.\n");
		return;
	}

	i = atoi(gi.argv(1));
	if (i < 1 || i > maxclients->value) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	targ = g_edicts + i;
	if (!targ->inuse) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	if (ent->client->resp.ctf_team==CTF_TEAM2 || gametype->value==GAME_CTF)
	{
		gi.cprintf(ent, PRINT_HIGH, "You may not transport when on Hard Team.\n");
		return;
	}

	Move_Client(ent,targ);
}

void Uptime(edict_t *ent)
{
	int temp;
	temp = server_time/10;
	gi.cprintf(ent,PRINT_HIGH,"   Server Uptime : %02d:%02d:%02d \n",temp/3600,(temp%3600)/60,temp%60);
}


void debug_log(char *log_this)
{
	struct	tm *current_date;
	time_t	time_date;
	char	tdate[256];
	if ((debug_file) && (gset_vars->debug))
	{
		
	//set the date
		time_date = time(NULL);                // DOS system call
		current_date = localtime(&time_date);  // DOS system call

		sprintf(tdate, "%02d/%02d/%02d %02d:%02d:%02d    %s",
			current_date->tm_mday,
			current_date->tm_mon + 1
			,current_date->tm_year-100,
			current_date->tm_hour,
			current_date->tm_min,
			current_date->tm_sec,
			log_this
			);
		fprintf (debug_file, "%s\n",tdate);
		fflush(debug_file);
	}
	
}

void open_debug_file(void)
{
	char	name[256];
	cvar_t	*tgame;
	cvar_t	*port;
	port = gi.cvar("port", "", 0);

	tgame = gi.cvar("game", "", 0);
	sprintf (name, "%s/%s/debug.log", tgame->string,port->string);

	debug_file = fopen (name, "a");
}

void close_debug_file(void)
{
	if (debug_file)
	fclose(debug_file);
}

void	generate_random_start_map(void)
{
	FILE	*temp_file;
	char	name[256];
	cvar_t	*tgame;
	int rnum;
	tgame = gi.cvar("game", "", 0);
	if (!*tgame->string)
	{
		sprintf (name, "jump/server_map.cfg");
	}
	else
	{
		sprintf (name, "%s/server_map.cfg", tgame->string);
	}
	

	temp_file = fopen (name, "wb");


	if (temp_file)
	{		
		rnum = rand()%maplist.nummaps;// * (maplist.nummaps-1);
		fprintf (temp_file, "map %s\n",maplist.mapnames[rnum]);
		fclose(temp_file);
	}
}






void Add_Remove_Entity(char *add_this,edict_t *ent)
{
	int i,temp;
	temp = -1;
	for (i=0;i<MAX_REMOVE_ENTITIES;i++)
	{
		if (!entity_removal_list[i].inuse)
		{
			strcpy(entity_removal_list[i].compare,add_this);
			entity_removal_list[i].inuse = true;
			Save_Remove_File(level.mapname);
			temp = i;
			break;
		}
	}

	if (temp!=-1)
		gi.cprintf(ent,PRINT_HIGH,"%s added to remove list at position %d\n",add_this,temp+1);
	else
		gi.cprintf(ent,PRINT_HIGH,"Remove list is full, remove an item\n");
}

void List_Remove_Entities(edict_t *ent)
{
	int i;
	gi.cprintf(ent,PRINT_HIGH,"Removal Entities :\n");
	for (i=0;i<MAX_REMOVE_ENTITIES;i++)
	{
		if (entity_removal_list[i].inuse)
		{
			gi.cprintf(ent,PRINT_HIGH,"%2d.  %s\n",i+1,entity_removal_list[i].compare);
		}
	}
}

qboolean remall_Apply()
{
	int i,i2;
	edict_t *ent_find;
	int temp = 0;
	qboolean skip_this;
	qboolean added_spawn;

	for (i=0;i<MAX_REMOVE_ENTITIES;i++)
	{
		if (entity_removal_list[i].inuse)
		{
			//check entities loaded, remove
			ent_find = NULL;
			while ((ent_find = G_Find_contains (ent_find, FOFS(classname), entity_removal_list[i].compare)) != NULL) {
/*				if (
					(strstr(ent_find->classname,"info_player_"))
					)
				{
					change_spawn_state = true;
					if
					((!level.got_spawn) && (!added_spawn))
				
				{
						break;
//						continue;
				}
*/
				//need to make sure we dont remove any added entities
				skip_this = false;
				for (i2=0;i2<MAX_ENTS;i2++)
				{
					if (level_items.ents[i2])
					{
						if (level_items.ents[i2]==ent_find)
						{
							skip_this = true;
							break;
						}
					}
				}
				if (!skip_this)
				{
					G_FreeEdict(ent_find);
					temp++;
				}
			}
		}
	}
	return temp;
}

void remall(edict_t *ent)
{
	int i;
	int temp;
	char	buf[128];
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;

	if (gi.argc() < 2)
	{
		List_Remove_Entities(ent);
		gi.cprintf(ent,PRINT_HIGH,"Options : apply, clear, list.  For simplicity, you can also use weapons, spawns, buttons as options\n");
		return;
	}


	strcpy(mset_vars->edited_by,ent->client->resp.admin_name);

	if (strcmp(gi.argv(1),"apply")==0)
	{
		i = remall_Apply();
		gi.cprintf(ent,PRINT_HIGH,"%d items have been removed\n",i);

		return;
	}
	if (strcmp(gi.argv(1),"buttons")==0)
	{
		strcpy(buf,"func_button");
		Add_Remove_Entity(buf,ent);
		strcpy(buf,"trigger_");
		Add_Remove_Entity(buf,ent);
		return;
	}
	if (strcmp(gi.argv(1),"spawns")==0)
	{
		strcpy(buf,"info_player_deathmatch");
		Add_Remove_Entity(buf,ent);
		strcpy(buf,"info_player_start");
		Add_Remove_Entity(buf,ent);
		return;
	}
	if (strcmp(gi.argv(1),"weapons")==0)
	{
		strcpy(buf,"weapon_");
		Add_Remove_Entity(buf,ent);
		strcpy(buf,"item_");
		Add_Remove_Entity(buf,ent);
		strcpy(buf,"key_");
		Add_Remove_Entity(buf,ent);
		return;
	}
	
	if (strcmp(gi.argv(1),"clear")==0)
	{
		if (gi.argc()<3)
			List_Remove_Entities(ent);
		else
		{
			if (strcmp(gi.argv(2),"all")==0)
			{
				for (i=0;i<MAX_REMOVE_ENTITIES;i++)
				{
					entity_removal_list[i].inuse = false;
					Save_Remove_File(level.mapname);
				}
				gi.cprintf(ent,PRINT_HIGH,"Removed All Items\n");
			}
			else
			{
				temp = atoi(gi.argv(2))-1;
				if ((temp<0) || (temp>=MAX_REMOVE_ENTITIES))
				{
					gi.cprintf(ent,PRINT_HIGH,"Invalid Entry\n");
					return;
				}
				if (!entity_removal_list[temp].inuse)
				{
					gi.cprintf(ent,PRINT_HIGH,"Invalid Entry\n");
					return;
				}
				entity_removal_list[temp].inuse = false;
				Save_Remove_File(level.mapname);
				gi.cprintf(ent,PRINT_HIGH,"Removed Item %d\n",temp+1);
			}
		}
		return;
	}
	else
	if (strcmp(gi.argv(1),"list")==0)
	{
		List_Remove_Entities(ent);
		return;
	}

	Add_Remove_Entity(gi.argv(1),ent);
}


entity_removal_list_t entity_removal_list[MAX_REMOVE_ENTITIES*2];

void Save_Remove_File(char *mapname)
{
	FILE	*f;
	char	name[256];
	int i;
	qboolean added = false;
	cvar_t	*tgame;
	
	tgame = gi.cvar("game", "", 0);

	sprintf (name, "%s/ent/%s.rem", tgame->string,mapname);

	f = fopen (name, "wb");

	fprintf (f, "JumpMod059\n");
	for (i=0;i<MAX_REMOVE_ENTITIES;i++)
	{
		if (entity_removal_list[i].inuse)
		{
			fprintf (f, "%s\n",entity_removal_list[i].compare);
			added = true;
		}
	}
	fclose(f);
	if (!added)
		remove(name);
}

void Add_Old_Remove_Values(int temp)
{
	int i = 0;
	
	if (temp & 1)
	{
		entity_removal_list[i].inuse = true;
		strcpy(entity_removal_list[i].compare,"weapon_");
		i++;
		entity_removal_list[i].inuse = true;
		strcpy(entity_removal_list[i].compare,"item_");
		i++;
		entity_removal_list[i].inuse = true;
		strcpy(entity_removal_list[i].compare,"key_");
		i++;
	}
	if (temp & 2)
	{
		entity_removal_list[i].inuse = true;
		strcpy(entity_removal_list[i].compare,"info_player_deathmatch");
		i++;
	}
	if (temp & 4)
	{
		entity_removal_list[i].inuse = true;
		strcpy(entity_removal_list[i].compare,"trigger_");
		i++;
		entity_removal_list[i].inuse = true;
		strcpy(entity_removal_list[i].compare,"func_button");
	}
}
void Load_Remove_File(char *mapname)
{
	FILE	*f;
	char	name[256];
	int i;
	qboolean default_done = false;
	cvar_t	*tgame;
	char temp[100];
	char temp_c;
	
	tgame = gi.cvar("game", "", 0);

	sprintf (name, "%s/ent/%s.rem", tgame->string,mapname);

	for (i=0;i<MAX_REMOVE_ENTITIES*2;i++)
	{
		entity_removal_list[i].inuse = false;
	}

	i = 0;
	f = fopen (name, "rb");
	if (!f)
		return;
     temp_c = fgetc(f);
	 if ((temp_c>=49) && (temp_c<=57))
	 {
		 Add_Old_Remove_Values((int)temp_c-48);
		 default_done =  true;
	 }
	else
	do 
     
	  { 
//		 strcpy(temp,"umpMod");
         fgets(temp,100, f);
		 if (temp[0])
		 {
			temp[strlen(temp)-1] = 0;
			if (!strstr(temp,"umpMod"))
			{
	 			strcpy(entity_removal_list[i].compare,temp);
				if (strlen(entity_removal_list[i].compare)>3)
				{
					entity_removal_list[i].inuse = true;
				}
				i++;
			}
		 }
      } while ((!feof(f)) && (strlen(temp)>3));
	  if ((i>1) && (!default_done))
	  {
		  if (strstr(entity_removal_list[i-2].compare,entity_removal_list[i-1].compare))
			entity_removal_list[i-1].inuse = false;
	  }
	fclose(f);
}

qboolean Can_Remove_Entity(char *entity_name)
{
	int i;
	for (i=0;i<MAX_REMOVE_ENTITIES;i++)
	{
		if (entity_removal_list[i].inuse)
		{
			if (strstr(entity_name,entity_removal_list[i].compare))
			{
				return true;
			}
		}
	}
	return false;
}

void autorecord_stop(edict_t *ent)
{
	stuffcmd(ent,"stop\n");
	ent->client->resp.auto_recording = false;
}

void autorecord_start(edict_t *ent)
{
	char stuff_this[256];
	sprintf(stuff_this,"stop;record %s_%s_%s\n",level.mapname,ent->client->pers.netname,ent->client->resp.auto_record_time);
	stuffcmd(ent,stuff_this);
	ent->client->resp.auto_recording = true;
}

void autorecord_newtime(edict_t *ent)
{
	struct	tm *current_date;
	time_t	time_date;

	time_date = time(NULL);                // DOS system call
	current_date = localtime(&time_date);  // DOS system call

	sprintf(ent->client->resp.auto_record_time, "%02d_%02d_%02d_%02d-%02d-%02d",
			current_date->tm_mday,
			current_date->tm_mon + 1
			,current_date->tm_year-100,
			current_date->tm_hour,
			current_date->tm_min,
			current_date->tm_sec
			);
}

void autorecord(edict_t *ent)
{
	if (ent->client->resp.auto_record_on)
	{
		ent->client->resp.auto_record_on = false;
		if (ent->client->resp.auto_recording)
			autorecord_stop(ent);
	}
	else
	{
		ent->client->resp.auto_record_on = true;
		autorecord_newtime(ent);
	}
	gi.cprintf(ent,PRINT_HIGH,"autorecord is %s\n",ent->client->resp.auto_record_on ? "on" : "off");

}

void pause_client(edict_t *ent)
{
	ent->client->resp.paused = true;
}

void unpause_client(edict_t *ent)
{
	ent->client->resp.paused = false;
}

void Apply_Paused_Details(edict_t *ent)
{
/*	ent->client->ps.pmove.pm_flags |= PMF_NO_PREDICTION;
	ent->client->ps.pmove.pm_type = PM_FREEZE;*/
}

void Kill_Hard(edict_t *ent)
{
	vec3_t	spawn_origin, spawn_angles;
	gclient_t	*client;
	int		i;
	gitem_t		*item;
	client = ent->client;

	ent->gravity = 1.0;
	ent->gravity2 = 1.0;


//  if ( Jet_Active(ent) )
//  {
//    Jet_BecomeExplosion( ent, 1000 );
    /*stop jetting when dead*/
//    ent->client->Jet_framenum = 0;
//  }
	hook_reset(ent->client->hook);
	CTFDeadDropFlag(ent);
	memset(ent->client->pers.inventory, 0, sizeof(ent->client->pers.inventory));
	ent->client->Jet_framenum = 0;

	SelectSpawnPoint (ent, spawn_origin, spawn_angles);
	ent->client->resp.finished = false;
	ent->viewheight = 22;
	ent->air_finished = level.time + 12;
	ent->waterlevel = 0;
	ent->watertype = 0;
	VectorClear (ent->velocity);

	client->ps.pmove.origin[0] = spawn_origin[0]*8;
	client->ps.pmove.origin[1] = spawn_origin[1]*8;
	client->ps.pmove.origin[2] = spawn_origin[2]*8;
//ZOID
	client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
//ZOID
	client->ps.gunindex = gi.modelindex(client->pers.weapon->view_model);
	VectorCopy (spawn_origin, ent->s.origin);
	ent->s.origin[2] += 1;	// make sure off ground
	VectorCopy (ent->s.origin, ent->s.old_origin);
	// set the delta angle
	for (i=0 ; i<3 ; i++)
		client->ps.pmove.delta_angles[i] = ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	VectorCopy (ent->s.angles, client->ps.viewangles);
	VectorCopy (ent->s.angles, client->v_angle);
	gi.linkentity (ent);


	if (gametype->value==GAME_CTF)
	{
		item = FindItem("Grapple");
		ent->client->pers.inventory[ITEM_INDEX(item)] = 1;
		// force the current weapon up
		client->newweapon = item;
		ChangeWeapon (ent);
	}
	else
	{
		item = FindItem("Blaster");
		client->newweapon = item;
		ChangeWeapon (ent);
	}

	Start_Recording(ent);
	ent->client->resp.replaying = 0;
ent->client->resp.replay_speed = REPLAY_SPEED_ONE;


	pause_client(ent);

	if (ent->client->resp.auto_record_on)
		if (ent->client->resp.ctf_team==CTF_NOTEAM)
		{
			if (ent->client->resp.auto_recording)
				autorecord_stop(ent);			
		}
		else if (ent->client->resp.ctf_team==CTF_TEAM2 || gametype->value==GAME_CTF)
		{
			autorecord_start(ent);
		}
		else if (ent->client->resp.ctf_team==CTF_TEAM1)
		{
			if (ent->client->resp.auto_recording)
				autorecord_stop(ent);			
		}

	if ((ent->client->resp.ctf_team==CTF_TEAM1) || (ent->client->resp.ctf_team==CTF_TEAM2))
		ent->client->ctf_regentime = level.time;

	ent->client->resp.jumps = 0;
	ent->client->resp.item_timer = 0;
	ent->client->resp.client_think_begin = 0;
	ent->health = mset_vars->health;
	if (gset_vars->respawn_sound)
	{
		//ent->s.event = EV_PLAYER_TELEPORT; //spawn sound
		jumpmod_sound(ent, false, gi.soundindex("misc/tele1.wav"), CHAN_ITEM, 1, ATTN_IDLE);
		//particles?
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_TELEPORT_EFFECT);
		gi.WritePosition(ent->s.origin);
		gi.multicast(ent->s.origin, MULTICAST_PHS);
	}

	if (ent->client->resp.rep_racing_delay)
		ent->client->resp.race_frame = (int)(ent->client->resp.rep_racing_delay*10);
	else
		ent->client->resp.race_frame = 2;

	KillMyRox(ent);

//	gi.bprintf(PRINT_HIGH,"Kill_hard\n");

}

void AlignEnt(edict_t *ent)
{
	int tent;

	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	if (ent->client->resp.ctf_team==CTF_TEAM2 && gametype->value!=GAME_CTF)
		return;

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}
	if (level_items.newent)
	{
		if (gi.argc() < 2)
		{
			show_ent_list(ent,0);
			gi.cprintf(ent,PRINT_HIGH,"Format : alignent <axis> <entity>\nexample align z 1\n");
			return;
		}
		if (gi.argc() < 3)
		{
			tent=closest_ent(ent)-1;
			if (tent<0)
				return;
		}
		else
		{

			tent = atoi(gi.argv(2))-1;

			if (!level_items.ents[tent])
			{
				gi.cprintf(ent,PRINT_HIGH,"Invalid Entity\n");
				return;
			}
		}

		gi.unlinkentity(level_items.newent);
		if (*gi.argv(1)=='x')
		{
			level_items.newent->s.origin[0] = level_items.ents[tent]->s.origin[0];
			level_items.newent->s.old_origin[0] = level_items.ents[tent]->s.old_origin[0];
		}
		else if (*gi.argv(1)=='y')
		{
			level_items.newent->s.origin[1] = level_items.ents[tent]->s.origin[1];
			level_items.newent->s.old_origin[1] = level_items.ents[tent]->s.old_origin[1];
		}
		else if (*gi.argv(1)=='z')
		{
			level_items.newent->s.origin[2] = level_items.ents[tent]->s.origin[2];
			level_items.newent->s.old_origin[2] = level_items.ents[tent]->s.old_origin[2];
		}
		else
		{
			gi.cprintf(ent,PRINT_HIGH,"Invalid axis, use either x,y,z\n");
			gi.linkentity(level_items.newent);
			return;
		}

		gi.linkentity(level_items.newent);

	}
}

void Ghost_Play_Frame(void)
{
	int i;
	vec3_t myvec;
	qboolean got_jumper_off;
	edict_t *ent;
	vec_t distance;
	if ((mset_vars->ghost) && (!level.status))
	{
	if (level_items.recorded_time_frames[0])
	{
		if (level.ghost_frame<level_items.recorded_time_frames[0])
		{
			if (!level.ghost)
			{
				level.ghost = G_Spawn();

				level.ghost->svflags = SVF_PROJECTILE; // special net code is used for projectiles
				level.ghost->movetype = MOVETYPE_NONE;
				level.ghost->clipmask = MASK_PLAYERSOLID;
				level.ghost->solid = SOLID_NOT;
				//level.ghost->s.effects = EF_COLOR_SHELL;
				VectorClear (level.ghost->mins);
				VectorClear (level.ghost->maxs);
//				level.ghost->model = "players/female/tris.md2";
				level.ghost->s.modelindex2 = 0;
				if (gset_vars->ghost_glow)
					level.ghost->s.effects = gset_vars->ghost_glow;
				if (!mset_vars->ghost_model || !ghost_model_list[mset_vars->ghost_model-1].name[0])
				{
					level.ghost->s.modelindex = 255;
					level.ghost->s.skinnum = 255;
				}
				else
				{
					level.ghost->s.modelindex = gi.modelindex (va("players/ghost/%s.md2",ghost_model_list[mset_vars->ghost_model-1].name));
					level.ghost->s.skinnum = 0;
				}
				level.ghost->dmg = 0;
				level.ghost->classname = "ghost";
				gi.unlinkentity (level.ghost);
			}
			else
			{
				if (!mset_vars->ghost_model || !ghost_model_list[mset_vars->ghost_model-1].name[0])
				{
					level.ghost->s.modelindex = 255;
					level.ghost->s.skinnum = 255;
				}
				else
				{
					level.ghost->s.modelindex = gi.modelindex (va("players/ghost/%s.md2",ghost_model_list[mset_vars->ghost_model-1].name));
					level.ghost->s.skinnum = 0;
				}
				gi.unlinkentity (level.ghost);
			}

			VectorCopy(level_items.recorded_time_data[0][level.ghost_frame].origin,level.ghost->s.origin);
			VectorCopy(level_items.recorded_time_data[0][level.ghost_frame].origin,level.ghost->s.old_origin);
			VectorCopy(level_items.recorded_time_data[0][level.ghost_frame].angle,level.ghost->s.angles);
#ifdef ANIM_REPLAY
			level.ghost->s.frame = (level_items.recorded_time_data[0][level.ghost_frame].frame & 255);
#endif

			got_jumper_off = false;
			if (gset_vars->hideghost)
			{
				distance = gset_vars->hideghost;
				for (i=0 ; i<maxclients->value ; i++)
				{
					ent = g_edicts + 1 + i;
					if (!ent->inuse || !ent->client || !ent->client->resp.hide_jumpers)
						continue;
					if (gametype->value==GAME_CTF)
					{
						if (ent->client->resp.ctf_team<CTF_TEAM1)
							continue;
					}
					// let ghosts disappear in easy too
					/*else
					{
						if (ent->client->resp.ctf_team!=CTF_TEAM2 )
							continue;
					}*/
					VectorSubtract(ent->s.origin,level_items.recorded_time_data[0][level.ghost_frame].origin,myvec);
					if (VectorLength(myvec)<distance)
					{
						got_jumper_off = true;
						break;				
					}
				}
			}
			if(got_jumper_off)
			{
				level.ghost->svflags = SVF_NOCLIENT;
			}
			else
			{
				level.ghost->svflags = SVF_PROJECTILE;
			}
			gi.linkentity (level.ghost);
			level.ghost_frame++;
		}
		else
		{
			level.ghost_frame = 0;
		}
	}
	}
	else
	{
		if (level.ghost)
		{
			G_FreeEdict(level.ghost);
		}
	}
}

void shiftent (edict_t *ent)
{
	int		tent;
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	if (ent->client->resp.ctf_team==CTF_TEAM2 && gametype->value!=GAME_CTF)
		return;

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}
	if (level_items.newent)
	{
		if (gi.argc() < 3)
		{
			show_ent_list(ent,0);
			gi.cprintf(ent,PRINT_HIGH,"Format : shiftent <axis> <units>\nexample shiftent z 1\n");
			return;
		}

		tent = atoi(gi.argv(2));

		gi.unlinkentity(level_items.newent);
		if (*gi.argv(1)=='x')
		{
			level_items.newent->s.origin[0] += tent;
			level_items.newent->s.old_origin[0] +=tent;
		}
		else if (*gi.argv(1)=='y')
		{
			level_items.newent->s.origin[1] += tent;
			level_items.newent->s.old_origin[1] +=tent;
		}
		else if (*gi.argv(1)=='z')
		{
			level_items.newent->s.origin[2] += tent;
			level_items.newent->s.old_origin[2] +=tent;
		}
		else
		{
			gi.cprintf(ent,PRINT_HIGH,"Invalid axis, use either x,y,z\n");
			gi.linkentity(level_items.newent);
			return;
		}

		gi.linkentity(level_items.newent);

	}
}


void cmsg(edict_t *ent)
{
	ent->client->resp.cmsg = !ent->client->resp.cmsg;
	gi.cprintf(ent,PRINT_HIGH,"Centerprint messages %s\n",ent->client->resp.cmsg ? "off" : "on");
}


void CTFUnSilence(edict_t *ent)
{
	int i;
	edict_t *targ;
	char text[1024];
	
	if (ent->client->resp.admin<aset_vars->ADMIN_SILENCE_LEVEL) {
		return;
	}

	if (gi.argc() < 2) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Who do you want to unsilence?\n");
		return;
	}

	if (*gi.argv(1) < '0' && *gi.argv(1) > '9') {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Specify the player number to unsilence.\n");
		return;
	}

	i = atoi(gi.argv(1));
	if (i < 1 || i > maxclients->value) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	targ = g_edicts + i;
	if (!targ->inuse) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	if (ent->client->resp.admin>=aset_vars->ADMIN_SILENCE_LEVEL) {
		//admin unsilence
		targ->client->resp.silence = false;
		targ->client->resp.silence_until = 0;
		sprintf(text,"unsilenced %s.",targ->client->pers.netname);    // 084_h3
		admin_log(ent,text);                                          // 084_h3

		gi.bprintf(PRINT_HIGH, "%s was unsilenced by %s.\n",   
					targ->client->pers.netname,ent->client->pers.netname);
	}
}
void Notify_Of_Team_Commands(edict_t *ent)
{
	UpdateThisUsersUID(ent,ent->client->pers.netname);

	if (ent->client->resp.ctf_team==CTF_TEAM1) {
		gi.cprintf(ent,PRINT_HIGH,"Team Easy: Use the commands store and recall to practice jumps.\n");
		if (!ent->client->resp.can_store) { // this only happens if a person has not placed a store
			M_droptofloor(ent); // drop them to the floor in case spawn is raised
			Cmd_Store_f(ent);
		}
	}
	else if (ent->client->resp.ctf_team==CTF_TEAM2) {
		gi.cprintf(ent,PRINT_HIGH,"Team Hard: Grab the rail and set a time!\n");
	}
}

void JumpChase(edict_t *ent)
{
	int i;
	edict_t *e;

	if (ent->client->resp.ctf_team!=CTF_NOTEAM || ent->client->resp.replaying)
		CTFObserver(ent);

	ent->client->resp.next_chasecam_toggle = level.time + 0.5;
	ent->client->resp.replay_speed = REPLAY_SPEED_ONE;
	ent->client->resp.replaying = 0;
	hud_footer(ent);
	if (ent->client->chase_target) {
		if (!ent->client->resp.chase_ineye)
		{
			ent->client->resp.chase_ineye = true;
			return;
		}
		ent->client->resp.chase_ineye = false;

//		if (ent->client->resp.chasecam_type)
		{
			ent->client->chase_target = NULL;
			ent->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
			PMenu_Close(ent);
			ent->client->resp.chasecam_type = 0;
			return;
		}
		ent->client->resp.chasecam_type++;	
		return;
	}

	for (i = 1; i <= maxclients->value; i++) {
		e = g_edicts + i;
		if (e->inuse && e->solid != SOLID_NOT) {
			ent->client->chase_target = e;
			memcpy(ent->client->resp.store[0].cpbox_checkpoint, e->client->resp.store[0].cpbox_checkpoint, sizeof(e->client->resp.store[0].cpbox_checkpoint)); //copy checkpoints
			hud_footer(ent);
			PMenu_Close(ent);
			ent->client->update_chase = true;
			return;
		}
	}
	ent->client->resp.chasecam_type=0;
	gi.cprintf(ent,PRINT_HIGH,"no one to chase\n");
//	SetLevelName(nochasemenu + jmenu_level);

//	PMenu_Close(ent);
//	PMenu_Open(ent, nochasemenu, -1, sizeof(nochasemenu) / sizeof(pmenu_t), NULL);
}

void SetSpinnyThing(void)
{
	vec3_t closest;
	vec3_t v1;
	edict_t *what;
	edict_t *closest_ent;
	if (level_items.recorded_time_frames[0])
	{
		if (level.spinnything)
			G_FreeEdict(level.spinnything);
		
			
	what = NULL;
   closest[0] = closest[1] = closest[2] = 0;
   closest_ent = NULL;
	while ((what = G_Find_contains(what, FOFS(classname), "weapon_")) != NULL) {
//		if (what == level.spinnything)
//			continue;
			VectorSubtract(level_items.recorded_time_data[0][level_items.recorded_time_frames[0]-1].origin, what->s.origin, v1);
			if (!VectorLength(closest))
			{
				VectorCopy(v1,closest);
				closest_ent = what;
			}
			else if (VectorLength(v1)<VectorLength(closest))
			{
				VectorCopy(v1,closest);
				closest_ent = what;
			}
	}

	if (closest_ent)
	{
			level.spinnything = G_Spawn();
			level.spinnything->movetype = MOVETYPE_NONE;
			level.spinnything->clipmask = MASK_PLAYERSOLID;
			level.spinnything->solid = SOLID_NOT;

			VectorCopy (closest_ent->s.origin, level.spinnything->s.origin);
			VectorCopy (closest_ent->s.origin, level.spinnything->s.old_origin);
			VectorCopy(closest_ent->s.angles, level.spinnything->s.angles);

			level.spinnything->svflags = 0;
			level.spinnything->s.renderfx |= RF_FULLBRIGHT;
			level.spinnything->s.effects = gset_vars->target_glow;
			VectorClear (level.spinnything->mins);
			VectorClear (level.spinnything->maxs);
			level.spinnything->s.modelindex = gi.modelindex ("models/jump/smallmodel/tris.md2");
			level.spinnything->dmg = 0;
			level.spinnything->classname = "spinnything";

			gi.linkentity (level.spinnything);

	}		
	
	}
}

void ServerError(char *error)
{
	gi.dprintf("%s\n",error);
	gi.AddCommandString("quit\n");
}

void FlashLight(edict_t *ent)
{
	ent->client->resp.flashlight = !ent->client->resp.flashlight;
	gi.cprintf(ent,PRINT_HIGH,"Flashlight is %s\n",ent->client->resp.flashlight ? "on" : "off");
}

char *HighAscii(char *str)
{
	int len =strlen(str);
	int i;
	if (len)
	{
		for (i=0;i<len;i++)
		{
			str[i] |= 128;
		}
	}
	return str;
}

void Cmd_Show_Help(edict_t *ent) 
{
	int i = 0;

	while (help_main[i])
	{
		gi.cprintf (ent, PRINT_HIGH,help_main[i] );
		i++;
	}


}

void SendFlashLight(edict_t *ent)
{
	vec3_t	forward, right, offset;
	vec3_t	start, end;
	trace_t	trace;

	AngleVectors(ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 0, random() < 0.2 ? crandom() : 0, ent->viewheight);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);
	VectorMA(start, 8192, forward, end);
	trace = gi.trace(start, NULL, NULL, end, ent, CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_DEADMONSTER);
//	VectorCopy(trace.endpos, ent->s.origin);
//	gi.linkentity(ent);
//	ent->nextthink = level.time + FRAMETIME;



	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_FLASHLIGHT);
//	gi.WritePosition(ent->s.origin);
	gi.WritePosition(trace.endpos);
	
	gi.WriteShort (ent-g_edicts);
	if (gset_vars->flashlight==2)
		gi.multicast (ent->s.origin, MULTICAST_PVS);
	else if (gset_vars->flashlight==1)
		gi.unicast (ent, MULTICAST_PVS);
}



void say_person(edict_t *ent)
{
	edict_t *targ;
	int		i;
	char	*p;
	char	text[2048];

	if (ent->client->resp.silence)
		return;

	if (gi.argc () < 3)
	{
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "format : say_person <number> <text>\n");
		return;
	}

	if (*gi.argv(1) < '0' && *gi.argv(1) > '9') {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "format : say_person <number> <text>\n");
		return;
	}

	i = atoi(gi.argv(1));
	if (i < 1 || i > maxclients->value) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "format : say_person <number> <text>\n");
		return;
	}

	targ = g_edicts + i;
	if (!targ->inuse) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "format : say_person <number> <text>\n");
		return;
	}


	Com_sprintf (text, sizeof(text), "[PM->%s: ", ent->client->pers.netname);

	{
		p = gi.args()+2;

		if (*p == '"')
		{
			p++;
			p[strlen(p)-1] = 0;
		}

		strcat(text, p);
	}


	// don't let text be too long for malicious reasons
	if (strlen(text) > 150)
		text[150] = 0;

	strcat(text, "]\n");

	if (CheckFlood(ent))
		return;

	gi.cprintf(targ, PRINT_CHAT, "%s", text);
	gi.cprintf(ent, PRINT_CHAT, "[Private Message sent to: %s]\n", targ->client->pers.netname);
}


//q2admin command queue code
void addCmdQueue(edict_t *ent, byte command, float timeout, unsigned long data, char *str)
{
  ent->client->cmdQueue[ent->client->maxCmds].command = command;
//  gi.cprintf(ent,PRINT_HIGH,"add %f %d\n",timeout,level.framenum);
  ent->client->cmdQueue[ent->client->maxCmds].timeout = level.framenum+ (timeout*10);
  ent->client->cmdQueue[ent->client->maxCmds].data = data;
  ent->client->cmdQueue[ent->client->maxCmds].str = str;
  
  ent->client->maxCmds++;
}


qboolean getCommandFromQueue(edict_t *ent, byte *command, unsigned long *data, char **str)
{
  int i;

  for(i = 0; i < ent->client->maxCmds; i++)
  {
    if(ent->client->cmdQueue[i].timeout < level.framenum)
    {
//		gi.cprintf(ent,PRINT_HIGH,"%f %d\n",ent->client->cmdQueue[i].timeout,level.framenum);
      // found good command..
      // get info to return
      *command = ent->client->cmdQueue[i].command;
      *data = ent->client->cmdQueue[i].data;

      if(str)
      {
        *str = ent->client->cmdQueue[i].str;
      }

      // remove command
      ent->client->maxCmds--;
      if(i < ent->client->maxCmds)
      {
        memmove(ent->client->cmdQueue + i, ent->client->cmdQueue + i + 1, (ent->client->maxCmds - i) * sizeof(CMDQUEUE));
      }
      return true;
    }
  }

  return false;
}

void removeClientCommand(edict_t *ent, byte command)
{
  int i = 0;

  while(i < ent->client->maxCmds)
  {
    if(ent->client->cmdQueue[i].command == command)
    {
      // remove command
      ent->client->maxCmds--;
      if(i < ent->client->maxCmds)
      {
        memmove(ent->client->cmdQueue + i, ent->client->cmdQueue + i + 1, (ent->client->maxCmds - i) * sizeof(CMDQUEUE));
      }
    }
    else
    {
      i++;
    }
  }
}


void removeClientCommands(edict_t *ent)
{
  ent->client->maxCmds = 0;
}


void Random_Teams(void)
{
	int		i;
	edict_t		*temp;
	qboolean	team = false;
//sprintf(text,"==== Creating Random Teams ====");
//debug_log(text);

	for (i=0 ; i<maxclients->value ; i++)
	{
			if (!g_edicts[i+1].inuse)
				continue;
			temp = &g_edicts[i+1];
			if (temp->client->resp.ctf_team!=CTF_NOTEAM)
			{
				if (team)
					addCmdQueue(temp,QCMD_FORCETEAM_EASY,(int)(random()*3),0,0);
				else
					addCmdQueue(temp,QCMD_FORCETEAM_HARD,(int)(random()*3),0,0);
				team = !team;
			}
			else
			{
				gi.cprintf(temp,PRINT_CHAT,"You have %d seconds to join up\n",gset_vars->overtimewait);
			}
			temp->client->resp.score = 0;
			temp->client->resp.recalls = 0;
	}
}

void OverTime_GiveAll(edict_t *temp,qboolean rocket)
{
	gclient_t *client;
	gitem_t		*item;

//sprintf(text,"==== Handing out the guns ====");
//debug_log(text);
			if (temp->client->resp.ctf_team!=CTF_NOTEAM)
			{
				
				client = temp->client;
				item = FindItem("Blaster");
				client->pers.inventory[client->pers.selected_item] = 1;
				if (rocket)
				{
					//give em all the goodies
					item = FindItem("Shotgun");
					client->pers.inventory[ITEM_INDEX(item)] = 1;
					item = FindItem("Super Shotgun");
					client->pers.inventory[ITEM_INDEX(item)] = 1;
					item = FindItem("Machinegun");
					client->pers.inventory[ITEM_INDEX(item)] = 1;
					item = FindItem("Chaingun");
					client->pers.inventory[ITEM_INDEX(item)] = 1;
					item = FindItem("Grenade Launcher");
					client->pers.inventory[ITEM_INDEX(item)] = 1;
					item = FindItem("Rocket Launcher");
					client->pers.inventory[ITEM_INDEX(item)] = 1;
					item = FindItem("HyperBlaster");
					client->pers.inventory[ITEM_INDEX(item)] = 1;

					item = FindItem("Bullets");
					client->pers.inventory[ITEM_INDEX(item)] = 50;
					item = FindItem("Shells");
					client->pers.inventory[ITEM_INDEX(item)] = 20;
					item = FindItem("Grenades");
					client->pers.inventory[ITEM_INDEX(item)] = 5;
					item = FindItem("Rockets");
					client->pers.inventory[ITEM_INDEX(item)] = 50;
					item = FindItem("Cells");
					client->pers.inventory[ITEM_INDEX(item)] = 50;


				}
				//give them the normal stuff
				client->pers.health			= gset_vars->overtimehealth;
				client->pers.max_health		= gset_vars->overtimehealth;

				client->pers.max_bullets	= 50;
				client->pers.max_shells		= 20;
				client->pers.max_rockets	= 10;
				client->pers.max_grenades	= 5;
				client->pers.max_cells		= 50;
				client->pers.max_slugs		= 30;


				client->pers.inventory[ITEM_INDEX(FindItem("Body Armor"))] = 150;
				
				item = FindItem("Slugs");
				client->pers.inventory[ITEM_INDEX(item)] = 30;

				item = FindItem("Railgun");
				client->pers.inventory[ITEM_INDEX(item)] = 1;
				client->pers.selected_item = ITEM_INDEX(item);
				client->pers.weapon = item;
				client->pers.lastweapon = item;
			}
}

int RemoveAllItems(void)
{
	edict_t *ent_find;
	int temp = 0;
	ent_find = NULL;
	while ((ent_find = G_Find_contains (ent_find, FOFS(classname), "weapon_")) != NULL) {
		G_FreeEdict(ent_find);
		temp++;
	}
	ent_find = NULL;
	while ((ent_find = G_Find_contains (ent_find, FOFS(classname), "item_")) != NULL) {
		G_FreeEdict(ent_find);
		temp++;
	}
	ent_find = NULL;
	while ((ent_find = G_Find_contains (ent_find, FOFS(classname), "key_")) != NULL) {
		G_FreeEdict(ent_find);
		temp++;
	}
	ent_find = NULL;
	while ((ent_find = G_Find_contains (ent_find, FOFS(classname), "ammo_")) != NULL) {
		G_FreeEdict(ent_find);
		temp++;
	}
	ent_find = NULL;
	while ((ent_find = G_Find_contains (ent_find, FOFS(classname), "stored_ent")) != NULL) {
		G_FreeEdict(ent_find);
		temp++;
	}
	ent_find = NULL;
	while ((ent_find = G_Find_contains (ent_find, FOFS(classname), "spinnything")) != NULL) {
		G_FreeEdict(ent_find);
		temp++;
	}
	return temp;
}



void Overtime_Kill(edict_t *ent)
{
	vec3_t	spawn_origin, spawn_angles;
	gclient_t	*client;
	int		i;
	gitem_t		*item;
	client = ent->client;

	hook_reset(ent->client->hook);

	memset(ent->client->pers.inventory, 0, sizeof(ent->client->pers.inventory));
	ent->client->Jet_framenum = 0;

	SelectSpawnPoint (ent, spawn_origin, spawn_angles);
	ent->client->resp.finished = false;
	ent->viewheight = 22;
	ent->air_finished = level.time + 12;
	ent->waterlevel = 0;
	ent->watertype = 0;
	VectorClear (ent->velocity);

	client->ps.pmove.origin[0] = spawn_origin[0]*8;
	client->ps.pmove.origin[1] = spawn_origin[1]*8;
	client->ps.pmove.origin[2] = spawn_origin[2]*8;
//ZOID
	client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
//ZOID
	client->ps.gunindex = gi.modelindex(client->pers.weapon->view_model);
	VectorCopy (spawn_origin, ent->s.origin);
	ent->s.origin[2] += 1;	// make sure off ground
	VectorCopy (ent->s.origin, ent->s.old_origin);
	// set the delta angle
	for (i=0 ; i<3 ; i++)
		client->ps.pmove.delta_angles[i] = ANGLE2SHORT(spawn_angles[i] - client->resp.cmd_angles[i]);

	ent->s.angles[PITCH] = 0;
	ent->s.angles[YAW] = spawn_angles[YAW];
	ent->s.angles[ROLL] = 0;
	VectorCopy (ent->s.angles, client->ps.viewangles);
	VectorCopy (ent->s.angles, client->v_angle);
	gi.linkentity (ent);


	item = FindItem("Blaster");
	client->newweapon = item;
	ChangeWeapon (ent);

	ent->client->resp.replaying = 0;
	ent->client->resp.replay_speed = REPLAY_SPEED_ONE;


	if (ent->client->resp.paused)
			unpause_client(ent);

	if ((ent->client->resp.ctf_team==CTF_TEAM1) || (ent->client->resp.ctf_team==CTF_TEAM2))
		ent->client->ctf_regentime = level.time;

	ent->client->resp.jumps = 0;
	ent->client->resp.item_timer = 0;
	ent->client->resp.client_think_begin = 0;
	ent->health = mset_vars->health;
	if (gset_vars->respawn_sound)
	{
		//ent->s.event = EV_PLAYER_TELEPORT; //spawn sound
		jumpmod_sound(ent, false, gi.soundindex("misc/tele1.wav"), CHAN_ITEM, 1, ATTN_IDLE);
		//particles?
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_TELEPORT_EFFECT);
		gi.WritePosition(ent->s.origin);
		gi.multicast(ent->s.origin, MULTICAST_PHS);
	}

}


void CTFApplyDegeneration(edict_t *ent)
{
	gclient_t *client;
	
	client = ent->client;
	if (!client)
		return;

	if (client->resp.ctf_team!=CTF_NOTEAM)
	if (level.status==LEVEL_STATUS_OVERTIME)
	{
		if (gset_vars->overtimetype!=OVERTIME_FAST)
		if (level.overtime>gset_vars->overtimewait)
		{
			if (ent->health>0)
			if (client->ctf_regentime < level.time) {
				client->ctf_regentime = level.time;
				ent->health --;
				client->ctf_regentime += 0.5;
			}

			if (ent->health<=0)
			{
				if ((ent->svflags & SVF_MONSTER) || (ent->client))
					ent->flags |= FL_NO_KNOCKBACK;
				meansOfDeath = MOD_DIED;
				player_die (ent, ent, ent, 100000, vec3_origin);
				return;
			}
		}
	}

}

void	SelectSpawnPointFromDemo (edict_t *ent, vec3_t origin, vec3_t angles)
{
	int frame;
	if (level_items.recorded_time_frames[0])
	{
		frame = random() * level_items.recorded_time_frames[0];
		VectorCopy (level_items.recorded_time_data[0][frame].origin, origin);
		origin[2] += 9;
		VectorCopy (level_items.recorded_time_data[0][frame].angle, angles);
	}


}


void ForceEveryoneToHard(void)
{
	int		i;
	edict_t		*temp;

	for (i=0 ; i<maxclients->value ; i++)
	{
			if (!g_edicts[i+1].inuse)
				continue;
			temp = &g_edicts[i+1];
			if (gametype->value!=GAME_CTF && temp->client->resp.ctf_team!=CTF_TEAM2)
			{
				addCmdQueue(temp,QCMD_FORCETEAM_HARD,(int)(random()*3),0,0);
			}
			else
			{
				//kill them
				Kill_Hard(temp);
			}

	}
}


void ForceEveryoneOutOfChase(void)
{
	int		i;
	edict_t		*temp;

	for (i=0 ; i<maxclients->value ; i++)
	{
			if (!g_edicts[i+1].inuse)
				continue;
			temp = &g_edicts[i+1];
		if (temp->client->chase_target)
		{
			temp->client->chase_target = NULL;
			temp->client->ps.pmove.pm_flags &= ~PMF_NO_PREDICTION;
		}
	}
}

void SendCenterToAll(char *send)
{
	int		i;
	edict_t		*temp;

	for (i=0 ; i<maxclients->value ; i++)
	{
			if (!g_edicts[i+1].inuse)
				continue;
			temp = &g_edicts[i+1];
		gi.centerprintf(temp, "******************\n\n%s\n\n******************",send);
	}
}

int CheckOverTimeRules(void)
{
	int	num_team1 = 0;
	int num_team2 = 0;
	int ret = 0;
	int		i;
	edict_t		*temp;

	if (level.overtime<=gset_vars->overtimewait)
	{
		if ((gset_vars->overtimewait-level.overtime) <= 10 && !ctfgame.countdown) {
			ctfgame.countdown = true;
			gi.positioned_sound (world->s.origin, world, CHAN_AUTO | CHAN_RELIABLE, gi.soundindex("world/10_0.wav"), 1, ATTN_NONE, 0);
			return 0;
		}
		return 0;
	}

	if (!ctfgame.fight)
	{
		ctfgame.fight = true;
		SendCenterToAll("FIGHT");
		gi.positioned_sound (world->s.origin, world, CHAN_AUTO | CHAN_RELIABLE, gi.soundindex("misc/tele_up.wav"), 1, ATTN_NONE, 0);
	}
	

	if (level.framenum & 31)
		return 0;
	for (i=0 ; i<maxclients->value ; i++)
	{
			if (!g_edicts[i+1].inuse)
				continue;
			temp = &g_edicts[i+1];
			if (temp->client->resp.ctf_team==CTF_TEAM1)
			{
				if (temp->health>0)
				num_team1++;
			}
			else if (temp->client->resp.ctf_team==CTF_TEAM2)
			{
				if (temp->health>0)
				num_team2++;
			}
	}

	if (!num_team1)
		ret |= 1;
	if (!num_team2)
		ret |= 2;
	return ret;
}

int CheckOverTimeLastManRules(void)
{
	int	num_team1 = 0;
	//get number of clients on either team
	int		i;
	edict_t		*temp;

	if (level.overtime<=gset_vars->overtimewait)
	{
		if ((gset_vars->overtimewait-level.overtime) <= 10 && !ctfgame.countdown) {
			ctfgame.countdown = true;
			gi.positioned_sound (world->s.origin, world, CHAN_AUTO | CHAN_RELIABLE, gi.soundindex("world/10_0.wav"), 1, ATTN_NONE, 0);
			return -1;
		}
		return -1;
	}

	if (!ctfgame.fight)
	{
		ctfgame.fight = true;
		SendCenterToAll("FIGHT");
		gi.positioned_sound (world->s.origin, world, CHAN_AUTO | CHAN_RELIABLE, gi.soundindex("misc/tele_up.wav"), 1, ATTN_NONE, 0);
	}
	

	if (level.framenum & 31)
		return -1;
	for (i=0 ; i<maxclients->value ; i++)
	{
			if (!g_edicts[i+1].inuse)
				continue;
			temp = &g_edicts[i+1];
			if (temp->client->resp.ctf_team!=CTF_NOTEAM)
			{
				if (temp->health>0)
				num_team1++;
			}
	}

	return num_team1;
}




void Add_Time(edict_t *ent) 
{
	int i;
	int timeleft;
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDTIME_LEVEL)
		return;

	if (gi.argc() < 2) {
		gi.cprintf(ent,PRINT_HIGH,"addtime <minutes>. typing addtime 0 will remove added time\n");
		gi.cprintf(ent,PRINT_HIGH,"%i minutes currently added\n",map_added_time);
		return;
	}

	i = atoi(gi.argv(1));
	if (i<-999 || i>999)
	{
		gi.cprintf(ent,PRINT_HIGH,"addtime <minutes>. typing addtime 0 will remove added time\n");
		gi.cprintf(ent,PRINT_HIGH,"%i minutes currently added\n",map_added_time);
		return;
	}

	if (gset_vars->admin_max_addtime)
	{
		if (mset_vars->addedtimeoverride)
		{
			if (map_added_time+i>=mset_vars->addedtimeoverride)
			{
				gi.cprintf(ent,PRINT_HIGH,"Added Time is currently at %i maximum for this level is %i\n",map_added_time,gset_vars->addedtimemap);
				return;
			}
		}
		if (gset_vars->addedtimemap)
			if (map_added_time+i>gset_vars->addedtimemap)
			{
				gi.cprintf(ent,PRINT_HIGH,"Added Time is currently at %i maximum for this level is %i\n",map_added_time,gset_vars->addedtimemap);
				return;
			}
	}

	
	map_added_time += i;
	if (!i)
	{
		map_added_time = 0;
	}
	Update_Added_Time();

	if (gset_vars->addtime_announce == 1)
		gi.bprintf(PRINT_HIGH, "%s added %i minutes (%i total time added)\n",ent->client->pers.netname, i, map_added_time);
	else
		gi.bprintf(PRINT_HIGH, "%i minutes added (%i total time added)\n", i, map_added_time);

	timeleft = ((int)(level.time / 60)) + (mset_vars->timelimit + map_added_time);
	if (timeleft < 0)
	{
		gi.bprintf(PRINT_HIGH,"%d\n",((int)(level.time / 60)) + (mset_vars->timelimit + map_added_time));
		End_Jumping();
		return;
	}
}

void D_Votes(edict_t *ent) 
{
	if (ent->client->resp.admin<aset_vars->ADMIN_VOTE_LEVEL)
		return;

	map_allow_voting = !map_allow_voting;
	gi.cprintf(ent,PRINT_HIGH,"Voting %s by %s\n",map_allow_voting ? "enabled" : "disabled", ent->client->pers.netname);

}

void WriteMapList(void)
{
	FILE	*f;
	int i;

	f = fopen (maplist.path, "wb");

	if (!f)
		return;
	fprintf(f,"[maplist]\n");
	for (i=0;i<MAX_MAPS;i++)
	{
		if (0 == strcmp(maplist.mapnames[i], ""))
			continue;

		fprintf (f, "%s\n", maplist.mapnames[i]);
	}
	fprintf (f, "###\n");
	fclose(f);
}

void AddMap(edict_t *ent) 
{
	char mapname[256];
	struct	tm *current_date;
	time_t	time_date;
	int		month,day,year;
	int i;  // _h2
	
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDMAP_LEVEL) {
		gi.cprintf(ent,PRINT_HIGH,"You must be a level %i admin to add maps.\n", aset_vars->ADMIN_ADDMAP_LEVEL);
		return;
	}

	if (gi.argc() < 2) {
		gi.cprintf(ent,PRINT_HIGH,"addmap <mapname> - use remmap <nr> to remove maps\n");
		return;
	}


	//set the date
		time_date = time(NULL);                // DOS system call
		current_date = localtime(&time_date);  // DOS system call
	
	strcpy(mapname,gi.argv(1));

        //see if map is already in list  // _h2
        for (i=0;i<maplist.nummaps;i++)  // _h2
        {  // _h2
                if (Q_stricmp(mapname,maplist.mapnames[i])==0)  // _h2
                {  // _h2
			gi.cprintf(ent,PRINT_HIGH,"'%s' is already in the maplist!\n",mapname);  // _h2
			return;  // _h2
                }  // _h2
        }  // _h2

        // Check that the map file exists.   // _h2
	if (!ValidateMap(mapname))  // _h2
        {  // _h2
		gi.cprintf(ent,PRINT_HIGH,"Unable to open file '%s.bsp'!\n",mapname);  // _h2
		return;  // _h2
	}  // _h2



//	sprintf (date_marker, "%i%i%i",current_date->tm_hour,current_date->tm_min,current_date->tm_sec);
	maplist.update[maplist.nummaps] = 0;//atoi(date_marker);

	strcpy(maplist.mapnames[maplist.nummaps],mapname);

	maplist.nummaps++;
	gi.bprintf(PRINT_HIGH,"%s has added %s to the map rotation.\n", ent->client->pers.netname, mapname);
	UpdateVoteMaps();

}

// ===================================================
// added by lilred

static const int points[] =
{
	25,20,16,13,11,10,9,8,7,6,5,4,3,2,1
};
void RemoveMap (edict_t* ent)
{
	int num;
	int i;
	int status;
	char	maplist_path[256];
	char	filename[256];
	cvar_t	*port;
	cvar_t	*tgame;
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDMAP_LEVEL)
		return;
	num = atoi(gi.argv(1));

	//gi.cprintf (ent, PRINT_HIGH, "%i\n", num);

	if (Q_stricmp(gi.argv(1), "") == 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "remmap <nr> - use addmap <mapname> to add a map.\n");
		return;
	}

	if (num>maplist.nummaps)
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid map number\n");
		return;
	}

	num--;
	if (num<0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Invalid map number\n");
		return;
	}
	for (i = 0; i < MAX_USERS; i++){
		if (maplist.times[num][i].time == 0)
			break;
		if (maplist.times[num][i].uid >= 0)
		{
			maplist.users[maplist.times[num][i].uid].completions--;
			if(points[i]>0)
				maplist.users[maplist.times[num][i].uid].score -= points[i];
			//removemapfrom_uid_file(maplist.times[num][i].uid);
			maplist.users[maplist.times[num][i].uid].maps_done[num] = 0;
		}
	}
	port = gi.cvar("port", "", 0);
	tgame = gi.cvar("game", "", 0);
	sprintf(filename, "%s/%s/%s.t", tgame->string, port->string, maplist.mapnames[num]);
	remove(filename);
	gi.cprintf(ent, PRINT_HIGH, "Removed %s from the maplist.\n", maplist.mapnames[num]);
	if (Q_stricmp(maplist.mapnames[num], level.mapname) == 0)
		remtimes(ent);
	strcpy(maplist.mapnames[num], "");
	sprintf (maplist_path, "%s/%s/maplist.ini", tgame->string,port->string);
	//write_map_file(level.mapname,level.mapnum);
	WriteMapList();
//	LoadMapList(maplist_path);
	UpdateVoteMaps();
	removed_map = true;

}

void Cmd_RepRepeat (edict_t *ent)
{
	if (!ent->client->resp.rep_repeat)
	{
		ent->client->resp.rep_repeat = 1;
		gi.cprintf (ent, PRINT_HIGH, "Replay repeating is ON.\n");
		return;
	}

	if (ent->client->resp.rep_repeat)
	{
		ent->client->resp.rep_repeat = 0;
		gi.cprintf (ent, PRINT_HIGH, "Replay repeating is OFF.\n");
		return;
	}
}

void Cmd_Debug (edict_t *ent)
{
	if (!ent->client->resp.debug)
	{
		ent->client->resp.debug = 1;
		gi.cprintf (ent, PRINT_HIGH, "Debug mode is ON.\n");
		return;
	}

	if (ent->client->resp.debug)
	{
		ent->client->resp.debug = 0;
		gi.cprintf (ent, PRINT_HIGH, "Debug mode is OFF.\n");
		return;
	}
}

void Cmd_UpdateScores(edict_t* ent)
{
	if (ent->client->resp.admin < aset_vars->ADMIN_UPDATESCORES_LEVEL)
		return;

	gi.bprintf(PRINT_HIGH, "Updating scores, please wait...\n");
	UpdateScores2_Israfel();
	write_users_file();
	gi.bprintf(PRINT_HIGH, "Done.\n");
}

// ===================

#define TAG_TIME 10;

void NotifyOfNewTag(edict_t *ent)
{
	int i;
	edict_t *e2;
		for (i = 1; i <= maxclients->value; i++) 
		{
			e2 = g_edicts + i;
			if (!e2->inuse)
				continue;
			if (e2!=ent)
			if (e2->client->resp.playtag)
			{
				gi.centerprintf(e2,"%s has been TAGGED!\n\n\n\nYou have 10 seconds to run away!\n",ent->client->pers.netname);
			}
		}
		gi.centerprintf(ent,"You have been TAGGED!\n\n\n\nYou have 10 seconds before you can TAG.\n",ent->client->pers.netname);
		gi.positioned_sound (world->s.origin, world, CHAN_AUTO | CHAN_RELIABLE, gi.soundindex("misc/talk1.wav"), 1, ATTN_NONE, 0);
}

void PassTag(edict_t *from)
{
	int i;
	edict_t *e2;
	edict_t *to = NULL;
	if (from->client->resp.tag_time==level.time)
	{
		for (i = 1; i <= maxclients->value; i++) 
		{
			e2 = g_edicts + i;
			if (!e2->inuse)
				continue;
			if (e2->client->resp.playtag)
			{
				gi.centerprintf(e2,"\nTAG BEGINS!\n");
			}
		}
	}
	else if (from->client->resp.tag_time<level.time)
	{

	to = findradius(to, from->s.origin, 10);
	if (to==from)
	to = findradius(to, from->s.origin, 10);
	if (to!=NULL)
	{
		from->client->resp.tagged = false;
		from->client->resp.tag_time = 0;

		to->client->resp.tagged = true;
		to->client->resp.tag_time = level.time + TAG_TIME;

		NotifyOfNewTag(to);
	}
	}

}

void NewTag(void)
{
	int i;
	int j = 0;
	edict_t *e2;
	int temp;
	int tag_list[32];
		for (i = 1; i <= maxclients->value; i++) 
		{
			e2 = g_edicts + i;
			if (!e2->inuse)
				continue;
			if (e2->client->resp.playtag)
			{
				e2->client->resp.tagged = false;
				e2->client->resp.tag_time = 0;
				tag_list[j] = i;
				j++;
			}
		}
		//j = num tagged
		if (j>1)
		{
			//do a new tag round
			temp = random() * j;
			e2 = g_edicts + tag_list[temp];
			//e2 is our new tagged person
			e2->client->resp.tagged = true;
			e2->client->resp.tag_time = level.time + TAG_TIME;
			NotifyOfNewTag(e2);
		}
}

void TagJoin(edict_t *ent)
{
	int i;
	edict_t *e2;
	qboolean got_tag = false;

	gi.cprintf(ent,PRINT_HIGH, "        Joined the game of TAG.\n"); 
		for (i = 1; i <= maxclients->value; i++) 
		{
			e2 = g_edicts + i;
			if (!e2->inuse)
				continue;
			if (e2->client->resp.playtag)
			if (e2!=ent)
			{
				gi.cprintf(e2,PRINT_HIGH, "        %s has joined the game of TAG.\n", 
					ent->client->pers.netname);

				if (e2->client->resp.tagged)
					got_tag = true;
			}
		}

	ent->client->resp.playtag = true;
	ent->client->resp.tagged = false;
	ent->client->resp.tag_time = 0;
	if (!got_tag)
		NewTag();
}

void TagLeave(edict_t *ent)
{
	int i;
	edict_t *e2;
	qboolean was_tagged;
	
	gi.cprintf(ent,PRINT_HIGH, "        Left the game of TAG.\n"); 
		for (i = 1; i <= maxclients->value; i++) 
		{
			e2 = g_edicts + i;
			if (!e2->inuse)
				continue;
			if (e2->client->resp.playtag)
			if (e2!=ent)
			{
				gi.cprintf(e2,PRINT_HIGH, "        %s has left the game of TAG.\n", 
					ent->client->pers.netname);
			}
		}
	ent->client->resp.playtag = false;
	was_tagged = ent->client->resp.tagged;
	ent->client->resp.tagged = false;
	ent->client->resp.tag_time = 0;
	if (was_tagged)
		NewTag();
}

void PlayTag(edict_t *ent)
{
	//make sure we are on hard
	if (ent->client->resp.ctf_team!=CTF_TEAM2)
		return;
	if (!ent->client->resp.playtag)
		TagJoin(ent);
	else
		TagLeave(ent);
}



int GetMID(char *mapname)
{
	int i;
	for (i=0;i<maplist.nummaps;i++)
	{
		if (strcmp(mapname,maplist.mapnames[i])==0)
		{
			//found map
			return i;
		}
	}
	//no map found
	return -1;
}



void stuff_client(edict_t *ent)
{
	int i;
	edict_t *targ;
	char send_string[1024];
	if (ent->client->resp.admin<aset_vars->ADMIN_STUFF_LEVEL)
		return;
	if (gi.argc()>2)
	{
		if (*gi.argv(1) < '0' && *gi.argv(1) > '9') {
			CTFPlayerList(ent);
			gi.cprintf(ent, PRINT_HIGH, "Specify the player number to stuff.\n");
			return;
		}

		i = atoi(gi.argv(1));
		if (i < 1 || i > maxclients->value) {
			CTFPlayerList(ent);
			gi.cprintf(ent, PRINT_HIGH, "Invalid player number.\n");
			return;
		}

		targ = g_edicts + i;
		if (!targ->inuse) {
			CTFPlayerList(ent);
			gi.cprintf(ent, PRINT_HIGH, "That player number is not connected.\n");
			return;
		}
		strcpy(send_string,gi.argv(2));
		if (gi.argc()>3)
			for (i=3;i<gi.argc();i++)
			{
				strcat(send_string," ");
				strcat(send_string,gi.argv(i));
			}
		stuffcmd(targ,send_string);
		gi.cprintf(ent,PRINT_HIGH,"Client (%s) has been stuffed!\n",targ->client->pers.netname);
		return;
	}

	CTFPlayerList(ent);
	gi.cprintf(ent,PRINT_HIGH,"stuff (client)\n");
}

void Slap_Him(edict_t *ent, edict_t *targ);
void SlapClient(edict_t *ent)
{
	int i;
	edict_t *targ;

	if (ent->client->resp.admin<aset_vars->ADMIN_SLAP_LEVEL)
		return;

	if (gi.argc() < 2) 
	{
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Who do you want to slap?\n");
		return;
	}

	if (strcmp(gi.argv(1),"all")==0)
	{
		for (i = 1; i <= maxclients->value; i++) 
		{
			targ = g_edicts + i;
			if (!targ->inuse)
				continue;
			if (targ->client->resp.ctf_team==CTF_TEAM1 && gametype->value!=GAME_CTF)
			{
				Slap_Him(targ,ent);
			}
		}
		return;
	}

	if (*gi.argv(1) < '0' && *gi.argv(1) > '9') 
	{
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Specify the player number to slap.\n");
		return;
	}

	i = atoi(gi.argv(1));
	if (i < 1 || i > maxclients->value) 
	{
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	targ = g_edicts + i;
	if (!targ->inuse) 
	{
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	if (targ->client->resp.ctf_team==CTF_TEAM2 || gametype->value==GAME_CTF)
	{
		gi.cprintf(ent, PRINT_HIGH, "You can not slap players on Hard Team.\n");
		//ThrowUpNow(targ,ent);
		return;
	}
	if (targ->client->resp.ctf_team==CTF_NOTEAM)
	{
		gi.cprintf(ent, PRINT_HIGH, "You can not slap spectators.\n");
		return;
	}

	if (ent->client->resp.admin>=aset_vars->ADMIN_SLAP_LEVEL) 
	{
		Slap_Him(targ,ent);
		return;
	}
}
//skaters code (unless he borrowed it from someone :) )
void Slap_Him(edict_t *ent, edict_t *targ)
{
	vec3_t	start;
	vec3_t	forward;
	vec3_t	end;
	vec3_t  randangle;

	randangle[0] = rand() / 100;
	randangle[1] = rand() / 100;
	randangle[2] = rand() / 100;

	VectorCopy(ent->s.origin, start);
	start[2] += ent->viewheight;
//	AngleVectors(ent->client->v_angle, forward, NULL, NULL);
	AngleVectors(randangle, forward, NULL, NULL);
	VectorMA(start, 8192, forward, end);
	if ( ent && ((ent->svflags & SVF_MONSTER) || (ent->client)) )
	{
		VectorScale(forward, -5000, forward);
		VectorAdd(forward, ent->velocity, ent->velocity);
	}
	gi.cprintf (ent, PRINT_HIGH, "You were slapped upside the head by %s!\n", targ->client->pers.netname);
}

void lock_ents(edict_t *ent)
{
	int i;
	edict_t *e2;
	level_items.locked = !level_items.locked;
	
	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL)
		return;
	
	if (level_items.locked)
	{
		level_items.locked_by=ent;
	}

	for (i = 1; i <= maxclients->value; i++) 
	{
		e2 = g_edicts + i;
		if (!e2->inuse)
			continue;
		if (e2!=ent)
		if (e2->client->resp.admin>=aset_vars->ADMIN_ADDENT_LEVEL)
		{
			gi.cprintf(e2,PRINT_HIGH,"%s has %s entity adding.\n",ent->client->pers.netname,level_items.locked ? "locked" : "unlocked");
		}
	}
	gi.cprintf(ent,PRINT_HIGH,"Entity adding has been %s.\n",level_items.locked ? "locked" : "unlocked");


}

void reset_map_played_count(edict_t *ent)
{
	int i;

	if (ent->client->resp.admin<aset_vars->ADMIN_RATERESET_LEVEL)
		return;

	if (maplist.nummaps)
	{
		for (i=0;i<maplist.nummaps;i++)
		{
			maplist.update[i] = 0;
		}
	}
	gi.cprintf(ent,PRINT_HIGH,"Map ratings cleared.\n");

}

void remove_maxsmins_boundary() {
	edict_t *ent;

	ent = g_edicts;
	while (ent = G_Find(ent,FOFS(classname),"maxsmins_boundary"))
	{
			G_FreeEdict(ent);
	}
}

void addclip_laser_think (edict_t *self){
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_DEBUGTRAIL);
	gi.WritePosition(self->pos1);
	gi.WritePosition(self->pos2);
	gi.multicast(self->s.origin, MULTICAST_PVS);

	self->nextthink = level.time + FRAMETIME;
}

void maxsmins_boundary(vec3_t start, vec3_t end){
	edict_t		*laser; //? or something

	laser = G_Spawn();
	VectorCopy(start,laser->pos1);
	VectorCopy(end,laser->pos2);
	laser->movetype = MOVETYPE_NONE;
    laser->solid = SOLID_NOT;
	laser->s.modelindex = 1;
	laser->classname = "maxsmins_boundary";
	laser->s.frame = 2;
	laser->s.skinnum = 0xf2f2f0f0;
	laser->think = addclip_laser_think;
	
	gi.linkentity(laser);

	laser->nextthink = level.time + 2;
}

void add_clip(edict_t *ent)
{
	char	action[16];
	int i;
	int cp;
	int cpmax;
	edict_t *tent;
	vec3_t center;
	vec3_t start;
	vec3_t end;

	if (ent->client->resp.admin<aset_vars->ADMIN_ADDENT_LEVEL){
		return;
	}
	if (ent->client->resp.ctf_team==CTF_TEAM2 && gametype->value!=GAME_CTF){
		return;
	}

	if ((level_items.locked) && (level_items.locked_by!=ent))
	{
		gi.cprintf(ent,PRINT_HIGH,"Entity adding has been locked by %s.\n",level_items.locked_by->client->pers.netname);
		return;
	}

	// set max cp value
	cpmax = sizeof(ent->client->resp.store[0].cpbox_checkpoint)/sizeof(int)-1;

	//no args, show ent list
	if (gi.argc() < 2) {
		gi.cprintf(ent, PRINT_HIGH, "Add 2 marks with addclip mark1/mark2, and create the ent with addclip create.\n");
		gi.cprintf(ent, PRINT_HIGH, "Make the ent a checkpoint by using addclip checkpoint (0 - %i)\n", cpmax);
		return;
	}
	
	strcpy(action,gi.argv(1));
	if (strcmp(action,"mark1")==0)
	{
		VectorCopy(ent->s.origin,level_items.clip1);
		level_items.clip1[2] -= 10;
		gi.cprintf(ent, PRINT_HIGH, "Mark 1 added at: %f - %f - %f\n",level_items.clip1[0],level_items.clip1[1],level_items.clip1[2]);
		remove_maxsmins_boundary(); //remove old boundary
	}
	else if (strcmp(action,"mark2")==0)
	{
		VectorCopy(ent->s.origin,level_items.clip2);
		level_items.clip2[2] -= 10;
		gi.cprintf(ent, PRINT_HIGH, "Mark 2 added at: %f - %f - %f\n",level_items.clip2[0],level_items.clip2[1],level_items.clip2[2]);
		remove_maxsmins_boundary(); //remove old boundary
	}

	if(level_items.clip1[0]!=0.0 && level_items.clip2[0]!=0.0){ 
		//Boundary?
		VectorCopy(level_items.clip1,start);
		VectorSet(end,start[0],start[1],level_items.clip2[2]);
		maxsmins_boundary(start,end);
		VectorSet(end,start[0],level_items.clip2[1],start[2]);
		maxsmins_boundary(start,end);
		VectorSet(end,level_items.clip2[0],start[1],start[2]);
		maxsmins_boundary(start,end);
		VectorSet(start,level_items.clip2[0],level_items.clip1[1],level_items.clip1[2]);
		VectorSet(end,start[0],level_items.clip2[1],start[2]);
		maxsmins_boundary(start,end);
		VectorSet(end,start[0],start[1],level_items.clip2[2]);
		maxsmins_boundary(start,end);
		VectorSet(start,level_items.clip1[0],level_items.clip1[1],level_items.clip2[2]);
		VectorSet(end,level_items.clip2[0],start[1],start[2]);
		maxsmins_boundary(start,end);
		
		VectorCopy(level_items.clip2,start);
		VectorSet(end,start[0],start[1],level_items.clip1[2]);
		maxsmins_boundary(start,end);
		VectorSet(end,start[0],level_items.clip1[1],start[2]);
		maxsmins_boundary(start,end);
		VectorSet(end,level_items.clip1[0],start[1],start[2]);
		maxsmins_boundary(start,end);
		VectorSet(start,level_items.clip1[0],level_items.clip2[1],level_items.clip2[2]);
		VectorSet(end,start[0],level_items.clip1[1],start[2]);
		maxsmins_boundary(start,end);
		VectorSet(end,start[0],start[1],level_items.clip1[2]);
		maxsmins_boundary(start,end);
		VectorSet(start,level_items.clip2[0],level_items.clip2[1],level_items.clip1[2]);
		VectorSet(end,level_items.clip1[0],start[1],start[2]);
		maxsmins_boundary(start,end);
	}

	//"addclip create" will create a clip between mark1 and mark2..
	//"addclip checkpoint id" (id = 1,2,3 etc) will create a checkpoint between mark1 and mark2..
	if (strcmp(action,"create")==0 || strcmp(action,"checkpoint")==0)
	{
		if(strcmp(action,"checkpoint")==0 && gi.argc() < 3){
			gi.cprintf(ent, PRINT_HIGH, "You need to give your checkpoint an ID from 0-%i (Ex: addclip checkpoint 1)\n", cpmax);
			return;
		}

		remove_maxsmins_boundary(); //remove old boundary
		//size of bound box
		tent = G_Spawn();
		if(level_items.clip1[0]>level_items.clip2[0])
			tent->maxs[0] = (level_items.clip1[0]-level_items.clip2[0])/2;
		else
			tent->maxs[0] = (level_items.clip2[0]-level_items.clip1[0])/2;

		if(level_items.clip1[1]>level_items.clip2[1])
			tent->maxs[1] = (level_items.clip1[1]-level_items.clip2[1])/2;
		else
			tent->maxs[1] = (level_items.clip2[1]-level_items.clip1[1])/2;

		if(level_items.clip1[2]>level_items.clip2[2])
			tent->maxs[2] = (level_items.clip1[2]-level_items.clip2[2])/2;
		else
			tent->maxs[2] = (level_items.clip2[2]-level_items.clip1[2])/2;

		tent->mins[0] = -tent->maxs[0];
		tent->mins[1] = -tent->maxs[1];
		tent->mins[2] = -tent->maxs[2];

		VectorSubtract(level_items.clip2,level_items.clip1,center);
		VectorScale(center,0.5,center);
		VectorAdd(center,level_items.clip1,center);
		VectorCopy(center,tent->s.origin);
		VectorCopy (tent->s.origin, tent->s.old_origin);
		tent->classname = "jump_clip";
		tent->svflags |= SVF_NOCLIENT;
		tent->movetype = MOVETYPE_NONE;
		if(strcmp(action,"checkpoint")==0){
			cp = atoi(gi.argv(2));

			// check for right cp values
			if (cp < 0 || cp > (sizeof(ent->client->resp.store[0].cpbox_checkpoint)/sizeof(int)) - 1) {
				gi.cprintf(ent, PRINT_HIGH, "Checkpoint value can only be between 0 and %i\n", cpmax);
				return;
			}
				
			tent->message = action;
			tent->solid = SOLID_TRIGGER;
			tent->count = cp;
		} else {
			tent->solid = SOLID_BBOX;
		}
		tent->s.modelindex = 1;
		gi.linkentity (tent);

		gi.linkentity (ent);


		for (i=0;i<MAX_ENTS;i++)
		{
			if (!level_items.ents[i])
			{
				level_items.ents[i] = tent;
				ED_CallSpawn (level_items.ents[i]);
				break;
			}
		}
		WriteEnts();
		if(strcmp(action,"checkpoint")==0){
			gi.cprintf(ent, PRINT_HIGH, "Checkpoint created with a checkpoint value of %i.\n", cp);
		}
		else {
			gi.cprintf(ent, PRINT_HIGH, "Jump_clip created.\n");
		}
		level_items.clip1[0] = 0;
		level_items.clip1[1] = 0;
		level_items.clip1[2] = 0;
		level_items.clip2[0] = 0;
		level_items.clip2[1] = 0;
		level_items.clip2[2] = 0;
		return;
	}

}

void addmaps(void)
{
	//find mid relating to string
	//if cant find ignore, else add
	FILE	*f;
	int i;
	char	name[128];
	cvar_t	*tgame;
	char temp[1024];
	qboolean got_match;

	tgame = gi.cvar("game", "", 0);

	if (!*tgame->string)
	{
		sprintf	(name, "jump/addmaps.ini");
	}
	else
	{
		sprintf (name, "%s/addmaps.ini", tgame->string);
	}

	i=0;

	f = fopen (name, "rb");
	if (!f)
	{
		return;
	}	

	while (!feof(f))
	{
		fscanf(f,"%s",&temp);
		//see if map is in list
		got_match = false;
		for (i=0;i<maplist.nummaps;i++)
		{
			if (Q_stricmp(temp,maplist.mapnames[i])==0)
			{
				//got match
				got_match = true;
			}
		}
		if (!got_match)
		{
			maplist.demoavail[maplist.nummaps] = false;
			maplist.update[maplist.nummaps] = 0;
			strncpy(maplist.mapnames[maplist.nummaps], temp, MAX_MAPNAME_LEN); 
			//UpdateTimes(maplist.nummaps);
			maplist.nummaps++;
			//new map added
			append_added_ini(temp);
			gi.bprintf(PRINT_HIGH,"%s has been added to the map rotation.\n", temp);
		}
	}


	fclose(f);
	remove(name);
}

void addsinglemap()
{
	qboolean got_match;
	int		i;
	char	*mapname;
	char	text[256];
	
	if(gi.argc()<3){
		gi.cprintf (NULL,PRINT_HIGH,"Correct cmd: sv addsinglemap <mapname>\n");
		return;
	}
	mapname = gi.argv(2);
	// Check that the map file exists.
	if (!ValidateMap(mapname)) 
    { 
		//ERROR: <mapname>.bsp noto found!
		gi.cprintf (NULL,PRINT_HIGH,"%s.bsp not found!\n", mapname); 
		return; 
	} 

	got_match = false;
	for (i=0;i<maplist.nummaps;i++)
	{
		if (Q_stricmp(mapname,maplist.mapnames[i])==0)
		{
			//got match
			got_match = true;
		}
	}
	if (!got_match)
	{
		maplist.demoavail[maplist.nummaps] = false;
		maplist.update[maplist.nummaps] = 0;
		strncpy(maplist.mapnames[maplist.nummaps], mapname, MAX_MAPNAME_LEN); 
		//UpdateTimes(maplist.nummaps);
		maplist.nummaps++;
		//new map added
		append_added_ini(mapname);

		sprintf(text,"say %s has been added to the map rotation.\n",mapname);
		//gi.cprintf (NULL,PRINT_HIGH,"%s",text);
		gi.AddCommandString(text);
	}
}
int num_time_votes;

void CTFVoteTime(edict_t *ent)
{
	int i;
	char text[1024];
	int diff;
	qboolean require_max = false;
	
	if (!map_allow_voting)
		return;

	if (ent->client->resp.silence)
		return;

	if ((level.time<20) && (ent->client->resp.admin<aset_vars->ADMIN_ADDTIME_LEVEL) && curclients > 2) // hannibal
	{
		gi.cprintf(ent,PRINT_HIGH,"Please wait %2.1f seconds before calling a vote\n",20.0-level.time);
		return;
	}


	if ((ent->client->resp.admin<aset_vars->ADMIN_ADDTIME_LEVEL) && gset_vars->notimevotetime >= level.time && curclients > 2)
	{
		gi.cprintf(ent,PRINT_HIGH,"Votes have been disabled for the first %d seconds of a map.\n",gset_vars->notimevotetime);
		return;
	}


/*	if (gi.argc() < 2) {
		gi.cprintf(ent,PRINT_HIGH,"Please provide a value\n");
		return;
	}

	if (*gi.argv(1) < '0' && *gi.argv(1) > '9') {
		gi.cprintf(ent, PRINT_HIGH, "Format : votetime <minutes>.\n");
		return;
	}*/

	i = atoi(gi.argv(1));

	if (i == 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "Format : votetime <minutes>.\n");
		return;
	}

	if (i<-1337 || i>1337)
	{
		gi.cprintf(ent, PRINT_HIGH, "Format : votetime <minutes>. You can only vote from -1337 to 1337 minutes.\n");
		return;
	}
	/*if ((mset_vars->timelimit*60)+(map_added_time*60)-level.time>600)
	{
		if (i>0)
		{
			gi.cprintf(ent, PRINT_HIGH, "You can only vote for more time when there are less than 10 minutes remaining.\n");
			return;
		}
	}*/

	diff = 0;

	if (ent->client->resp.admin<aset_vars->ADMIN_ADDTIME_LEVEL)
	{
		if (mset_vars->addedtimeoverride)
		{
			if (map_added_time+i>=mset_vars->addedtimeoverride)
			{
				diff = mset_vars->addedtimeoverride - map_added_time;
				if (gset_vars->voteextratime)			
					require_max = true;
				else
				{
					gi.cprintf(ent,PRINT_HIGH,"Added Time is currently at %i maximum for this level is %i\n",map_added_time,gset_vars->addedtimemap);
					return;
				}
			}
		}
		if (gset_vars->addedtimemap)
			if (map_added_time+i>gset_vars->addedtimemap)
			{
				diff = gset_vars->addedtimemap - map_added_time;
				if (gset_vars->voteextratime)			
					require_max = true;
				else
				{
					gi.cprintf(ent,PRINT_HIGH,"Added Time is currently at %i maximum for this level is %i\n",map_added_time,gset_vars->addedtimemap);
					return;
				}
			}
		
	}
	if (require_max && i>0 && map_added_time+i>gset_vars->voteextratime)
	{
		gi.cprintf(ent, PRINT_HIGH, "You may not vote more than %d minutes extra time per map.\n",gset_vars->voteextratime);
		return;
	}

	//if require max and diff is more than 3mins, set our request to diff and disable require max
	if (require_max && diff>3)
	{
		require_max = false;
		i = diff;
		
	}
/*	if (i <= 0 || i > 30 || map_added_time+i>30) {
		gi.cprintf(ent, PRINT_HIGH, "Invalid number or exceeds added time allowed.\n");
		return;
	}*/

	if ((ent->client->resp.num_votes>= gset_vars->max_votes) && (ent->client->resp.admin < aset_vars->ADMIN_NOMAXVOTES_LEVEL))   // _h2
	{
		gi.cprintf(ent,PRINT_HIGH,"You had %d elections fail and cannot call anymore.\n",gset_vars->max_votes);   // _h2
		return;
	}

	if ((ClientIsBanned(ent,BAN_MAPVOTE)) && (ent->client->resp.admin < aset_vars->ADMIN_ADDTIME_LEVEL))
	{
		gi.cprintf(ent,PRINT_HIGH,"You are not allowed to vote for time.\n");
		return;
	}


	if ((ent->client->resp.num_votes==gset_vars->max_votes) && (ent->client->resp.admin < aset_vars->ADMIN_NOMAXVOTES_LEVEL))
	{
		//Last of their 3 votes, ban them from voting for incase they try again.
		AddTempBan(ent,BAN_MAPVOTE | BAN_SILENCEVOTE | BAN_VOTETIME | BAN_BOOT);
	}

	sprintf(text, "%s has requested adding %i minutes extra time ", 
			ent->client->pers.netname, i);
	//require_max = true;
	if (CTFBeginElection(ent, ELECT_ADDTIME, text,require_max))
	{		
		num_time_votes++;
		gi.configstring (CONFIG_JUMP_VOTE_INITIATED,HighAscii(va("Vote by %s",ent->client->pers.netname)));
		gi.configstring (CONFIG_JUMP_VOTE_TYPE,va("Time: %d minutes",i));
		ctfgame.ekicknum = i;
		ctfgame.ekick = ent;
		if (ctfgame.needvotes==0)
			CTFWinElection(0, NULL);
	}
}


void showfps(edict_t *ent)
{
	int i;
	edict_t	*e2;
	float fps;
	int j;
	char txt[255];
	Com_sprintf(txt, sizeof(txt), "            cl_maxfps actual\n");
	gi.cprintf(ent, PRINT_HIGH, "%s\n", HighAscii(txt));
	for (i = 1; i <= maxclients->value; i++) 
	{
		e2 = g_edicts + i;
		if (!e2->inuse)
			continue;

		fps = 0;
		for (j=0;j<5;j++)
			fps += e2->client->resp.msec_history[j];
		if (fps)
		{
			fps = 5000 / fps ;
			gi.cprintf(ent,PRINT_HIGH,"%-16s %4d %6.1f\n",e2->client->pers.netname,e2->client->pers.fps,fps);
		}
	}


}

int get_admin_id(char *givenpass,char *givenname)
{
	int got_level = -1;
	int i;

	for (i=0;i<num_admins;i++)
	{
		if (!admin_pass[i].level)
			break;
		if ((strcmp(givenpass,admin_pass[i].password) == 0) && (strcmp(givenname,admin_pass[i].name) == 0))
		{
			got_level = i;
			break;
		}
	}
	return got_level;
}

void change_admin_pass(edict_t *ent)
{
	int aid;
	if (!ent->client->resp.admin)
	{
		gi.cprintf(ent,PRINT_HIGH,"You need to be logged in before changing password.\n");
		return;
	}

	if (gi.argc() < 4)
	{
		gi.cprintf(ent,PRINT_HIGH,"Format : changepass name password newpass\n");
		return;
	}

	if (strlen(gi.argv(3))<5)
	{
		gi.cprintf(ent,PRINT_HIGH,"Minimum password length is 5 letters.\n");
		return;

	}
	aid = get_admin_id(gi.argv(2),gi.argv(1));
	
	if (aid<0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Format : changepass name password newpass\n");
		return;
	}

	if (admin_pass[aid].level<=ent->client->resp.admin)
	{
		//only allow them to change the password of admins below or equal
		strcpy(admin_pass[aid].password,gi.argv(3));
		Write_Admin_cfg();
		Read_Admin_cfg();
		gi.cprintf(ent,PRINT_HIGH,"Admin %s password has been changed to %s.\n",admin_pass[aid].name,admin_pass[aid].password);
	}
	else
	{
		gi.cprintf(ent,PRINT_HIGH,"unable to change that users password.\n");
		return;
	}

}


/*void tower_think(edict_t *ent)
{
	ent->nextthink = level.time + 1 * FRAMETIME;
	ent->s.angles[1]+=5;
	if (ent->s.angles[1]>=360)
		ent->s.angles[1]=0;
	ent->s.angles[0]+=3;
	if (ent->s.angles[0]>20)
		ent->s.angles[0]=-20;
}

#define MAX_TOWER 64
void Create_Tower(edict_t *us)
{
	edict_t *ent;
	int i;
	int t = 0;
	int u = 0;
	vec3_t	mins = {-16, -16, -24};
	vec3_t	maxs = {16, 16, 32};
	int type,size;

	if (us->client->resp.admin<aset_vars->ADMIN_MAX_LEVEL)
	{
		//return;
	}
	if (gi.argc() < 3)
	{
		gi.cprintf(us,PRINT_HIGH,"Format : createtower type size. type 0 = walthru type 1 = solid\n");
		return;
	}
	size = atoi(gi.argv(2));
	type = atoi(gi.argv(1));
	for (i=0;i<size;i++)
	{
		ent = G_Spawn();
		VectorCopy(us->s.origin,ent->s.origin);
		VectorCopy(us->s.angles,ent->s.angles);
		ent->s.angles[0] = rand()&20;
		ent->s.angles[1] = (rand()&180);
		ent->s.origin[2]+=t;
		ent->s.origin[0]+=u;
		t+=56;
		u+=5;
		ent->svflags = SVF_PROJECTILE;
		ent->movetype = MOVETYPE_NONE;
		ent->clipmask = MASK_PLAYERSOLID;
		if (!type)
			ent->solid = SOLID_NOT;
		else
			ent->solid = SOLID_BBOX;
		VectorCopy (mins, ent->mins);
		VectorCopy (maxs, ent->maxs);
		ent->model = "players/male/tris.md2";
		ent->s.skinnum = 0;
		ent->s.modelindex = 255;		// will use the skin specified model
		ent->s.modelindex2 = 255;		// custom gun model
		ent->s.modelindex = gi.modelindex ("players/male/tris.md2");
		ent->s.frame = rand()&30;
		ent->dmg = 0;
		ent->classname = "tower";
		//ent->think = tower_think;
		//ent->nextthink = level.time + 2 * FRAMETIME;

		gi.linkentity (ent);
	}

}

*/
/* bad maps */

void Overide_Vote_Maps(edict_t *ent)
{
	int i;

	if (ent->client->resp.admin<aset_vars->ADMIN_NEXTMAPS_LEVEL)
		return;

	if (gi.argc() < 2)
	{
		gi.cprintf(ent,PRINT_HIGH,"format : nextmaps <1> <2> <3>\n");
		return;
	}
	if (gi.argc()>=4)
	{
		map1 = atoi(gi.argv(1));
		if (!map1)
		{
			for (i=0;i<maplist.nummaps;i++)
				if (strcmp(maplist.mapnames[i],gi.argv(1))==0)
				{
					map1 = i;
					break;
				}
		}
		else
			map1--;
		map2 = atoi(gi.argv(2));
		if (!map2)
		{
			for (i=0;i<maplist.nummaps;i++)
				if (strcmp(maplist.mapnames[i],gi.argv(2))==0)
				{
					map2 = i;
					break;
				}
		}
		else
			map2--;
		map3 = atoi(gi.argv(3));
		if (!map3)
		{
			for (i=0;i<maplist.nummaps;i++)
				if (strcmp(maplist.mapnames[i],gi.argv(3))==0)
				{
					map3 = i;
					break;
				}
		}
		else
			map3--;
	}
	else if (gi.argc()>=3)
	{
		map1 = atoi(gi.argv(1));
		if (!map1)
		{
			for (i=0;i<maplist.nummaps;i++)
				if (strcmp(maplist.mapnames[i],gi.argv(1))==0)
				{
					map1 = i;
					break;
				}
		}
		else
			map1--;
		map2 = atoi(gi.argv(2));
		if (!map2)
		{
			for (i=0;i<maplist.nummaps;i++)
				if (strcmp(maplist.mapnames[i],gi.argv(2))==0)
				{
					map2 = i;
					break;
				}
		}
		else
			map2--;
		map3 = map2;
	}
	else if (gi.argc()>=2)
	{
		map1 = atoi(gi.argv(1));
		if (!map1)
		{
			for (i=0;i<maplist.nummaps;i++)
				if (strcmp(maplist.mapnames[i],gi.argv(1))==0)
				{
					map1 = i;
					break;
				}
		}
		else
			map1--;
		map2 = map3 = map1;
	}

	gi.cprintf(ent,PRINT_HIGH,"1. %s 2. %s 3. %s chosen\n",maplist.mapnames[map1],maplist.mapnames[map2],maplist.mapnames[map3]);
	admin_overide_vote_maps = true;
	Update_Next_Maps();
}


void sort_maps(edict_t *ent)
{
	char t_name[128];
	float t_up;
	int i;
	int j;

	if (ent->client->resp.admin<aset_vars->ADMIN_SORTMAPS_LEVEL)
		return;

    for ( i = 0; i < maplist.nummaps-1; ++i )
      for ( j = 1; j < maplist.nummaps-i; ++j )
		if (strcmp(maplist.mapnames[j-1],maplist.mapnames[j])>0)
		{
			t_up = maplist.update[j-1];
			maplist.update[j-1] = maplist.update[j];
			maplist.update[j] = t_up;
			strcpy(t_name,maplist.mapnames[j-1]);
			strcpy(maplist.mapnames[j-1],maplist.mapnames[j]);
			strcpy(maplist.mapnames[j],t_name);
		}
		WriteMapList();
		write_map_file(level.mapname,level.mapnum);
		gi.cprintf(ent,PRINT_HIGH,"Maplist has been sorted\n");
		gi.AddCommandString("set sv_allow_map 1\n");
		gi.AddCommandString("map forkjumping\n");
		
}
/*

void open_uid_file(int uid,edict_t *ent)
{
	int index;
	char	buffer[128];
	FILE	*f;
	int i;
	cvar_t	*port;
	cvar_t	*tgame;
	char	name[256];

	if (ent)
	index = ent-g_edicts-1;
	else
		index = 0;

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

	if (!*tgame->string)
	{
		sprintf	(name, "jump/%s/%i.u", port->string,uid);
	}
	else
	{
		sprintf (name, "%s/%s/%i.u", tgame->string,port->string,uid);
	}
	overall_completions[index].loaded = true;
	for (i=0;i<MAX_MAPS;i++)
		overall_completions[index].maps[i] = 0;

	f = fopen (name, "rb");
	if (!f)
	{
		return;
	}
	while (!feof(f))
	{

		fscanf(f, "%s", &buffer);
		for (i=0;i<maplist.nummaps;i++)
		{
			if (strcmp(buffer,maplist.mapnames[i])==0)
			{
				//gi.dprintf("%s completed for UID %i\n", buffer, uid);
				overall_completions[index].maps[i] = 1;
			}		
		}
		//gi.dprintf("checking mapname %s for user UID: %i\n", buffer, uid);
		//buffer = mapname, see if it matches current
		
	}
	fclose(f);
	//for (j = 0; j < count; j++)
	//{
		
	//}
	//open_map_file(level.mapname, false);
}
*/

qboolean open_uid_file_compare(edict_t *ent)
{
	int index;
	char	buffer[128];
	FILE	*f;
	int i;
	cvar_t	*port;
	cvar_t	*tgame;
	char	name[256];
	
	if (ent)
		index = ent-g_edicts-1;
	else
		return false;
		

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);

	for (i=0;i<MAX_MAPS;i++)
		compare_users[index].user1.maps[i] = compare_users[index].user2.maps[i] = 0;

	if (!*tgame->string)
		sprintf	(name, "jump/%s/%i.u", port->string,compare_users[index].user1.uid);
	else
		sprintf (name, "%s/%s/%i.u", tgame->string,port->string,compare_users[index].user1.uid);

	f = fopen (name, "rb");
	if (!f)
		return false;
	while (!feof(f))
	{

		fscanf(f, "%s", &buffer);
		for (i=0;i<maplist.nummaps;i++)
		{
			if (strcmp(buffer,maplist.mapnames[i])==0)
			{
				compare_users[index].user1.maps[i] = 1;
			}		
		}
		
	}
	fclose(f);
	if (!*tgame->string)
		sprintf	(name, "jump/%s/%i.u", port->string,compare_users[index].user2.uid);
	else
		sprintf (name, "%s/%s/%i.u", tgame->string,port->string,compare_users[index].user2.uid);

	f = fopen (name, "rb");
	if (!f)
		return false;
	while (!feof(f))
	{

		fscanf(f, "%s", &buffer);
		for (i=0;i<maplist.nummaps;i++)
		{
			if (strcmp(buffer,maplist.mapnames[i])==0)
			{
				compare_users[index].user2.maps[i] = 1;
				
			}		
		}
		
	}
	fclose(f);
	return true;
}

void append_added_ini(char *mapname)
{
	char	buffer[128];
	FILE	*f;
	cvar_t	*port;
	cvar_t	*tgame;
	char	name[256];
	struct	tm *current_date;
	time_t	time_date;
	int		month,day,year;

	tgame = gi.cvar("game", "", 0);
	port = gi.cvar("port", "", 0);
	if (!*tgame->string)
	{
		sprintf	(name, "jump/%s/added.ini", port->string);
	}
	else
	{
		sprintf (name, "%s/%s/added.ini", tgame->string,port->string);
	}

	f = fopen (name, "a");

	//set the date
		time_date = time(NULL);                // DOS system call
		current_date = localtime(&time_date);  // DOS system call
		month = current_date->tm_mon + 1;
		day = current_date->tm_mday;
		year = current_date->tm_year;
	sprintf(buffer, "%02d/%02d/%02d",day, month,year-100);

	fprintf(f,"%s\n%s\n\n",buffer,mapname);
	fclose(f);
}

// By LilRedTheJumper
qboolean ValidateMap (char *mapname)
{
	FILE* f;
	cvar_t* tgame;
	char* mapn;
	
	tgame = gi.cvar ("game", "", 0);
	mapn = va ("%s/maps/%s.bsp", tgame->string, mapname); // get full path for the map if
	// it exists in the mod/maps folder
	f = fopen (mapn, "r");
	if (f) // test if the map exists in the mod/maps folder
	{
		fclose (f);
		return true;
	}

	mapn = va ("baseq2/maps/%s.bsp", mapname); // get the path to the baseq2/maps folder with
	// the mapname
	f = fopen (mapn, "r");
	if (f) // test if it exists in the baseq2/maps folder
	{
		fclose (f);
		return true;
	}
	
	return false; // it doesnt exist in the mod/maps folder or baseq2/maps folder, thus
	// quake2 can't reach it, so return false
}

ind_map_t individual_map[MAX_USERS];
int num_map_recs;

qboolean HTML_ReadTemplate(char *filename)
{
	FILE *tfile;
	tfile = fopen (filename,"r");
	if (tfile)
	{
		fseek (tfile , 0 , SEEK_END);		
		html_data.len = ftell(tfile);
		rewind(tfile);
		memset(html_data.tplate,0,sizeof(html_data.tplate));
		fread(html_data.tplate,1,html_data.len,tfile);
		fclose(tfile);
		return true;
	}
	html_data.len = 0;
	return false;
}

void HTML_WriteTemplate(void)
{
	fwrite(html_data.tplate,1,html_data.len,html_data.file);
}

void HTML_WriteBuffer(void)
{
	fwrite(html_data.buffer,1,html_data.len,html_data.file);
}


void HTML_Replace(char *string,char *search, char *insert)
{
	char *replace;
	int i;	
	replace = strstr(string,search);
	if (!replace)
		return;

	while (replace)
	{
	for (i=0;i<strlen(insert);i++)
	{
		replace[i] = insert[i];
	}
	//fill out with spaces
	for (i=strlen(insert);i<strlen(search);i++)
	{
		replace[i] = 32;
	}
	replace = strstr(string,search);
	}

}

void sort_ind_map_recs( int n )
{
	int trec;
	float time;

	int i;
	int j;
  for ( i = 0; i < n-1; ++i )
    for ( j = 1; j < n-i; ++j )
      if (individual_map[j-1].time >= individual_map[j].time
		   )
	  {
		trec = individual_map[j-1].maplist_uid;
		time = individual_map[j-1].time;


        individual_map[j-1].maplist_uid = individual_map[j].maplist_uid;
        individual_map[j-1].time = individual_map[j].time;

        individual_map[j].maplist_uid = trec;
        individual_map[j].time = time;
	  }
}


void CreateHTML(edict_t *ent,int type,int usenum)
{
	//urm so what we doing then?
	//dumdedum, brain get working plz

	//1. 1st places set today html
	//2. top completions/highscores thingy
	//3. list of all players and their 1st/2nd etc places
	//4. list of all players completion %
	//5. list of all maps

	int i;
	int i2;
	cvar_t* tgame;
	char buffer[256];
	char datebuf[256];
	struct	tm *current_date;
	time_t	time_date;
	int		month,day,year;


	tgame = gi.cvar ("game", "", 0);

	switch (type) {
	case HTML_PLAYERS_SCORES :
		//go thru all players in sorted order
		html_data.file = fopen(va("%s/_html/players.html",tgame->string),"wb");
		if (!html_data.file)
			return;
		if (HTML_ReadTemplate(va("%s/_html/_player_scores_top%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();

		if (!HTML_ReadTemplate(va("%s/_html/_player_scores_template%i.html",tgame->string,gset_vars->html_profile)))
			return;

		for (i=0;i<maplist.sort_num_users;i++)
		{
			if (!maplist.sorted_users[i].score)
				break;
			strcpy(html_data.buffer,html_data.tplate);
		
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION,va("%i",i+1));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_NAME,maplist.users[maplist.sorted_users[i].uid].name);
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID,va("%i",maplist.sorted_users[i].uid));
//			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID_HTML,va("%s.html",maplist.users[maplist.sorted_users[i].uid].name));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID_HTML,va("%i.html",maplist.sorted_users[i].uid));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION1,va("%i",maplist.users[maplist.sorted_users[i].uid].points[0]));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION2,va("%i",maplist.users[maplist.sorted_users[i].uid].points[1]));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION3,va("%i",maplist.users[maplist.sorted_users[i].uid].points[2]));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION4,va("%i",maplist.users[maplist.sorted_users[i].uid].points[3]));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION5,va("%i",maplist.users[maplist.sorted_users[i].uid].points[4]));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_SCORE,va("%i",maplist.users[maplist.sorted_users[i].uid].score));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_COMPLETION,va("%i",0));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_TOTAL,va("%i",maplist.users[maplist.sorted_users[i].uid].score));

			HTML_WriteBuffer();
		}

		if (HTML_ReadTemplate(va("%s/_html/_player_scores_bottom%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();
		break;
	case HTML_PLAYERS_PERCENTAGE :
		//go thru all players in sorted order
		html_data.file = fopen(va("%s/_html/player_completions.html",tgame->string),"wb");
		if (!html_data.file)
			return;
		if (HTML_ReadTemplate(va("%s/_html/_player_completions_top%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();

		if (!HTML_ReadTemplate(va("%s/_html/_player_completions_template%i.html",tgame->string,gset_vars->html_profile)))
			return;

		
		for (i=0;i<maplist.sort_num_users;i++)
		{
			if (!maplist.sorted_completions[i].score)
				break;
			strcpy(html_data.buffer,html_data.tplate);
		
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION,va("%i",i+1));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_NAME,maplist.users[maplist.sorted_completions[i].uid].name);
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID,va("%i",maplist.sorted_completions[i].uid));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID_HTML,va("%i.html",maplist.sorted_completions[i].uid));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_COMPLETION,va("%i",maplist.users[maplist.sorted_completions[i].uid].completions));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_PERCENTAGE,va("%2.1f %%",(double)maplist.users[maplist.sorted_completions[i].uid].completions/(double)maplist.nummaps*100));

			HTML_WriteBuffer();
		}

		if (HTML_ReadTemplate(va("%s/_html/_player_completions_bottom%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();
		break;
	case HTML_MAPS :
		html_data.file = fopen(va("%s/_html/maps.html",tgame->string),"wb");
		if (!html_data.file)
			return;

		if (HTML_ReadTemplate(va("%s/_html/_maps_top%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();

		if (!HTML_ReadTemplate(va("%s/_html/_maps_template%i.html",tgame->string,gset_vars->html_profile)))
			return;

		
		for (i=0;i<maplist.nummaps;i++)
		{
			strcpy(html_data.buffer,html_data.tplate);
		
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION,va("%i",i+1));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_MAPNAME,maplist.mapnames[i]);
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_MAPNAME_HTML,va("%s.html",maplist.mapnames[i]));
			if (maplist.times[i][0].time)
			{
				HTML_Replace(html_data.buffer,HTML_TEMPLATE_MAPBESTTIME,va("%8.3f",maplist.times[i][0].time));
				HTML_Replace(html_data.buffer,HTML_TEMPLATE_NAME,maplist.users[maplist.times[i][0].uid].name);
			}
			else
			{
				HTML_Replace(html_data.buffer,HTML_TEMPLATE_MAPBESTTIME," ");
				HTML_Replace(html_data.buffer,HTML_TEMPLATE_NAME," ");
			}
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_MAPPLAYED,va("%i",maplist.update[i]));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID,va("%i",maplist.times[i][0].uid));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID_HTML,va("%i.html",maplist.times[i][0].uid));

			HTML_WriteBuffer();
		}

		if (HTML_ReadTemplate(va("%s/_html/_maps_bottom%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();
		break;
	case HTML_FIRST :
		//get current date and go thru 1st places, output all that match
		if (!gset_vars->html_bestscores)
			return;
		html_data.file = fopen(va("%s/_html/firstplaces.html",tgame->string),"wb");
		if (!html_data.file)
			return;

		if (HTML_ReadTemplate(va("%s/_html/_first_places_top%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();

		if (!HTML_ReadTemplate(va("%s/_html/_first_places_template%i.html",tgame->string,gset_vars->html_profile)))
			return;
		//add a gset to specify 

		strcpy(html_data.buffer,html_data.tplate);
	//set the date
		time_date = time(NULL);                // DOS system call
		current_date = localtime(&time_date);  // DOS system call
		month = current_date->tm_mon + 1;
		day = current_date->tm_mday;
		year = current_date->tm_year;
		sprintf(datebuf, "%02d/%02d/%02d",day, month,year-100);
		i2 = 0;
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_DATE,datebuf);
		for (i=0;i<maplist.nummaps;i++)
		{
			if (maplist.times[i][0].time)
			{
				if (strcmp(maplist.times[i][0].date,datebuf)==0)
				{
					sprintf(buffer,"%s%02i",HTML_TEMPLATE_MAPNAME_HTML,i2+1);
					HTML_Replace(html_data.buffer,buffer,va("%s.html",maplist.mapnames[i]));
					sprintf(buffer,"%s%02i",HTML_TEMPLATE_MAPNAME,i2+1);
					HTML_Replace(html_data.buffer,buffer,va("%s",maplist.mapnames[i]));

					sprintf(buffer,"%s%02i",HTML_TEMPLATE_NAME,i2+1);
					HTML_Replace(html_data.buffer,buffer,va("%s",maplist.users[maplist.times[i][0].uid].name));
					sprintf(buffer,"%s%02i",HTML_TEMPLATE_TIME,i2+1);
					HTML_Replace(html_data.buffer,buffer,va("%8.3f",maplist.times[i][0].time));
					i2++;
					if (i2>=gset_vars->html_firstplaces)
						break;

				}
			}
		}
		if (i2<gset_vars->html_firstplaces)
		{
			for (i=i2;i<gset_vars->html_firstplaces;i++)
			{
			//finish off
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_MAPNAME_HTML,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_MAPNAME,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_NAME,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_TIME,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			}
		}

		HTML_WriteBuffer();
		if (HTML_ReadTemplate(va("%s/_html/_first_places_bottom%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();
		break;

		break;
	case HTML_INDIVIDUALS :
		//too complicated with such shit code to get times added
		html_data.file = fopen(va("%s/_html/%i.html",tgame->string,usenum),"wb");
		if (!html_data.file)
			return;

		if (HTML_ReadTemplate(va("%s/_html/_player_scores_top%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();

		if (!HTML_ReadTemplate(va("%s/_html/_player_scores_template%i.html",tgame->string,gset_vars->html_profile)))
			return;

		strcpy(html_data.buffer,html_data.tplate);
		
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION," ");
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_NAME,maplist.users[usenum].name);
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID,va("%i",usenum));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID_HTML,va("%i.html",usenum));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION1,va("%i",maplist.users[usenum].points[0]));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION2,va("%i",maplist.users[usenum].points[1]));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION3,va("%i",maplist.users[usenum].points[2]));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION4,va("%i",maplist.users[usenum].points[3]));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION5,va("%i",maplist.users[usenum].points[4]));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_SCORE,va("%i",maplist.users[usenum].score));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_COMPLETION,va("%i",0));
		HTML_Replace(html_data.buffer,HTML_TEMPLATE_TOTAL,va("%i",maplist.users[usenum].score));

		HTML_WriteBuffer();
		
		if (HTML_ReadTemplate(va("%s/_html/_individual_user_top%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();

		if (!HTML_ReadTemplate(va("%s/_html/_individual_user_template%i.html",tgame->string,gset_vars->html_profile)))
			return;

		//load uid file
		//open_uid_file(usenum,NULL);
		i2 = 1;
		for (i=0;i<MAX_MAPS;i++)
		{
			if (maplist.users[usenum].maps_done[i])
			{
				strcpy(html_data.buffer,html_data.tplate);
		
				HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION,va("%i",i2));
				HTML_Replace(html_data.buffer,HTML_TEMPLATE_MAPNAME,va("%s",maplist.mapnames[i]));
				HTML_Replace(html_data.buffer,HTML_TEMPLATE_MAPNAME_HTML,va("%s.html",maplist.mapnames[i]));
				
				HTML_WriteBuffer();
				i2++;
			}
		}

		if (HTML_ReadTemplate(va("%s/_html/_individual_user_bottom%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();

		break;
	case HTML_INDIVIDUAL_MAP:
		num_map_recs = 0;
		for (i=0;i<MAX_USERS;i++)
		{
			if (maplist.times[level.mapnum][i].time)
			{
				//add to our list
				individual_map[num_map_recs].maplist_uid = i;
				individual_map[num_map_recs].time = maplist.times[level.mapnum][i].time;

				num_map_recs++;
			}
		}
		if (!num_map_recs)
			return;
		//sort ind map recs
		sort_ind_map_recs(num_map_recs);
		//write it
		html_data.file = fopen(va("%s/_html/%s.html",tgame->string,maplist.mapnames[usenum]),"wb");
		if (!html_data.file)
			return;
		if (HTML_ReadTemplate(va("%s/_html/_individual_map_top%i.html",tgame->string,gset_vars->html_profile)))
		{
			strcpy(html_data.buffer,html_data.tplate);
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_MAPNAME,maplist.mapnames[usenum]);
			HTML_WriteBuffer();
		}

		if (!HTML_ReadTemplate(va("%s/_html/_individual_map_template%i.html",tgame->string,gset_vars->html_profile)))
			return;

		for	(i=0;i<num_map_recs;i++)
		{
			strcpy(html_data.buffer,html_data.tplate);
		
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_POSITION,va("%i",i+1));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_NAME,maplist.users[maplist.times[level.mapnum][individual_map[i].maplist_uid].uid].name);
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_DATE,maplist.times[level.mapnum][individual_map[i].maplist_uid].date);
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID,va("%i",maplist.times[level.mapnum][individual_map[i].maplist_uid].uid));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_UID_HTML,va("%i.html",maplist.times[level.mapnum][individual_map[i].maplist_uid].uid));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_TIME,va("%8.3f",maplist.times[level.mapnum][individual_map[i].maplist_uid].time));
			HTML_Replace(html_data.buffer,HTML_TEMPLATE_COMPLETION,va("%i",maplist.times[level.mapnum][individual_map[i].maplist_uid].completions));

			HTML_WriteBuffer();
		}

		if (HTML_ReadTemplate(va("%s/_html/_individual_map_bottom%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();
		break;
	case HTML_BESTSCORES :
		if (!gset_vars->html_bestscores)
			return;
		html_data.file = fopen(va("%s/_html/bestscores.html",tgame->string),"wb");
		if (!html_data.file)
			return;

		if (HTML_ReadTemplate(va("%s/_html/_bestscores_top%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();

		if (!HTML_ReadTemplate(va("%s/_html/_bestscores_template%i.html",tgame->string,gset_vars->html_profile)))
			return;
		//add a gset to specify 

		strcpy(html_data.buffer,html_data.tplate);
		for (i=0;i<gset_vars->html_bestscores;i++)
		{		
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_UID_HTML,i+1);
			HTML_Replace(html_data.buffer,buffer,va("%i.html",maplist.sorted_completions[i].uid));
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_NAME_COMP,i+1);
			HTML_Replace(html_data.buffer,buffer,va("%s",maplist.users[maplist.sorted_completions[i].uid].name));
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_PERCENTAGE,i+1);
			if (maplist.sorted_completions[i].score)
			{
				HTML_Replace(html_data.buffer,buffer,va("%2.1f %%",(double)maplist.sorted_completions[i].score/(double)maplist.nummaps*100));
			}
			else
			{
				HTML_Replace(html_data.buffer,buffer,"0");
			}
		}
		for (i=gset_vars->html_bestscores-1;i<50;i++)
		{		
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_UID_HTML,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_NAME_COMP,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_PERCENTAGE,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			
		}

		for (i=0;i<gset_vars->html_bestscores;i++)
		{		
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_UID2_HTML,i+1);
			HTML_Replace(html_data.buffer,buffer,va("%i.html",maplist.sorted_users[i].uid));
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_NAME,i+1);
			HTML_Replace(html_data.buffer,buffer,va("%s",maplist.users[maplist.sorted_users[i].uid].name));
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_POSITION,i+1);
			HTML_Replace(html_data.buffer,buffer,va("%i.",i+1));
		}
		for (i=gset_vars->html_bestscores-1;i<50;i++)
		{		
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_UID2_HTML,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_NAME,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			sprintf(buffer,"%s%02i",HTML_TEMPLATE_POSITION,i+1);
			HTML_Replace(html_data.buffer,buffer," ");
			
		}
		HTML_WriteBuffer();
		if (HTML_ReadTemplate(va("%s/_html/_bestscores_bottom%i.html",tgame->string,gset_vars->html_profile)))
			HTML_WriteTemplate();
		break;
	};	

	if (html_data.file)
		fclose(html_data.file);
}

void Cmd_Idle(edict_t *ent) {
	if (ent->client->pers.idle_player_state != PLAYERIDLE_STATE_SELF) {
		gi.cprintf(ent, PRINT_HIGH, "You are now marked as idle!\n");
		ent->client->pers.idle_player_state = PLAYERIDLE_STATE_SELF;
	}
	else {
		gi.cprintf(ent, PRINT_HIGH, "You are no longer idle! Welcome back.\n");
		ent->client->pers.idle_player_state = PLAYERIDLE_STATE_NONE;
	}

}

void Cmd_Race (edict_t *ent)
{
	float delay = 0;
	int i;
    int race_this = 0;
#ifndef RACESPARK
	gi.cprintf(ent,PRINT_HIGH,"Replay racing not available.\n");
	return;
#else
	if (!gset_vars->allow_race_spark)
	{
		gi.cprintf(ent,PRINT_HIGH,"The replay racing spark is disabled\n");
		ent->client->resp.rep_racing = false;
		return;
	}

	if (gi.argc()>=2) // multiple args
	{
		if (!strcmp(gi.argv(1),"now"))
			race_this = MAX_HIGHSCORES;
		else if (!strcmp(gi.argv(1), "off")) { // turn racing off
			char txt[255];
			Com_sprintf(txt, sizeof(txt), "No longer racing.\n");
			gi.cprintf(ent, PRINT_HIGH, "%s\n", HighAscii(txt));
			ent->client->resp.rep_racing = false;
			hud_footer(ent);
			return;
		}
		else if (!strcmp(gi.argv(1),"delay")) { // add a delay to the race
			if (gi.argc()<3) {
				gi.cprintf(ent,PRINT_HIGH,"Enter a value from 0.0 to 10.0 seconds for the delay. For example: race delay 0.5\n");
				return;
			}
			else {
				delay = atof(gi.argv(2));
				if (delay<0)
					delay = 0;
				if (delay>10)
					delay = 10;
				ent->client->resp.rep_racing_delay = delay;
				gi.cprintf(ent,PRINT_HIGH,"Race delay is %1.1f\n",delay);
				return;
			}
		}
		else {
			race_this = atoi(gi.argv(1));
			race_this--;
		}
	}

	// the race number provided is higher than max replays on the map
	if (race_this<0 || race_this>MAX_HIGHSCORES)
		race_this = 0;

	if (!level_items.recorded_time_frames[race_this]) {
		ent->client->resp.rep_racing = false;
		gi.cprintf(ent,PRINT_HIGH,"There is no demo to race. Choose one from below:\n");
		char txt[255];
		Com_sprintf(txt, sizeof(txt), "\nNo. Player             Time\n");
		gi.cprintf(ent, PRINT_HIGH, "%s\n", HighAscii(txt));
		for (i=0;i<MAX_HIGHSCORES;i++) {
			if (level_items.recorded_time_frames[i])
				gi.cprintf(ent,PRINT_HIGH,"%2d. %-16s %8.3f\n",i+1,maplist.users[maplist.times[level.mapnum][i].uid].name,maplist.times[level.mapnum][i].time);
		}
		return;
	}

	// race number is usable
	ent->client->resp.rep_racing = true;
	ent->client->resp.rep_race_number = race_this;
	hud_footer(ent);

	char txt[255];
	if (race_this==MAX_HIGHSCORES) // replay now, from above
		Com_sprintf(txt, sizeof(txt), "Now racing replay 1: %s\n", maplist.users[maplist.times[level.mapnum][0].uid].name);
	else
		Com_sprintf(txt, sizeof(txt), "Now racing replay %d: %s\n", (int)(race_this+1), maplist.users[maplist.times[level.mapnum][race_this].uid].name);

	// player gave no further arguments, tell them what they could do next time
	if (gi.argc() == 1)
		Com_sprintf(txt, sizeof(txt), "Other options: race delay <num>, race off, race now, race <demonumber>\n");



#endif
}

void KillMyRox(edict_t *ent)
{
	edict_t		*rox;

	if (!ent->client->resp.shotproj)
		return;

	rox = g_edicts;
	while (rox = G_Find(rox,FOFS(classname),"rocket"))
	{
		if (rox->owner == ent)
			G_FreeEdict(rox);
	}

	rox = g_edicts;
	while (rox = G_Find(rox,FOFS(classname),"grenade"))
	{
		if (rox->owner == ent)
			G_FreeEdict(rox);
	}

	rox = g_edicts;
	while (rox = G_Find(rox,FOFS(classname),"hgrenade"))
	{
		if (rox->owner == ent)
			G_FreeEdict(rox);
	}

	rox = g_edicts;
	while (rox = G_Find(rox,FOFS(classname),"bfg blast"))
	{
		if (rox->owner == ent)
			G_FreeEdict(rox);
	}

	ent->client->resp.shotproj = false;
}


void Cmd_Whois(edict_t *ent)
{
	int i;
	edict_t *targ;

	if (ent->client->resp.admin<aset_vars->ADMIN_IP_LEVEL)
	{
		return;
	}

	if (gi.argc() < 2) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Format: whois [player number]\n");
		return;
	}

	if (*gi.argv(1) < '0' && *gi.argv(1) > '9') {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Format: whois [player number]\n");
		return;
	}

	i = atoi(gi.argv(1));
	if (i < 1 || i > maxclients->value) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "Invalid player number.\n");
		return;
	}

	targ = g_edicts + i;
	if (!targ->inuse) {
		CTFPlayerList(ent);
		gi.cprintf(ent, PRINT_HIGH, "That player number is not connected.\n");
		return;
	}

	gi.cprintf(ent,PRINT_HIGH,"%d = %s at %s\n",i,targ->client->pers.netname,targ->client->pers.userip);
}

void Cmd_DummyVote(edict_t *ent)
{
	char text[1024];
	
	if (ent->client->resp.admin<aset_vars->ADMIN_DUMMYVOTE_LEVEL)
	{
		return;
	}

	if (gi.argc() < 2)
	{
		gi.cprintf(ent,PRINT_HIGH,"Provide a message for the vote.\n");
		return;
	}

	sprintf(text, "%s %s", 
			ent->client->pers.netname, gi.args());
	if (CTFBeginElection(ent, ELECT_DUMMY, text,false))
	{		
		gi.configstring (CONFIG_JUMP_VOTE_INITIATED,HighAscii(va("Vote by %s",ent->client->pers.netname)));
		gi.configstring (CONFIG_JUMP_VOTE_TYPE,va("%s",gi.args()));
//		ctfgame.ekicknum = i;
		ctfgame.ekick = ent;
		if (ctfgame.needvotes==0)
			CTFWinElection(0, NULL);
	}
}

void Cmd_IneyeToggle(edict_t *ent)
{
	ent->client->resp.chase_ineye = !ent->client->resp.chase_ineye;
	if (!ent->client->resp.chase_ineye)
		gi.cprintf(ent,PRINT_HIGH,"Entering 3rd person chasecam.\n");
	else
		gi.cprintf(ent,PRINT_HIGH,"Entering in-eye chasecam.\n");
}

qboolean CheckIPMatch(char *ipmask,char *ip)
{
	int i;
	int maskparts[4];
	int ipparts[4];
	sscanf(ipmask,"%d.%d.%d.%d",&maskparts[0],&maskparts[1],&maskparts[2],&maskparts[3]);
	sscanf(ip,"%d.%d.%d.%d",&ipparts[0],&ipparts[1],&ipparts[2],&ipparts[3]);

	for (i=0;i<4;i++)
	{
		if (maskparts[i] != 256)
			if (maskparts[i] != ipparts[i])
				return false;
	}
	return true;
}


void LoadBans()
{
	FILE	*f;
	cvar_t	*tgame;
	char name[256];
	char temp[256];
	int i;

	tgame = gi.cvar("game", "", 0);

	sprintf (name, "%s/jumpbans.ini", tgame->string);

	f = fopen (name, "rb");
	if (!f)
	{
		return;
	}	


	fscanf(f,"%s",&temp);
	if (!strstr(temp,"Bans"))
	{
		//Invalid bans file
        fclose(f);
		return;
	} 
	
	i = 0;
	while ((!feof(f)) && (i<MAX_BANS))
	{
		fscanf(f,"%s %i %lu %ld",&bans[i].idstring,&bans[i].ipban,&bans[i].banflags,&bans[i].expiry);
		bans[i].inuse = true;
		i++;
	}
	bans[i - 1].inuse = false;
	fclose(f);
	ExpireBans();
}


void WriteBans()
{
	FILE	*f;
	cvar_t	*tgame;
	char name[256];
	int i;

	tgame = gi.cvar("game", "", 0);

	sprintf (name, "%s/jumpbans.ini", tgame->string);

	f = fopen (name, "wb");
	if (!f)
	{
		return;
	}

	fprintf(f,"Bans\n");

	for (i = 0;i < MAX_BANS;i++)
	{
		if (bans[i].inuse)
			fprintf(f,"%s %i %lu %ld \n",bans[i].idstring,bans[i].ipban,bans[i].banflags,bans[i].expiry);

	}

	fclose(f);
}

void ListBans(edict_t *ent)
{
	int i;
	int page;
	char *time_str;

	if (ent->client->resp.admin<aset_vars->ADMIN_BAN_LEVEL)
	{
		return;
	}

	page = atoi(gi.argv(1));
	if (!page) page = 1;


	gi.cprintf(ent,PRINT_HIGH," ID |       IP/Name       |        Expiry time        | Ban flags\n");
	gi.cprintf(ent,PRINT_HIGH,"------------------------------------------------------------------\n");
	for (i=(page - 1) * 10;i < MAX_BANS;i++)
	{
		if (i > page * 10)
			break;
		if (!bans[i].inuse)
			continue;
		time_str = ctime(&bans[i].expiry);
		time_str[strlen(time_str) - 1] = '\0';
		gi.cprintf(ent,PRINT_HIGH," %-2d | %-19s | %-25s | %lu \n",i,bans[i].idstring,bans[i].expiry ? time_str : "None",bans[i].banflags);
	}
	gi.cprintf(ent,PRINT_HIGH,"------------------------------------------------------------------\n");
	gi.cprintf(ent,PRINT_HIGH,"Displaying page %i\n",page);
}

void AddBan(edict_t *ent)
{
	int i;
	long hours;
	qboolean ipban;
	char *time_str;

	i=0;
	while(bans[i].inuse)
		i++;

	if (ent->client->resp.admin<aset_vars->ADMIN_BAN_LEVEL)
	{
		return;
	}

	if (gi.argc() < 5)
	{
		gi.cprintf(ent,PRINT_HIGH,"Use: addban [IP/NAME] [ip address or name] [length of ban in hours, use 0 for a 1 map ban and -1 for no expiry] [banflags, type banflags for more info on this]\n");
		return;
	}


	if (Q_stricmp(gi.argv(1),"IP") == 0)
		ipban = true;
	else if (Q_stricmp(gi.argv(1),"NAME") == 0)
		ipban = false;
	else
	{
		gi.cprintf(ent,PRINT_HIGH,"Use: addban [IP/NAME] [ip address or name] [length of ban in hours, use 0 for a 1 map ban and -1 for no expiry] [banflags, type banflags for more info on this]\n");
		return;
	}
	
	sscanf(gi.argv(3),"%ld",&hours);
	if (hours == (-1))
		bans[i].expiry = 0;
	else
		bans[i].expiry = time(NULL) + (hours * 3600);


	strcpy(bans[i].idstring,gi.argv(2));
	sscanf(gi.argv(4),"%lu",&bans[i].banflags);
	bans[i].ipban = ipban;
	bans[i].inuse = true;

	time_str = ctime(&bans[i].expiry);
	time_str[strlen(time_str) - 1] = '\0';

	gi.cprintf(ent,PRINT_HIGH," ID |       IP/Name       |        Expiry time        | Ban flags\n");
	gi.cprintf(ent,PRINT_HIGH,"------------------------------------------------------------------\n");
	gi.cprintf(ent,PRINT_HIGH," %-2d | %-19s | %-25s | %lu \n",i,bans[i].idstring,bans[i].expiry ? time_str : "None",bans[i].banflags);


	WriteBans();
}

void RemBan(edict_t *ent)
{
	if (ent->client->resp.admin<aset_vars->ADMIN_BAN_LEVEL)
	{
		return;
	}

	if (gi.argc()<2)
	{
		ListBans(ent);
		gi.cprintf(ent,PRINT_HIGH,"Use: remban [ban id]\n");
		return;
	}

	bans[atoi(gi.argv(1))].inuse = false;

	WriteBans();

}

void BanFlags(edict_t *ent)
{
	unsigned long addedflags;
	unsigned long thisflag;
	int i;
	if (ent->client->resp.admin<aset_vars->ADMIN_BAN_LEVEL)
	{
		return;
	}

	if (gi.argc()<2)
	{
		gi.cprintf(ent,PRINT_HIGH,"1    Client cannot connect at all.\n");
		gi.cprintf(ent,PRINT_HIGH,"2    Client is silenced on entry.\n");
		gi.cprintf(ent,PRINT_HIGH,"4    Client cannot propose map votes.\n");
		gi.cprintf(ent,PRINT_HIGH,"8    Client cannot vote for time.\n");
		gi.cprintf(ent,PRINT_HIGH,"16   Client cannot vote to boot other players.\n");
		gi.cprintf(ent,PRINT_HIGH,"32   Client cannot vote to silence other players.\n");
		gi.cprintf(ent,PRINT_HIGH,"128  Client will not automatically receive temporary admin.\n");
		gi.cprintf(ent,PRINT_HIGH,"256  Client cannot move.\n");
		gi.cprintf(ent,PRINT_HIGH,"512  Client can spectate but not play.\n");
		gi.cprintf(ent,PRINT_HIGH,"Type banflags followed by the number corresponding to the ban types wanted.\neg. banflags 4 8 16 32 to disallow all votes.\n");
		return;
	}
	
	addedflags = 0;
	for (i = 1;i < gi.argc();i++)
	{
		sscanf(gi.argv(i),"%lu",&thisflag);
		addedflags += thisflag;
	}

	gi.cprintf(ent,PRINT_HIGH,"Use %lu as banflags with 'addban' command.\n",addedflags);
}

void ExpireBans()
{
	int i;
	long now;
	qboolean changedbans;
	changedbans = false;
	now = time(NULL);
	for (i = 0;i < MAX_BANS;i++)
	{
		if (!bans[i].inuse)
			continue;
		if (!bans[i].expiry)
			continue;
		if (bans[i].expiry <= now)
		{
			bans[i].inuse = false;
			changedbans = true;
		}
	}
	if (changedbans) 
		WriteBans();
}

unsigned long GetBanLevel(edict_t *targ,char *userinfo)
{
	int i;
	unsigned long banlevel;
	char *name;
	name = Info_ValueForKey (userinfo, "name");
	banlevel = 0;
	for (i=0;i < MAX_BANS;i++)
	{
		if (!bans[i].inuse)
			continue;
		if (bans[i].ipban)
		{
			if (CheckIPMatch(bans[i].idstring,targ->client->pers.userip))
				banlevel |= bans[i].banflags;
		}
		else
		{
			if (Q_stricmp(bans[i].idstring,name) == 0)
				banlevel |= bans[i].banflags;
		}
	}
	return banlevel;
}

qboolean ClientIsBanned(edict_t *ent,unsigned long bancheck)
{
	if (ent->client->pers.banlevel & bancheck)
		return true;
	else
		return false;
}

void AddTempBan(edict_t *ent,unsigned long bantype)
{
	int i;

	i=0;
	while(bans[i].inuse)
		i++;
	
	ent->client->pers.banlevel |= bantype;
	bans[i].banflags = bantype;
	bans[i].expiry = time(NULL);
	strcpy(bans[i].idstring,ent->client->pers.userip);
	bans[i].ipban = true;
	bans[i].inuse = true;

	WriteBans();
}

void ApplyBans(edict_t *ent,char *s)
{
	char temp[72];
/*
#define BAN_CONNECTION 1		//Client cannot connect at all
#define BAN_SILENCE 2			//Client is silenced on entry
#define BAN_MAPVOTE 4			//Client cannot propose map votes
#define BAN_VOTETIME 8			//Client cannot vote for time
#define BAN_BOOT 16				//Client cannot vote to boot other players
#define BAN_SILENCEVOTE 32		//Client cannot vote to silence other players
#define BAN_TEMPADMIN 128		//Client will not automatically receive temporary admin when 5 minutes of a game is remaining
#define BAN_MOVE 256			//Client cannot move :D
#define BAN_PLAY 512			//Client can spectate but not play
#define BAN_KICK_BAN 1024		//Same as BAN_CONNECTION but player is told they will be allowed back next map
*/
	if (ClientIsBanned(ent,BAN_CONNECTION))
	{
		gi.cprintf(ent,PRINT_HIGH,"You are banned.\n");
		sprintf(temp,"kick %d\n",ctfgame.ekicknum);
		gi.AddCommandString(temp);
	}
	if ((ClientIsBanned(ent,BAN_SILENCE)) && (ent->client->resp.silence == false))
	{
		gi.bprintf(PRINT_HIGH,"%s was silenced.\n",s);
		ent->client->resp.silence = true;
		ent->client->resp.silence_until = 0;
	}
}

/*
void reset_maps_completed(edict_t *ent)
{
	int index,i;
	int prev_uid;
	int maps_completed;
	int user;
	index = ent-g_edicts-1;

	//store our uid
	prev_uid = (ent->client->resp.uid-1);

	//write off current times
	write_map_file(level.mapname,level.mapnum);   // 084_h3

	//loop thru users
	for (user=0;user<maplist.num_users;user++)
	{
		//reset map completed
		maps_completed = 0;
		//open this users file
		open_uid_file(user,ent);
		//loop thru maps
		for (i=0;i<MAX_MAPS;i++)
		{
			//map completed
			if (overall_completions[index].maps[i]==1)
				maps_completed++;
		}
		maplist.users[user].completions = maps_completed;
	}

	if (prev_uid>=0)
		open_uid_file(prev_uid,ent);

}
*/

void Update_Added_Time(void)
{
	char temp[32],str_added_time[32];
	int i;
	i = map_added_time;
	if (i<-99)
		i = -99;
	else if (i>999)
		i = 999;
	if (i<0)
		Com_sprintf(temp,sizeof(temp),"%d",i);
	else
		Com_sprintf(temp,sizeof(temp),"+%d",i);
	Com_sprintf(str_added_time,sizeof(str_added_time),"%4s",temp);
	if (strlen(str_added_time)>4)
		str_added_time[4] = 0;
	gi.configstring (CONFIG_JUMP_ADDED_TIME,      str_added_time);

}

void UpdateVoteMenu(void)
{
	if (ctfgame.election != ELECT_NONE)
	{
		if (ctfgame.electtime > level.time)
		{
			gi.configstring (CONFIG_JUMP_VOTE_REMAINING,va("%d seconds",(int)(ctfgame.electtime-level.time)));
			gi.configstring (CONFIG_JUMP_VOTE_CAST,va("Votes: %d of %d",ctfgame.evotes,ctfgame.needvotes));
			return;
		}
	}
}

static qboolean showhud = true;
void ToggleHud(edict_t *ent)
{
	char this_map[64];
	char s[64];
	char str[2048];
	int i;
	if (ent->client->resp.admin<aset_vars->ADMIN_TOGGLEHUD_LEVEL)
		return;
	strcpy(this_map,prev_levels[0].mapname);
	for (i=0;i<strlen(this_map);i++)
		this_map[i] |= 128;

	Com_sprintf(s, sizeof(s), "Hud is now %s", (showhud ? "off." : "on."));
	gi.cprintf(ent, PRINT_HIGH, "%s\n", HighAscii(s));

	showhud = !showhud;
	if (showhud)
	{
		Com_sprintf(str,sizeof(str),ctf_statusbar,
			this_map,prev_levels[1].mapname,prev_levels[2].mapname,prev_levels[3].mapname);
		gi.configstring (CS_STATUSBAR, str);
//		gi.configstring (CS_STATUSBAR, ctf_statusbar);
	}
	else
		gi.configstring (CS_STATUSBAR, "");

}


void Lastseen_Update(edict_t *ent)
{
	int uid;
	uid = (ent->client->resp.uid-1);
	if (uid<0 || uid>=MAX_USERS)
		return;
	//for some reason uid = 0 still happens when on easy. oddness
//	if (ent->client->resp.ctf_team == CTF_TEAM2)
	{
		maplist.users[uid].lastseen = Get_Timestamp();
	}
}

void Lastseen_Save(void)
{
	FILE	*f;
	char	name[256];
	cvar_t	*port;
	cvar_t	*tgame;
	int i;
	char port_d[32];

	tgame = gi.cvar("game", "jump", 0);
    port = gi.cvar("port", "27910", 0);
	
	strcpy(port_d,port->string);
	if (!port_d[0])
		strcpy(port_d,"27910");

	if (!*tgame->string)
	{
		sprintf	(name, "jump/%s/lastseen.ini",port_d);
	}
	else
	{
		sprintf (name, "%s/%s/lastseen.ini", tgame->string,port_d);
	}
	f = fopen (name, "wb");

	if (!f)
	{
		return;
	}

	for (i=0;i<MAX_USERS;i++)
	{
		if (!maplist.users[i].lastseen)
			continue;
		fprintf (f, " %d %d",i,maplist.users[i].lastseen);
	}

	fclose(f);

}


void Lastseen_Load(void)
{
	FILE	*f;
	char	name[256];
	cvar_t	*port;
	cvar_t	*tgame;
	int i;
	char port_d[32];
	int uid;
	int lastseen;

	tgame = gi.cvar("game", "jump", 0);
    port = gi.cvar("port", "27910", 0);
	
	strcpy(port_d,port->string);
	if (!port_d[0])
		strcpy(port_d,"27910");

	if (!*tgame->string)
	{
		sprintf	(name, "jump/%s/lastseen.ini", port_d);
	}
	else
	{
		sprintf (name, "%s/%s/lastseen.ini", tgame->string,port_d);
	}
	f = fopen (name, "rb");

	if (!f)
	{
		return;
	}


	i = 0;
	while (!feof(f))
	{
		if (i>MAX_USERS)
			break;
		fscanf(f, "%i", &uid);
		fscanf(f, "%i", &lastseen);
		if (uid>=0 && uid <MAX_USERS)
		{
			maplist.users[uid].lastseen = lastseen;
		}
		i++;
	}

	fclose(f);
}

// !seen name (lowercase ok)
void Lastseen_Command(edict_t *ent)
{
	char name[255];
	char txt[1024];
	int uid;
	int timenow;
	int i;
	int temp;
	int diff;
	int days,hours,mins,secs;

	int offset = 0;
	timenow = Get_Timestamp();
	
	if (gi.argc() == 2)
	{
        const char* argv1 = gi.argv(1);
        qboolean is_name = false;
        for (int i = 0; argv1[i] != '\0'; ++i) {
            // If there is any non-numeric character, recognize it as a name
            if (argv1[i] < '0' || argv1[i] > '9') {
                is_name = true;
                break;
            }
        }

        if (!is_name) {
            // page number
            offset = atoi(gi.argv(1));

            // If a user's name is all numbers, we can recognize it if it's larger than the max pages
            int max_pages = ceil(maplist.sort_num_users / 20.0);
            if (offset > max_pages) {
                is_name = true;
            }

            // If a user's name is all numbers and is less than the number of max pages,
            // we'll have to fix this if this ever comes up :)
        }

		if (is_name)
		{
			Com_sprintf(name,sizeof(name),"%s",gi.argv(1));
			uid = GetPlayerUid_NoAdd(name);
			if (uid<0)
			{
				gi.cprintf(ent,PRINT_HIGH,"Could not find %s\n",name);
				return;
			}
			if (!maplist.users[uid].lastseen)
			{
				gi.cprintf(ent,PRINT_HIGH,"There is no !seen data available for %s\n",maplist.users[uid].name);
				return;
			}
			diff = timenow - maplist.users[uid].lastseen;
			days = (int)floor(diff/86400);
			diff -= days*86400;
			hours = (int)floor(diff/3600);
			diff -= hours*3600;
			mins = (int)floor(diff/60);
			diff -= mins*60;
			secs = diff;
			gi.cprintf(ent,PRINT_HIGH,"%s was last seen %d days %d hours %d mins %d secs ago\n",maplist.users[uid].name,days,hours,mins,secs);
			return;
		}
	}
	//list pages

 	offset--;
   if (offset<0)
	   offset = 0;
	
   gi.cprintf (ent, PRINT_HIGH, "--------------------------------------------------\n"); 
	Com_sprintf(txt,sizeof(txt),"No. Name       Days:Hrs:Mins:Secs");
	gi.cprintf (ent, PRINT_HIGH,"%s\n",HighAscii(txt));

   for (i=(20*offset); (i<maplist.sort_num_users) && (i<(20*offset)+20); i++) 
   { 
	  temp = maplist.sorted_completions[i].uid;
	  if (temp>=0)
	  {
		  Com_sprintf(name,sizeof(name),maplist.users[temp].name);
		if (maplist.users[temp].lastseen)
		{
			diff = timenow - maplist.users[temp].lastseen;
			days = (int)floor(diff/86400);
			diff -= days*86400;
			hours = (int)floor(diff/3600);
			diff -= hours*3600;
			mins = (int)floor(diff/60);
			diff -= mins*60;
			secs = diff;
			Com_sprintf(txt,sizeof(txt),"%-3d %-16s  %02d:%02d:%02d:%02d", i+1, name,days,hours,mins,secs);
			  if (Can_highlight_Name(name))
				  gi.cprintf (ent, PRINT_HIGH,"%s\n",HighAscii(txt));
			  else	
				  gi.cprintf (ent, PRINT_HIGH,"%s\n",txt);
		}
		else
		{
			Com_sprintf(txt,sizeof(txt), "%-3d %-16s    <unknown>", i+1, name,maplist.users[temp].completions);
			  if (Can_highlight_Name(name))
				  gi.cprintf (ent, PRINT_HIGH,"%s\n",HighAscii(txt));
			  else	
				  gi.cprintf (ent, PRINT_HIGH,"%s\n",txt);
		}
	  }
   } 

   gi.cprintf (ent, PRINT_HIGH, "Page %d/%1.0f (%i users). Use !seen <page>\n",(offset+1),ceil(maplist.sort_num_users/20.0),maplist.sort_num_users); 
   gi.cprintf (ent, PRINT_HIGH, "Type !seen <name> to view a specific user\n"); 

   gi.cprintf (ent, PRINT_HIGH, "--------------------------------------------------\n"); 


}

int Get_Timestamp(void)
{
	time_t seconds;
	int ret;
	seconds = time(NULL);
	ret = (int)seconds;
	return ret;
}

void Cmd_Cleanhud(edict_t *ent)
{
	ent->client->resp.cleanhud = !ent->client->resp.cleanhud;
	if (!ent->client->resp.cleanhud)
		gi.cprintf(ent,PRINT_HIGH,"Hud returned to normal\n");
	else
		gi.cprintf(ent,PRINT_HIGH,"Clean hud enabled. Type cleanhud again to return to normal.\n");
	hud_footer(ent);
}

void Copy_Recording(int uid)
{
	FILE	*f;
	char	name[256];
	cvar_t	*tgame;

	tgame = gi.cvar("game", "jump", 0);

	//dj3 demo
	sprintf (name, "%s/jumpdemo/%s_%d.dj3", tgame->string,level.mapname,uid);
	//load to read
	f = fopen (name, "rb");
	//if exists we do not want to save again
	if (f)
	{		
		//close return
		fclose(f);
		return;
	}


	//doesnt exist, so save
	f = fopen (name, "wb");
	if (!f)
		return;

	fwrite(level_items.recorded_time_data[0],sizeof(record_data),level_items.recorded_time_frames[0],f);
	fclose(f);
}

//maybe base this off total map time, not hardcoded values
void Update_Skill(void)
{
	int i,i2;
	int skill;
	float diff;
	memset(map_skill,0,sizeof(map_skill));
	for (i=0;i<10;i++)
	{
		for (i2=0;i2<i;i2++)
			map_skill[i][i2] = '+';
	}
	memset(map_skill2,0,sizeof(map_skill2));
	for (i=0;i<10;i++)
	{
		for (i2=0;i2<i;i2++)
			map_skill2[i][i2] = 13;
	}
	memset(maplist.skill,0,sizeof(maplist.skill));
	for (i=0;i<maplist.nummaps;i++)
	{
		//less than 6 times
		if (!maplist.times[i][5].time)
		{
			skill = 5;
			
		}
		//less than 10 times
		else if (!maplist.times[i][9].time)
		{
			skill = 4;
		}
		//spot 10 set, check to see if there is a spot 11 but NOT a spot 15, this would also be a skill 4
		else if (maplist.times[i][10].time && !maplist.times[i][14].time)
		{
			skill = 4;
		}
		//more than or equal to 10 times set
		else
		{
			for (i2=0;i2<MAX_HIGHSCORES;i2++)
				if (!maplist.times[i][i2].time)
					break;
			i2--;
			//wont happen but ya never know
			if (i2<0)
			{
				gi.dprintf("i2 = %i\n", i2);
				skill = 1;
			}
			else
			{
				
				diff = (maplist.times[i][i2].time / maplist.times[i][0].time);
				if (diff<1.2)
					skill = 1;
				else if (diff < 1.5)
					skill = 2;
				else if (diff > 2)
					skill = 4;
				else
					skill = 3;
			}
		}
		maplist.skill[i] = skill;
	}	
}

void ShowCurrentVotelist(edict_t *ent,int offset) 
{ 
   int i; 
	char name[64];
	int mapnum;
   offset--;
   if (offset<0)
	   offset = 0;
	
   gi.cprintf (ent, PRINT_HIGH, "--------------------------------------------------------\n"); 

   for (i=(20*offset); (i<maplist.nummaps) && (i<(20*offset)+20); i++) 
   { 	 
	   mapnum = votemaplist[i];
	   if (maplist.times[mapnum][0].uid>=0)
	   {
		  Com_sprintf(name,sizeof(name),maplist.users[maplist.times[mapnum][0].uid].name);
		  Highlight_Name(name);
  		  gi.cprintf (ent, PRINT_HIGH, "%-3d %5s %-20s %5d times\n", i+1, map_skill[maplist.skill[mapnum]],maplist.mapnames[mapnum],maplist.update[mapnum]);
	   }
	   else
			gi.cprintf (ent, PRINT_HIGH, "%-3d %5s %-20s %5d times\n", i+1, map_skill[maplist.skill[mapnum]],maplist.mapnames[mapnum],maplist.update[mapnum]);
   } 

   gi.cprintf (ent, PRINT_HIGH, "Page %d/%1.0f (%i maps). Use votelist <page> or <letter>\n",(offset+1),ceil(maplist.nummaps/20.0),maplist.nummaps); 

   gi.cprintf (ent, PRINT_HIGH, "--------------------------------------------------------\n"); 
} 

model_list_t model_list[32];
int model_list_count;

model_list_t ghost_model_list[128];
int ghost_model_list_count;

qboolean admin_model_exists;

static byte invis_pcx[900] = {

0x0A, 0x05, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0xFC, 0x00,
0xD0, 0x03, 0x00, 0x00, 0xA4, 0x54, 0x13, 0x00, 0xAC, 0x54, 0x13, 0x00, 0x08, 0x00, 0x00, 0x00,
0x0E, 0x00, 0x00, 0x00, 0xBC, 0x59, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xB8, 0x32, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFD, 0x7F,
0x00, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDD, 0x02, 0x20, 0x77, 0xC3, 0x01,
0x67, 0xC0, 0xD8, 0x02, 0x20, 0x77, 0xC3, 0x01, 0xD0, 0xBB, 0xD7, 0x03, 0x60, 0x77, 0xC3, 0x01,
0xA7, 0xC2, 0xD8, 0x02, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1E, 0xDD, 0x02, 0x20, 0x77, 0xC3, 0x01,
0x67, 0xC0, 0xD8, 0x02, 0x20, 0x77, 0xC3, 0x01, 0x08, 0x55, 0x13, 0x00, 0x60, 0x77, 0xC3, 0x01,
0xC1, 0xFF, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x1F, 0x1F, 0x1F, 0x2F, 0x2F, 0x2F,
0x3F, 0x3F, 0x3F, 0x4B, 0x4B, 0x4B, 0x5B, 0x5B, 0x5B, 0x6B, 0x6B, 0x6B, 0x7B, 0x7B, 0x7B, 0x8B,
0x8B, 0x8B, 0x9B, 0x9B, 0x9B, 0xAB, 0xAB, 0xAB, 0xBB, 0xBB, 0xBB, 0xCB, 0xCB, 0xCB, 0xDB, 0xDB,
0xDB, 0xEB, 0xEB, 0xEB, 0x63, 0x4B, 0x23, 0x5B, 0x43, 0x1F, 0x53, 0x3F, 0x1F, 0x4F, 0x3B, 0x1B,
0x47, 0x37, 0x1B, 0x3F, 0x2F, 0x17, 0x3B, 0x2B, 0x17, 0x33, 0x27, 0x13, 0x2F, 0x23, 0x13, 0x2B,
0x1F, 0x13, 0x27, 0x1B, 0x0F, 0x23, 0x17, 0x0F, 0x1B, 0x13, 0x0B, 0x17, 0x0F, 0x0B, 0x13, 0x0F,
0x07, 0x0F, 0x0B, 0x07, 0x5F, 0x5F, 0x6F, 0x5B, 0x5B, 0x67, 0x5B, 0x53, 0x5F, 0x57, 0x4F, 0x5B,
0x53, 0x4B, 0x53, 0x4F, 0x47, 0x4B, 0x47, 0x3F, 0x43, 0x3F, 0x3B, 0x3B, 0x3B, 0x37, 0x37, 0x33,
0x2F, 0x2F, 0x2F, 0x2B, 0x2B, 0x27, 0x27, 0x27, 0x23, 0x23, 0x23, 0x1B, 0x1B, 0x1B, 0x17, 0x17,
0x17, 0x13, 0x13, 0x13, 0x8F, 0x77, 0x53, 0x7B, 0x63, 0x43, 0x73, 0x5B, 0x3B, 0x67, 0x4F, 0x2F,
0xCF, 0x97, 0x4B, 0xA7, 0x7B, 0x3B, 0x8B, 0x67, 0x2F, 0x6F, 0x53, 0x27, 0xEB, 0x9F, 0x27, 0xCB,
0x8B, 0x23, 0xAF, 0x77, 0x1F, 0x93, 0x63, 0x1B, 0x77, 0x4F, 0x17, 0x5B, 0x3B, 0x0F, 0x3F, 0x27,
0x0B, 0x23, 0x17, 0x07, 0xA7, 0x3B, 0x2B, 0x9F, 0x2F, 0x23, 0x97, 0x2B, 0x1B, 0x8B, 0x27, 0x13,
0x7F, 0x1F, 0x0F, 0x73, 0x17, 0x0B, 0x67, 0x17, 0x07, 0x57, 0x13, 0x00, 0x4B, 0x0F, 0x00, 0x43,
0x0F, 0x00, 0x3B, 0x0F, 0x00, 0x33, 0x0B, 0x00, 0x2B, 0x0B, 0x00, 0x23, 0x0B, 0x00, 0x1B, 0x07,
0x00, 0x13, 0x07, 0x00, 0x7B, 0x5F, 0x4B, 0x73, 0x57, 0x43, 0x6B, 0x53, 0x3F, 0x67, 0x4F, 0x3B,
0x5F, 0x47, 0x37, 0x57, 0x43, 0x33, 0x53, 0x3F, 0x2F, 0x4B, 0x37, 0x2B, 0x43, 0x33, 0x27, 0x3F,
0x2F, 0x23, 0x37, 0x27, 0x1B, 0x2F, 0x23, 0x17, 0x27, 0x1B, 0x13, 0x1F, 0x17, 0x0F, 0x17, 0x0F,
0x0B, 0x0F, 0x0B, 0x07, 0x6F, 0x3B, 0x17, 0x5F, 0x37, 0x17, 0x53, 0x2F, 0x17, 0x43, 0x2B, 0x17,
0x37, 0x23, 0x13, 0x27, 0x1B, 0x0F, 0x1B, 0x13, 0x0B, 0x0F, 0x0B, 0x07, 0xB3, 0x5B, 0x4F, 0xBF,
0x7B, 0x6F, 0xCB, 0x9B, 0x93, 0xD7, 0xBB, 0xB7, 0xCB, 0xD7, 0xDF, 0xB3, 0xC7, 0xD3, 0x9F, 0xB7,
0xC3, 0x87, 0xA7, 0xB7, 0x73, 0x97, 0xA7, 0x5B, 0x87, 0x9B, 0x47, 0x77, 0x8B, 0x2F, 0x67, 0x7F,
0x17, 0x53, 0x6F, 0x13, 0x4B, 0x67, 0x0F, 0x43, 0x5B, 0x0B, 0x3F, 0x53, 0x07, 0x37, 0x4B, 0x07,
0x2F, 0x3F, 0x07, 0x27, 0x33, 0x00, 0x1F, 0x2B, 0x00, 0x17, 0x1F, 0x00, 0x0F, 0x13, 0x00, 0x07,
0x0B, 0x00, 0x00, 0x00, 0x8B, 0x57, 0x57, 0x83, 0x4F, 0x4F, 0x7B, 0x47, 0x47, 0x73, 0x43, 0x43,
0x6B, 0x3B, 0x3B, 0x63, 0x33, 0x33, 0x5B, 0x2F, 0x2F, 0x57, 0x2B, 0x2B, 0x4B, 0x23, 0x23, 0x3F,
0x1F, 0x1F, 0x33, 0x1B, 0x1B, 0x2B, 0x13, 0x13, 0x1F, 0x0F, 0x0F, 0x13, 0x0B, 0x0B, 0x0B, 0x07,
0x07, 0x00, 0x00, 0x00, 0x97, 0x9F, 0x7B, 0x8F, 0x97, 0x73, 0x87, 0x8B, 0x6B, 0x7F, 0x83, 0x63,
0x77, 0x7B, 0x5F, 0x73, 0x73, 0x57, 0x6B, 0x6B, 0x4F, 0x63, 0x63, 0x47, 0x5B, 0x5B, 0x43, 0x4F,
0x4F, 0x3B, 0x43, 0x43, 0x33, 0x37, 0x37, 0x2B, 0x2F, 0x2F, 0x23, 0x23, 0x23, 0x1B, 0x17, 0x17,
0x13, 0x0F, 0x0F, 0x0B, 0x9F, 0x4B, 0x3F, 0x93, 0x43, 0x37, 0x8B, 0x3B, 0x2F, 0x7F, 0x37, 0x27,
0x77, 0x2F, 0x23, 0x6B, 0x2B, 0x1B, 0x63, 0x23, 0x17, 0x57, 0x1F, 0x13, 0x4F, 0x1B, 0x0F, 0x43,
0x17, 0x0B, 0x37, 0x13, 0x0B, 0x2B, 0x0F, 0x07, 0x1F, 0x0B, 0x07, 0x17, 0x07, 0x00, 0x0B, 0x00,
0x00, 0x00, 0x00, 0x00, 0x77, 0x7B, 0xCF, 0x6F, 0x73, 0xC3, 0x67, 0x6B, 0xB7, 0x63, 0x63, 0xA7,
0x5B, 0x5B, 0x9B, 0x53, 0x57, 0x8F, 0x4B, 0x4F, 0x7F, 0x47, 0x47, 0x73, 0x3F, 0x3F, 0x67, 0x37,
0x37, 0x57, 0x2F, 0x2F, 0x4B, 0x27, 0x27, 0x3F, 0x23, 0x1F, 0x2F, 0x1B, 0x17, 0x23, 0x13, 0x0F,
0x17, 0x0B, 0x07, 0x07, 0x9B, 0xAB, 0x7B, 0x8F, 0x9F, 0x6F, 0x87, 0x97, 0x63, 0x7B, 0x8B, 0x57,
0x73, 0x83, 0x4B, 0x67, 0x77, 0x43, 0x5F, 0x6F, 0x3B, 0x57, 0x67, 0x33, 0x4B, 0x5B, 0x27, 0x3F,
0x4F, 0x1B, 0x37, 0x43, 0x13, 0x2F, 0x3B, 0x0B, 0x23, 0x2F, 0x07, 0x1B, 0x23, 0x00, 0x13, 0x17,
0x00, 0x0B, 0x0F, 0x00, 0x00, 0xFF, 0x00, 0x23, 0xE7, 0x0F, 0x3F, 0xD3, 0x1B, 0x53, 0xBB, 0x27,
0x5F, 0xA7, 0x2F, 0x5F, 0x8F, 0x33, 0x5F, 0x7B, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD3, 0xFF,
0xFF, 0xA7, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0x53, 0xFF, 0xFF, 0x27, 0xFF, 0xEB, 0x1F, 0xFF, 0xD7,
0x17, 0xFF, 0xBF, 0x0F, 0xFF, 0xAB, 0x07, 0xFF, 0x93, 0x00, 0xEF, 0x7F, 0x00, 0xE3, 0x6B, 0x00,
0xD3, 0x57, 0x00, 0xC7, 0x47, 0x00, 0xB7, 0x3B, 0x00, 0xAB, 0x2B, 0x00, 0x9B, 0x1F, 0x00, 0x8F,
0x17, 0x00, 0x7F, 0x0F, 0x00, 0x73, 0x07, 0x00, 0x5F, 0x00, 0x00, 0x47, 0x00, 0x00, 0x2F, 0x00,
0x00, 0x1B, 0x00, 0x00, 0xEF, 0x00, 0x00, 0x37, 0x37, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF,
0x2B, 0x2B, 0x23, 0x1B, 0x1B, 0x17, 0x13, 0x13, 0x0F, 0xEB, 0x97, 0x7F, 0xC3, 0x73, 0x53, 0x9F,
0x57, 0x33, 0x7B, 0x3F, 0x1B, 0xEB, 0xD3, 0xC7, 0xC7, 0xAB, 0x9B, 0xA7, 0x8B, 0x77, 0x87, 0x6B,
0x57, 0x9F, 0x5B, 0x53
};
void Create_Invis_Skin(void)
{
	char name[255];
	FILE	*f;
	Com_sprintf(name,sizeof(name),"baseq2/players/female/");
	FS_CreatePath(name);
	sprintf (name, "baseq2/players/female/invis.pcx");
	f = fopen(name,"wb");
	fwrite(invis_pcx,900,1,f);
	fclose(f);
	sprintf (name, "baseq2/players/female/invis_i.pcx");
	f = fopen(name,"wb");
	fwrite(invis_pcx,900,1,f);
	fclose(f);
}

void Load_Model_List(void)
{
	cvar_t	*tgame;
	int i;
	char name[255];
	FILE	*f;
	FILE	*f2;
	char model[255];
	//start with admin model, verify it exists
	tgame = gi.cvar("game", "", 0);

	admin_model_exists = true;
	model_list_count = ghost_model_list_count = 0;
	sprintf (name, "%s/players/%s/tris.md2", tgame->string,gset_vars->admin_model);
	f = fopen (name, "r");
	if (!f)
	{
		sprintf (name, "baseq2/players/%s/tris.md2", gset_vars->admin_model);
		f = fopen (name, "r");
		if (!f)
		{
			admin_model_exists = false;
		}
		else
			fclose(f);
	}
	else
		fclose(f);

	sprintf (name, "%s/models.cfg", tgame->string);
	f = fopen (name, "rb");
	if (f)
	{
		for (i=0;i<32;i++)
			memset(model_list[i].name,0,sizeof(model_list[i].name));
		//go thru random model list
		
		while (!feof(f))
		{
			if (model_list_count>32)
				break;
			fscanf(f, "%s", model);
			sprintf (name, "%s/players/%s/tris.md2", tgame->string,model);
			f2 = fopen (name, "r");
			if (!f2)
			{
				sprintf (name, "baseq2/players/%s/tris.md2", model);
				f2 = fopen (name, "r");
				if (!f2)
				{
					continue;
				}
				else
					fclose(f2);
			}
			else
				fclose(f2);
			Com_sprintf(model_list[model_list_count].name,sizeof(model_list[model_list_count].name),"%s",model);
			model_list_count++;
		}
		fclose(f);
	}
	sprintf (name, "%s/ghost_models.cfg", tgame->string);
	f = fopen (name, "rb");
	if (f)
	{
		for (i=0;i<128;i++)
			memset(ghost_model_list[i].name,0,sizeof(ghost_model_list[i].name));
		//go thru random model list
		
		while (!feof(f))
		{
			if (ghost_model_list_count>128)
				break;
			fscanf(f, "%s", model);
			sprintf (name, "%s/players/ghost/%s.md2", tgame->string,model);
			f2 = fopen (name, "r");
			if (!f2)
			{
				sprintf (name, "baseq2/players/ghost/%s.md2", model);
				f2 = fopen (name, "r");
				if (!f2)
				{
					continue;
				}
				else
					fclose(f2);
			}
			else
				fclose(f2);
			Com_sprintf(ghost_model_list[ghost_model_list_count].name,sizeof(ghost_model_list[ghost_model_list_count].name),"%s",model);
			ghost_model_list_count++;
		}
		fclose(f);
	}

}


void SkinList(edict_t *ent)
{
	int i;
	char txt[255];
	if (ent->client->resp.admin<aset_vars->ADMIN_MODEL_LEVEL)
		return;

	if (!model_list_count)
	{
		gi.cprintf(ent,PRINT_HIGH,"No skins available\n");
		return;
	}
	Com_sprintf(txt,sizeof(txt),"No. Skin");
	gi.cprintf(ent,PRINT_HIGH,"%s\n",HighAscii(txt));
	for (i=0;i<model_list_count;i++)
	{
		gi.cprintf(ent,PRINT_HIGH,"%2i. %s\n",i+1,model_list[i].name);
	}
	gi.cprintf(ent,PRINT_HIGH,"Type skin <name> to apply\n");

}

compare_users_t compare_users[24];
void Compare_Users_Info1(edict_t *ent)
{
	int index;
	char txt[255];
	index = ent-g_edicts-1;
		Com_sprintf(txt,sizeof(txt),"Comparison profiles for %s and %s loaded",maplist.users[compare_users[index].user1.uid].name,maplist.users[compare_users[index].user2.uid].name);
		gi.cprintf(ent,PRINT_HIGH,"\n%s\n",HighAscii(txt));
		gi.cprintf(ent,PRINT_HIGH,"You may now compare maps completed for both players\n");
		gi.cprintf(ent,PRINT_HIGH,"compare yes yes - displays maps both players have completed\n");
		gi.cprintf(ent,PRINT_HIGH,"compare  no  no - displays maps neither player has completed\n");
		gi.cprintf(ent,PRINT_HIGH,"compare yes  no - displays maps %s has completed and %s has not\n",maplist.users[compare_users[index].user1.uid].name,maplist.users[compare_users[index].user2.uid].name);
		gi.cprintf(ent,PRINT_HIGH,"compare  no yes - displays maps %s has not completed and %s has\n",maplist.users[compare_users[index].user1.uid].name,maplist.users[compare_users[index].user2.uid].name);
		gi.cprintf(ent,PRINT_HIGH,"\nto load other profiles simply type compare <name> <name>\n\n");
}
void Compare_Users(edict_t *ent)
{
	int i,i2;
	int index;
	char name1[255];
	char name2[255];
	char txt[255];
	qboolean display1;
	qboolean display2;
	int offset = 0;
	int type1 = -1;
	int uid1;
	int uid2;
	int type2 = -1;

	index = ent-g_edicts-1;

	if (gi.argc() < 2)
	{
		if (!compare_users[index].user1.loaded && !compare_users[index].user2.loaded)
		{
			Com_sprintf(txt,sizeof(txt),"Compare 2 players map completions");
			gi.cprintf(ent,PRINT_HIGH,"%s\n",HighAscii(txt));
			gi.cprintf(ent,PRINT_HIGH,"compare <name> <name> to load those profiles\n");
			gi.cprintf(ent,PRINT_HIGH,"example: compare %s %s\n",maplist.users[maplist.sorted_users[0].uid].name,maplist.users[maplist.sorted_users[1].uid].name);
			return;
		}
		else
		{
			Compare_Users_Info1(ent);
			return;
		}
	}
	strncpy(name1,gi.argv(1),sizeof(name1)-1);
	strncpy(name2,gi.argv(2),sizeof(name2)-1);

	if (strcmp(name1,"yes")==0)
		type1 = 1;
	else if (strcmp(name1,"no")==0)
		type1 = 0;
	if (strcmp(name2,"yes")==0)
		type2 = 1;
	else if (strcmp(name2,"no")==0)
		type2 = 0;

	if (type1<0 || type2<0)
	{
		uid1 = GetPlayerUid_NoAdd(name1);
		uid2 = GetPlayerUid_NoAdd(name2);
		if (uid1<0)
		{
			gi.cprintf(ent,PRINT_HIGH,"Cannot find player %s\n",name1);
			return;
		}
		if (uid2<0)
		{
			gi.cprintf(ent,PRINT_HIGH,"Cannot find player %s\n",name2);
			return;
		}
		/*if (uid1!=compare_users[index].user1.uid || uid2!=compare_users[index].user2.uid)
		{
			if (compare_users[index].last_load+10>Get_Timestamp())
			{
				gi.cprintf(ent,PRINT_HIGH,"You can only load a new comparison profile once every 10 seconds.\n");
				return;
			}
			//load them
			compare_users[index].user1.uid = uid1;
			compare_users[index].user2.uid = uid2;
			compare_users[index].last_load = Get_Timestamp();
			if (!open_uid_file_compare(ent))
			{
				gi.cprintf(ent,PRINT_HIGH,"There is a problem with the compare function. Please notify an admin if possible.\n");
				return;
			}
		}
		compare_users[index].user1.loaded = true;
		compare_users[index].user2.loaded = true;
		Compare_Users_Info1(ent);
		return;*/
	}
	//profiles loaded and we have a yes no request
	if (gi.argc() == 4)
	{
		offset = atoi(gi.argv(3));
	}
	if (offset<=0 || offset> 200)
		offset=1;

	i2 = 0;
	i = 0;
	strcpy(name1,maplist.users[uid1].name);
	strcpy(name2,maplist.users[uid2].name);
	gi.cprintf(ent,PRINT_HIGH,"-------------------------------------------------------------\n");
	Com_sprintf(txt,sizeof(txt),"Comparing                      %12s %12s",name1,name2);
	gi.cprintf(ent,PRINT_HIGH,"%s\n",HighAscii(txt));
	while(i<(offset*20))
	{
		while (i2<maplist.nummaps)
		{
			//test case, both havnt done
			display1 = false;
			display2 = false;
			if (type1 && maplist.users[uid1].maps_done[i2])
				display1 = true;
			if (type2 && maplist.users[uid2].maps_done[i2])
				display2 = true;
			if (!type1 && !maplist.users[uid1].maps_done[i2])
				display1 = true;
			if (!type2 && !maplist.users[uid2].maps_done[i2])
				display2 = true;
			if (display1 && display2)
			{
				if (i>=(offset-1)*20)
				{
				   if (maplist.times[i2][0].uid>=0)
				   {
  					  gi.cprintf (ent, PRINT_HIGH, "%-3d %5s %-20s %12s %12s\n", i,map_skill[maplist.skill[i2]],						  
						  maplist.mapnames[i2],
						  (type1 ? "yes" : " no"),(type2 ? "yes" : " no"));
				   }
				   else
						gi.cprintf (ent, PRINT_HIGH, "%-3d %5s %-20s %s %s\n", i, map_skill[maplist.skill[i2]],maplist.mapnames[i2],(type1 ? "yes" : " no"),(type2 ? "yes" : " no"));

				}
				i2++;
				break;
			}
			i2++;
		}
		if (i2>maplist.nummaps)
		{
			//gone thru all maps, break out
			break;
		}
		i++;
	}
	gi.cprintf(ent,PRINT_HIGH,"Type compare %s %s %d for the next page\n",(type1 ? "yes" : "no"),(type2 ? "yes" : "no"),offset+1);
	gi.cprintf(ent,PRINT_HIGH,"-------------------------------------------------------------\n");

}

void Update_Next_Maps(void) {
	char txt[255];
	char mapname[32];
	char longmapname[64];

	if (strlen(maplist.mapnames[map1]) > 16) {
		strcpy(longmapname, maplist.mapnames[map1]);
		memcpy(mapname, &longmapname[0], 13);
		mapname[13] = '\0';
		Com_sprintf(mapname, sizeof(mapname), "%s...", mapname);
	} else {
		strcpy(mapname, maplist.mapnames[map1]);
	}

	if (maplist.times[map1][0].time>0)
		Com_sprintf(txt,sizeof(txt),"%5s %-16s%16s %8.3f",map_skill2[maplist.skill[map1]],mapname,maplist.users[maplist.times[map1][0].uid].name,maplist.times[map1][0].time);
	else
		Com_sprintf(txt,sizeof(txt),"%5s %-16s",map_skill2[maplist.skill[map1]],mapname);
	gi.configstring (CONFIG_JUMP_NEXT_MAP1,txt);

	if (strlen(maplist.mapnames[map2]) > 16) {
		strcpy(longmapname, maplist.mapnames[map2]);
		memcpy(mapname, &longmapname[0], 13);
		mapname[13] = '\0';
		Com_sprintf(mapname, sizeof(mapname), "%s...", mapname);
	}
	else {
		strcpy(mapname, maplist.mapnames[map2]);
	}
	if (maplist.times[map2][0].time>0)
		Com_sprintf(txt,sizeof(txt),"%5s %-16s%16s %8.3f",map_skill2[maplist.skill[map2]],mapname,maplist.users[maplist.times[map2][0].uid].name,maplist.times[map2][0].time);
	else
		Com_sprintf(txt,sizeof(txt),"%5s %-16s",map_skill2[maplist.skill[map2]],mapname);
	gi.configstring (CONFIG_JUMP_NEXT_MAP2,txt);

	if (strlen(maplist.mapnames[map3]) > 16) {
		strcpy(longmapname, maplist.mapnames[map3]);
		memcpy(mapname, &longmapname[0], 13);
		mapname[13] = '\0';
		Com_sprintf(mapname, sizeof(mapname), "%s...", mapname);
	}
	else {
		strcpy(mapname, maplist.mapnames[map3]);
	}
	if (maplist.times[map3][0].time>0)
		Com_sprintf(txt,sizeof(txt),"%5s %-16s%16s %8.3f",map_skill2[maplist.skill[map3]],mapname,maplist.users[maplist.times[map3][0].uid].name,maplist.times[map3][0].time);
	else
		Com_sprintf(txt,sizeof(txt),"%5s %-16s",map_skill2[maplist.skill[map3]],mapname);
	gi.configstring (CONFIG_JUMP_NEXT_MAP3,txt);
}

void Apply_Nominated_Map(char *mapname)
{
	int i;
	if (admin_overide_vote_maps)
		return;

	nominated_map = true;
	for (i=0;i<maplist.nummaps;i++)
	{
		if (Q_stricmp(maplist.mapnames[i],mapname)==0)
		{
			map1 = i;
			break;
		}
	}
	//set map1 to this map
}

int number_of_jumpers_off = 0;
void Jumpers_Update_Skins(edict_t *ent)
{
	int j;
	edict_t *invis;
	char s[1024];
	j = ent->client->resp.cur_jumper;
	if (j<0 || j>=16)
		j = ent->client->resp.cur_jumper = 0;

	invis = g_edicts + 1 + j;
	if (!invis->inuse || !invis->client)
	{
		ent->client->resp.cur_jumper++;
		return;
	}
	gi.WriteByte (svc_configstring);
	gi.WriteShort(CS_PLAYERSKINS+j);
	Com_sprintf(s,sizeof(s),"%s\\female/invis",invis->client->pers.netname);
	gi.WriteString (s);
	gi.unicast(ent, true);
	ent->client->resp.cur_jumper++;

}

void Jumpers_on_off(edict_t *ent)
{
	
	edict_t *invis;
	int i;
	char s[255];
	ent->client->resp.hide_jumpers = !ent->client->resp.hide_jumpers;
	Com_sprintf(s,sizeof(s),"Players models/sounds are now %s",(ent->client->resp.hide_jumpers ? "OFF." : "ON."));
	gi.cprintf(ent,PRINT_HIGH,"%s\n",HighAscii(s));
	if (!ent->client->resp.hide_jumpers)
	{
		for (i=0 ; i<maxclients->value ; i++)
		{
			invis = g_edicts + 1 + i;
			if (!invis->inuse || !invis->client)
				continue;
			gi.WriteByte (svc_configstring);
			gi.WriteShort(CS_PLAYERSKINS+i);
			gi.WriteString (invis->client->pers.skin);
			gi.unicast(ent, true);
			
		}
	}
	else
	{
		//gi.cprintf(ent,PRINT_HIGH,"Attempting to download invisible skin.\n");
		//stuffcmd(ent,"download players/female/invis.pcx\n");
	}
	
}

void Cpsound_on_off(edict_t *ent)
{
	char s[255];
	ent->client->resp.mute_cps = !ent->client->resp.mute_cps;
	Com_sprintf(s,sizeof(s),"Checkpoint sounds are now %s",(ent->client->resp.mute_cps ? "off." : "on."));
	gi.cprintf(ent,PRINT_HIGH,"%s\n",HighAscii(s));
}

//get cp crossing time from the replay.
void mute_cprep_on_off(edict_t *ent)
{
	char s[255];
	ent->client->resp.mute_cprep = !ent->client->resp.mute_cprep;
	Com_sprintf(s, sizeof(s), "Showing replays checkpoint-time is now %s", (ent->client->resp.mute_cprep ? "off." : "on."));
	gi.cprintf(ent, PRINT_HIGH, "%s\n", HighAscii(s));
}

void Showtimes_on_off(edict_t *ent)
{
	char s[255];
	ent->client->resp.showtimes = !ent->client->resp.showtimes;
	Com_sprintf(s, sizeof(s), "Showing all times is now %s", (ent->client->resp.showtimes ? "on." : "off."));
	gi.cprintf(ent, PRINT_HIGH, "%s\n", HighAscii(s));
}


void	FS_CreatePath (char *path)
{
	char	*ofs;

	for (ofs = path+1 ; *ofs ; ofs++)
	{
		if (*ofs == '/')
		{	// create the directory
			*ofs = 0;
#if defined(WIN32)
			_mkdir(path);
#else
			mkdir(path,0777);
#endif
			*ofs = '/';
		}
	}
}

void Cmd_1st(edict_t *ent)
{
	int i;
	struct	tm *current_date;
	time_t	time_date;
	int		month,day,year;
	char today[32];
	char yest[32];
	char name[255];
	int done = 0;
	int offset;
	int start;
	int finish;
	offset = atoi(gi.argv(1));
	if (offset<=0 || offset> 20)
		offset=1;
	finish = offset*20;
	offset--;
	start = offset*20;

	//set the date
		time_date = time(NULL);                // DOS system call
		current_date = localtime(&time_date);  // DOS system call
		month = current_date->tm_mon + 1;
		day = current_date->tm_mday;
		year = current_date->tm_year;

	Com_sprintf(today,sizeof(today),"%02d/%02d/%02d",day, month,year-100);
		time_date = time(NULL)-86400;                // DOS system call
		current_date = localtime(&time_date);  // DOS system call
		month = current_date->tm_mon + 1;
		day = current_date->tm_mday;
		year = current_date->tm_year;

	Com_sprintf(yest,sizeof(yest),"%02d/%02d/%02d",day, month,year-100);

	Com_sprintf(name,sizeof(name),"First places set in the last 24 hours");
	gi.cprintf (ent, PRINT_HIGH,"\n%s\n",HighAscii(name));	
   gi.cprintf (ent, PRINT_HIGH, "---------------------------------------------------\n"); 
	for (i=0;i<maplist.nummaps;i++)
	{
		if (done>=finish)
			break;
		if (maplist.times[i][0].time>0)
		{
			if (!strcmp(maplist.times[i][0].date,today) || !strcmp(maplist.times[i][0].date,yest))
			{
			  done++;
			  if (done<start)
				  continue;
			  Com_sprintf(name,sizeof(name),maplist.users[maplist.times[i][0].uid].name);
			  Highlight_Name(name);
  			  gi.cprintf (ent, PRINT_HIGH, "%-2i %5s %-20s %-16s %-8.3f\n",done,map_skill[maplist.skill[i]],maplist.mapnames[i],name,maplist.times[i][0].time);
			}
		}
		else
		{
			continue;
		}
	}
	gi.cprintf (ent, PRINT_HIGH,"                   type 1st %d to see the next page\n",(offset+2));	
   gi.cprintf (ent, PRINT_HIGH, "---------------------------------------------------\n\n"); 
}

/*
void Changename(edict_t *ent)
{
	char origname[255];
	char newname[255];
	int origid;
	int newid;
	int i;

	if (ent->client->resp.admin<aset_vars->ADMIN_CHANGENAME_LEVEL)
		return;

	if (gi.argc() < 3) 
	{
		gi.cprintf(ent,PRINT_HIGH,"format: changename orig_name new_name\nWarning: Any name change will force the current map to be reloaded.\n");
		return;
	}
	Com_sprintf(origname,sizeof(origname),gi.argv(1));
	Com_sprintf(newname,sizeof(newname),gi.argv(2));
	origid = GetPlayerUid_NoAdd(origname);
	newid = GetPlayerUid_NoAdd(newname);
	if (origid<0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Cannot find player %s\n",origname);
		return;
	}
	if (newid>=0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Player %s already exists. Try renaming them first or use a different name\n",newname);
		return;
	}
	//change the name
	for (i=0;i<strlen(newname);i++)
	{
		if (newname[i]==' ')
			newname[i]='_';
	}	

	strcpy(maplist.users[origid].name,newname);	

	for (i=0;i<MAX_HIGHSCORES*2;i++)
	{
		if (maplist.times[level.mapnum][i].uid==origid)
		{
			strcpy(maplist.times[level.mapnum][i].name,newname);
			strcpy(maplist.users[maplist.times[level.mapnum][i].uid].name,newname);
		}
	}
	gi.bprintf(PRINT_CHAT,"%s has had their name changed to %s by an admin.\nRestarting map to save changes.\n",origname,newname);
	strncpy(level.forcemap, level.mapname, sizeof(level.forcemap) - 1);
	EndDMLevel();

}
*/
void Cmd_Stats(edict_t *ent)
{
	char name[255];
	int i;
	char txt[1024];
	int uid;
	int points;
	int offset;
	int start;
	int done = 0;
	int finish;

	if (gi.argc() < 2)
	{
		gi.cprintf(ent,PRINT_HIGH,"format: !stats <playername>\n");
		return;
	}
	Com_sprintf(name,sizeof(name),"%s",gi.argv(1));
	uid = GetPlayerUid_NoAdd(name);
	if (uid<0)
	{
		gi.cprintf(ent,PRINT_HIGH,"Could not find %s\n",name);
		return;
	}
	if (gi.argc()==2)
	{
		gi.cprintf (ent, PRINT_HIGH, "\n-------------------------------------------");
		Com_sprintf(txt,sizeof(txt),"Statistics for %s:",maplist.users[uid].name);
		gi.cprintf(ent,PRINT_HIGH,"\n%s\n",HighAscii(txt));
		gi.cprintf(ent,PRINT_HIGH,"\xb1st %3d  \xb6th %3d \xb1\xb1th %3d\n",maplist.users[uid].points[0],maplist.users[uid].points[5],maplist.users[uid].points[10]);
		gi.cprintf(ent,PRINT_HIGH,"\xb2nd %3d  \xb7th %3d \xb1\xb2th %3d\n",maplist.users[uid].points[1],maplist.users[uid].points[6],maplist.users[uid].points[11]);
		gi.cprintf(ent,PRINT_HIGH,"\xb3rd %3d  \xb8th %3d \xb1\xb3th %3d\n",maplist.users[uid].points[2],maplist.users[uid].points[7],maplist.users[uid].points[12]);
		gi.cprintf(ent,PRINT_HIGH,"\xb4th %3d  \xb9th %3d \xb1\xb4th %3d\n",maplist.users[uid].points[3],maplist.users[uid].points[8],maplist.users[uid].points[13]);
		gi.cprintf(ent,PRINT_HIGH,"\xb5th %3d \xb1\xb0th %3d \xb1\xb5th %3d\n",maplist.users[uid].points[4],maplist.users[uid].points[9],maplist.users[uid].points[14]);
		gi.cprintf(ent,PRINT_HIGH,"Total Maps Completed %4d\n",maplist.users[uid].completions);
		gi.cprintf(ent,PRINT_HIGH,"Remaining            %4d\n",(maplist.nummaps-maplist.users[uid].completions));
		gi.cprintf(ent,PRINT_HIGH,"\nType !stats %s 1 to see 1st places.\nType !stats %s 2 to see 2nd places.\nand so on...\n",name,name);
		gi.cprintf (ent, PRINT_HIGH, "-------------------------------------------\n\n");
		
//		gi.cprintf(ent,PRINT_HIGH,"\n%s\n",HighAscii(txt));
//		gi.cprintf(ent,PRINT_HIGH,"\n 1st %3d   11th %3d   21st %3d   31st %3d   41st %3d\n",maplist.users[uid].points[0],maplist.users[uid].points[10],maplist.users[uid].points[20],maplist.users[uid].points[30],maplist.users[uid].points[40]);
//		gi.cprintf(ent,PRINT_HIGH," 2nd %3d   12th %3d   22nd %3d   32nd %3d   42nd %3d\n",maplist.users[uid].points[1],maplist.users[uid].points[11],maplist.users[uid].points[21],maplist.users[uid].points[31],maplist.users[uid].points[41]);
//		gi.cprintf(ent,PRINT_HIGH," 3rd %3d   13th %3d   23rd %3d   33rd %3d   43rd %3d\n",maplist.users[uid].points[2],maplist.users[uid].points[12],maplist.users[uid].points[22],maplist.users[uid].points[32],maplist.users[uid].points[42]);
//		gi.cprintf(ent,PRINT_HIGH," 4th %3d   14th %3d   24th %3d   34th %3d   44th %3d\n",maplist.users[uid].points[3],maplist.users[uid].points[13],maplist.users[uid].points[23],maplist.users[uid].points[33],maplist.users[uid].points[43]);
//		gi.cprintf(ent,PRINT_HIGH," 5th %3d   15th %3d   25th %3d   35th %3d   45th %3d\n",maplist.users[uid].points[4],maplist.users[uid].points[14],maplist.users[uid].points[24],maplist.users[uid].points[34],maplist.users[uid].points[44]);
//		gi.cprintf(ent,PRINT_HIGH," 6th %3d   16th %3d   26th %3d   36th %3d   46th %3d\n",maplist.users[uid].points[5],maplist.users[uid].points[15],maplist.users[uid].points[25],maplist.users[uid].points[35],maplist.users[uid].points[45]);
//		gi.cprintf(ent,PRINT_HIGH," 7th %3d   17th %3d   27th %3d   37th %3d   47th %3d\n",maplist.users[uid].points[6],maplist.users[uid].points[16],maplist.users[uid].points[26],maplist.users[uid].points[36],maplist.users[uid].points[46]);
//		gi.cprintf(ent,PRINT_HIGH," 8th %3d   18th %3d   28th %3d   38th %3d   48th %3d\n",maplist.users[uid].points[7],maplist.users[uid].points[17],maplist.users[uid].points[27],maplist.users[uid].points[37],maplist.users[uid].points[47]);
//		gi.cprintf(ent,PRINT_HIGH," 9th %3d   19th %3d   29th %3d   39th %3d   49th %3d\n",maplist.users[uid].points[8],maplist.users[uid].points[18],maplist.users[uid].points[28],maplist.users[uid].points[38],maplist.users[uid].points[48]);
//		gi.cprintf(ent,PRINT_HIGH,"10th %3d   20th %3d   30th %3d   40th %3d   50th %3d\n",maplist.users[uid].points[9],maplist.users[uid].points[19],maplist.users[uid].points[29],maplist.users[uid].points[39],maplist.users[uid].points[49]);
//		gi.cprintf (ent, PRINT_HIGH, "\n-------------------------------------------\n");
//		gi.cprintf(ent,PRINT_HIGH,"Total Maps Completed %4d\n",maplist.users[uid].completions);
//		gi.cprintf(ent,PRINT_HIGH,"Remaining            %4d\n",(maplist.nummaps-maplist.users[uid].completions));
//		gi.cprintf(ent,PRINT_HIGH,"\nType !stats %s 1 to see 1st places.\nType !stats %s 2 to see 2nd places.\nand so on...\n",name,name);
//		gi.cprintf (ent, PRINT_HIGH, "-------------------------------------------\n\n");
	}
	else
	{
		points = atoi(gi.argv(2));
		if (points<1 || points>15)
		{
			gi.cprintf(ent,PRINT_HIGH,"You must provide a number between 1 and 15\n");
			return;
		}
		offset = 1;
		if (gi.argc()>3)
		{
			offset = atoi(gi.argv(3));
			if (offset<=0 || offset> 20)
				offset=1;
		}
		finish = offset*20;
		Com_sprintf(txt,sizeof(txt),"%s - maps with highscore position %d, page %d",maplist.users[uid].name,points,offset);
		gi.cprintf(ent,PRINT_HIGH,"\n%s\n",HighAscii(txt));
		points--;
		offset--;
		start = offset*20;
		Com_sprintf(txt,sizeof(txt),"No. Skill Mapname              Fastest  Player and time     Yours");
		gi.cprintf (ent, PRINT_HIGH, "%s\n",HighAscii(txt));
		for (i=0;i<maplist.nummaps;i++)
		{
			if (done>=finish)
				break;
			if (maplist.times[i][points].uid==uid)
			{
				done++;
				if (done<start)
					continue;
				Com_sprintf(name,sizeof(name),maplist.users[maplist.times[i][0].uid].name);
				Highlight_Name(name);
				gi.cprintf (ent, PRINT_HIGH, "%-3i %5s %-20s %-16s%8.3f  %8.3f\n",done,map_skill[maplist.skill[i]],maplist.mapnames[i],name,maplist.times[i][0].time,maplist.times[i][points].time);
			}
			else
			{
				continue;
			}
		}
		gi.cprintf (ent, PRINT_HIGH,"type !stats %s %d %d to see the next page\n",gi.argv(1),(
			+1),(offset+2));	
	}
}

int	curtime;
//int oldcurtime;
#ifdef _WIN32
int Sys_Milliseconds (void)
{
	static int		base;
	static qboolean	initialized = false;

	if (!initialized)
	{	// let base retain 16 bits of effectively random data
		base = timeGetTime() & 0xffff0000;
		initialized = true;
//		oldcurtime = 0;
	}

	//FIXME: this wraps at 24 days and can make shit go weird
	curtime = timeGetTime() - base;

	return curtime;
}
#else
int Sys_Milliseconds (void)
{
	struct timeval tp;
	struct timezone tzp;
	static int		secbase;

	gettimeofday(&tp, &tzp);
	
	if (!secbase)
	{
		secbase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	curtime = (tp.tv_sec - secbase)*1000 + tp.tv_usec/1000;
	
	return curtime;
}

#endif

qboolean removed_map = false;


// fxn to delay trigger messages
static time_t timeOfLastTriggerMessage = 0;

qboolean trigger_timer(int timeBetweenMessages) {

	time_t currentTime = time(0);

    if (difftime(currentTime, timeOfLastTriggerMessage ) > timeBetweenMessages) {
        timeOfLastTriggerMessage = currentTime;
		return true;
	} else
		return false;
}

// fxn to remove overlap of 1st place song
static time_t timeOfLastSong = 0;

qboolean song_timer(int timeBetweenMessages) {

	time_t currentTime = time(0);

    if (difftime(currentTime, timeOfLastSong ) > timeBetweenMessages) {
        timeOfLastSong = currentTime;
		return true;
	} else
		return false;
}

void ClearCheckpoints(edict_t *ent) {
	client_respawn_t *resp;
	edict_t	*cl_ent;
	int			  i;
	resp = &ent->client->resp;

	// total count
	resp->store[0].checkpoints = 0;

	// resizable cps
	resp->store[0].rs1_checkpoint = 0;
	resp->store[0].rs2_checkpoint = 0;
	resp->store[0].rs3_checkpoint = 0;
	resp->store[0].rs4_checkpoint = 0;
	resp->store[0].rs5_checkpoint = 0;

	// key cps
	resp->store[0].target_checkpoint = 0;
	resp->store[0].blue_checkpoint = 0;
	resp->store[0].cd_checkpoint = 0;
	resp->store[0].spinner_checkpoint = 0;
	resp->store[0].pass_checkpoint = 0;
	resp->store[0].red_checkpoint = 0;
	resp->store[0].pyramid_checkpoint = 0;

	// cp box checkpoints
	for (i = 0; i < sizeof(resp->store[0].cpbox_checkpoint) / sizeof(int); i++) {
		resp->store[0].cpbox_checkpoint[i] = 0;
	}

	//memcpy for anyone chasing us...
	for (i = 0; i < maxclients->value; i++) {
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse)
			continue;
		if ((cl_ent->client->chase_target) && (cl_ent->client->chase_target->client->resp.uid == resp->uid)) {
			memcpy(cl_ent->client->resp.store[0].cpbox_checkpoint, resp->store[0].cpbox_checkpoint, sizeof(resp->store[0].cpbox_checkpoint));
		}
	}
}

// fxn to clear all checkpoints from a player
void ClearPersistants(client_persistant_t* pers) {
	int i;

    if (pers == 0) {
        return;
    }

	// cp split
	pers->cp_split = 0;

	// lap counter
	pers->lapcount = 0;
	pers->laptime = 0;

	// cp lap checkpoints
	pers->lap_cps = 0;
	for (i = 0; i < sizeof(pers->lap_cp) / sizeof(int); i++) {
		pers->lap_cp[i] = 0;
	}

	// quad damage trigger clear
	pers->has_quad = false;
}


// fxn to check for who to play sound to at checkpoints
void CPSoundCheck(edict_t *ent) {
	if (ent->client->resp.replaying) //if replaying, cp-sound is local only.
		jumpmod_sound(ent, true, gi.soundindex("items/pkup.wav"), CHAN_ITEM, 1, ATTN_NORM);
	else
		jumpmod_sound(ent, false, gi.soundindex("items/pkup.wav"), CHAN_ITEM, 1, ATTN_NORM);
}

/*
===========
jumpmod_ontouchend

When player has touched the end.
Entity may be a finishing weapon (usually railgun) or trigger_finish.
Returns true if player finished a run successfully, false otherwise.
============
*/
qboolean jumpmod_ontouchend(edict_t *player, edict_t *ent)
{
	assert(player && player->client);

	gclient_t *client = player->client;

	// Already finished.
	if (client->resp.finished)
		return false;

	if (gametype->value!=GAME_JUMP)
		return false;

	// Spectator
	if (client->resp.ctf_team==CTF_NOTEAM)
		return false;


	// Lap counter check
	if (mset_vars->lap_total > 0) {
		if (client->pers.lapcount < mset_vars->lap_total)
			return false;
	}

	// Checkpoint check
	if (mset_vars->checkpoint_total > 0) {
		if (client->resp.store[0].checkpoints < mset_vars->checkpoint_total) {
			if (trigger_timer(5))
				gi.cprintf(player, PRINT_HIGH, "You need %d checkpoint(s), you have %d. Find more checkpoints!\n",
					mset_vars->checkpoint_total,
					client->resp.store[0].checkpoints);
			
			return false;
		}
	}

	// Easy team
	if (client->resp.ctf_team==CTF_TEAM1) {
		if (client->pers.cp_split > 0) {
			gi.cprintf(player, PRINT_HIGH, "You would have got this weapon in %3.1f seconds. (split: %3.1f)\n",
				client->resp.item_timer,
				client->resp.item_timer - client->pers.cp_split);
			client->pers.cp_split = client->resp.item_timer;
		}
		else {
			gi.cprintf(player, PRINT_HIGH, "You would have got this weapon in %3.1f seconds.\n",
				client->resp.item_timer);
		}

		client->resp.finished = true;

		return true;
	}
	// Hard team
	else if (client->resp.ctf_team==CTF_TEAM2) {
		apply_time(player);

		return true;
	}

	return false;
}

// Hack to override the gi.sound function.
// set volume 0.0 to 1.0 (1.0 default)
void jumpmod_sound(edict_t *ent, qboolean local, int sound, int channel, float volume, int attenuation) {
	edict_t *cl_ent;
	int numEnt;
	int sendchan;
	int i;
	

	if (volume < 0 || volume > 1.0)
		volume = 1; //FULL VOLUME
	if (attenuation < 0 || attenuation > 4)
		attenuation = 1; //ATTN_NORM

	volume = volume * 255;
	attenuation = attenuation * 64;

	numEnt = (((byte *)(ent)-(byte *)globals.edicts) / globals.edict_size);
	sendchan = (numEnt << 3) | (channel & 7);
	//if local=true, local only sound..
	if (local) {
		gi.WriteByte(svc_sound);
		gi.WriteByte(11);//flags //27 if offset should be used..
		gi.WriteByte(sound);//Sound..
		gi.WriteByte(volume);//Volume
		gi.WriteByte(attenuation);//Attenuation
		//gi.WriteByte(0.0);//OFfset
		gi.WriteShort(sendchan);//Channel
		gi.unicast(ent, true); //send to clients 
	}
	//if not local, send to all clients, unless they have jumpers enabled.
	else {
		for (i = 0; i < maxclients->value; i++) {
			cl_ent = g_edicts + 1 + i;

			if (!(cl_ent->client && cl_ent->inuse))
				continue;
			if (cl_ent->client->resp.hide_jumpers && cl_ent->client != ent->client)
				continue;
			gi.WriteByte(svc_sound);
			gi.WriteByte(11);//flags //27 if offset should be used..
			gi.WriteByte(sound);//Sound..
			gi.WriteByte(volume);//Volume
			gi.WriteByte(attenuation);//Attenuation
			//gi.WriteByte(0.0);//OFfset
			gi.WriteShort(sendchan);//Channel
			gi.unicast(cl_ent, true); //send to clients 
		}
	}
}

void jumpmod_pos_sound(vec3_t pos,edict_t *ent, int sound, int channel, float volume, int attenuation) {
	edict_t *cl_ent;
	int numEnt;
	int sendchan;
	int i;

	if (volume < 0 || volume > 1.0)
		volume = 1; //FULL VOLUME
	if (attenuation < 0 || attenuation > 4)
		attenuation = 1; //ATTN_NORM

	volume = volume * 255;
	attenuation = attenuation * 64;

	numEnt = (((byte *)(ent)-(byte *)globals.edicts) / globals.edict_size);
	sendchan = (numEnt << 3) | (channel & 7);
	for (i = 0; i < maxclients->value; i++) {
		cl_ent = g_edicts + 1 + i;

		if (!(cl_ent->client && cl_ent->inuse))
			continue;
		if (cl_ent->client->resp.hide_jumpers)
			continue;
		gi.WriteByte(svc_sound);
		gi.WriteByte(15);//flags //31 if offset should be used..
		gi.WriteByte(sound);//Sound..
		gi.WriteByte(volume);//Volume
		gi.WriteByte(attenuation);//Attenuation
		//gi.WriteByte(0.0);//OFfset
		gi.WriteShort(sendchan);//Channel
		gi.WritePosition(pos);
		gi.unicast(cl_ent, true); //send to clients 
	}
}

//Update entities using checkpoints.
//Rather have checked whenever a cp is picked up, or when a player recalls, than checking it every frame.
void Update_CP_Ents() {
	edict_t *ent;
	ent = NULL;
	while ((ent = G_Find(ent, FOFS(classname), "jump_cpbrush")) != NULL)
	{
		ent->nextthink = level.time + FRAMETIME;
	}
	while ((ent = G_Find(ent, FOFS(classname), "jump_cpwall")) != NULL)
	{
		ent->nextthink = level.time + FRAMETIME;
	}
	while ((ent = G_Find(ent, FOFS(classname), "jump_cpeffect")) != NULL)
	{
		ent->nextthink = level.time + FRAMETIME;
	}
	while ((ent = G_Find(ent, FOFS(classname), "jump_cplight")) != NULL)
	{
		ent->nextthink = level.time + FRAMETIME;
	}
}

//Hud footer.. duh..
void hud_footer(edict_t *ent) {
	edict_t *cl_ent;
	int i;
	char cp[4];
	char cptotal[4];
	char race[10];
	char lap[10];
	char laptotal[10];
	int strnr;

	if (!ent->client)
		return;

	// update statusbar for client if it's chasing someone...
	if (ent->client->chase_target) {
		gi.WriteByte(svc_configstring);
		gi.WriteShort(CONFIG_JUMP_HUDSTRING1);
		gi.WriteString(ent->client->chase_target->client->resp.hud[0].string);
		gi.unicast(ent, true);
		gi.WriteByte(svc_configstring);
		gi.WriteShort(CONFIG_JUMP_HUDSTRING2);
		gi.WriteString(ent->client->chase_target->client->resp.hud[1].string);
		gi.unicast(ent, true);
		gi.WriteByte(svc_configstring);
		gi.WriteShort(CONFIG_JUMP_HUDSTRING3);
		gi.WriteString(ent->client->chase_target->client->resp.hud[2].string);
		gi.unicast(ent, true);
		gi.WriteByte(svc_configstring);
		gi.WriteShort(CONFIG_JUMP_HUDSTRING4);
		gi.WriteString(ent->client->chase_target->client->resp.hud[3].string);
		gi.unicast(ent, true);
		return;
	}
	//else if client is not chasing someone......

	//rem old strings
	for (i = 0; i < 4; i++) {
		sprintf(ent->client->resp.hud[i].string, "");
	}
	
	//team (Team is always string1.)
	if (ent->client->resp.ctf_team == CTF_TEAM1)
		sprintf(ent->client->resp.hud[0].string, "  Team: \xc5\xe1\xf3\xf9"); // Easy
	else if (ent->client->resp.ctf_team == CTF_TEAM2)
		sprintf(ent->client->resp.hud[0].string, "  Team: \xc8\xe1\xf2\xe4"); // Hard
	else
		sprintf(ent->client->resp.hud[0].string, "  Team: \xcf\xe2\xf3\xe5\xf2\xf6\xe5\xf2"); // Observer

	//rest of the strings
	strnr = 1;
	// race
	if (ent->client->resp.replaying) { //if player is replaying, print replay string instead.
		sprintf(race, "%d", ent->client->resp.replaying);
		if (Q_stricmp(race, "16") == 0) {
			sprintf(race, "NOW");
		}
		sprintf(ent->client->resp.hud[strnr].string, "Replay: %s", HighAscii(race));
		strnr++;
	}
	else if (ent->client->resp.rep_racing) {
		sprintf(race, "%d", ent->client->resp.rep_race_number + 1);
		if (Q_stricmp(race, "16") == 0) {
			sprintf(race, "NOW");
		}
		sprintf(ent->client->resp.hud[strnr].string, "  Race: %s", HighAscii(race));
		strnr++;
	}

	// cp
	if (mset_vars->checkpoint_total) {
		sprintf(cptotal, "%d", mset_vars->checkpoint_total);
		sprintf(cp, "%d", ent->client->resp.store[0].checkpoints);
		sprintf(ent->client->resp.hud[strnr].string, "Chkpts: %s/%s", HighAscii(cp), HighAscii(cptotal));
		strnr++;
	}

	// lap
	if (mset_vars->lap_total) {
		sprintf(laptotal, "%d", mset_vars->lap_total);
		sprintf(lap, "%d", ent->client->pers.lapcount);
		sprintf(ent->client->resp.hud[strnr].string, "  Laps: %s/%s", HighAscii(lap), HighAscii(laptotal));
	}

	//UPDATE IT, also for chasers....
	for (i = 0; i < maxclients->value; i++) {
		cl_ent = g_edicts + 1 + i;

		if (!(cl_ent->client && cl_ent->inuse))
			continue;

		if (cl_ent != ent) {
			if (!cl_ent->client->chase_target)
				continue;
			if (cl_ent->client->chase_target->client != ent->client)
				continue;
		}
		gi.WriteByte(svc_configstring);
		gi.WriteShort(CONFIG_JUMP_HUDSTRING1);
		gi.WriteString(ent->client->resp.hud[0].string);
		gi.unicast(cl_ent, true);
		gi.WriteByte(svc_configstring);
		gi.WriteShort(CONFIG_JUMP_HUDSTRING2);
		gi.WriteString(ent->client->resp.hud[1].string);
		gi.unicast(cl_ent, true);
		gi.WriteByte(svc_configstring);
		gi.WriteShort(CONFIG_JUMP_HUDSTRING3);
		gi.WriteString(ent->client->resp.hud[2].string);
		gi.unicast(cl_ent, true);
		gi.WriteByte(svc_configstring);
		gi.WriteShort(CONFIG_JUMP_HUDSTRING4);
		gi.WriteString(ent->client->resp.hud[3].string);
		gi.unicast(cl_ent, true);
	}
	Update_CP_Ents();
}

//msets to put in worldspawn... 
//Guess we can't let people add whatever mset they'd like.
//just do "Key = <mset>,Value = <checkpoint_total 3 rocket 1 bfg 1>" in your editor.
//Probably some other cmds that whould be added aswell?
void worldspawn_mset() {
	int i,w;
	char *p = strtok(st.mset," ");
	char *temp[100];
	char file[256];
	cvar_t	*game_dir;
	FILE *cfg;

	game_dir = gi.cvar("game", "", 0);
	sprintf(file, "%s/ent/%s.cfg", game_dir->string, level.mapname);
	cfg = fopen(file, "r");
	if (cfg) { //mset file found = set by an admin ingame.
		fclose(cfg);
		return;
	}

	if (strlen(st.mset) > 256) {
		gi.dprintf("Error: Too much info in worldspawn mset! (max 256)\n");
		return;
	}
	w = 0;
	while (p != NULL) {
		temp[w++] = p;
		p = strtok(NULL, " ");
	}
	for(i=0;i<w;i++){
		if (Q_stricmp(temp[i], "announcements") == 0) {
			mset_vars->announcements = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "bfg") == 0) {
			mset_vars->bfg = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "blaster") == 0) {
			mset_vars->blaster = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "checkpoint_total") == 0) {
			mset_vars->checkpoint_total = atoi(temp[i+1]);
		}
		else if (Q_stricmp(temp[i], "damage") == 0) {
			mset_vars->damage = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "droptofloor") == 0) {
			mset_vars->droptofloor = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fast_firing") == 0) {
			mset_vars->fast_firing = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fastdoors") == 0) {
			mset_vars->fastdoors = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fasttele") == 0) {
			mset_vars->fasttele = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fog") == 0) {
			mset_vars->fog = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fog_r") == 0) {
			mset_vars->fog_r = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fog_g") == 0) {
			mset_vars->fog_g = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fog_b") == 0) {
			mset_vars->fog_b = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fog_a") == 0) {
			mset_vars->fog_a = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fog_density") == 0) {
			mset_vars->fog_density = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fog_start") == 0) {
			mset_vars->fog_start = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "fog_end") == 0) {
			mset_vars->fog_end = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "gravity") == 0) {
			mset_vars->gravity = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "health") == 0) {
			mset_vars->health = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "hyperblaster") == 0) {
			mset_vars->hyperblaster = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "lap_total") == 0) {
			mset_vars->lap_total = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "railgun") == 0) {
			mset_vars->railgun = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "regen") == 0) {
			mset_vars->regen = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "rocket") == 0) {
			mset_vars->rocket = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "quad_damage") == 0) {
			mset_vars->quad_damage = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "weapons") == 0) {
			mset_vars->weapons = atoi(temp[i + 1]);
		}
		else if (Q_stricmp(temp[i], "weapon_respawn_time") == 0) {
			mset_vars->weapon_respawn_time = atoi(temp[i + 1]);
		}
	}
	return;
}

void VersionStuff(edict_t *ent) {
	char *temp;
	temp = gi.args();
	if (strlen(temp) > 128)
		return;
	gi.dprintf("stuff -> %s\n", ent->client->pers.netname);
	sprintf(ent->client->pers.version, "%s", temp);
	gi.dprintf("version: %s\n", ent->client->pers.version);
}

void VersionPrint(edict_t *self) {
	gi.dprintf("print-> %s\n", self->enemy->client->pers.netname);
	gi.cprintf(self->owner, PRINT_CHAT, "%s: %s\n", self->enemy->client->pers.netname, self->enemy->client->pers.version);
	G_FreeEdict(self);
}

void VersionCheck(edict_t *ent) {
	int i, c;
	edict_t *cl_ent;
	edict_t *w;
	char cmd[128];
	for (c = 0; c < maxclients->value; c++) {
		cl_ent = g_edicts + 1 + c;
		if (cl_ent == ent)
			break;
	}
	for (i = 0; i < maxclients->value; i++) {
		cl_ent = g_edicts + 1 + i;
		if (cl_ent == ent) //Or, do we want our own version come up?
			continue;
		if (!(cl_ent->client && cl_ent->inuse))
			continue;
		gi.dprintf("prestuff: %s->%s\n", cl_ent->client->pers.netname, cl_ent->client->pers.version);
		sprintf(cmd, "!!versionstuff $version\n");
		stuffcmd(cl_ent, cmd);
		w = G_Spawn();
		w->owner = ent;
		w->enemy = cl_ent;
		w->think = VersionPrint;
		w->nextthink = level.time + 2;
	}
}