#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cassert>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/callback.h"
#include "ns3/internet-module.h"
#include "voip.h"
using namespace ns3;
#define MAXONVOIP 0.1
#define MINONVOIP 0.2
#define MAXOFFVOIP 0.8
#define MINOFFVOIP 0.9

/*
  Constructor de la clase Voip.
  Hereda el constructor de OnOffHelper
  Configura la aplicación estableciendo
  una tasa y OnTime, OffTime
  SOBRE UDP
*/
VoipHelper::VoipHelper(Ipv4Address address, uint16_t port)
  :OnOffHelper("ns3::UdpSocketFactory", Address (InetSocketAddress (address,port)))
{
  //Creamos las variables aleatorias para fijar On/Off Time
  varon=CreateObject<UniformRandomVariable>();
  varon->SetAttribute("Max", DoubleValue(MAXONVOIP));
  varon->SetAttribute("Min", DoubleValue(MINONVOIP));
  varoff=CreateObject<UniformRandomVariable>();
  varoff->SetAttribute("Max", DoubleValue(MAXOFFVOIP));
  varoff->SetAttribute("Min", DoubleValue(MINOFFVOIP));
  //Configuramos la aplicación OnOff
  SetConstantRate (DataRate ("500kbps"));
  SetAttribute("OnTime", PointerValue(varon));
  SetAttribute("OffTime", PointerValue(varoff));
  

}
VoipHelper::~VoipHelper()
{

}