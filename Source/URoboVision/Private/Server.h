// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "Sockets.h"
#include "Networking.h"
#include "PacketBuffer.h"
#include <thread>

class UROBOVISION_API TCPServer
{
private:
  FSocket *ListenSocket;
  FSocket *ClientSocket;

  std::thread Thread;
  volatile bool Running;

  void ServerLoop();
  bool ListenConnections();

public:
  // This pointer has to be set before starting the server
  TSharedPtr<PacketBuffer> Buffer;

  TCPServer();
  ~TCPServer();

  void Start(const int32 ServerPort, bool BindToAnyIp);
  void Stop();

  bool HasClient() const;

};
