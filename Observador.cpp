#include "Observador.h"


Observador::Observador()
{
  m_enviados=0;
  m_recibidos=0;
  m_perdidos=0;
  m_QoS=0;
  m_usuarios=0;
}


Observador::~Observador()
{
}

void
Observador::PktEnviado(Pkt<Const Packet>paquete);
{
  t_enviado=Simulator::Now();
  m_enviados=m_enviados++
  uint64_t id = paquete->GetUid();
 array[id]=t_enviados
}

void 
Observador::PktRecibido(Pkt<const Packet>paquete, const Address & dir)
{
  std::map<uint64_t,Time>::iterator aux=array.find(paquete->GetUid());
  if (array.end() == aux)
    {
       NS_LOG_INFO("NO ESTA EN LA ESTRUCTURA");
     }
      else
        {
          NS_LOG_INFO("SI ESTA EN LA ESTRUCTURA");
          Time Taux =array[paquete->GetUid()];
          Jitter.Update((Simulator::Now()-Taux).GetMilliSeconds());
          m_recibidos=m_recibidos+1;
          array.erase(aux);
        }
}

float
Observador::QoSActual();
{
  m_perdidos=array.Size();
  m_QoS=((m_recibidos)/m_enviados)*100; //referente a paquetes perdidos
 return m_QoS; 
}




float 
Observador::ActualizaJitter()
{
  return Jitter.Mean();
}

Void
Observador:Reset()
{
  Jitter.Reset();
}
