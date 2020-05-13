

//Structures
typedef struct
{
	int		uid;
	char	date[32];
	float	time;
	int		completions;
	qboolean fresh;
} times_record;

typedef struct
{
	char	name[128];
	int		points[MAX_HIGHSCORES];
	int		score;
	float	israfel;
	qboolean inuse;
	int		completions;
	int		lastseen;
	int		maps_with_points;
	int		maps_with_1st;
} users_record;

typedef struct
{
	float israfel;
	int	score;
	int	uid;
	int pos;
} users_sort_record;

typedef struct
{
	char		path[512];
	int			nummaps;          // number of maps in list 
	char		mapnames[MAX_MAPS][MAX_MAPNAME_LEN];
	int			currentmap;       // index to current map 
	qboolean	demoavail[MAX_MAPS];
	int			update[MAX_MAPS];
	times_record times[MAX_MAPS][MAX_USERS];
	int			skill[MAX_MAPS];
	int			num_users;

	users_record		users[MAX_USERS];
	users_sort_record	sorted_users[MAX_USERS];
	users_sort_record	sorted_israfel[MAX_USERS];
	users_sort_record	sorted_completions[MAX_USERS];

	int			sort_num_users;
	int			sort_num_users_israfel;
	char		rotationflag;     // set to ML_ROTATE_* 
} maplist_t;


extern maplist_t	maplist;

//void		UpdateTimes(int mid);
void		EmptyTimes(int mid);
void		ClearScores(void);
void		UpdateScores(void);
void		ShowMapTimes(edict_t *ent);
void		ShowPlayerTimes(edict_t *ent);
void		ShowPlayerScores(edict_t *ent);
void		sort_users_israfel(int n);
void		remtime(edict_t *ent);
void		remtimes(edict_t *ent);
int			FindMaplistUID(int mid, int uid);

qboolean maplist_log(edict_t *ent, int uid, float time, char *date);
void sort_maplist_times();
void open_map_file(char *filename, qboolean apply);
void write_map_file(char *filename, int mapnum);
//extern times_record maplist.times[level.mapnum][MAX_USERS];
void UpdateThisUsersUID(edict_t *ent, char *name);
void open_users_file();
void write_users_file(void);
void list_mapsleft(edict_t *ent);
void list_mapsdone(edict_t *ent);
void append_uid_file(int uid, char *filename);
void clear_uid_info(int num);
void open_uid_file(int uid, edict_t *ent);
void write_uid_file(int uid, edict_t *ent);
void resync(qboolean overide);
void removemapfrom_uid_file(int uid);
void resync(qboolean overide);

