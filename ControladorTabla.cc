/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include "ControladorTabla.h"

NS_LOG_COMPONENT_DEFINE ("Controla");


ControladorTabla::ControladorTabla (Ptr<Ipv4> uno , Ptr<Ipv4> dos)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_uno = uno; 
  m_dos = dos;
  m_metrica = 0;
  m_if3 = 3;
  m_if4 = 4;  
}

void 
ControladorTabla::cambiaEnlace2 () {

  m_uno->SetMetric (m_if3, 1-m_metrica);
  m_uno->SetMetric (m_if4, m_metrica);
  
  m_dos->SetMetric (m_if3, 1-m_metrica);
  m_dos->SetMetric (m_if4, m_metrica);  
  
  Ipv4GlobalRoutingHelper::RecomputeRoutingTables();
  
  if (m_metrica == 0)
    m_metrica = 1;
  else
    m_metrica = 0;
}

void 
ControladorTabla::cambiaEnlace (Time inicio, Time fin, Time salto) {
  
  // The first ifIndex is 0 for loopback, then the first p2p is numbered 1, etc.
  //uint32_t if1 = 1;
  //uint32_t if2 = 2;
  uint32_t if3 = 3;
  uint32_t if4 = 4;
  Time actual = inicio;
  uint16_t metrica = 0;

  while (actual.GetSeconds() < fin.GetSeconds() ) {
    Simulator::Schedule (actual-Seconds(0.001),&Ipv4::SetMetric, m_uno, if3, 1-metrica);
    Simulator::Schedule (actual-Seconds(0.001),&Ipv4::SetMetric, m_uno, if4, metrica);
  
    Simulator::Schedule (actual-Seconds(0.001),&Ipv4::SetMetric, m_dos, if3, 1-metrica);
    Simulator::Schedule (actual-Seconds(0.001),&Ipv4::SetMetric, m_dos, if4, metrica);  
  
    Simulator::Schedule (actual, &Ipv4GlobalRoutingHelper::RecomputeRoutingTables);
    actual = actual + salto;
  
    if (metrica == 0)
      metrica = 1;
    else
      metrica = 0;
  }
}
