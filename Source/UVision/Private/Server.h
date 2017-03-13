// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Sockets.h"
#include "Networking.h"
#include "PacketBuffer.h"
#include <thread>

class UVISION_API TCPServer
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

  void Start(const int32 ServerPort);
  void Stop();

  bool HasClient() const;

};
