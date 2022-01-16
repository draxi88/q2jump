#include "g_local.h"
#include <curl/curl.h>

#define Equal(a,b) (strcmp(a, b) == 0)
#define Equal_N(a, b, n) (strncmp(a, b, n) == 0)


memory_s json_memory = { 0 };

char *remQuotes(char *str, qboolean last) {
	if (last)
		str[strlen(str) - 1] = '\0';
	else
		str[strlen(str) - 2] = '\0';
	memmove(str, str + 1, strlen(str));
	return str;
}

void openRepUrls() {
	FILE *file;
	char filename[128];
	char tempname[6];
	char tempurl[128];
	cvar_t	*tgame;
	tgame = gi.cvar("game", "", 0);
	sprintf(filename, "%s/rp_urls.cfg", tgame->string);

	file = fopen(filename, "r");
	if (!file) {
		gi.dprintf("Could not open file %s!\n", filename);
		return;
	}
	int i = 0;
	while (!feof(file)) {
		fscanf(file, "%s %s", tempname, tempurl);
		strcpy(rp_url[i].name, tempname);
		strcpy(rp_url[i].url, tempurl);
		i++;
	}
	fclose(file);
}

long getfilesize(char *filename) {
	FILE *f = fopen(filename, "r");
	if (f == NULL)
		return -1;
	if (fseek(f, 0, SEEK_END) < 0) {
		fclose(f);
		return -1;
	}
	long length = ftell(f);
	fclose(f);
	return length;
}

void loadReplays(char *mapname) {
	int i;
	char file[128];
	cvar_t	*tgame;
	tgame = gi.cvar("game", "", 0);
	gi.dprintf("Loading replays\n");
	for (i = 0; i < MAX_HIGHSCORES; i++) {
		if (strlen(jsonmaptimes[i].replay) < 20)
			continue;
		sprintf(file, "%s/remotedemos/%i_%s_%i.dj3", tgame->string, jsonmaptimes[i].server, mapname, jsonmaptimes[i].uid);
		if (getfilesize(file) != CURL_GetFileSize(jsonmaptimes[i].replay)) {
			CURL_DownloadFile(jsonmaptimes[i].replay, file);
		}
	}
}

static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	memory_s *mem = (memory_s *)userp;

	char *ptr = realloc(mem->response, mem->size + realsize + 1);
	if (ptr == NULL)
		return 0;  /* out of memory! */

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;

	return realsize;
}

void strip(char *s) {
	char *p2 = s;
	while (*s != '\0') {
		if (*s != '\t' && *s != '\n') {
			*p2++ = *s++;
		}
		else {
			++s;
		}
	}
	*p2 = '\0';
}
char *encode_string(char *st)
{
	char *newstr, *tmpstr, *str = st;
	int nspaces = 0, len = 0;
	while (*str != '\0')
	{
		if (*str == ' ') nspaces++;
		len++; str++;
	}
	if ((newstr = tmpstr = malloc(len + 2 * nspaces + 1)) == 0) return 0;
	str = st;
	while (*str != '\0')
	{
		if (*str == '#')
		{
			*tmpstr++ = '%';
			*tmpstr++ = '2';
			*tmpstr++ = '3';
		}
		else *tmpstr++ = *str;
		str++;
	}
	*tmpstr = '\0';
	return newstr;
}
void openMapfile(char *map) {
	int i = 0;
	char *ptr;
	char *mapname;
	memset(&jsonmaptimes, 0, sizeof(maptime)*MAX_USERS);
	mapname = encode_string(map);
	ptr = strtok(json_memory.response, " \t\n");
	//gi.dprintf("ptr:%s\n", ptr);
	while (ptr != NULL) {
		if (Equal(ptr, "{")) {
			ptr = strtok(NULL, " \t\n");
			while (!Equal(ptr, "}") && !Equal(ptr, "},")) {
				if (Equal(ptr, "\"server\":")) {
					ptr = strtok(NULL, " \t\n");
					jsonmaptimes[i].server = atoi(remQuotes(ptr, false));
				}
				else if (Equal(ptr, "\"name\":")) {
					ptr = strtok(NULL, " \t\n");
					strcpy(jsonmaptimes[i].name, remQuotes(ptr, false));
				}
				else if (Equal(ptr, "\"uid\":")) {
					ptr = strtok(NULL, " \t\n");
					jsonmaptimes[i].uid = atoi(remQuotes(ptr, false));
				}
				else if (Equal(ptr, "\"date\":")) {
					ptr = strtok(NULL, " \t\n");
					strcpy(jsonmaptimes[i].date, remQuotes(ptr, false));
				}
				else if (Equal(ptr, "\"time\":")) {
					ptr = strtok(NULL, " \t\n");
					jsonmaptimes[i].time = atof(remQuotes(ptr, false));
				}
				else if (Equal(ptr, "\"comp\":")) {
					ptr = strtok(NULL, " \t\n");
					jsonmaptimes[i].comp = atoi(remQuotes(ptr, true));
				}
				ptr = strtok(NULL, " \t\n");
			}
			sprintf(jsonmaptimes[i].replay, "%s%s_%i.dj3", rp_url[jsonmaptimes[i].server].url, mapname, jsonmaptimes[i].uid);
			strcpy(jsonmaptimes[i].mapname, level.mapname);
			i++;
		}
		ptr = strtok(NULL, " \t\n");
	}
}
void loadMaptimes(char *mapname) {
	CURL *curl;
	int result;
	char *newmapname;
	int i;
	newmapname = encode_string(mapname);
	memset(&json_memory, 0, sizeof(json_memory));
	curl = curl_easy_init();
	char url[128];
	sprintf(url, "http://q2jump.net/maps/%s.json", newmapname);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&json_memory);
	//curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	result = curl_easy_perform(curl);
	if (result != CURLE_OK) {
		gi.dprintf("ERROR: json download failed");
	}
	curl_easy_cleanup(curl);
}

void json_loadMaptimes(char *mapname) {
	loadMaptimes(mapname);
	openRepUrls();
	gi.dprintf("rep_urls loaded!\n");
	openMapfile(mapname);
	gi.dprintf("mapfile loaded!\n");
	loadReplays(mapname);
	gi.dprintf("replays loaded!\n");
	//CURL_DownloadReplays();
}

void json_printMaptimes(char *mapname, edict_t *ent) {
	int i = 0;
	char *ptr;
	char server[16];
	char name[16];
	char date[16];
	char head[128];
	float time;
	int comp;

	loadMaptimes(mapname);

	strcpy(head, "No  Player            Date          Time   Completions");
	gi.cprintf(ent, PRINT_HIGH, "-----------------------------------------\n");
	gi.cprintf(ent, PRINT_HIGH, "Global Times for %s\n", mapname);
	gi.cprintf(ent, PRINT_HIGH, "%s\n", HighAscii(head));
	ptr = strtok(json_memory.response, " \t\n");
	while (ptr != NULL) {
		if (Equal(ptr, "{")) {
			ptr = strtok(NULL, " \t\n");
			while (!Equal(ptr, "}") && !Equal(ptr, "},")) {
				ptr = strtok(NULL, " \t\n");
				if (Equal(ptr, "\"server\":")) {
					ptr = strtok(NULL, " \t\n");
					strcpy(server, rp_url[atoi(remQuotes(ptr, false))].name);
				}
				else if (Equal(ptr, "\"name\":")) {
					ptr = strtok(NULL, " \t\n");
					strcpy(name, remQuotes(ptr, false));
				}
				else if (Equal(ptr, "\"date\":")) {
					ptr = strtok(NULL, " \t\n");
					strcpy(date, remQuotes(ptr, false));
				}
				else if (Equal(ptr, "\"time\":")) {
					ptr = strtok(NULL, " \t\n");
					time = atof(remQuotes(ptr, false));
				}
				else if (Equal(ptr, "\"comp\":")) {
					ptr = strtok(NULL, " \t\n");
					comp = atoi(remQuotes(ptr, true));
				}
			}
			Highlight_Name(name);
			gi.cprintf(ent, PRINT_HIGH, "%-3d %-16s  %s   %8.3f   %3i\n", i + 1, name, date, time, comp);
			i++;
			if (i >= MAX_HIGHSCORES)
				break;
		}
		ptr = strtok(NULL, " \t\n");
	}
}