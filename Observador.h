/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/packet.h>
#include "ns3/nstime.h"
#include "ns3/average.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/csma-module.h"
#include "ns3/address.h"

using namespace ns3;


class Observador
{
public:
  Observador                 ();
  void     PaqueteEnviado    (Ptr<const Packet> paquete);
  void     PaqueteEnviado2   (Ptr<const Packet> paquete);
  void     PaqueteEnviado3   (Ptr<const Packet> paquete);
  void     PaqueteEnviado4   (Ptr<const Packet> paquete);

private:
  
  uint64_t  m_paquetes;
  uint64_t  m_paquetes2;
  uint64_t  m_paquetes3;
  uint64_t  m_paquetes4;

};


