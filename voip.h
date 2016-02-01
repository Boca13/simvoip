/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "Central.h"
#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/onoff-application.h"
#include "Observador.h"

    
using namespace ns3;

  class voip : public Application
  {
    public:
   
      // Constructor de la clase, incializa Parametros: m-central=central, m_ocupado=false, m_tasa=tasa,
      // m_tamPaquete = tamanio, m_timepo = tiempo, etc.
      voip (Central * centralita, uint64_t tamPkt, Time media, Time duracion, DataRate tasaCodec[2], Address IP,Ptr<Node> node );
	  // Constructor por defecto para el new
	  voip();
	  // Operador de copia
	  voip& operator=(const voip& otra) {
		  this->m_appc = otra.m_appc;
		  this->m_AppOnOff = otra.m_AppOnOff;
		  this->m_centralita = otra.m_centralita;
		  this->m_destino = otra.m_destino;
		  this->m_duracion = otra.m_duracion;
		  this->m_IP = otra.m_IP;
		  this->m_llamadaactual = otra.m_llamadaactual;
		  this->m_node = otra.m_node;

		  this->m_numeroNodo = otra.m_numeroNodo;
		  this->m_ocupado = otra.m_ocupado;
		  this->m_proximallamada = otra.m_proximallamada;
		  this->m_startEvent = otra.m_startEvent;
		  this->m_startTime = otra.m_startTime;
		  this->m_stopEvent = otra.m_stopEvent;
		  this->m_stopTime = otra.m_stopTime;
		  this->m_tamPaquete = otra.m_tamPaquete;
		  this->m_tasa = otra.m_tasa;
		  
		  return *this;
	  }

      void setDestino    (Address destino);
      void setTamPaquete (uint64_t tamPaquete);
      //void setTiempo     (Ptr<RandomVariableStream> tiempo);
      bool getOcupado();
   
    private:
      // Heredadas de Aplication.
      // Inicializa (si hace falta) y llama a ProgramaLlamada 
      virtual void StartApplication (void){

    	  	 //Aqui debe estar la suscripcion a trazas de esta aplicacion (si la hay)
    	  ProgramaLlamada();

      }  
      // Cancela las llamadas que haya (tanto pendientes como cursandose)
      virtual void StopApplication (void){

    	  Simulator::Cancel(m_proximallamada);
    	  Simulator::Cancel(m_llamadaactual);
        
      }   
  
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
      uint64_t        m_tamPaquete;     
      Central *		  m_centralita;
      EventId         m_proximallamada;     // Para poder cancelarla.
      EventId         m_llamadaactual;      // Para poder cancelarla.  
      Time			  m_duracion; // Duracion de la llamada.
      Address		  m_IP;
      UniformRandomVariable tiempo_entre_llamadas;
      UniformRandomVariable duracion_de_llamada;
      UniformRandomVariable tasa_llamadas;
      uint32_t		 	m_tasa;
      Ptr<UniformRandomVariable> varon;
      Ptr<UniformRandomVariable> varoff;
      Ptr<Node>      	m_node;
      OnOffHelper  		m_AppOnOff;
      uint32_t 			m_numeroNodo;
      ApplicationContainer m_appc;

    
  };
   

  
