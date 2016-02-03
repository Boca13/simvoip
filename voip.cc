#include "voip.h"
#include "Central.h"

using namespace ns3;
#define MAXONVOIP 0.1
#define MINONVOIP 0.2
#define MAXOFFVOIP 0.8
#define MINOFFVOIP 0.9


NS_LOG_COMPONENT_DEFINE("voip");


voip::voip(Central * centralita, uint64_t tamPkt, Time media, Time duracion, DataRate tasaCodec[2], Ipv4Address IP, Ptr<Node> node) : Application()
{
	tiempo_entre_llamadas = CreateObject<ExponentialRandomVariable>();
	duracion_de_llamada = CreateObject<ExponentialRandomVariable>();
	tasa_llamadas = CreateObject<UniformRandomVariable>();
	m_centralita = centralita;
	m_ocupado = false;
	m_tamPaquete = tamPkt;
	tiempo_entre_llamadas->SetAttribute("Mean", DoubleValue(media.GetSeconds()));
	duracion_de_llamada->SetAttribute("Mean", DoubleValue(duracion.GetSeconds()));
	uint32_t minTasa = tasaCodec[0].GetBitRate();
	uint32_t maxTasa = tasaCodec[1].GetBitRate();
	m_tasa = tasa_llamadas->GetValue(minTasa, maxTasa);
	m_IP = IP;
	m_node = node;
	m_numeroNodo = m_centralita->registrar(m_IP, this);
	m_AppOnOff = NULL;
}


TypeId voip::GetTypeId(void)
{
	static TypeId tid = TypeId("voip").SetParent<Object>();
	return tid;
}


void
voip::setDestino(Address destino) {

	m_destino = destino;
}


void
voip::setTamPaquete(uint64_t tamPkt) {

	m_tamPaquete = tamPkt;

}



bool
voip::getOcupado() {
	return m_ocupado;
}


void voip::CancelaLlamada() {
	NS_LOG_DEBUG("[voip] Se va a cancelar el evento con UID " << m_proximallamada.GetUid() << " en el nodo " << m_numeroNodo);
	/*if (m_llamadaactual.IsRunning())
		Simulator::Cancel(m_llamadaactual);
	if (m_proximallamada.IsRunning())*/
		Simulator::Cancel(m_proximallamada);
}

void voip::ProgramaLlamada() {
	Time tiempo = Seconds(tiempo_entre_llamadas->GetValue());
	//NS_LOG_DEBUG("[voip] Tiempo entre llamadas: "<< tiempo_entre_llamadas->GetValue());

	NS_LOG_DEBUG("[voip] El nodo "<< m_numeroNodo<<" programa una llamada PARA el tiempo "<< (Simulator::Now()+tiempo).GetSeconds() );

	m_proximallamada = Simulator::Schedule(tiempo, &voip::Llama, this); //m_tiempo no es tipo Time, falla

}



void
voip::Llama() {
	
	//Me pongo en contacto con la centralita
	m_ocupado = true;
	m_llamadaactual = m_proximallamada;
	//m_proximallamada = EventId();
	m_duracion = Seconds(duracion_de_llamada->GetValue());
	
	voip * puntero_llamado = NULL;
	Ipv4Address direccion_envio = m_centralita->llamar(m_numeroNodo, m_duracion, &puntero_llamado);

	if (direccion_envio==Ipv4Address((uint32_t)0)||(puntero_llamado==NULL))
	{
		NS_LOG_DEBUG("[voip] El telefono llamado esta ocupado. Programando una llamada para más adelante.");
		ProgramaLlamada();
	}
	else
	{
		NS_LOG_DEBUG("[voip] El nodo " << m_numeroNodo << " llama al nodo " << puntero_llamado->GetNumeroNodo() << " en el instante " << Simulator::Now().GetSeconds() << "durante " << m_duracion.GetSeconds());

		Simulator::ScheduleNow(&voip::Descuelga, puntero_llamado, m_IP, m_duracion);

		Address dEnvio = Address(InetSocketAddress(direccion_envio, 5006));

		m_AppOnOff = new OnOffHelper("ns3::UdpSocketFactory", dEnvio);

		NS_LOG_DEBUG("[voip] Creado OnOffHelper que enviará a la dirección " << dEnvio);
		NS_LOG_DEBUG("[voip] Creado OnOffHelper que enviará a la dirección IP " << direccion_envio << ", puerto " << 5006);

		m_AppOnOff->SetConstantRate(DataRate(m_tasa));
		m_AppOnOff->SetAttribute("PacketSize", UintegerValue(m_tamPaquete));




		varon = CreateObject<UniformRandomVariable>();
		varon->SetAttribute("Max", DoubleValue(MAXONVOIP));
		varon->SetAttribute("Min", DoubleValue(MINONVOIP));

		varoff = CreateObject<UniformRandomVariable>();
		varoff->SetAttribute("Max", DoubleValue(MAXOFFVOIP));
		varoff->SetAttribute("Min", DoubleValue(MINOFFVOIP));
		//Configuramos la aplicación OnOff

		m_AppOnOff->SetAttribute("OnTime", PointerValue(varon));
		m_AppOnOff->SetAttribute("OffTime", PointerValue(varoff));




		m_appc = m_AppOnOff->Install(m_node); //Pasarle por parametro al puntero al nodo.
		//NS_LOG_DEBUG("[voip] La direccion es: "<<m_IP);
		//NS_LOG_DEBUG("[voip] El nodo es: " << m_node->GetId());
		m_appc.Start(Simulator::Now());

		m_cuelga= Simulator::Schedule(m_duracion, &voip::Cuelga, this);
	}
}



void
voip::Cuelga() {

	//llamo al OnOffStopApplication
	m_appc.Stop(Simulator::Now());
	m_ocupado = 0;
	//Llamo a programallamada
	ProgramaLlamada();
	delete(m_AppOnOff);
	m_AppOnOff = NULL;

}

void voip::Descuelga(Address destino, Time duracion) {

	CancelaLlamada();


	m_AppOnOff = new OnOffHelper("ns3::UdpSocketFactory", destino);

	m_AppOnOff->SetConstantRate(DataRate(m_tasa));


	NS_LOG_DEBUG ("Soy el nodo: "<<m_numeroNodo << " y recibo una llamada de: "<< destino << "durante: "<< duracion.GetSeconds() << " en el instante:" << Simulator::Now().GetSeconds()  );

	varon = CreateObject<UniformRandomVariable>();
	varon->SetAttribute("Max", DoubleValue(MAXONVOIP));
	varon->SetAttribute("Min", DoubleValue(MINONVOIP));

	varoff = CreateObject<UniformRandomVariable>();
	varoff->SetAttribute("Max", DoubleValue(MAXOFFVOIP));
	varoff->SetAttribute("Min", DoubleValue(MINOFFVOIP));
	//Configuramos la aplicación OnOff

	m_AppOnOff->SetAttribute("OnTime", PointerValue(varon));
	m_AppOnOff->SetAttribute("OffTime", PointerValue(varoff));


	m_appc = m_AppOnOff->Install(m_node); //Pasarle por parametro al puntero al nodo.
	m_appc.Start(Simulator::Now());

	m_llamadaactual = Simulator::Schedule(m_duracion, &voip::Cuelga, this);

}
void voip::StopApplication(){

NS_LOG_DEBUG("Entro en StopApplication en el tiempo: "<<Simulator::Now().GetSeconds());
	/*Cuelga();
	 Simulator::Cancel(m_proximallamada);
	 Simulator::Cancel(m_llamadaactual);
	 Simulator::Cancel(m_cuelga);
*/
//Simulator::Stop();


}

void voip::StartApplication (void){

	NS_LOG_DEBUG("Entro en StartApplication en el tiempo: "<<Simulator::Now().GetSeconds());

	ProgramaLlamada();

}






