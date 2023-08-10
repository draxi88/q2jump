#include <string.h> // strcmp()
#include <orca/discord.h>
#include "discord.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BOT_TOKEN "OTAyODYzMDkyNDk5NzU1MDE5.GzHILe.SfaYzdWKbahJvgES7hOUW-z_tcnl19z-cBOG88"

void on_ready(struct discord *client) 
{
  const struct discord_user *bot = discord_get_self(client);
  log_info("Logged in as %s!", bot->username);
}

void on_message(struct discord *client, const struct discord_message *msg)
{
  if (strcmp(msg->content, "ping") != 0)
    return; // ignore messages that aren't 'ping'

  discord_async_next(client, NULL); // make next request non-blocking (OPTIONAL)
  struct discord_create_message_params params = { .content = "pong" };
  discord_create_message(client, msg->channel_id, &params, NULL);
}

void threadDiscordBot(void *vargp){
  struct discord *client = discord_init(BOT_TOKEN);
  discord_set_on_ready(client, &on_ready);
  discord_set_on_message_create(client, &on_message);
  discord_run(client);
}

int StartDiscordBot(void)
{
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, threadDiscordBot, NULL);
  pthread_join(thread_id, NULL);
}