#include "g_local.h"


char zbbuffer2[256];
int mset_timelimit;
mset_vars_t mset_vars[1];
gset_vars_t gset_vars[1];
aset_vars_t aset_vars[1];


zbotcmd_t zbotCommands[] =
{

	//----------------------------
	//         mset's
	//----------------------------
	{
	  0,1,0,
	  "addedtimeoverride",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->addedtimeoverride,
	},
	{
	  0,1,0,
	  "allowsrj",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->allowsrj,
	},
	{
	  0,1,0,
	  "announcements",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->announcements,
	},
	{
	  0,1,0,
	  "bfg",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->bfg,
	},
	{
	  0,1,0,
	  "blaster",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->blaster,
	},
	{
	  0,28,0,
	  "checkpoint_total",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->checkpoint_total,
	},
	{
	  0,1,0,
	  "cmsg",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->cmsg,
	},
	{
	  0,1,1,
	  "damage",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->damage,
	},
	{
	  0,1,1,
	  "droptofloor",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->droptofloor,
	},
	{
	  0,0,0,
	  "edited_by",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_STRING,
	  &mset_vars->edited_by,
	},
	{
	  0,1,1,
	  "falldamage",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->falldamage,
	},
	{
	  0,1,0,
	  "fast_firing",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fast_firing,
	},
	{
	  0,1,0,
	  "fastdoors",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fastdoors,
	},
	{
	  0,1,0,
	  "fasttele",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fasttele,
	},
	{
	  0,3,0,
	  "fog",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fog,
	},
	{
	  0,255,125,
	  "fog_r",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fog_r,
	},
	{
	  0,255,125,
	  "fog_g",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fog_g,
	},
	{
	  0,255,135,
	  "fog_b",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fog_b,
	},
	{
	  0,255,130,
	  "fog_a",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fog_a,
	},
	{
	  0,100,20,
	  "fog_density",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fog_density,
	},
	{
	  0,10000,1,
	  "fog_start",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fog_start,
	},
	{
	  0,10000,2500,
	  "fog_end",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->fog_end,
	},
	{
	  0,1,1,
	  "ghost",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->ghost,
	},
	{
	  0,128,0,
	  "ghosty_model",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->ghost_model,
	},
	{
	  -10000,10000,800,
	  "gravity",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->gravity,
	},
	{
	  0,999,400,
	  "health",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->health,
	},
	{
	  0,1,0,
	  "hyperblaster",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->hyperblaster,
	},
	{
	  0,100,0,
	  "lap_total",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->lap_total,
	},
	{
	  0,6,0,
	  "quad_damage",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->quad_damage,
	},
	{
	  -100,100,100,
	  "regen",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->regen,
	},
	{
	  0,1,0,
	  "rocket",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->rocket,
	},
	{
	  0,1,0,
	  "rocketjump_fix",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->rocketjump_fix,
	},
	{
	  0,1,0,
	  "singlespawn",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->singlespawn,
	},
	{
	  0,1,0,
	  "slowdoors",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->slowdoors,
	},
	{
	  0,999,20,
	  "timelimit",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->timelimit,
	},
	{
	  0,1,0,
	  "weapons",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->weapons,
	},
	{
	  0,1000,0,
	  "weapon_respawn_time",
	  CMDWHERE_CFGFILE | CMD_MSET,
	  CMDTYPE_NUMBER,
	  &mset_vars->weapon_respawn_time,
	},

	//----------------------------
	//         gset's
	//----------------------------
	{
	  0,999,10,
	  "addedtimemap",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->addedtimemap,
	},
	{
	  0,1,0,
	  "gaddedtimeoverride",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->addedtimeoverride,
	},
	{
	  0,1,1,
	  "addtime_announce",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->addtime_announce,
	},
	{
	  0,1,0,
	  "admin_max_addtime",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->admin_max_addtime,
	},
	{
	  0,0,0,
	  "admin_model",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_STRING,
	  &gset_vars->admin_model,
	},
	{
	  0,1,1,
	  "allow_admin_boot",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->allow_admin_boot,
	},
  #ifdef RACESPARK
	{
	  0,1,1,
	  "allow_race_spark",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->allow_race_spark,
	},
  #endif
	{
	  0,1,0,
	  "gallowsrj",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->allowsrj,
	},
	{
	  0,1,0,
	  "gannouncements",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->announcements,
	},
	{
	  0,100,10,
	  "autotime",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->autotime,
	},
	{
	  0,2147483647,0,
	  "gbest_time_glow",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->best_time_glow,
	},
	{
	  0,1,0,
	  "gbfg",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->bfg,
	},
	{
	  0,1,0,
	  "gblaster",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->blaster,
	},
	{
	  0,28,0,
	  "gcheckpoint_total",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->checkpoint_total,
	},
	{
	  0,1,0,
	  "gcmsg",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->cmsg,
	},
	{
	  0,1,1,
	  "cvote_announce",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->cvote_announce,
	},
	{
	  0,1,1,
	  "gdamage",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->damage,
	},
	{
	  0,1,1,
	  "debug",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->debug,
	},
	{
	  0,1,0,
	  "dev",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->dev,
	},
	{
	  0,1,1,
	  "gdroptofloor",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->droptofloor,
	},
	{
	  0,0,0,
	  "gedited_by",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_STRING,
	  &gset_vars->mset->edited_by,
	},
	{
	  0,1,1,
	  "gfalldamage",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->falldamage,
	},
	{
	  0,1,0,
	  "gfast_firing",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fast_firing,
	},
	{
	  0,1,0,
	  "gfastdoors",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fastdoors,
	},
	{
	  0,1,0,
	  "gfasttele",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fasttele,
	},
	{
	  0,2,1,
	  "flashlight",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->flashlight,
	},
	{
	  0,3,0,
	  "gfog",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fog,
	},
	{
	  0,255,125,
	  "gfog_r",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fog_r,
	},
	{
	  0,255,125,
	  "gfog_g",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fog_g,
	},
	{
	  0,255,125,
	  "gfog_b",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fog_b,
	},
	{
	  0,255,135,
	  "gfog_a",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fog_a,
	},
	{
	  0,100,20,
	  "gfog_density",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fog_density,
	},
	{
	  0,10000,1,
	  "gfog_start",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fog_start,
	},
	{
	  0,10000,2500,
	  "gfog_end",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->fog_end,
	},
	{
	  0,1,1,
	  "fpskick",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->fpskick,
	},
	{
	  0,1,1,
	  "gghost",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->ghost,
	},
	{
	  0,2147483647,0,
	  "ghost_glow",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->ghost_glow,
	},
	{
	  0,128,0,
	  "gghosty_model",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->ghost_model,
	},
	{
	  0,9999999,0,
	  "glow_admin",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->glow_admin,
	},
	{
	  0,1,1,
	  "glow_multi",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->glow_multi,
	},
	{
	  -10000,10000,800,
	  "ggravity",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->gravity,
	},
	{
	  0,999,400,
	  "ghealth",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->health,
	},
	{
	  0,2000,300,
	  "hideghost",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->hideghost,
	},
	{
	  0,1,1,
	  "holdtime",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->holdtime,
	},
	{
	  0,1,1,
	  "hook",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->hook,
	},
	{
	  1,10000,750,
	  "hookpull",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->hookpull,
	},
	{
	  1,10000,1200,
	  "hookspeed",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->hookspeed,
	},
	{
	  0,50,8,
	  "html_bestscores",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->html_bestscores,
	},
	{
	  0,1,1,
	  "html_create",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->html_create,
	},
	{
	  0,20,10,
	  "html_firstplaces",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->html_firstplaces,
	},
	{
	  1,9,1,
	  "html_profile",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->html_profile,
	},
	{
	  0,1,0,
	  "ghyperblaster",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->hyperblaster,
	},
	{
	  1,999,50,
	  "intermission",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->intermission,
	},
	{
	  0,1,0,
	  "invis",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->invis,
	},
	{
	  0,1,1,
	  "jetpack",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->jetpack,
	},
	{
	  0,500,1,
	  "kill_delay",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->kill_delay,
	},
	{
	  0,100,0,
	  "glap_total",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->lap_total,
	},
	{
	  0,1,1,
	  "map_end_warn_sounds",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->map_end_warn_sounds,
	},
	{
	  0,1,0,
	  "maplist_times",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->maplist_times,
	},
	{
	  0,MAX_MAPMEM,7,
	  "maps_pass",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->maps_pass,
	},
	{
	  0,10,3,
	  "max_votes",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->max_votes,
	},
	{
	  0,0,0,
	  "model_store",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_STRING,
	  &gset_vars->model_store,
	},
	{
	  0,10800,300,
	  "nomapvotetime",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->nomapvotetime,
	},
	{
	  0,10800,300,
	  "notimevotetime",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->notimevotetime,
	},
	{
	  0,10000,17,
	  "numberone_length",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->numberone_length,
	},
	{
	  0,0,0,
	  "numberone_wav",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_STRING,
	  &gset_vars->numberone_wav,
	},
	{
	  1,9,1,
	  "numsoundwavs",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->numsoundwavs,
	},
	{
	  1,999,25,
	  "overtimegainedhealth",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->overtimegainedhealth,
	},
	{
	  1,999,150,
	  "overtimehealth",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->overtimehealth,
	},
	{
	  1,60,3,
	  "overtimelimit",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->overtimelimit,
	},
	{
	  1,50,1,
	  "overtimerandom",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->overtimerandom,
	},
	{
	  0,4,2,
	  "overtimetype",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->overtimetype,
	},
	{
	  11,60,20,
	  "overtimewait",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->overtimewait,
	},
	{
	  0,999,1,
	  "playsound",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->playsound,
	},
	{
	  0,1,0,
	  "playtag",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->playtag,
	},
	{
	  0,1,1,
	  "pvote_announce",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->pvote_announce,
	},
	{
	  0,6,0,
	  "gquad_damage",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->quad_damage,
	},
	{
	  -100,100,100,
	  "gregen",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->regen,
	},
	{
	  0,1,1,
	  "respawn_sound",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->respawn_sound,
	},
	{
	  0,1,0,
	  "grocket",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->rocket,
	},
	{
	  0,1,0,
	  "store_safe",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->store_safe,
	},
	{
	  0,1,0,
	  "gsinglespawn",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->singlespawn,
	},
	{
	  0,1,0,
	  "gslowdoors",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->slowdoors,
	},
	{
	  0,2147483647,2,
	  "target_glow",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->target_glow,
	},
	{
	  0,1,1,
	  "temp_ban_on_kick",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->temp_ban_on_kick,
	},
	{
	  0,1000,0,
	  "time_adjust",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->time_adjust,
	},
	{
	  0,999,20,
	  "gtimelimit",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->timelimit,
	},
	{
	  0,1,0,
	  "gtourney",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->tourney,
	},
	{
	  0,1,0,
	  "transparent",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->transparent,
	},
	{
	  0,120,60,
	  "voteextratime",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->voteextratime,
	},
	{
	  0,1000,20,
	  "voteseed",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->voteseed,
	},
	{
	  1,60,20,
	  "votingtime",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->votingtime,
	},
	{
	  0,1,1,
	  "walkthru",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->walkthru,
	},
	{
	  1,99999,500,
	  "weapon_fire_min_delay",
	  CMDWHERE_CFGFILE | CMD_GSET,
	  CMDTYPE_NUMBER,
	  &gset_vars->weapon_fire_min_delay,
	},
	{
	  0,1000,0,
	  "gweapon_respawn_time",
	  CMDWHERE_CFGFILE | CMD_GSET | CMD_GSETMAP,
	  CMDTYPE_NUMBER,
	  &gset_vars->mset->weapon_respawn_time,
	},

	//----------------------------
	//         aset's
	//----------------------------

	  {
	  1,20,1,
	  "ADMIN_ADDBALL_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_ADDBALL_LEVEL
	  },

	  {
	  1,20,3,
	  "ADMIN_ADDENT_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_ADDENT_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_ADDMAP_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_ADDMAP_LEVEL
	  },

	  {
	  1,20,2,
	  "ADMIN_ADDTIME_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_ADDTIME_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_ADMINEDIT_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_ADMINEDIT_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_BAN_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_BAN_LEVEL
	  },

	  {
	  1,20,2,
	  "ADMIN_BOOT_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_BOOT_LEVEL
	  },

	  {
	  1,20,1,
	  "ADMIN_BRING_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_BRING_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_CHANGENAME_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_CHANGENAME_LEVEL
	  },

	  {
	  1,20,1,
	  "ADMIN_DUMMYVOTE_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_DUMMYVOTE_LEVEL
	  },

	  {
	  1,20,1,
	  "ADMIN_GIVE_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_GIVE_LEVEL
	  },

	  {
	  1,20,1,
	  "ADMIN_GIVEALL_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_GIVEALL_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_GSET_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_GSET_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_IP_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_IP_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_MAX_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_MAX_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_MODEL_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_MODEL_LEVEL
	  },

	  {
	  1,20,4,
	  "ADMIN_MSET_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_MSET_LEVEL
	  },

	  {
	  1,20,2,
	  "ADMIN_NEXTMAPS_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_NEXTMAPS_LEVEL
	  },

	  {
	  1,20,2,
	  "ADMIN_NOMAXVOTES_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_NOMAXVOTES_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_RATERESET_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_RATERESET_LEVEL
	  },

	  {
	  1,20,4,
	  "ADMIN_REMTIMES_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_REMTIMES_LEVEL
	  },

	  {
	  1,20,1,
	  "ADMIN_SILENCE_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_SILENCE_LEVEL
	  },

	  {
	  1,20,1,
	  "ADMIN_SLAP_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_SLAP_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_SORTMAPS_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_SORTMAPS_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_STUFF_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_STUFF_LEVEL
	  },

	  {
	  1,20,1,
	  "ADMIN_THROWUP_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_THROWUP_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_TOGGLEHUD_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_TOGGLEHUD_LEVEL
	  },

	  {
	  1,20,5,
	  "ADMIN_UPDATESCORES_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_UPDATESCORES_LEVEL
	  },

	  {
	  1,20,2,
	  "ADMIN_VOTE_LEVEL",
	  CMDWHERE_CFGFILE | CMD_ASET,
	  CMDTYPE_NUMBER,
	  &aset_vars->ADMIN_VOTE_LEVEL
	  }

};

#define ZBOTCOMMANDSSIZE    (sizeof(zbotCommands) / sizeof(zbotCommands[0]))
char zbbuffer[0x10000];


void List_mset_commands(edict_t *ent, int offset)
{
	int i;
	int printed;
	int listed;

	offset--;

	gi.cprintf(ent, PRINT_HIGH, "-------------------------------------------------\n");

	printed = 0;
	i = 0;
	listed = 0;
	while (i < ZBOTCOMMANDSSIZE)
	{
		if (printed >= 20)
			break;


		if (zbotCommands[i].cmdwhere & CMD_MSET)
		{
			if (listed < offset * 20)
			{
				//skip thru until we get to right page

			}
			else
			{
				//print it
				gi.cprintf(ent, PRINT_HIGH, "  %-32s", zbotCommands[i].cmdname);
				switch (zbotCommands[i].cmdtype)
				{
				case CMDTYPE_LOGICAL:
					gi.cprintf(ent, PRINT_HIGH, "  %d\n", *((qboolean *)zbotCommands[i].datapoint));
					break;
				case CMDTYPE_NUMBER:
					gi.cprintf(ent, PRINT_HIGH, "  %d\n", *((int *)zbotCommands[i].datapoint));
					break;
				case CMDTYPE_STRING:
					gi.cprintf(ent, PRINT_HIGH, "  %s\n", zbotCommands[i].datapoint);

					break;
				}
				printed++;
			}
			listed++;
		}
		i++;
	}

	gi.cprintf(ent, PRINT_HIGH, "Page %d/%1.0f (%i commands).\n", (offset + 1), ceil(num_mset_commands / 20.0), num_mset_commands);

	gi.cprintf(ent, PRINT_HIGH, "-------------------------------------------------\n");
}

void List_gset_commands(edict_t *ent, int offset)
{
	int i, i2;
	int printed;
	int listed;
	char temp[64];
	offset--;

	gi.cprintf(ent, PRINT_HIGH, "-------------------------------------------------\n");

	printed = 0;
	i = 0;
	listed = 0;
	while (i < ZBOTCOMMANDSSIZE)
	{
		if (printed >= 20)
			break;


		if (zbotCommands[i].cmdwhere & CMD_GSET)
		{
			if (listed < offset * 20)
			{
				//skip thru until we get to right page

			}
			else
			{
				if (zbotCommands[i].cmdwhere & CMD_GSETMAP)
				{
					for (i2 = 1; i2 < strlen(zbotCommands[i].cmdname); i2++)
					{
						temp[i2 - 1] = zbotCommands[i].cmdname[i2];
					}
					temp[strlen(zbotCommands[i].cmdname) - 1] = 0;
					//print it
					gi.cprintf(ent, PRINT_HIGH, "  %-32s", temp);
				}
				else
				{
					//print it
					gi.cprintf(ent, PRINT_HIGH, "  %-32s", zbotCommands[i].cmdname);
				}
				switch (zbotCommands[i].cmdtype)
				{
				case CMDTYPE_LOGICAL:
					gi.cprintf(ent, PRINT_HIGH, "  %d\n", *((qboolean *)zbotCommands[i].datapoint));
					break;
				case CMDTYPE_NUMBER:
					gi.cprintf(ent, PRINT_HIGH, "  %d\n", *((int *)zbotCommands[i].datapoint));
					break;
				case CMDTYPE_STRING:
					gi.cprintf(ent, PRINT_HIGH, "  %s\n", zbotCommands[i].datapoint);

					break;
				}

				printed++;
			}
			listed++;
		}
		i++;
	}

	gi.cprintf(ent, PRINT_HIGH, "Page %d/%1.0f (%i commands).\n", (offset + 1), ceil(num_gset_commands / 20.0), num_gset_commands);

	gi.cprintf(ent, PRINT_HIGH, "-------------------------------------------------\n");
}

void List_aset_commands(edict_t *ent, int offset)
{
	int i;
	int printed;
	int listed;
	offset--;

	gi.cprintf(ent, PRINT_HIGH, "-------------------------------------------------\n");

	printed = 0;
	i = 0;
	listed = 0;
	while (i < ZBOTCOMMANDSSIZE)
	{
		if (printed >= 20)
			break;


		if (zbotCommands[i].cmdwhere & CMD_ASET)
		{
			if (listed < offset * 20)
			{
				//skip thru until we get to right page

			}
			else
			{
				gi.cprintf(ent, PRINT_HIGH, "  %-32s", zbotCommands[i].cmdname);
				switch (zbotCommands[i].cmdtype)
				{
				case CMDTYPE_LOGICAL:
					gi.cprintf(ent, PRINT_HIGH, "  %d\n", *((qboolean *)zbotCommands[i].datapoint));
					break;
				case CMDTYPE_NUMBER:
					gi.cprintf(ent, PRINT_HIGH, "  %d\n", *((int *)zbotCommands[i].datapoint));
					break;
				case CMDTYPE_STRING:
					gi.cprintf(ent, PRINT_HIGH, "  %s\n", zbotCommands[i].datapoint);

					break;
				}
				printed++;
			}
			listed++;
		}
		i++;
	}

	gi.cprintf(ent, PRINT_HIGH, "Page %d/%1.0f (%i commands).\n", (offset + 1), ceil(num_aset_commands / 20.0), num_aset_commands);

	gi.cprintf(ent, PRINT_HIGH, "-------------------------------------------------\n");
}

void MSET(edict_t *ent)
{
	int i;
	qboolean valid_command = false;
	char temp[256];
	cvar_t	*game_dir;
	game_dir = gi.cvar("game", "", 0);

	if (ent->client->resp.admin < aset_vars->ADMIN_MSET_LEVEL)
		return;

	if (gi.argc() < 2) {
		List_mset_commands(ent, 1);
		return;
	}

	i = atoi(gi.argv(1));
	if (i)
	{
		List_mset_commands(ent, i);
		return;
	}


	if (!strstr(gi.argv(1), "edited_by"))
		strcpy(mset_vars->edited_by, ent->client->resp.admin_name);
	if (strcmp(gi.argv(1), "clear") == 0)
	{
		CopyGlobalToLocal();
		sprintf(temp, "%s/ent/%s.cfg", game_dir->string, level.mapname);
		remove(temp);
		gi.cprintf(ent, PRINT_HIGH, "MSET cleared\n");
		return;
	}


	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if (zbotCommands[i].cmdwhere & CMD_MSET)
		{
			if (startContains(zbotCommands[i].cmdname, gi.argv(1)))
			{

				//valid command, modify settings
				processCommand(i, 2, ent);
				valid_command = true;
				sprintf(temp, "%s/ent/%s.cfg", game_dir->string, level.mapname);
				writeMapCfgFile(temp);

				break;
			}
		}
	}

	if (!valid_command)
	{
		gi.cprintf(ent, PRINT_HIGH, "Invalid command\n");
	}

	hud_footer(ent);
}

void GSET(edict_t *ent)
{
	int i;
	qboolean valid_command = false;
	char temp[256];
	cvar_t	*game_dir;
	game_dir = gi.cvar("game", "", 0);

	if (ent->client->resp.admin < aset_vars->ADMIN_GSET_LEVEL)
		return;

	if (gi.argc() < 2) {
		List_gset_commands(ent, 1);
		return;
	}

	i = atoi(gi.argv(1));
	if (i)
	{
		List_gset_commands(ent, i);
		return;
	}


	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if (zbotCommands[i].cmdwhere & CMD_GSET)
		{
			if (zbotCommands[i].cmdwhere & CMD_GSETMAP)
				sprintf(temp, "g%s", gi.argv(1));
			else
				sprintf(temp, gi.argv(1));

			if (startContains(zbotCommands[i].cmdname, temp))
			{
				//valid command, modify settings
				processCommand(i, 2, ent);
				valid_command = true;
				sprintf(temp, "%s/jump_mod.cfg", game_dir->string);
				writeMainCfgFile(temp);
				break;
			}
		}
	}
	if (!valid_command)
	{
		gi.cprintf(ent, PRINT_HIGH, "Invalid command\n");
	}


}

void ASET(edict_t *ent)
{
	int i;
	qboolean valid_command = false;
	char temp[256];
	cvar_t	*game_dir;
	game_dir = gi.cvar("game", "", 0);

	if (ent->client->resp.admin < aset_vars->ADMIN_MAX_LEVEL)
		return;

	if (gi.argc() < 2) {
		List_aset_commands(ent, 1);
		return;
	}

	i = atoi(gi.argv(1));
	if (i)
	{
		List_aset_commands(ent, i);
		return;
	}


	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if (zbotCommands[i].cmdwhere & CMD_ASET)
		{
			sprintf(temp, gi.argv(1));

			if (startContains(zbotCommands[i].cmdname, temp))
			{
				//valid command, modify settings
				processCommand(i, 2, ent);
				valid_command = true;
				sprintf(temp, "%s/jump_mod.cfg", game_dir->string);
				writeMainCfgFile(temp);
				break;
			}
		}
	}
	if (!valid_command)
	{
		gi.cprintf(ent, PRINT_HIGH, "Invalid command\n");
	}


}


void Change_Ghost_Model(edict_t *ent)
{
	int i;
	char temp[512];
	qboolean done = true;
	cvar_t	*game_dir;
	game_dir = gi.cvar("game", "", 0);

	if (ent->client->resp.admin < aset_vars->ADMIN_MSET_LEVEL)
		return;

	if (gi.argc() < 2) {
		//list all available? need to load them somehow first		
		if (ghost_model_list_count)
		{
			memset(temp, 0, sizeof(temp));
			for (i = 0; i < ghost_model_list_count; i++)
			{
				done = false;
				if (strlen(temp))
					Com_sprintf(temp, sizeof(temp), "%-12s %2d.%-12s", temp, i + 1, ghost_model_list[i].name);
				else
					Com_sprintf(temp, sizeof(temp), "%2d.%-12s", i + 1, ghost_model_list[i].name);
				if (strlen(temp) > 70)
				{
					gi.cprintf(ent, PRINT_HIGH, "%s\n", temp);
					memset(temp, 0, sizeof(temp));
					done = true;
				}
			}
			if (!done)
				gi.cprintf(ent, PRINT_HIGH, "%s\n", temp);
			Com_sprintf(temp, sizeof(temp), "ghost <number> to set the ghost for the current map.");
			gi.cprintf(ent, PRINT_HIGH, "%s\n", HighAscii(temp));
			gi.cprintf(ent, PRINT_HIGH, "gset ghosty_model <number> to apply for all maps\n");
		}
		else
		{
			gi.cprintf(ent, PRINT_HIGH, "There are no ghost models available\n");
		}
		return;
	}

	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if (zbotCommands[i].cmdwhere & CMD_MSET)
		{
			if (startContains(zbotCommands[i].cmdname, "ghosty_model"))
			{

				//valid command, modify settings
				processCommand(i, 1, ent);
				sprintf(temp, "%s/ent/%s.cfg", game_dir->string, level.mapname);
				writeMapCfgFile(temp);

				break;
			}
		}
	}
}


qboolean writeMapCfgFile(char *cfgfilename)
{
	char	temp[256];
	int i, i2;
	FILE *cfg_file;
	int comparison;
	qboolean added_line = false;

	cfg_file = fopen(cfgfilename, "wb");


	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if (zbotCommands[i].cmdwhere & CMD_MSET)
		{
			sprintf(temp, "g%s", zbotCommands[i].cmdname);
			for (i2 = 0; i2 < ZBOTCOMMANDSSIZE; i2++)
			{
				if (zbotCommands[i2].cmdwhere & CMD_GSET)
					if (startContains(zbotCommands[i2].cmdname, temp))
					{

						switch (zbotCommands[i].cmdtype)
						{
						case CMDTYPE_LOGICAL:
							comparison = *((qboolean *)zbotCommands[i].datapoint) != *((qboolean *)zbotCommands[i2].datapoint);
							break;
						case CMDTYPE_NUMBER:
							comparison = *((int *)zbotCommands[i].datapoint) != *((int *)zbotCommands[i2].datapoint);
							break;
						case CMDTYPE_STRING:
							comparison = (strcmp(zbotCommands[i].datapoint, zbotCommands[i2].datapoint) != 0);
							break;
						}

						if (comparison)
						{

							switch (zbotCommands[i].cmdtype)
							{
							case CMDTYPE_LOGICAL:
								fprintf(cfg_file, "%s \"%s\"\n", zbotCommands[i].cmdname, *((qboolean *)zbotCommands[i].datapoint) ? "Yes" : "No");
								break;

							case CMDTYPE_NUMBER:
								fprintf(cfg_file, "%s \"%d\"\n", zbotCommands[i].cmdname, *((int *)zbotCommands[i].datapoint));
								break;

							case CMDTYPE_STRING:
								fprintf(cfg_file, "%s \"%s\"\n", zbotCommands[i].cmdname, (char *)zbotCommands[i].datapoint);
								break;
							}

							added_line = true;
						}
						break;
					}
			}
		}
	}
	fclose(cfg_file);
	if (!added_line)
	{
		remove(cfgfilename);
		return false;
	}
	return true;
}

qboolean writeMainCfgFile(char *cfgfilename)
{
	int i;
	FILE *cfg_file;
	qboolean added_line = false;

	cfg_file = fopen(cfgfilename, "wb");


	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if ((zbotCommands[i].cmdwhere & CMD_GSET) || (zbotCommands[i].cmdwhere & CMD_ASET))
		{
			switch (zbotCommands[i].cmdtype)
			{
			case CMDTYPE_LOGICAL:
				fprintf(cfg_file, "%s \"%s\"\n", zbotCommands[i].cmdname, *((qboolean *)zbotCommands[i].datapoint) ? "Yes" : "No");
				break;
			case CMDTYPE_NUMBER:
				fprintf(cfg_file, "%s \"%d\"\n", zbotCommands[i].cmdname, *((int *)zbotCommands[i].datapoint));
				break;
			case CMDTYPE_STRING:
				fprintf(cfg_file, "%s \"%s\"\n", zbotCommands[i].cmdname, (char *)zbotCommands[i].datapoint);
				break;
			}
			added_line = true;
		}
	}
	fclose(cfg_file);
	if (!added_line)
	{
		remove(cfgfilename);
		return false;
	}
	return true;
}

qboolean readCfgFile(char *cfgfilename)
{
	FILE *cfgfile;
	char buff1[256];
	char buff2[256];

	cfgfile = fopen(cfgfilename, "rt");
	if (!cfgfile) return false;

	while (fgets(zbbuffer, 256, cfgfile) != NULL)
	{
		char *cp = zbbuffer;

		SKIPBLANK(cp);

		if (!(cp[0] == ';' || cp[0] == '\n' || isBlank(cp)))
		{
			if (breakLine(cp, buff1, buff2, sizeof(buff2) - 1))
			{
				int i;

				for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
				{
					if ((zbotCommands[i].cmdwhere & CMDWHERE_CFGFILE) && startContains(zbotCommands[i].cmdname, buff1))
					{
						if (zbotCommands[i].initfunc)
						{
							(*zbotCommands[i].initfunc)(buff2);
						}
						else switch (zbotCommands[i].cmdtype)
						{
						case CMDTYPE_LOGICAL:
							*((qboolean *)zbotCommands[i].datapoint) = getLogicalValue(buff2);
							break;

						case CMDTYPE_NUMBER:
							*((int *)zbotCommands[i].datapoint) = atoi(buff2);
							break;

						case CMDTYPE_STRING:
							strcpy(zbotCommands[i].datapoint, buff2);
							break;
						}

						break;
					}
				}
			}
		}
	}

	fclose(cfgfile);

	return true;
}

void processCommand(int cmdidx, int startarg, edict_t *ent)
{
	if (gi.argc() > startarg)
	{
		switch (zbotCommands[cmdidx].cmdtype)
		{
		case CMDTYPE_LOGICAL:
			*((qboolean *)zbotCommands[cmdidx].datapoint) = getLogicalValue(gi.argv(startarg));
			break;

		case CMDTYPE_NUMBER:
			//bounds clamping
			if (
				(atoi(gi.argv(startarg)) < zbotCommands[cmdidx].min)
				||
				(atoi(gi.argv(startarg)) > zbotCommands[cmdidx].max)
				)
			{
				//clamp
				*((int *)zbotCommands[cmdidx].datapoint) = zbotCommands[cmdidx].default_val;
			}
			else
				*((int *)zbotCommands[cmdidx].datapoint) = atoi(gi.argv(startarg));
			break;

		case CMDTYPE_STRING:
			processstring(zbotCommands[cmdidx].datapoint, gi.argv(startarg), 255, 0);
			break;
		}
	}

	switch (zbotCommands[cmdidx].cmdtype)
	{
	case CMDTYPE_LOGICAL:
		gi.cprintf(ent, PRINT_HIGH, "%s = %s\n", zbotCommands[cmdidx].cmdname, *((qboolean *)zbotCommands[cmdidx].datapoint) ? "Yes" : "No");
		break;

	case CMDTYPE_NUMBER:
		gi.cprintf(ent, PRINT_HIGH, "%s = %d\n", zbotCommands[cmdidx].cmdname, *((int *)zbotCommands[cmdidx].datapoint));
		break;

	case CMDTYPE_STRING:
		gi.cprintf(ent, PRINT_HIGH, "%s = %s\n", zbotCommands[cmdidx].cmdname, (char *)zbotCommands[cmdidx].datapoint);
		break;
	}
}

void CopyGlobalToLocal(void)
{

	memcpy(mset_vars, gset_vars->mset, sizeof(mset_vars));
}

void CopyLocalToGlobal(void)
{
	memcpy(gset_vars->mset, mset_vars, sizeof(mset_vars));
}

int num_gset_commands = 0;
int num_aset_commands = 0;
int num_mset_commands = 0;
void SetDefaultValues(void)
{
	int i;

	// msets/gsets
	gset_vars->addedtimemap = 15;
	gset_vars->mset->addedtimeoverride = 0;
	gset_vars->addtime_announce = 1;
	gset_vars->admin_max_addtime = 0;
	strcpy(gset_vars->admin_model, "guard");
	gset_vars->allow_admin_boot = 1;
#ifdef RACESPARK
	gset_vars->allow_race_spark = 1;
#endif
	gset_vars->mset->allowsrj = 0;
	gset_vars->mset->announcements = 0;
	gset_vars->autotime = 10;
	gset_vars->best_time_glow = 0;
	gset_vars->mset->bfg = 0;
	gset_vars->mset->blaster = 0;
	gset_vars->mset->checkpoint_total = 0;
	gset_vars->mset->cmsg = 0;
	gset_vars->cvote_announce = 1;
	gset_vars->mset->damage = 1;
	gset_vars->debug = 0;
	gset_vars->dev = 0;
	gset_vars->mset->droptofloor = 1;
	strcpy(gset_vars->mset->edited_by, "NA");
	gset_vars->mset->falldamage = 1;
	gset_vars->mset->fast_firing = 0;
	gset_vars->mset->fastdoors = 0;
	gset_vars->mset->fasttele = 0;
	gset_vars->flashlight = 1;
	gset_vars->mset->fog = 1;
	gset_vars->mset->fog_r = 50;
	gset_vars->mset->fog_g = 50;
	gset_vars->mset->fog_b = 50;
	gset_vars->mset->fog_a = 50;
	gset_vars->mset->fog_density = 2;
	gset_vars->mset->fog_start = 1;
	gset_vars->mset->fog_end = 2000;
	gset_vars->fpskick = 1;
	gset_vars->mset->lap_total = 0;
	gset_vars->mset->ghost = 1;
	gset_vars->ghost_glow = 0;
	gset_vars->mset->ghost_model = 0;
	gset_vars->glow_admin = 0;
	gset_vars->glow_multi = 1;
	gset_vars->mset->gravity = 800;
	gset_vars->mset->health = 400;
	gset_vars->hideghost = 300;
	gset_vars->holdtime = 1;
	gset_vars->hook = 1;
	gset_vars->hookpull = 750;
	gset_vars->hookspeed = 1200;
	gset_vars->mset->hyperblaster = 0;
	gset_vars->html_bestscores = 8;
	gset_vars->html_create = 0;
	gset_vars->html_firstplaces = 10;
	gset_vars->html_profile = 1;
	gset_vars->intermission = 50;
	gset_vars->invis = 0;
	gset_vars->jetpack = 1;
	gset_vars->kill_delay = 1;
	gset_vars->map_end_warn_sounds = 1;
	gset_vars->maplist_times = 0;
	gset_vars->maps_pass = 5;
	gset_vars->max_votes = 3;
	strcpy(gset_vars->model_store, "models/monsters/commandr/head/tris.md2");
	gset_vars->nomapvotetime = 300;
	gset_vars->notimevotetime = 300;
	gset_vars->numberone_length = 17;
	strcpy(gset_vars->numberone_wav, "numberone.wav");
	gset_vars->numsoundwavs = 1;
	gset_vars->overtimegainedhealth = 25;
	gset_vars->overtimehealth = 150;
	gset_vars->overtimelimit = 3;
	gset_vars->overtimerandom = 1;
	gset_vars->overtimetype = 1;
	gset_vars->overtimewait = 20;
	gset_vars->playsound = 10;
	gset_vars->playtag = 0;
	gset_vars->pvote_announce = 1;
	gset_vars->mset->regen = 100;
	gset_vars->mset->quad_damage = 0;
	gset_vars->respawn_sound = 1;
	gset_vars->mset->rocket = 0;
	gset_vars->store_safe = 0;
	gset_vars->mset->singlespawn = 0;
	gset_vars->mset->slowdoors = 0;
	gset_vars->target_glow = 2;
	gset_vars->temp_ban_on_kick = 0;
	gset_vars->time_adjust = 0;
	gset_vars->mset->timelimit = 20;
	gset_vars->tourney = 0;
	gset_vars->transparent = 0;
	gset_vars->voteextratime = 60;
	gset_vars->voteseed = 20;
	gset_vars->votingtime = 20;
	gset_vars->walkthru = 1;
	gset_vars->weapon_fire_min_delay = 500;
	gset_vars->mset->weapon_respawn_time = 0;

	// asets
	aset_vars->ADMIN_ADDBALL_LEVEL = 1;
	aset_vars->ADMIN_ADDENT_LEVEL = 3;
	aset_vars->ADMIN_ADDMAP_LEVEL = 5;
	aset_vars->ADMIN_ADDTIME_LEVEL = 2;
	aset_vars->ADMIN_ADMINEDIT_LEVEL = 5;
	aset_vars->ADMIN_BAN_LEVEL = 5;
	aset_vars->ADMIN_BOOT_LEVEL = 2;
	aset_vars->ADMIN_BRING_LEVEL = 1;
	aset_vars->ADMIN_CHANGENAME_LEVEL = 5;
	aset_vars->ADMIN_DUMMYVOTE_LEVEL = 1;
	aset_vars->ADMIN_GIVE_LEVEL = 1;
	aset_vars->ADMIN_GIVEALL_LEVEL = 1;
	aset_vars->ADMIN_GSET_LEVEL = 5;
	aset_vars->ADMIN_IP_LEVEL = 5;
	aset_vars->ADMIN_MAX_LEVEL = 5;
	aset_vars->ADMIN_MODEL_LEVEL = 5;
	aset_vars->ADMIN_MSET_LEVEL = 4;
	aset_vars->ADMIN_NEXTMAPS_LEVEL = 2;
	aset_vars->ADMIN_NOMAXVOTES_LEVEL = 2;
	aset_vars->ADMIN_RATERESET_LEVEL = 5;
	aset_vars->ADMIN_REMTIMES_LEVEL = 4;
	aset_vars->ADMIN_SILENCE_LEVEL = 1;
	aset_vars->ADMIN_SLAP_LEVEL = 1;
	aset_vars->ADMIN_SORTMAPS_LEVEL = 5;
	aset_vars->ADMIN_STUFF_LEVEL = 5;
	aset_vars->ADMIN_THROWUP_LEVEL = 1;
	aset_vars->ADMIN_TOGGLEHUD_LEVEL = 5;
	aset_vars->ADMIN_UPDATESCORES_LEVEL = 5;
	aset_vars->ADMIN_VOTE_LEVEL = 2;

	num_gset_commands = 0;
	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if (zbotCommands[i].cmdwhere & CMD_GSET)
		{
			num_gset_commands++;
		}
	}
	num_mset_commands = 0;
	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if (zbotCommands[i].cmdwhere & CMD_MSET)
		{
			num_mset_commands++;
		}
	}

	num_aset_commands = 0;
	for (i = 0; i < ZBOTCOMMANDSSIZE; i++)
	{
		if (zbotCommands[i].cmdwhere & CMD_ASET)
		{
			num_aset_commands++;
		}
	}
}
