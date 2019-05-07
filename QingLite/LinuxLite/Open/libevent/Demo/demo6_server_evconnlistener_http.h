#pragma once
#include <arpa/inet.h>
#include <signal.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>



std::map<std::string, std::string> g_FileTypeMap;

void SendHTTPHeader(struct bufferevent *bev, int State, const char *StateDescription, const char *FileType, long FileSize)
{
    char HeaderBuffer[1024];
    memset(HeaderBuffer, 0, sizeof(HeaderBuffer));

    sprintf(HeaderBuffer, "HTTP/1.1 %d %s\r\n", State, StateDescription);
    sprintf(HeaderBuffer + strlen(HeaderBuffer), "Content-Type:%s\r\n", FileType);
    sprintf(HeaderBuffer + strlen(HeaderBuffer), "Content-Length:%ld\r\n", FileSize);
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
    char PathBuffer[1024];
    char TimeString[64];

    char SendBuffer[4096];
    memset(SendBuffer, 0, sizeof(SendBuffer));
    sprintf(SendBuffer, "<html><head><meta charset=\"utf-8\"><title>Qing Server</title></head>");
    sprintf(SendBuffer + strlen(SendBuffer), "<body><h1>Floder: %s</h1><table>", dirname);

    int ClientSocket = bufferevent_getfd(bev);
    printf("\nClient Socket = %d, Floder:\r\n", ClientSocket);

    struct stat FileStat;
    struct dirent **DirentInfo;
    int FlieCount = scandir(dirname, &DirentInfo, NULL, alphasort);
    for (int FileIndex = 0; FileIndex<FlieCount; ++FileIndex)
    {
        const char *FileName = DirentInfo[FileIndex]->d_name;
        if (strcmp(FileName, ".") == 0)
        {
            continue;
        }

        printf("\t%s\n", FileName);
        sprintf(PathBuffer, "%s%s", dirname, DirentInfo[FileIndex]->d_name);
        if (lstat(PathBuffer, &FileStat) < 0)
        {
            sprintf(SendBuffer + strlen(SendBuffer),
                "<tr><td><a href=\"%s\">%s</a></td></tr>\n",
                FileName, FileName);

            printf("What ??? File name = %s\n", FileName);
        }
        else
        {
            strftime(TimeString, sizeof(TimeString), "\t  %d  %b   %Y  %H:%M", localtime(&FileStat.st_mtime));
            if (S_ISDIR(FileStat.st_mode))
            {
                sprintf(SendBuffer + strlen(SendBuffer),
                    "<tr><td><a href=\"%s/\">%s/</a></td><td>%s</td><td>%ld</td></tr>\n",
                    FileName, FileName, TimeString, FileStat.st_size);
            }
            else
            {
                sprintf(SendBuffer + strlen(SendBuffer),
                    "<tr><td><a href=\"%s\">%s</a></td><td>%s</td><td>%ld</td></tr>\n",
                    FileName, FileName, TimeString, FileStat.st_size);
            }
        }
    }

    printf("\r\n");
    sprintf(SendBuffer + strlen(SendBuffer), "</table></body></html>\n");
    bufferevent_write(bev, SendBuffer, strlen(SendBuffer));
}

void SendHTTPError(struct bufferevent *bev)
{
    SendHTTPHeader(bev, 404, "File Not Found", "text/html", -1);
    SendFile("404.html", bev);
}

int ProcessGetRequest(struct bufferevent *bev, const char *MethodBuffer, char *PathBuffer)
{
    if (PathBuffer == NULL || strcasecmp("GET", MethodBuffer) != 0)
    {
        SendHTTPError(bev);
        return -1;
    }

    std::string ActualllyPath;
    char *DecodeURI = evhttp_uridecode(PathBuffer, 0, NULL);
    if (strcmp(DecodeURI, "/") == 0)
    {
        ActualllyPath = "./";
    }
    else
    {
        ActualllyPath.append(".");
        ActualllyPath.append(DecodeURI);
    }

    printf("Actually path = %s\r\n", ActualllyPath.c_str());

    struct stat ActuallyPathStat;
    if (stat(ActualllyPath.c_str(), &ActuallyPathStat) < 0)
    {
        printf("ERROR: Stat actually path failed.\n\n");
        SendHTTPError(bev);
        return -1;
    }

    if (S_ISDIR(ActuallyPathStat.st_mode))
    {
        SendHTTPHeader(bev, 200, "OK", g_FileTypeMap["html"].c_str(), -1);
        SendDirectory(bev, ActualllyPath.c_str());
    }
    else
    {
        const char *FileExtension = strrchr(ActualllyPath.c_str(), '.') + 1;
        std::map<std::string, std::string>::iterator it = g_FileTypeMap.find(FileExtension);
        const char *FileType = (it != g_FileTypeMap.end()) ? it->second.c_str() : g_FileTypeMap["misc"].c_str();
        SendHTTPHeader(bev, 200, "OK", FileType, ActuallyPathStat.st_size);
        SendFile(ActualllyPath.c_str(), bev);
        printf("\r\n");
    }

    return 0;
}

void CallBack6_Read(struct bufferevent *bev, void *UserData)
{
    char ReadBuffer[4096], MethodBuffer[512], PathBuffer[1024], ProtocolBuffer[512];
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
    else
    {
        printf("Hello???\n\n");
    }

    bufferevent_free(bev);
}

void CallBack6_Listen(struct evconnlistener *Listener, evutil_socket_t Socket, struct sockaddr *sa, int socklen, void *user_data)
{
    printf("Accept client socket = %d.\n\n", Socket);

    struct event_base *base = (event_base*)user_data;
    struct bufferevent *bev = bufferevent_socket_new(base, Socket, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_flush(bev, EV_READ | EV_WRITE, BEV_NORMAL);
    bufferevent_setcb(bev, CallBack6_Read, NULL, CallBack6_Event, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
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
    //initialize content type map
    {
        g_FileTypeMap["txt"] = "text/plain";
        g_FileTypeMap["c"] = "text/plain";
        g_FileTypeMap["h"] = "text/plain";
        g_FileTypeMap["html"] = "text/html";
        g_FileTypeMap["htm"] = "text/htm";
        g_FileTypeMap["css"] = "text/css";
        g_FileTypeMap["gif"] = "image/gif";
        g_FileTypeMap["jpg"] = "image/jpeg";
        g_FileTypeMap["jpeg"] = "image/jpeg";
        g_FileTypeMap["png"] = "image/png";
        g_FileTypeMap["ogg"] = "application/ogg";
        g_FileTypeMap["pdf"] = "application/pdf";
        g_FileTypeMap["ps"] = "application/postscript";
        g_FileTypeMap["au"] = "audio/basic";
        g_FileTypeMap["wav"] = "audio/wav";
        g_FileTypeMap["mid"] = "audio/midi";
        g_FileTypeMap["midi"] = "audio/midi";
        g_FileTypeMap["mp3"] = "audio/mpeg";
        g_FileTypeMap["avi"] = "video/x-msvideo";
        g_FileTypeMap["qt"] = "video/quicktime";
        g_FileTypeMap["mov"] = "video/quicktime";
        g_FileTypeMap["mpeg"] = "video/mpeg";
        g_FileTypeMap["mpe"] = "video/mpeg";
        g_FileTypeMap["vrml"] = "model/vrml";
        g_FileTypeMap["misc"] = "application/misc";
    }

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

    struct evconnlistener *Listener = evconnlistener_new_bind(
        base,
        CallBack6_Listen,
        base,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        10,
        (struct sockaddr*)&BindAddress,
        sizeof(BindAddress));

    if (Listener == NULL)
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

    evconnlistener_free(Listener);
    event_free(SigintEvent);
    event_base_free(base);
}
