
#define CMDWHERE_CFGFILE          0x01
#define CMDWHERE_CLIENTCONSOLE    0x02
#define CMDWHERE_SERVERCONSOLE    0x04

#define CMD_MSET          8
#define CMD_RSET          16
#define CMD_GSET          32
#define CMD_GSETMAP      64
#define CMD_ASET          128

// type of command

#define CMDTYPE_NONE      0
#define CMDTYPE_LOGICAL   1
#define CMDTYPE_NUMBER    2
#define CMDTYPE_STRING    3


typedef void CMDRUNFUNC(int startarg, edict_t *ent, int client);
typedef void CMDINITFUNC(char *arg);



typedef struct
{
	int min;
	int max;
	int default_val;
	char *cmdname;
	byte  cmdwhere;
	byte  cmdtype;
	void *datapoint;
	CMDRUNFUNC *runfunc;
	CMDINITFUNC *initfunc;

} zbotcmd_t;

extern zbotcmd_t zbotCommands[];

// msets
typedef struct
{
	int addedtimeoverride;
	int allowsrj;
	int announcements;
	int bfg;
	int blaster;
	int checkpoint_total;
	int cmsg;
	int damage;
	int droptofloor;
	char edited_by[256];
	int falldamage;
	int fast_firing;
	int fastdoors;
	int fasttele;
	int fog;
	int fog_r;
	int fog_g;
	int fog_b;
	int fog_a;
	int fog_density;
	int fog_start;
	int fog_end;
	int ghost;
	int ghost_model;
	int gravity;
	int health;
	int hyperblaster;
	int lap_total;
	int quad_damage;
	int railgun;
	int regen;
	int rocket;
	int rocketjump_fix;
	int singlespawn;
	int slowdoors;
	int timelimit;
	int weapons;
	int weapon_respawn_time;
} mset_vars_t;

// gsets that aren't msets
typedef struct
{
	mset_vars_t mset[1]; // includes the mset only cmds
	int addedtimemap;
	int addtime_announce;
	int admin_max_addtime;
	char admin_model[255];
	int allow_admin_boot;
	int autotime;
	int autohop;
	int best_time_glow;
	int flashlight;
	int hookspeed;
	int hookpull;
	int respawn_sound;
	int glow_admin;
	int glow_multi;
	int time_adjust;
	int hook;
	int playtag;
	int invis;
	int jetpack;
	int transparent;
	int walkthru;
	int debug;
	int dev;
	char model_store[256];
	char numberone_wav[256];
	int	overtimerandom;
	int overtimelimit;
	int votingtime;
	int overtimetype;
	int overtimewait;
	int overtimehealth;
	int overtimegainedhealth;
	int maplist_times;
	int playsound;
	int voteseed;
	int numsoundwavs;
	int store_safe;
	int intermission;
	int weapon_fire_min_delay;
	int html_profile;
	int html_create;
	int html_bestscores;
	int html_firstplaces;
#ifdef RACESPARK
	int allow_race_spark;
#endif
	int nomapvotetime;
	int notimevotetime;
	int maps_pass;
	int ghost_glow;
	int map_end_warn_sounds;   // hann
	int max_votes;   // _h2
	int temp_ban_on_kick;
	int holdtime;
	int pvote_announce;
	unsigned int	hideghost;
	int cvote_announce;
	unsigned int voteextratime;
	int numberone_length;
	unsigned int fpskick;
	unsigned int kill_delay;
	unsigned int target_glow;
	unsigned int tourney;
} gset_vars_t;

typedef struct
{
	int ADMIN_ADDBALL_LEVEL;
	int ADMIN_ADDENT_LEVEL;
	int ADMIN_ADDMAP_LEVEL;
	int ADMIN_ADDTIME_LEVEL;
	int ADMIN_ADMINEDIT_LEVEL;
	int ADMIN_BAN_LEVEL;
	int ADMIN_BOOT_LEVEL;
	int ADMIN_BRING_LEVEL;
	int ADMIN_CHANGENAME_LEVEL;
	int ADMIN_DUMMYVOTE_LEVEL;
	int ADMIN_GIVE_LEVEL;
	int ADMIN_GIVEALL_LEVEL;
	int ADMIN_GSET_LEVEL;
	int ADMIN_IP_LEVEL;
	int ADMIN_MAX_LEVEL;
	int ADMIN_MODEL_LEVEL;
	int ADMIN_MSET_LEVEL;
	int ADMIN_NEXTMAPS_LEVEL;
	int ADMIN_NOMAXVOTES_LEVEL;
	int ADMIN_RATERESET_LEVEL;
	int ADMIN_REMTIMES_LEVEL;
	int ADMIN_SILENCE_LEVEL;
	int ADMIN_SLAP_LEVEL;
	int ADMIN_SORTMAPS_LEVEL;
	int ADMIN_STUFF_LEVEL;
	int ADMIN_THROWUP_LEVEL;
	int ADMIN_TOGGLEHUD_LEVEL;
	int ADMIN_UPDATESCORES_LEVEL;
	int ADMIN_VOTE_LEVEL;

} aset_vars_t;

extern mset_vars_t mset_vars[1];
extern gset_vars_t gset_vars[1];
extern aset_vars_t aset_vars[1];
extern char zbbuffer[0x10000];
extern char zbbuffer2[256];