/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/core-module.h>
#include "Observador.h"
#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"

NS_LOG_COMPONENT_DEFINE ("Observador");

Observador::Observador()
{
  NS_LOG_FUNCTION_NOARGS();
  m_paquetes=0;
  m_paquetes2=0;
  m_paquetes3=0;
  m_paquetes4=0;
}

Observador::Observador(Ptr<Ipv4> uno , Ptr<Ipv4> dos)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_paquetes=0;
  m_paquetes2=0;
  m_paquetes3=0;
  m_paquetes4=0;
  m_colaR1I1=0;
  m_colaR1I2=0;
  m_colaR2I1=0;
  m_colaR2I2=0;
  m_controla = CreateObject<ControladorTabla> (uno,dos);
   
}


void
Observador::PaqueteEnviado (Ptr<const Packet> paquete)
{
  /*
     Ptr<Packet>          packet  = paquete->Copy();
     Ipv4Address          source;
     PppHeader            header;
     Ipv4Header           headerip;

     packet->RemoveHeader(header);
     // Compruebo que sea un paquete IP/UDP
     // Al sólo haber un aplicación que use IP/UDP puedo asegurar
     // que pertenece a la aplicación este paquete.
     if (header.GetProtocol() == 33) {
       packet->RemoveHeader(headerip);
       source = headerip.GetSource();
       uint32_t origen  = source.Get();
       uint32_t octeto4 = (origen >> (0)) & 0xFF;
       uint32_t octeto3 = (origen >> (8)) & 0xFF;
       uint32_t octeto2 = (origen >> (16)) & 0xFF;
       uint32_t octeto1 = (origen >> (24)) & 0xFF;
       NS_LOG_INFO ("Paquete enviado en el instante " << (float) Simulator::Now().GetSeconds() << "s desde: " << (unsigned int) octeto1 << "."
                   << (unsigned int) octeto2 << "." << (unsigned int) octeto3 << "." << (unsigned int) octeto4 );
     }
     else {
       NS_LOG_INFO ("Error paquete");
     }
  */
  NS_LOG_INFO ("Llega un paquete por interfaz 1 a Router2");
  m_paquetes++;
   
}

void
Observador::PaqueteEnviado2 (Ptr<const Packet> paquete)
{
  /*
     Ptr<Packet>          packet  = paquete->Copy();
     Ipv4Address          source;
     PppHeader            header;
     Ipv4Header           headerip;

     packet->RemoveHeader(header);
     // Compruebo que sea un paquete IP/UDP
     // Al sólo haber un aplicación que use IP/UDP puedo asegurar
     // que pertenece a la aplicación este paquete.
     if (header.GetProtocol() == 33) {
       packet->RemoveHeader(headerip);
       source = headerip.GetSource();
       uint32_t origen  = source.Get();
       uint32_t octeto4 = (origen >> (0)) & 0xFF;
       uint32_t octeto3 = (origen >> (8)) & 0xFF;
       uint32_t octeto2 = (origen >> (16)) & 0xFF;
       uint32_t octeto1 = (origen >> (24)) & 0xFF;
       NS_LOG_INFO ("Paquete enviado en el instante " << (float) Simulator::Now().GetSeconds() << "s desde: " << (unsigned int) octeto1 << "."
                   << (unsigned int) octeto2 << "." << (unsigned int) octeto3 << "." << (unsigned int) octeto4 );
     }
     else {
       NS_LOG_INFO ("Error paquete");
     }
  */
  NS_LOG_INFO ("Llega un paquete por interfaz 2 a Router2");
  m_paquetes2++;
   
}

void
Observador::PaqueteEnviado3 (Ptr<const Packet> paquete)
{
  /*
     Ptr<Packet>          packet  = paquete->Copy();
     Ipv4Address          source;
     PppHeader            header;
     Ipv4Header           headerip;

     packet->RemoveHeader(header);
     // Compruebo que sea un paquete IP/UDP
     // Al sólo haber un aplicación que use IP/UDP puedo asegurar
     // que pertenece a la aplicación este paquete.
     if (header.GetProtocol() == 33) {
       packet->RemoveHeader(headerip);
       source = headerip.GetSource();
       uint32_t origen  = source.Get();
       uint32_t octeto4 = (origen >> (0)) & 0xFF;
       uint32_t octeto3 = (origen >> (8)) & 0xFF;
       uint32_t octeto2 = (origen >> (16)) & 0xFF;
       uint32_t octeto1 = (origen >> (24)) & 0xFF;
       NS_LOG_INFO ("Paquete enviado en el instante " << (float) Simulator::Now().GetSeconds() << "s desde: " << (unsigned int) octeto1 << "."
                   << (unsigned int) octeto2 << "." << (unsigned int) octeto3 << "." << (unsigned int) octeto4 );
     }
     else {
       NS_LOG_INFO ("Error paquete");
     }
  */
  NS_LOG_INFO ("Llega un paquete por interfaz 1 a Router1");
  m_paquetes3++;
   
}

void
Observador::PaqueteEnviado4 (Ptr<const Packet> paquete)
{
  /*
     Ptr<Packet>          packet  = paquete->Copy();
     Ipv4Address          source;
     PppHeader            header;
     Ipv4Header           headerip;

     packet->RemoveHeader(header);
     // Compruebo que sea un paquete IP/UDP
     // Al sólo haber un aplicación que use IP/UDP puedo asegurar
     // que pertenece a la aplicación este paquete.
     if (header.GetProtocol() == 33) {
       packet->RemoveHeader(headerip);
       source = headerip.GetSource();
       uint32_t origen  = source.Get();
       uint32_t octeto4 = (origen >> (0)) & 0xFF;
       uint32_t octeto3 = (origen >> (8)) & 0xFF;
       uint32_t octeto2 = (origen >> (16)) & 0xFF;
       uint32_t octeto1 = (origen >> (24)) & 0xFF;
       NS_LOG_INFO ("Paquete enviado en el instante " << (float) Simulator::Now().GetSeconds() << "s desde: " << (unsigned int) octeto1 << "."
                   << (unsigned int) octeto2 << "." << (unsigned int) octeto3 << "." << (unsigned int) octeto4 );
     }
     else {
       NS_LOG_INFO ("Error paquete");
     }
  */
  NS_LOG_INFO ("Llega un paquete por interfaz 2 a Router1");
  m_paquetes4++;
   
}


void    
Observador::SumaColaR1I1 (Ptr<const Packet> paquete) {
  NS_LOG_FUNCTION_NOARGS ();
  m_colaR1I1++;
  if (m_colaR1I1 == 3) {
    m_controla->cambiaEnlace2();
    NS_LOG_INFO("Cola llena, cambio enrutamiento");
  }
}
void     
Observador::RestaColaR1I1 (Ptr<const Packet> paquete) {
  NS_LOG_FUNCTION_NOARGS ();
  m_colaR1I1--;
}
void    
Observador::SumaColaR1I2 (Ptr<const Packet> paquete) {
  NS_LOG_FUNCTION_NOARGS ();
  m_colaR1I2++;
  if (m_colaR1I2 == 3) {
    m_controla->cambiaEnlace2();
    NS_LOG_INFO("Limito cola superado, cambio enrutamiento");
  }
}
void     
Observador::RestaColaR1I2 (Ptr<const Packet> paquete) {
  NS_LOG_FUNCTION_NOARGS ();
  m_colaR1I2--;
}
void    
Observador::SumaColaR2I1 (Ptr<const Packet> paquete) {
  NS_LOG_FUNCTION_NOARGS ();
  m_colaR2I1++;
  if (m_colaR2I1 == 3) {
    m_controla->cambiaEnlace2();
    NS_LOG_INFO("Limite cola superado, cambio enrutamiento");
  }
}
void     
Observador::RestaColaR2I1 (Ptr<const Packet> paquete) {
  NS_LOG_FUNCTION_NOARGS ();
  m_colaR2I1--;
}
void    
Observador::SumaColaR2I2 (Ptr<const Packet> paquete) {
  NS_LOG_FUNCTION_NOARGS ();
  m_colaR2I2++;
  if (m_colaR2I2 == 3) {
    m_controla->cambiaEnlace2();
    NS_LOG_INFO("Limite cola superado, cambio enrutamiento");
  }
}
void     
Observador::RestaColaR2I2 (Ptr<const Packet> paquete) {
  NS_LOG_FUNCTION_NOARGS ();
  m_colaR2I2--;
}
  
