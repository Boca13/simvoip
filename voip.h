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


using namespace ns3;

class VoipHelper : public OnOffHelper
{
public:
  VoipHelper (Ipv4Address address, uint16_t port);
  ~VoipHelper ();

private:
  Ptr<UniformRandomVariable> varon;
  Ptr<UniformRandomVariable> varoff;

};