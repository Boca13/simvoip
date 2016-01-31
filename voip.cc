#include "voip.h"

//#include "centralita.h"
#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/onoff-application.h"
#include "voip.h"
    
using namespace ns3;
#define MAXONVOIP 0.1
#define MINONVOIP 0.2
#define MAXOFFVOIP 0.8
#define MINOFFVOIP 0.9



voip::voip (Ptr<Centralita> central, uint64_t tamPkt, DataRate tasa, Ptr<RandomVariableStream> tiempo){

	

}



void
voip::setTasa (DataRate m_tasa){


}


void
voip::setDestino    (Address destino){




}


void
voip::setTamPaquete (uint64_t tamPaquete){




}
void
voip::setTiempo (Ptr<RandomVariableStream> tiempo){

m_tiempo = tiempo;



}


bool
voip::getOcupado(){
return m_ocupado;
}


void voip::CancelaLlamada (){

Simulator::Cancel(m_proximallamada);
}

void voip::ProgramaLlamada (){

Simulator::Schedule (m_tiempo, NULL, Llama ); //m_tiempo no es tipo Time, falla

}



void
voip::Llama (){

	//Me pongo en contacto con la centralita


	//Configuro OnOff?


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



void
voip::Cuelga(){

	//llamo al OnOffStopApplication


	//Llamo a programallamada
	ProgramaLlamada();


}

void voip::Descuelga(Address destino, Time duracion){

	CancelaLlamada();

	//ConfiguroOnOffApplication


}
