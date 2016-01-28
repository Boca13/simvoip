/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "Observador.h"
   
using namespace ns3;
   
NS_LOG_COMPONENT_DEFINE ("distri");
   


int main (int argc, char *argv[])
{
  // The below value configures the default behavior of global routing.
  // By default, it is disabled.  To respond to interface events, set to true
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
  
  // Allow the user to override any of the defaults and the above
  // Bind ()s at run-time, via command-line arguments
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (6);
  NodeContainer n0n2 = NodeContainer (c.Get (0), c.Get (2));
  NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));  
  NodeContainer n3n4 = NodeContainer (c.Get (3), c.Get (4));
  NodeContainer n3n5 = NodeContainer (c.Get (3), c.Get (5));
  NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));
  NodeContainer n2n3v2 = NodeContainer (c.Get(2), c.Get (3));
  
  InternetStackHelper internet;
  internet.Install (c);
  
  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d0d2 = p2p.Install (n0n2);
  NetDeviceContainer d1d2 = p2p.Install (n1n2);
  NetDeviceContainer d3d4 = p2p.Install (n3n4);
  NetDeviceContainer d3d5 = p2p.Install (n3n5);
  NetDeviceContainer d2d3 = p2p.Install (n2n3);
  NetDeviceContainer d2d3v2 = p2p.Install (n2n3v2); 
  
  // Later, we add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i2 = ipv4.Assign (d0d2);  
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i4 = ipv4.Assign (d3d4);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i5 = ipv4.Assign (d3d5);  
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);     
  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3v2 = ipv4.Assign (d2d3v2);
  
  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  // Create the OnOff application to send UDP datagrams of size
  // 210 bytes at a rate of 448 Kb/s

  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;   // Discard port (RFC 863)
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                        InetSocketAddress (i3i4.GetAddress (1), port));
  onoff.SetConstantRate (DataRate ("2Mbps"));
  onoff.SetAttribute ("PacketSize", UintegerValue (50));
  
  ApplicationContainer apps = onoff.Install (c.Get (0));
  apps.Start (Seconds (2));
  apps.Stop (Seconds (8.0));

  OnOffHelper onoff2 ("ns3::UdpSocketFactory",
                        InetSocketAddress (i3i5.GetAddress (1), port));
  onoff2.SetConstantRate (DataRate ("2Mbps"));
  onoff2.SetAttribute ("PacketSize", UintegerValue (50));
  
  ApplicationContainer apps2 = onoff2.Install (c.Get (1));
  apps2.Start (Seconds (2));
  apps2.Stop (Seconds (8.0));

  OnOffHelper onoff3 ("ns3::UdpSocketFactory",
                        InetSocketAddress (i0i2.GetAddress (0), port));
  onoff3.SetConstantRate (DataRate ("1Mbps"));
  onoff3.SetAttribute ("PacketSize", UintegerValue (50));
  
  ApplicationContainer apps3 = onoff3.Install (c.Get (4));
  apps3.Start (Seconds (2));
  apps3.Stop (Seconds (8.0));

  OnOffHelper onoff4 ("ns3::UdpSocketFactory",
                        InetSocketAddress (i1i2.GetAddress (0), port));
  onoff4.SetConstantRate (DataRate ("1Mbps"));
  onoff4.SetAttribute ("PacketSize", UintegerValue (50));
  
  ApplicationContainer apps4 = onoff4.Install (c.Get (5));
  apps4.Start (Seconds (2));
  apps4.Stop (Seconds (8.0));
  

  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                           Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  apps = sink.Install (c.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (9.0));

  PacketSinkHelper sink2 ("ns3::UdpSocketFactory",
                           Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  apps2 = sink2.Install (c.Get (1));
  apps2.Start (Seconds (1.0));
  apps2.Stop (Seconds (9.0));

  PacketSinkHelper sink3 ("ns3::UdpSocketFactory",
                           Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  apps3 = sink3.Install (c.Get (4));
  apps3.Start (Seconds (1.0));
  apps3.Stop (Seconds (9.0));
  
  PacketSinkHelper sink4 ("ns3::UdpSocketFactory",
                           Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  apps4 = sink4.Install (c.Get (5));
  apps4.Start (Seconds (1.0));
  apps4.Stop (Seconds (9.0));
 
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("dynamic-global-routing.tr");
  p2p.EnableAsciiAll (stream);
  internet.EnableAsciiIpv4All (stream);
  
  p2p.EnablePcapAll ("dynamic-global-routing");
  
  Ptr<Node> n2 = c.Get (2);
  Ptr<Ipv4> ipv42 = n2->GetObject<Ipv4> ();

  Ptr<Node> n3 = c.Get (3);
  Ptr<Ipv4> ipv43 = n3->GetObject<Ipv4> ();
  
  /* Caso cambio dinamico
    ControladorTabla controlador = ControladorTabla(ipv42,ipv43);
    controlador.cambiaEnlace(Time("2s"), Time("9s"), Time("1s"), ipv42, ipv43);
    Observador = Observador();
  */

  // Trace routing tables 
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("dynamic-global-routing.routes-2", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (12), routingStream);
  Ptr<OutputStreamWrapper> routingStream2 = Create<OutputStreamWrapper> ("dynamic-global-routing.routes-1", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (6), routingStream2);
   
  // SUSCRIPCION TRAZAS
  Observador observador = Observador(ipv42,ipv43);
  d2d3.Get(1)->GetObject<PointToPointNetDevice>()->TraceConnectWithoutContext ("MacRx", MakeCallback(&Observador::PaqueteEnviado, &observador));
  d2d3v2.Get(1)->GetObject<PointToPointNetDevice>()->TraceConnectWithoutContext ("MacRx", MakeCallback(&Observador::PaqueteEnviado2, &observador));
  d2d3.Get(0)->GetObject<PointToPointNetDevice>()->TraceConnectWithoutContext ("MacRx", MakeCallback(&Observador::PaqueteEnviado3, &observador));
  d2d3v2.Get(0)->GetObject<PointToPointNetDevice>()->TraceConnectWithoutContext ("MacRx", MakeCallback(&Observador::PaqueteEnviado4, &observador));

  // SUSCRIPCION TRAZAS COLA 
  PointerValue puntero ;
  d2d3.Get(1)->GetAttribute("TxQueue", puntero);
  Ptr<Object>  objeto = puntero.GetObject();
  Ptr<DropTailQueue> cola = objeto->GetObject<DropTailQueue>();
  cola->TraceConnectWithoutContext ("Enqueue", MakeCallback(&Observador::SumaColaR1I1,&observador));
  cola->TraceConnectWithoutContext ("Dequeue", MakeCallback(&Observador::RestaColaR1I1,&observador));
  d2d3v2.Get(1)->GetAttribute("TxQueue", puntero);
  objeto = puntero.GetObject();
  cola = objeto->GetObject<DropTailQueue>();
  cola->TraceConnectWithoutContext ("Enqueue", MakeCallback(&Observador::SumaColaR1I2,&observador));
  cola->TraceConnectWithoutContext ("Dequeue", MakeCallback(&Observador::RestaColaR1I2,&observador));
  d2d3.Get(0)->GetAttribute("TxQueue", puntero);
  objeto = puntero.GetObject();
  cola = objeto->GetObject<DropTailQueue>();
  cola->TraceConnectWithoutContext ("Enqueue", MakeCallback(&Observador::SumaColaR2I1,&observador));
  cola->TraceConnectWithoutContext ("Dequeue", MakeCallback(&Observador::RestaColaR2I1,&observador));
  d2d3v2.Get(0)->GetAttribute("TxQueue", puntero);
  objeto = puntero.GetObject();
  cola = objeto->GetObject<DropTailQueue>();
  cola->TraceConnectWithoutContext ("Enqueue", MakeCallback(&Observador::SumaColaR2I2,&observador));
  cola->TraceConnectWithoutContext ("Dequeue", MakeCallback(&Observador::RestaColaR2I2,&observador));

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

}

