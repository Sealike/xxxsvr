#include "rediscommand.h"
#include "json/cJSON.h"
#include "client.h"
#include <time.h>
static char g_redisbuffer[64 * 1024];
int   process_push_str(redisContext* c, char* key, char *value)
{
	if (c == NULL) return 0;
	char key1[64] = { 0 };
	strcpy(key1, key);
	strcat(key1, "  %b");
	char buffer[1024] = { 0 };
	sprintf(buffer, "set %s", key1);
	const char* command1 = buffer;
	redisReply* r = (redisReply*)redisCommand(c, command1, value, strlen(value));

	if (NULL == r)
	{
		printf("Execut command1 failure\n");
		return 0;
	}
	if (!(r->type == REDIS_REPLY_STATUS && strcmp(r->str, "OK") == 0))
	{
		printf("Failed to execute command[%s]\n", command1);
		freeReplyObject(r);
		return 0;
	}
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command1);
	return 1;
}
int        set_activetime(redisContext* c, char* key, int iseconds)//设置生存时间
{
	if (c == NULL || key == NULL) return 0;
	char buffer[1024] = { 0 };
	sprintf(buffer, "expire %s %d", key, iseconds);
	const char* command1 = buffer;
	redisReply* r = (redisReply*)redisCommand(c, command1);
	if (NULL == r)
	{
		printf("Execut command1 failure\n");
		return 0;
	}
	if (r->type != REDIS_REPLY_INTEGER)
	{
		printf("Failed to execute command[%s]\n", command1);
		freeReplyObject(r);
		return 0;
	}
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command1);
	return 1;
}
char*    process_get_str(redisContext* c, char* key)
{
	if (c == NULL) return NULL;

	char buffer[1024] = { 0 };
	char *value = NULL;

	sprintf(buffer, "get %s", key);  //返回首元素
	const char* command1 = buffer;
	redisReply* r = (redisReply*)redisCommand(c, command1);

	if (NULL == r)
	{
		printf("Execut command1 failure\n");
		return NULL;
	}
	if (r->type != REDIS_REPLY_STRING)
	{
		printf("Failed to execute command[%s]\n", command1);
		freeReplyObject(r);
		return NULL;
	}

	value = malloc(strlen(r->str) + 1);
	strcpy(value, r->str);

	freeReplyObject(r);

	return value;
}
