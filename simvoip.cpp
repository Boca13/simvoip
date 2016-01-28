/*
Parámetros de entrada del programa:
· Número de usuarios en sede 1
· Número de usuarios en sede 2
· Distintas velocidades de enlace que se pueden contratar con el operador
· Tasa media de ocurrencia de llamadas de los usuarios
· Calidad de servicio requerida
· Intervalo de la tasa de bits del códec de audio.

Parámetros de salida del programa:
· Número de enlaces y velocidad a contratar con el operador.
· Número máximo de usuarios que pueden hablar a la vez.
· Parámetros de calidad conseguidos finalmente (retardo etc.)
· Gráficas:
· Una gráfica para cada número de enlaces con la evolución de la QoS respecto a la capacidad de enlace.
· Una gráfica para cada número de enlaces con la evolución de los parámetros usados para medir la QoS según la capacidad de enlace.

*/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/gnuplot.h"
#include "Observador.h"

#define DURACION 120.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Simvoip");

// Punto 3D
typedef struct {
	uint8_t enlaces;
	uint8_t velocidad;
	double qos;
}Punto;


/*
*	Recibe el n�mero de enlaces y su velocidad y el objeto observador a utilizar
*	devuelve la QoS conseguida
*/
double simular(Punto * resultado, std::map<uint8_t, DataRate> velocidades, Observador *observador,uint16_t telef1,uint16_t telef2, uint8_t enlaces);


using namespace ns3;

int main(int argc, char *argv[])
{
	double qos_objetivo = 1.0;
	uint32_t telef1 = 100;
	uint32_t telef2 = 100;
	std::string sVelocidades = "1Mbps;2Mbps;5Mbps;10Mbps;20Mbps;30Mbps;50Mbps;100Mbps;200Mbps;300Mbps";
	double tasaLlamadas = 0.67;
	std::string sTasaCodec = "64kbps;96kbps";
	DataRate tasaCodec[2];	// Forman el intervalo en el que estará la tasa distribuida uniformemente
	std::map<uint8_t, DataRate> velocidades;	// Velocidades permitidas

	// Obtener parámetros de cmd
	CommandLine cmd;
	cmd.AddValue("qos", "Calidad de servicio objetivo", qos_objetivo);
	cmd.AddValue("tA", "Número de usuarios en la sede A", telef1);
	cmd.AddValue("tB", "Número de usuarios en la sede B", telef2);
	cmd.AddValue("velocidades", "velocidades de enlace que se pueden contratar con el operador separadas por ;", sVelocidades);
	cmd.AddValue("tasaLlamadas", "Tasa media de ocurrencia de llamadas de los usuarios", tasaLlamadas);
	cmd.AddValue("tasaCodec", "Intervalo de la tasa de bits del códec de audio", sTasaCodec);
	cmd.Parse(argc, argv);


	// Parsear cadena introduciendo velocidades de enlace posibles en el map
	size_t posicion = 0;
	uint8_t n = 0;
	while ((posicion = sVelocidades.find(";")) != std::string::npos) {
		// Guardar cada elemento en el map
		velocidades[n] = DataRate(sVelocidades.substr(0, posicion));
		sVelocidades.erase(0, posicion + 1);
	}

	// Parsear intervalo de tasa del codec
	n = 0;
	while ((posicion = sTasaCodec.find(";")) != std::string::npos) {
		tasaCodec[n] = DataRate(sTasaCodec.substr(0, posicion));
		sTasaCodec.erase(0, posicion + 1);
	}

	// TODO Preparar gráficas
	// Gráfica 1
	Gnuplot plot1;
	plot1.SetTitle("---");
	plot1.SetLegend("Tiempo medio de permanencia en el estado On", "Porcentaje de paquetes correctamente transmitidos");
	// Gráfica 2
	Gnuplot plot2;
	plot2.SetTitle("---");
	plot2.SetLegend("Tiempo medio de permanencia en el estado On", "Latencia (us)");

	
	Time::SetResolution(Time::US);

	Observador * observador = new Observador();
	Punto anterior = { 0,0,0.0 };
	Punto resultado;
	// Los resultados de las simulaciones se guardan en una estructura para el algoritmo de predicci�n
	while (resultado.qos < qos_objetivo)
	{
		simular(&resultado, velocidades, observador, telef1, telef2);
		// ALGORITMO DE PREDICCIÓN LINEALIZANDO
		// y = mx + n
		// m = dy/dx (delta)
		// x = enlaces*velocidad
		double m = ((resultado.qos - anterior.qos) / ((resultado.enlaces*velocidades[resultado.velocidad].GetBitRate() / 1e6) - (anterior.enlaces*velocidades[anterior.velocidad].GetBitRate() / 1e6)));
		// qos = mx -> x = qos/m
		uint32_t x = qos_objetivo / m;
		// Pasar el resultado al anterior
		anterior = resultado;
		// Cambiar coordenadas de resultado para la siguiente iteración
		// Enlaces mínimos
		resultado.enlaces = 0;
		resultado.velocidad = 0;
		while ((x / velocidades[resultado.velocidad].GetBitRate() / 1e6)>resultado.enlaces)
		{
			if (resultado.velocidad == (velocidades.size() - 1))
			{
				resultado.enlaces++;
				resultado.velocidad = 0;
			}
			else
			{
				resultado.velocidad++;
			}

		}
		delete observador;
	}

	return 0;
}
//todo decidir que velocidades tendra la red interna de cada sede, por ahora supongo
//velocidades de 100Mbps
//todo posible error: a los Routers les estoy instalando las apps que llevan los telefonos y les estoy dando direccionamiento
//solucionar eso


/*
 * velocidades[resultado->velocidad]; ---> velocidad que utilizo en los enlaces.
 *
 *
 *
 */


double simular(Punto * resultado, std::map<uint8_t, DataRate> velocidades, Observador *observador, uint16_t telef1, uint16_t telef2)
{

	Ptr<UniformRandomVariable> varon;
	Ptr<UniformRandomVariable> varoff;


	//primero creamos los R de salida de cada central
	NodeContainer R;
	R.Create(2);
	//Estos nodos irán unidos por enlaces p2p


	//Configuro la topologia p2p
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", DataRateValue (velocidades[resultado->velocidad]));
	p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

	//Creo los enlaces p2p entre centrales

	NetDeviceContainer Rdevices;

	for (uint8_t j=0;j<resultado->enlaces;j++){

		NetDeviceContainer enlaceRouters = p2p.Install (NodeContainer (R.Get(0), R.Get(1) ) );

		Rdevices.Add (enlaceRouters.Get(0));
		Rdevices.Add (enlaceRouters.Get(1));
	}




	//----Esto seria para las centrales. Ahora los telefonos/bridges
	NodeContainer terminales1; //Sede1
	NodeContainer terminales2; //Sede 2
	terminales1.Create(telef1);
	terminales2.Create(telef2);

	//Añado el R al nodecontainer de los telefonos para realizar los enlaces despues
	terminales1.Add(R.Get(0));
	terminales2.Add(R.Get(1));



	NodeContainer Bridge1;//Sede 1
	NodeContainer Bridge2;//Sede 2

	PointToPointHelper p2pInterno;
	p2pInterno.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2pInterno.SetChannelAttribute ("Delay", StringValue ("2ms"));

	NetDeviceContainer terminales1Devices;
	NetDeviceContainer Bridge1Devices;



	for(uint16_t j=0;j<telef1+1;j++){
		NetDeviceContainer enlace1 = p2pInterno.Install (NodeContainer(terminales1.Get(j),Bridge1)  );
		terminales1Devices.Add (enlace1.Get(0));
		Bridge1Devices.Add (enlace1.Get(1));
	}


	NetDeviceContainer terminales2Devices;
	NetDeviceContainer Bridge2Devices;

	for(uint16_t j=0;j<telef2+1;j++){
		NetDeviceContainer enlace2 = p2pInterno.Install (NodeContainer(terminales2.Get(j),Bridge2)  );
		terminales2Devices.Add (enlace2.Get(0));
		Bridge2Devices.Add (enlace2.Get(1));
	}



	//Una vez todos los enlaces creados, creo el bridge que conmutara paquetes
	Ptr<Node> Puente1 = Bridge1.Get (0);
	BridgeHelper b1;
	b1.Install (Puente1, Bridge1Devices);

	Ptr<Node> Puente2 = Bridge1.Get (0);
	BridgeHelper b2;
	b2.Install (Puente2, Bridge2Devices);

	//Añado la pila TCP/IP a los dispositivos
	InternetStackHelper stack;

	stack.Install(R);
	stack.Install(terminales1);
	stack.Install(terminales2);

	Ipv4AddressHelper ipv4;
	Ipv4InterfaceContainer telefonosSede1;
	ipv4.SetBase ("10.1.0.0","255.255.0.0");
	telefonosSede1 = ipv4.Assign (terminales1Devices);

	Ipv4AddressHelper ipv4v2;
	Ipv4InterfaceContainer telefonosSede2;
	ipv4v2.SetBase ("10.2.0.0","255.255.0.0");
	telefonosSede2 = ipv4v2.Assign (terminales2Devices);


	Ipv4AddressHelper ipv4v3;
	Ipv4InterfaceContainer routers;
	ipv4v3.SetBase ("10.3.0.0","255.255.255.0");
	routers = ipv4v3.Assign (Rdevices);


	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
	//Creo aplicacion UDP sumidero y tal.

	//Establezco un sumidero de paquetes en cada telefono
	//Utilizamos el puerto 5600 para el sumidero
	uint16_t port = 5600;
	PacketSinkHelper sink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));


	ApplicationContainer sumidero1 = sink.Install(terminales1);
	ApplicationContainer sumidero2 = sink.Install(terminales2);

	//Creo aplicacion que envia paquetes ON/OFF para simular llamada voip

	//Problema con direccionamiento

	 OnOffHelper clientes ("ns3::UdpSocketFactory",
		                        Address (InetSocketAddress (Ipv4Address::GetAny (), port)));

	ApplicationContainer tx1 = clientes.Install(terminales1);
	ApplicationContainer tx2 = clientes.Install(terminales2);

	#define MAXONVOIP 0.1
	#define MINONVOIP 0.2
	#define MAXOFFVOIP 0.8
	#define MINOFFVOIP 0.9

	varon=CreateObject<UniformRandomVariable>();
	  varon->SetAttribute("Max", DoubleValue(MAXONVOIP));
	  varon->SetAttribute("Min", DoubleValue(MINONVOIP));
	  varoff=CreateObject<UniformRandomVariable>();
	  varoff->SetAttribute("Max", DoubleValue(MAXOFFVOIP));
	  varoff->SetAttribute("Min", DoubleValue(MINOFFVOIP));
	  //Configuramos la aplicación OnOff
	  clientes.SetConstantRate (DataRate ("64kbps"));
	  clientes.SetAttribute("OnTime", PointerValue(varon));
	  clientes.SetAttribute("OffTime", PointerValue(varoff));



	//Cambio de cola de ROUTER 1 (por si hace falta)
	PointerValue tmp;
		 	  R.Get(0)->GetObject<NetDevice>()->GetAttribute("TxQueue",tmp);
		 	  Ptr<Object> txQueue = tmp.GetObject();
		 	  Ptr<DropTailQueue> dtq = txQueue->GetObject <DropTailQueue>();
		 	  NS_ASSERT (dtq!=0);
		 	  UintegerValue limit;
		 	  dtq->GetAttribute("MaxPackets",limit);
		 	  NS_LOG_INFO ("Tamaño de la cola del Router 1: "<<limit.Get()<<" paquetes" );
		 	  dtq->SetAttribute("MaxPackets",UintegerValue(100));
		 	  dtq->GetAttribute("MaxPackets",limit);
		 	  NS_LOG_INFO("Tamaño de la cola del Router 1 cambiado a: "<<limit.Get()<<" paquetes");

	//Cambio de cola de ROUTER 2 (por si hace falta)
	PointerValue tmp;
		 	  R.Get(1)->GetObject<NetDevice>()->GetAttribute("TxQueue",tmp);
		 	  Ptr<Object> txQueue = tmp.GetObject();
		 	  Ptr<DropTailQueue> dtq = txQueue->GetObject <DropTailQueue>();
		 	  NS_ASSERT (dtq!=0);
		 	  UintegerValue limit;
		 	  dtq->GetAttribute("MaxPackets",limit);
		 	  NS_LOG_INFO ("Tamaño de la cola del Router 2: "<<limit.Get()<<" paquetes" );
		 	  dtq->SetAttribute("MaxPackets",UintegerValue(100));
		 	  dtq->GetAttribute("MaxPackets",limit);
		 	  NS_LOG_INFO("Tamaño de la cola del Router 2 cambiado a: "<<limit.Get()<<" paquetes");



	//SUSCRIPCION DE TRAZAS



	tx1.Start (Seconds (2.0));
	tx2.Start (Seconds (2.0));
	tx1.Stop (Seconds (15.0));
	tx2.Stop (Seconds (15.0));

	Simulator::Run ();
	Simulator::Destroy ();





}
