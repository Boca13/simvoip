/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "centralita.h"
#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/onoff-application.h"
    
namespace ns3 {
 
  class Address;
  class RandomVariableStream;
   
  class Voip : public Application 
  {
    public:
      static TypeId GetTypeId (void);
   
      // Constructor de la clase, incializa Parametros: m-central=central, m_ocupado=false, m_tasa=tasa,
      // m_tamPaquete = tamanio, m_timepo = tiempo, etc.
      Voip (Ptr<Centralita> central, uint64_t tasa, DataRate tasa, Ptr<RandomVariableStream> tiempo);   
      virtual ~Voip();   
      void setTasa       (DataRate m_tasa);
      void setDestino    (Address destino);
      void setTamPaquete (uint64_t tamPaquete);
      void setTiempo     (Ptr<RandomVariableStream> tiempo);
      bool getOcupado();
   
    private:
      // Heredadas de Aplication.
      // Inicializa (si hace falta) y llama a ProgramaLlamada 
      virtual void StartApplication (void);   
      // Cancela las llamadas que haya (tanto pendientes como cursandose)
      virtual void StopApplication (void);   
  
      // Cancela la proxima llamada (m_proximaLLamada)
      void CancelaLlamada ();
      // Programa (schedule) una llamada a la funcion Llama() en m_tiempo
      void ProgramaLlamada ();
      // Se pone en contacto con centralita, le indica que quiere hacer una llamada
      // de m_duracion tiempo. La centralita le devuelve un Address del destino.
      // Configura un OnOffAplication adecuadamente y hace un Start a
      // OnOffAplication. Por ultimo, programa (schedule) una 
      // llamada a la funcion Cuelga() en m_duracion 
      void Llama ();
      // Hace una llamada al Stop de OnOffApplication y llama al funcion ProgramaLlamada
      void Cuelga ();
      // Cancela (CancelaLLamada) la llamada pendiente y configura un OnOffAplication 
      // adecuadamente y hace un Start a OnOffAplication al destino indicado. Por ultimo, 
      // programa (schedule) una llamada a la funcion Cuelga() en duracion  
      void Descuelga (Address destino, Time duracion); 
      
      Address         m_destino;         
      bool            m_ocupado;            // Indica si el terminal esta ocupado.
      DataRate        m_tasa;
      uint64_t        m_tamPaquete;     
      Ptr<Centralita> m_centralita;          
      EventId         m_proximallamada;     // Para poder cancelarla.
      EventId         m_llamadaactual;      // Para poder cancelarla.  
      TypeId          m_tid;                
      Ptr<RandomVariableStream> m_tiempo;   // Tiempo medio entre llamadas  
      Ptr<RandomVariableStream> m_duracion; // Duracion de la llamada.
      Ptr<OnOffApplication> m_onoff;        
    
  };
   
} // namespace ns3
  
