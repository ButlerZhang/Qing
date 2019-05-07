#pragma once
#include <arpa/inet.h>
#include <signal.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>



//16进制数转化为10进制, return 0不会出现
int hexit(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;

    return 0;
}

//解码：处理类似%20之类的内容。
//(1)%20 URL编码中的‘ ’(space);html中的‘ ’(space)是&nbsp
//(2) %21 '!', %22 '"', %23 '#', %24 '$',%25 '%', %26 '&', %27 ''', %28 '('
void Decode(char *to, char *from)
{
    for (; *from != '\0'; ++to, ++from)
    {
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
        {
            // 依次判断from中 %20 三个字符
            *to = static_cast<char>(hexit(from[1]) * 16 + hexit(from[2]));
            // 移过已经处理的两个字符(%21指针指向1),表达式3的++from还会再向后移一个字符
            from += 2;
        }
        else {
            *to = *from;
        }
    }

    *to = '\0';
}

//编码：用作回写浏览器时，将除字母数字及/_.-~以外的字符转义后回写。
void Encode(char* to, size_t tosize, const char* from)
{
    for (int tolen = 0; *from != '\0' && static_cast<size_t>(tolen + 4) < tosize; ++from)
    {
        if (isalnum(*from) || strchr("/_.-~", *from) != (char*)0)
        {
            *to = *from;
            ++to;
            ++tolen;
        }
        else
        {
            sprintf(to, "%%%02x", (int)*from & 0xff);
            to += 3;
            tolen += 3;
        }
    }

    *to = '\0';
}

const char *GetFileType(char *name)
{
    char *dot = strrchr(name, '.');    //自右向左查找‘.’字符;如不存在返回NULL

    if (dot == (char*)0)
        return "text/plain; charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp(dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}

void SendHTTPHeader(struct bufferevent *bev, int no, const char* desp, const char *type, long len)
{
    char HeaderBuffer[1024];
    memset(HeaderBuffer, 0, sizeof(HeaderBuffer));

    sprintf(HeaderBuffer, "HTTP/1.1 %d %s\r\n", no, desp);
    sprintf(HeaderBuffer + strlen(HeaderBuffer), "Content-Type:%s\r\n", type);
    sprintf(HeaderBuffer + strlen(HeaderBuffer), "Content-Length:%ld\r\n", len);
    sprintf(HeaderBuffer + strlen(HeaderBuffer), "Connection: close\r\n\r\n");

    bufferevent_write(bev, HeaderBuffer, strlen(HeaderBuffer));
}

void SendFile(const char *FileName, struct bufferevent *bev)
{
    char ReadBuffer[4096];
    memset(ReadBuffer, 0, sizeof(ReadBuffer));

    ssize_t ret = 0;
    int fd = open(FileName, O_RDONLY);
    while ((ret = read(fd, ReadBuffer, sizeof(ReadBuffer))))
    {
        bufferevent_write(bev, ReadBuffer, ret);
        memset(ReadBuffer, 0, ret);
    }

    close(fd);
}

void SendDirectory(struct bufferevent *bev, const char *dirname)
{
    char EncodedName[1024];
    char PathBuffer[1024];
    char TimeString[64];
    struct stat sb;
    struct dirent **DirentInfo;

    char SendBuffer[4096];
    memset(SendBuffer, 0, sizeof(SendBuffer));

    sprintf(SendBuffer, "<html><head><meta charset=\"utf-8\"><title>Qing Server</title></head>");
    sprintf(SendBuffer + strlen(SendBuffer), "<body><h1>Current Directory: %s</h1><table>", dirname);

    int FlieCount = scandir(dirname, &DirentInfo, NULL, alphasort);
    for (int FileIndex = 0; FileIndex<FlieCount; ++FileIndex)
    {
        Encode(EncodedName, sizeof(EncodedName), DirentInfo[FileIndex]->d_name);
        sprintf(PathBuffer, "%s%s", dirname, DirentInfo[FileIndex]->d_name);
        printf("############# path = %s\n", PathBuffer);

        if (lstat(PathBuffer, &sb) < 0)
        {
            sprintf(SendBuffer + strlen(SendBuffer),
                "<tr><td><a href=\"%s\">%s</a></td></tr>\n",
                EncodedName, DirentInfo[FileIndex]->d_name);
        }
        else
        {
            strftime(TimeString, sizeof(TimeString), "\t  %d  %b   %Y  %H:%M", localtime(&sb.st_mtime));
            if (S_ISDIR(sb.st_mode))
            {
                sprintf(SendBuffer + strlen(SendBuffer),
                    "<tr><td><a href=\"%s/\">%s/</a></td><td>%s</td><td>%ld</td></tr>\n",
                    EncodedName, DirentInfo[FileIndex]->d_name, TimeString, sb.st_size);
            }
            else
            {
                sprintf(SendBuffer + strlen(SendBuffer),
                    "<tr><td><a href=\"%s\">%s</a></td><td>%s</td><td>%ld</td></tr>\n",
                    EncodedName, DirentInfo[FileIndex]->d_name, TimeString, sb.st_size);
            }
        }
    }

    sprintf(SendBuffer + strlen(SendBuffer), "</table></body></html>");
    bufferevent_write(bev, SendBuffer, strlen(SendBuffer));
    printf("Process directory over.\n");
}

void SendHTTPError(struct bufferevent *bev)
{
    SendHTTPHeader(bev, 404, "File Not Found", "text/html", -1);
    SendFile("404.html", bev);
}

int ProcessGetRequest(struct bufferevent *bev, const char *MethodBuffer, char *PathBuffer)
{
    if (strcasecmp("GET", MethodBuffer) != 0)
    {
        return -1;
    }

    //get method ...
    Decode(PathBuffer, PathBuffer);
    printf("After decode, Path = %s\n", PathBuffer);

    char *pf = &PathBuffer[1];
    if (strcmp(PathBuffer, "/") == 0 || strcmp(PathBuffer, "/.") == 0)
    {
        pf = (char*)"./";
    }

    printf("Request Resource Path =  %s, pf = %s\n", PathBuffer, pf);

    struct stat sb;
    if (stat(pf, &sb) < 0)
    {
        printf("Open file err\n");
        SendHTTPError(bev);
        return -1;
    }

    if (S_ISDIR(sb.st_mode))
    {
        SendHTTPHeader(bev, 200, "OK", GetFileType((char*)".html"), -1);
        SendDirectory(bev, pf);
    }
    else
    {
        SendHTTPHeader(bev, 200, "OK", GetFileType(pf), sb.st_size);
        SendFile(pf, bev);
    }

    return 0;
}

void CallBack6_Read(struct bufferevent *bev, void *user_data)
{
    char ReadBuffer[4096], MethodBuffer[50], PathBuffer[4096], ProtocolBuffer[32];
    memset(ProtocolBuffer, 0, sizeof(ProtocolBuffer));
    memset(MethodBuffer, 0, sizeof(MethodBuffer));
    memset(ReadBuffer, 0, sizeof(ReadBuffer));
    memset(PathBuffer, 0, sizeof(PathBuffer));

    bufferevent_read(bev, ReadBuffer, sizeof(ReadBuffer));
    //printf("Read buffer = %s", ReadBuffer);

    sscanf(ReadBuffer, "%[^ ] %[^ ] %[^ \r\n]", MethodBuffer, PathBuffer, ProtocolBuffer);
    //printf("Protocol = %s\n", ProtocolBuffer);
    //printf("Method = %s\n", MethodBuffer);
    printf("Request path = %s\n", PathBuffer);

    if (strcasecmp(MethodBuffer, "GET") == 0)
    {
        ProcessGetRequest(bev, MethodBuffer, PathBuffer);
    }
}

void CallBack6_Event(struct bufferevent *bev, short events, void *user_data)
{
    int ClientSocket = bufferevent_getfd(bev);
    if (events & BEV_EVENT_EOF)
    {
        printf("Client = %d connection closed.\n\n", ClientSocket);
    }
    else if (events & BEV_EVENT_ERROR)
    {
        printf("Client = %d got an error = %s.\n\n", ClientSocket, strerror(errno));
    }

    bufferevent_free(bev);
}

void CallBack6_Listen(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data)
{
    printf("Accept client socket = %d.\n\n", fd);

    struct event_base *base = (event_base*)user_data;
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (bev != NULL)
    {
        bufferevent_flush(bev, EV_READ | EV_WRITE, BEV_NORMAL);
        bufferevent_setcb(bev, CallBack6_Read, NULL, CallBack6_Event, NULL);
        bufferevent_enable(bev, EV_READ | EV_WRITE);
    }
    else
    {
        printf("Error constructing bufferevent!\n");
    }
}

void CallBack6_Signal(evutil_socket_t sig, short events, void *user_data)
{
    struct event_base *base = (event_base *)user_data;
    struct timeval delay = { 1, 0 };

    printf("Caught an interrupt signal; exiting cleanly in one seconds.\n");
    event_base_loopexit(base, &delay);
}

void demo6_server_evconnlistener_http(const char *ServerIP, int Port)
{
    struct event_base *base = event_base_new();
    if (base == NULL)
    {
        printf("ERROR: event init failed.\n");
        return;
    }

    struct sockaddr_in BindAddress;
    memset(&BindAddress, 0, sizeof(BindAddress));
    BindAddress.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP, &(BindAddress.sin_addr));
    BindAddress.sin_port = htons(static_cast<uint16_t>(Port));

    struct evconnlistener *listener = evconnlistener_new_bind(
        base,
        CallBack6_Listen,
        base,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        -1,
        (struct sockaddr*)&BindAddress,
        sizeof(BindAddress));

    if (listener == NULL)
    {
        printf("Could not create a listener.\n");
        event_base_free(base);
        return;
    }

    struct event *SigintEvent = evsignal_new(base, SIGINT, CallBack6_Signal, (void *)base);
    if (SigintEvent == NULL || event_add(SigintEvent, NULL) < 0)
    {
        printf("Could not create/add SIGINT event.\n");
        return;
    }

    printf("HTTP Server start dispatch...\n\n");
    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_free(SigintEvent);
    event_base_free(base);
}
