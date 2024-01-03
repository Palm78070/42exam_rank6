#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>

int maxSock;

char *msg;

int g_cliId[5000];
char *cliBuff[5000];

char buff_sd[1001];
char buff_rd[1001];

fd_set rd_set, wrt_set, atv_set;

void ft_error(char *s)
{
	perror(s);
	exit(1);
}

char *str_join(char *buff, char *add)
{
	int len_buff = (buff == 0) ? 0 : strlen(buff);
	int len_add = (add == 0) ? 0 : strlen(add);

	char *res = malloc(sizeof(*res) * (len_buff + len_add + 1));
	if (!res)
		return (0);
	res[0] = 0;
	if (buff)
	{
		strcat(res, buff);
		free(buff);
	}
	if (add)
		strcat(res, add);
	return (res);
}

int extract_msg(char **buff, char **msg)
{
	int i = 0;
	*msg = 0;
	if (buff == 0)
		return (0);
	char *newbuff;
	while ((*buff)[i])
	{
		if ((*buff)[i] == '\n')
		{
			newbuff = calloc(strlen(*buff + i + 1), sizeof(*newbuff));
			if (!newbuff)
				return (-1);
			strcpy(newbuff, (*buff + i + 1));
			*msg = *buff;
			(*msg)[i + 1] = 0;
			*buff = newbuff;
			return (1);
		}
		++i;
	}
	return (0);
}

void send_msg(int fd)
{
	for (int sockId = 3; sockId <= maxSock; sockId++)
	{
		if (FD_ISSET(sockId, &wrt_set) && sockId != fd)
		{
			send(sockId, buff_sd, strlen(buff_sd), 0);
			if (msg)
				send(sockId, msg, strlen(msg), 0);
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
		ft_error("Wrong number of arguments\n");
	int sockfd, connfd, cliId;
	cliId = 0;
	socklen_t len_cli;
	struct sockaddr_in servaddr, cliaddr;
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	servaddr.sin_port = htons(atoi(argv[1]));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		ft_error("Fatal error\n");

	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		ft_error("Fatal error\n");

	if (listen(sockfd, SOMAXCONN) < 0)
		ft_error("Fatal error\n");

	len_cli = sizeof(cliaddr);
	maxSock = sockfd;
	FD_ZERO(&atv_set);
	FD_SET(sockfd, &atv_set);

	while (1)
	{
		rd_set = wrt_set = atv_set;
		if (select(maxSock + 1, &rd_set, &wrt_set, 0, 0) <= 0)
			continue;
		if (FD_ISSET(sockfd, &rd_set))
		{
			connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len_cli);
			if (connfd < 0)
				ft_error("Fatal error\n");
			FD_SET(connfd, &atv_set);
			maxSock = (connfd > maxSock) ? connfd : maxSock;
			g_cliId[connfd] = cliId++;
			sprintf(buff_sd, "server: client %d just arrived\n", g_cliId[connfd]);
			send_msg(connfd);
			cliBuff[connfd] = 0;
			continue;
		}
		for (int sockId = 3; sockId <= maxSock; sockId++)
		{
			if (FD_ISSET(sockId, &rd_set) && sockId != sockfd)
			{
				int rd = recv(sockId, buff_rd, 1000, 0);
				if (rd <= 0)
				{
					FD_CLR(sockId, &atv_set);
					sprintf(buff_sd, "server: client %d just left\n", g_cliId[sockId]);
					send_msg(sockId);
					close(sockId);
					if (cliBuff[sockId] != 0)
						free(cliBuff[sockId]);
				}
				else
				{
					buff_rd[rd] = 0;
					cliBuff[sockId] = str_join(cliBuff[sockId], buff_rd);
					msg = 0;
					while (extract_msg(&cliBuff[sockId], &msg))
					{
						sprintf(buff_sd, "client %d: ", g_cliId[sockId]);
						send_msg(sockId);
						if (msg)
						{
							free(msg);
							msg = 0;
						}
					}
				}
			}
		}
	}
	return (0);
}

// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdlib.h>
// #include <netinet/in.h>
// #include <sys/socket.h>

// int maxSock;
// char *msg = NULL;

// int g_cliId[5000];
// char *cliBuff[5000];

// char buff_sd[1001];
// char buff_rd[1001];

// fd_set rd_set, wrt_set, atv_set;

// void ft_error(char *s)
// {
// 	perror(s);
// 	exit(1);
// }

// char *str_join(char *buff, char *add)
// {
// 	int len_buff = (buff == 0) ? 0 : strlen(buff);
// 	int len_add = (add == 0) ? 0 : strlen(add);

// 	char *res = malloc(sizeof(*res) * (len_buff + len_add + 1));
// 	if (!res)
// 		return (0);
// 	res[0] = 0;
// 	if (buff)
// 	{
// 		strcat(res, buff);
// 		free(buff);
// 	}
// 	if (add)
// 		strcat(res, add);
// 	return (res);
// }

// int extract_msg(char **buff, char **msg)
// {
// 	int i = 0;
// 	char *newbuff;
// 	*msg = 0;
// 	if (*buff == 0)
// 		return (0);

// 	while ((*buff)[i])
// 	{
// 		if ((*buff)[i] == '\n')
// 		{
// 			newbuff = calloc(strlen(*buff + i + 1) + 1, sizeof(*newbuff));
// 			if (!newbuff)
// 				return (-1);
// 			strcpy(newbuff, (*buff + i + 1));
// 			*msg = *buff;
// 			(*msg)[i + 1] = 0;
// 			*buff = newbuff;
// 			return (1);
// 		}
// 		++i;
// 	}
// 	return (0);
// }

// void send_msg(int fd)
// {
// 	for (int sockId = 3; sockId <= maxSock; sockId++)
// 	{
// 		if (FD_ISSET(sockId, &wrt_set) && sockId != fd)
// 		{
// 			send(sockId, buff_sd, strlen(buff_sd), 0);
// 			if (msg)
// 				send(sockId, msg, strlen(msg), 0);
// 		}
// 	}
// }

// int main(int argc, char **argv)
// {
// 	if (argc != 2)
// 		ft_error("Wrong number of arguments\n");
// 	int sockfd, connfd, cliId;
// 	cliId = 0;
// 	socklen_t len_cli;
// 	struct sockaddr_in servaddr, cliaddr;
// 	bzero(&servaddr, sizeof(servaddr));

// 	servaddr.sin_family = AF_INET;
// 	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
// 	servaddr.sin_port = htons(atoi(argv[1]));

// 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (sockfd < 0)
// 		ft_error("Fatal error\n");
// 	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
// 		ft_error("Fatal error\n");
// 	if (listen(sockfd, SOMAXCONN) < 0)
// 		ft_error("Fatal error\n");
// 	maxSock = sockfd;
// 	len_cli = sizeof(cliaddr);
// 	FD_ZERO(&atv_set);
// 	FD_SET(sockfd, &atv_set);

// 	while (1)
// 	{
// 		rd_set = wrt_set = atv_set;
// 		if (select(maxSock + 1, &rd_set, &wrt_set, 0, 0) <= 0)
// 			continue;
// 		if (FD_ISSET(sockfd, &rd_set))
// 		{
// 			connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len_cli);
// 			if (connfd < 0)
// 				ft_error("Fatal error\n");
// 			g_cliId[connfd] = cliId++;
// 			FD_SET(connfd, &atv_set);
// 			maxSock = (connfd > maxSock) ? connfd : maxSock;
// 			sprintf(buff_sd, "server: client %d just arrived\n", g_cliId[connfd]);
// 			send_msg(connfd);
// 			cliBuff[connfd] = 0;
// 			continue;
// 		}
// 		for (int sockId = 3; sockId <= maxSock; sockId++)
// 		{
// 			if (FD_ISSET(sockId, &rd_set) && sockId != sockfd)
// 			{
// 				int rd = recv(sockId, buff_rd, 1000, 0);
// 				if (rd <= 0)
// 				{
// 					FD_CLR(sockId, &atv_set);
// 					sprintf(buff_sd, "server: client %d just left\n", g_cliId[sockId]);
// 					send_msg(sockId);
// 					close(sockId);
// 					if (cliBuff[sockId] != 0)
// 						free(cliBuff[sockId]);
// 				}
// 				else
// 				{
// 					buff_rd[rd] = 0;
// 					cliBuff[sockId] = str_join(cliBuff[sockId], buff_rd);
// 					msg = 0;
// 					while (extract_msg(&cliBuff[sockId], &msg))
// 					{
// 						sprintf(buff_sd, "client %d: ", g_cliId[sockId]);
// 						send_msg(sockId);
// 						if (msg)
// 							free(msg);
// 						msg = 0;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return (0);
// }
