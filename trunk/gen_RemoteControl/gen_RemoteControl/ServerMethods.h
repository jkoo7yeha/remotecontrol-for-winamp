#pragma once

extern int const startWinsock();
extern int const startSocket();
extern void startServer();

extern void shutdownSocket();
extern void stopServer(bool showLogMessage);

extern int const rawSend(const char *parameter);

extern int const synchronize();
extern int const sendCover(const char* prefix, const int & number);

extern void sendTrackInfo(const int & number);