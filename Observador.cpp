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
Observador::PktEncolado(Pkt<Const Packet>paquete)
{
   t_encolado=Simulator::Now();
  uint64_t id = paquete->GetUid();
 array[id]=t_encolado
}




void
Observador::PktEnviado(Pkt<Const Packet>paquete)
{
  /*suponemos que el paquete está en la estructura*/
    std::map<uint64_t,Time>::iterator aux=array.find(paquete->GetUid());
     Time Taux =array[paquete->GetUid()]; 
     Jitter.Update((Simulator::Now()-Taux).GetMilliSeconds());
     m_enviados=m_enviados++
  
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
          Retardo.Update((Simulator::Now()-Taux).GetMilliSeconds());
          m_recibidos=m_recibidos+1;
          array.erase(aux);
        }
}

float
Observador::QoSActual();
{
  
  /*
  modelo E 
  QoS= (Ro − Is) − Id − Ie-eff + A 
  jitter >= 100ms
  retardo >= 150ms
  % >=1
  
  */
  m_perdidos=array.Size();
  /* debe ser menor a  un 0.01 o un 1%*/
  m_porcentaje=((m_perdidos)/m_enviados)*100; //referente a paquetes perdidos
  
  
  m_QoS=
 return m_QoS; 
}




float 
Observador::ActualizaJitter()
{
  /* el jitter debe se menor de 100 ms */
  return Jitter.Mean();
}

float
Observador::ActualizaRetardo()
{
  /* el retardo debe ser menor de 150ms */
  return Retardo.Mean();
}

Void
Observador:Reset()
{
  Jitter.Reset();
  Retardo.Reset();
}
