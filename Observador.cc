#include "Observador.h"

NS_LOG_COMPONENT_DEFINE ("Observador");
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
Observador::PktEnviado(Ptr<const Packet>paquete)
{
  /*suponemos que el paquete está en la estructura*/


  	t_encolado=Simulator::Now();
 	uint64_t id = paquete->GetUid();
 	array[id]=t_encolado;
	m_enviados++;
  
}



void 
Observador::PktRecibido(Ptr<const Packet>paquete, const Address & dir)
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
	  Time Ahora = Simulator::Now();
          Retardo.Update((Ahora-Taux).GetMilliSeconds());

	if(t_TiempoPaqueteAnterior != 0)
	  Jitter.Update(((Ahora-Taux)-t_TiempoPaqueteAnterior).GetMilliSeconds());

          m_recibidos=m_recibidos+1;
	  t_TiempoPaqueteAnterior=(Ahora-Taux);
          array.erase(aux);
        }
}

float
Observador::QoSActual()
{
  
   /*
    modelo E 
    QoS= (Ro − Is) − Id − Ie-eff + A 
   jitter >= 100ms
    retardo >= 150ms
    % >=1
  
    */
   m_perdidos=array.size();
   /* debe ser menor a  un 0.01 o un 1%*/
   m_porcentaje=((m_perdidos)/m_enviados)*100; //referente a paquetes perdidos
  
  
    m_QoS=(m_porcentaje/PORCENTAJE + Jitter.Mean()/JITTER + Retardo.Mean()/RETARDO)/3;
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

void
Observador::Reset()
{
  Jitter.Reset();
  Retardo.Reset();
}
