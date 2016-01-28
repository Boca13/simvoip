/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/packet.h>
#include "ns3/nstime.h"
#include "ns3/average.h"
#include "ns3/global-value.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;


class ControladorTabla : public Object
{
public:
  ControladorTabla (Ptr<Ipv4> uno , Ptr<Ipv4> dos);
  void cambiaEnlace2 ();
  void cambiaEnlace (Time inicio, Time fin, Time salto);

private:
  
  Ptr<Ipv4> m_uno;
  Ptr<Ipv4> m_dos;
  uint32_t m_if3;
  uint32_t m_if4;
  uint16_t m_metrica;

};
