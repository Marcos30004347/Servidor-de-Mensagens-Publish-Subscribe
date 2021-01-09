#include "request.h"

void reply_t_send(int client, const char* message)
{
    write(client, message, strlen(message)); 
}