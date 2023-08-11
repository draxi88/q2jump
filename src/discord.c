#include <string.h> // strcmp()
#include "g_local.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define __discord
#ifdef __discord
#include <orca/discord.h>
#endif
#define BOT_TOKEN "OTAyODYzMDkyNDk5NzU1MDE5.GzHILe.SfaYzdWKbahJvgES7hOUW-z_tcnl19z-cBOG88"
struct discord *client;
void on_ready(struct discord *client) 
{
  const struct discord_user *bot = discord_get_self(client);
  log_info("Logged in as %s!", bot->username);
}

void on_message(struct discord *client, const struct discord_message *msg)
{
  if (strstr(msg->content, "!say") != 0) {

    discord_async_next(client, NULL); // make next request non-blocking (OPTIONAL)
    struct discord_create_message_params params = { .content = "message sent to server.." };
    discord_create_message(client, msg->channel_id, &params, NULL);
    gi.bprintf(PRINT_CHAT, "%s: %s",msg->author->username, msg->content->);
  }
}

void discord_send_message(char *text){
  client = discord_init(BOT_TOKEN);
  struct discord_create_message_params params = { .content = text };
  discord_create_message(client, 596343551886098445, &params, NULL);
}

void StartDiscordBot(void)
{
  if (fork() == 0){
    client = discord_init(BOT_TOKEN);
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message);
    discord_run(client);
  } else {
    return;
  }
}
#undef __discord