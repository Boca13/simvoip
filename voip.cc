#include "voip.h"
#include "Central.h"
    
using namespace ns3;
#define MAXONVOIP 0.1
#define MINONVOIP 0.2
#define MAXOFFVOIP 0.8
#define MINOFFVOIP 0.9


voip::voip (Central * centralita, uint64_t tamPkt, Time media, Time duracion, DataRate tasaCodec[2], Address IP, Ptr<Node> node){

	m_centralita = centralita;
	m_ocupado = false;
	m_tamPaquete = tamPkt;
	tiempo_entre_llamadas.SetAttribute("Mean",DoubleValue(media.GetSeconds()) );
	duracion_de_llamada.SetAttribute("Mean", DoubleValue (duracion.GetSeconds()));
    uint32_t minTasa = tasaCodec[0].GetBitRate();
    uint32_t maxTasa = tasaCodec[1].GetBitRate();
    m_tasa = tasa_llamadas.GetValue(minTasa,maxTasa);
    m_IP = IP;
    m_node = node;
    m_numeroNodo = m_centralita->registrar (m_IP);
	
}




void
voip::setDestino (Address destino){

	m_destino = destino;
}


void
voip::setTamPaquete (uint64_t tamPkt){

	m_tamPaquete = tamPkt;

}



bool
voip::getOcupado(){
return m_ocupado;
}


void voip::CancelaLlamada (){

Simulator::Cancel(m_proximallamada);
}

void voip::ProgramaLlamada (){

	m_proximallamada=Simulator::Schedule (Time (tiempo_entre_llamadas.GetValue()) , this, Llama ); //m_tiempo no es tipo Time, falla

}



void
voip::Llama (){

	//Me pongo en contacto con la centralita
	m_ocupado=true;
	m_llamadaactual = m_proximallamada;
	m_duracion = Time(duracion_de_llamada.GetValue());

		Address direccion_envio = m_centralita->llamar ( m_numeroNodo, m_duracion);

	//Configuro OnOff?

		m_AppOnOff = OnOffHelper("ns3::UdpSocketFactory", direccion_envio);

		m_AppOnOff.SetConstantRate (DataRate( m_tasa) );
		m_AppOnOff.SetAttribute("PacketSize",UintegerValue( m_tamPaquete) );




	  varon=CreateObject<UniformRandomVariable>();
	  varon->SetAttribute("Max", DoubleValue(MAXONVOIP));
	  varon->SetAttribute("Min", DoubleValue(MINONVOIP));

	  varoff=CreateObject<UniformRandomVariable>();
	  varoff->SetAttribute("Max", DoubleValue(MAXOFFVOIP));
	  varoff->SetAttribute("Min", DoubleValue(MINOFFVOIP));
	  //Configuramos la aplicación OnOff

	  m_AppOnOff.SetAttribute("OnTime", PointerValue(varon));
	  m_AppOnOff.SetAttribute("OffTime", PointerValue(varoff));




	  m_appc = m_AppOnOff.Install(m_node); //Pasarle por parametro al puntero al nodo.
	  m_appc.Start(Simulator::Now());
	  Simulator::Schedule (Simulator::Now()+m_duracion,this, Cuelga);

}



void
voip::Cuelga(){

	//llamo al OnOffStopApplication
	m_appc.Stop(Simulator::Now());
	m_ocupado=0;
	//Llamo a programallamada
	ProgramaLlamada();


}

void voip::Descuelga(Address destino, Time duracion){

	CancelaLlamada();


	m_AppOnOff = OnOffHelper("ns3::UdpSocketFactory", destino);

			m_AppOnOff.SetConstantRate (DataRate( m_tasa) );




		  varon=CreateObject<UniformRandomVariable>();
		  varon->SetAttribute("Max", DoubleValue(MAXONVOIP));
		  varon->SetAttribute("Min", DoubleValue(MINONVOIP));

		  varoff=CreateObject<UniformRandomVariable>();
		  varoff->SetAttribute("Max", DoubleValue(MAXOFFVOIP));
		  varoff->SetAttribute("Min", DoubleValue(MINOFFVOIP));
		  //Configuramos la aplicación OnOff

		  m_AppOnOff.SetAttribute("OnTime", PointerValue(varon));
		  m_AppOnOff.SetAttribute("OffTime", PointerValue(varoff));




		  m_appc = m_AppOnOff.Install(m_node); //Pasarle por parametro al puntero al nodo.
		  m_appc.Start(Simulator::Now());
		  Simulator::Schedule (Simulator::Now()+m_duracion,this, Cuelga);

}
