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
#include "ControladorTabla.h"

using namespace ns3;


class Observador
{
public:
  Observador                 ();
  Observador                 (Ptr<Ipv4> uno , Ptr<Ipv4> dos);
  void     PaqueteEnviado    (Ptr<const Packet> paquete);
  void     PaqueteEnviado2   (Ptr<const Packet> paquete);
  void     PaqueteEnviado3   (Ptr<const Packet> paquete);
  void     PaqueteEnviado4   (Ptr<const Packet> paquete);
  void     SumaColaR1I1      (Ptr<const Packet> paquete);
  void     RestaColaR1I1     (Ptr<const Packet> paquete);
  void     SumaColaR1I2      (Ptr<const Packet> paquete);
  void     RestaColaR1I2     (Ptr<const Packet> paquete);
  void     SumaColaR2I1      (Ptr<const Packet> paquete);
  void     RestaColaR2I1     (Ptr<const Packet> paquete);
  void     SumaColaR2I2      (Ptr<const Packet> paquete);
  void     RestaColaR2I2     (Ptr<const Packet> paquete);

private:
  
  uint64_t  m_paquetes;
  uint64_t  m_paquetes2;
  uint64_t  m_paquetes3;
  uint64_t  m_paquetes4;
  uint64_t  m_colaR1I1;
  uint64_t  m_colaR1I2;
  uint64_t  m_colaR2I1;
  uint64_t  m_colaR2I2;
  Ptr<ControladorTabla> m_controla; 
};



