#include "g_local.h"
#include <curl/curl.h>

const char *mainURL = "http://46.165.236.118/~quake2/quake2/";

void CURL_DownloadFile(char *url, char *filename) {
	CURL *curl;
	FILE *fp;
	fp = fopen(filename, "wb");
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
	CURLcode result = curl_easy_perform(curl);
	fclose(fp);
	if (result != CURLE_OK) {
		gi.dprintf("ERROR: download failed (%s)\n", url);
		remove(filename);
	}
	curl_easy_cleanup(curl);
}

void CURL_DownloadMapents(char *mapname) {
	char filename[128];
	char url[128];
	cvar_t	*tgame;
	tgame = gi.cvar("game", "", 0);
	//download mapsent..
	sprintf(filename, "%s/mapsent/%s.ent", tgame->string, mapname);
	sprintf(url, "%s%s", mainURL, filename);
	CURL_DownloadFile(url, filename);
	//download old map.cfg  
	sprintf(filename, "%s/ent/%s.cfg", tgame->string, mapname);
	sprintf(url, "%s%s", mainURL, filename);
	CURL_DownloadFile(url, filename);
}

double CURL_GetFileSize(char *url) {
	CURL *curl = curl_easy_init();
	double dl;
	curl_easy_setopt(curl, CURLOPT_URL, url);
	CURLcode result = curl_easy_perform(curl);
	if (!result) {
		result = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &dl);
	}
	curl_easy_cleanup(curl);
	return dl;
}


//Discord webhooks
typedef struct {
	struct {
		char url[256];
		char flag[16];
		qboolean loaded;
	} firsts, chat;
}webhook_s;
webhook_s webhook;
qboolean webhook_loaded;

void webhooks_load() {
	FILE *file;
	char filename[128];
	char throw[32];
	char temp[32];
	char flag[32];
	cvar_t	*tgame;
	tgame = gi.cvar("game", "", 0);
	sprintf(filename, "%s/webhook.cfg", tgame->string);
	file = fopen(filename, "r");
	if (!file) {
		gi.dprintf("Could not open file (%s)!\n", filename);
		return;
	}
	int i = 0;
	while (!feof(file)) {
		fscanf(file, "%s %s %s", throw, flag, temp);
		if (strcmp(throw, "firsts") == 0) {
			strcpy(webhook.firsts.url, temp);
			strcpy(webhook.firsts.flag, flag);
			webhook.firsts.loaded = true;
		}
		else if (strcmp(throw, "chat") == 0) {
			strcpy(webhook.chat.url, temp);
			strcpy(webhook.chat.flag, flag);
			webhook.chat.loaded = true;
		}
		i++;
		if (feof(file))
			break;
	}
	if (webhook.firsts.loaded || webhook.chat.loaded) {
		webhook_loaded = true;
	}
	fclose(file);
}

void CURL_SendHttp(char *url, char *string) {
	CURL *curl = curl_easy_init();
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, string);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
	CURLcode result = curl_easy_perform(curl);
	if (result != CURLE_OK) {
		gi.dprintf("curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
	}
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}
void CURL_SendFirstplace(char *str, char *mapname) {
	if (!webhook_loaded)
		webhooks_load();
	if (!webhook.firsts.loaded)
		return;
	char string[256];
	sprintf(string, "{\"content\":\":first_place: %s __**[%s](http://q2jump.net/map.html?map=%s): %s:first_place:**__ \"}", webhook.firsts.flag, mapname, mapname, str);
	gi.dprintf("%s\n", webhook.firsts.url);
	gi.dprintf("%s\n", string);
	CURL_SendHttp(webhook.firsts.url, string);
}

void CURL_SendChat(char *str, qboolean chat) {
	if (!webhook_loaded)
		webhooks_load();
	if (!webhook.chat.loaded)
		return;
	char string[128];
	sprintf(string, "{\"content\":\"```diff\\r%s%s``` \"}", (chat ? "+" : "-"), str);
	CURL_SendHttp(webhook.chat.url, string);
}

/* NOT WORKING
void CURL_DownloadReplays(){
	CURL *handles[MAX_HIGHSCORES];
	CURLM *multi_handle;
	int i;
	int running = 1;
	FILE *fp[MAX_HIGHSCORES];
	char filename[32];
	char *ptr;
	char file[128];
	cvar_t	*tgame;
	tgame = gi.cvar("game", "", 0);

	curl_global_init(CURL_GLOBAL_ALL);
	gi.dprintf("download multi\n");
	for(i=0;i<MAX_HIGHSCORES;i++){
		handles[i] = curl_easy_init();
		ptr = strtok(jsonmaptimes[i].replay,"/");
		while(ptr != NULL) {
			strcpy(filename,ptr);
			ptr = strtok(NULL,"/");
		}
		gi.dprintf("filename%i: %s\n",i,filename);
		sprintf(file,"%s/remotedemos/%s",tgame->string,filename);
		fp[i] = fopen(file,"wb");
		curl_easy_setopt(handles[i],CURLOPT_URL,jsonmaptimes[i].replay);
		curl_easy_setopt(handles[i],CURLOPT_WRITEDATA,fp[i]);
		curl_easy_setopt(handles[i], CURLOPT_VERBOSE, 1);
		curl_easy_setopt(handles[i], CURLOPT_FAILONERROR, 1L);
		curl_easy_setopt(handles[i], CURLOPT_TIMEOUT, 5L);
		curl_easy_setopt(handles[i], CURLOPT_CONNECTTIMEOUT, 5L);
		curl_easy_setopt(handles[i], CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(handles[i], CURLOPT_SSL_VERIFYPEER, 0L);
	}

	multi_handle = curl_multi_init();
	for(i=0;i<MAX_HIGHSCORES;i++) {
		curl_multi_add_handle(multi_handle, handles[i]);
	}
	CURLMcode mc;
	do{
		gi.dprintf("running:%i.\n",running);
		mc = curl_multi_perform(multi_handle, &running);
		if (running) {
		   mc = curl_multi_poll(multi_handle, NULL, 0, 1000, NULL);
		}
	} while (running && mc == CURLE_OK);
	gi.dprintf("cleanup!\n");
	curl_multi_cleanup(multi_handle);
	for(i=0;i<MAX_HIGHSCORES;i++){
		curl_easy_cleanup(handles[i]);
		fclose(fp[i]);
	}
	curl_global_cleanup();
}*/