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
#include "ns3/bridge-module.h"

#define DURACION 120.0
#define MAXONVOIP 0.1
#define MINONVOIP 0.2
#define MAXOFFVOIP 0.8
#define MINOFFVOIP 0.9

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
void simular(Punto * resultado, std::map<uint8_t, DataRate> velocidades, Observador *observador,uint16_t telef1,uint16_t telef2);
void cambiaEnlace(Time inicio, Time fin, Time salto, Ptr<Ipv4> R1, Ptr<Ipv4> R2, uint8_t interfaz);

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
	Punto resultado = { 3,0,0.0 };
	uint32_t iteraciones = 0;
	// Los resultados de las simulaciones se guardan en una estructura para el algoritmo de predicci�n
	NS_LOG_INFO("Comenzando simulación");

	// FASE I: Acercamiento rápido.
	// Se busca el óptimo dando grandes pasos aproximando linealmente. Cuando se supere la QoS requerida se pasará a la segunda fase.
	NS_LOG_DEBUG("FASE I");
	while (resultado.qos < qos_objetivo)
	{
		// Cuenta del tiempo que tarda la iteración
		Time elapsed = Seconds(time(0));
		NS_LOG_DEBUG("Iteración número " << ++iteraciones);
		NS_LOG_DEBUG("	Usando " << resultado.enlaces << " enlaces a " << velocidades[resultado.velocidad]);

		simular(&resultado, velocidades, observador, telef1, telef2);
		NS_LOG_DEBUG("	QoS obtenida: " << resultado.qos);

		// ALGORITMO DE PREDICCIÓN LINEALIZANDO
		// y = mx + n
		// m = dy/dx (delta)
		// x = enlaces*velocidad
		NS_LOG_DEBUG("	PREDICCIÓN");
		// Calcular pendiente de la recta
		double m = ((resultado.qos - anterior.qos) / ((resultado.enlaces*velocidades[resultado.velocidad].GetBitRate() / 1e6) - (anterior.enlaces*velocidades[anterior.velocidad].GetBitRate() / 1e6)));
		NS_LOG_DEBUG("	La m obtenida es " << m);
		// qos = mx -> x = qos/m
		uint32_t x = qos_objetivo / m;
		NS_LOG_DEBUG("	Suponemos que la x óptima estará en torno a " << x);
		// Pasar el resultado al anterior
		anterior = resultado;
		// Cambiar coordenadas de resultado para la siguiente iteración
		// Enlaces mínimos
		resultado.enlaces = 0;
		resultado.velocidad = 0;
		// Repartir la x obtenida en velocidad y número de enlaces de forma que haya el mínimo número de enlaces
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
		elapsed = Seconds(time(0)) - elapsed;
		NS_LOG_DEBUG("La iteración " << iteraciones << " ha tardado " << elapsed.GetSeconds() << " segundos.");
	}
	// FASE II: Descenso fino
	// Ahora se bajan los parámetros de entrada poco a poco hasta que la QoS obtenida baja de la requerida.
	// Cuando esto ocurra el algoritmo termina, dando como solución los parámetros de la simulación anterior.
	NS_LOG_DEBUG("FASE II");
	while (resultado.qos > qos_objetivo)
	{
		// Cuenta del tiempo que tarda la iteración
		Time elapsed = Seconds(time(0));
		if (resultado.velocidad == 0)
		{
			resultado.velocidad = (velocidades.size() - 1);
			resultado.enlaces--;
		}
		else
		{
			resultado.velocidad--;
		}

		NS_LOG_DEBUG("Iteración número " << ++iteraciones);
		NS_LOG_DEBUG("	Usando " << resultado.enlaces << " enlaces a " << velocidades[resultado.velocidad]);
		simular(&resultado, velocidades, observador, telef1, telef2);
		NS_LOG_DEBUG("	QoS obtenida: " << resultado.qos);

		// Pasar el resultado al anterior
		anterior = resultado;

		elapsed = Seconds(time(0)) - elapsed;
		NS_LOG_DEBUG("La iteración " << iteraciones << " ha tardado " << elapsed.GetSeconds() << " segundos.");
	}
	// El resultado final es el de la estructura anterior
	NS_LOG_INFO("¡Simulación finalizada!");
	NS_LOG_INFO("La solución óptima es utilizar " << (unsigned int) anterior.enlaces << " enlaces con una tasa de " << anterior.velocidad << ".");
	NS_LOG_INFO("Se obtiene una calidad de servicio de " << anterior.qos);
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


void simular(Punto * resultado, std::map<uint8_t, DataRate> velocidades, Observador *observador, uint16_t telef1, uint16_t telef2)
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
		//En los netdevices 0-2-4...van los enlaces del Router 1
		//En los netdevices 1-3-5... van los enlaces del Router 2


	std::cout << "En Rdevices tengo: "<<Rdevices.GetN() / 2 << "enlaces" << std::endl;


	//----Esto seria para las centrales. Ahora los telefonos/bridges
	NodeContainer terminales1; //Sede1
	NodeContainer terminales2; //Sede 2
	terminales1.Create(telef1);
	terminales2.Create(telef2);

	//Añado el R al nodecontainer de los telefonos para realizar los enlaces despues
	terminales1.Add(R.Get(0));
	terminales2.Add(R.Get(1));


	PointToPointHelper p2pInterno;
	p2pInterno.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2pInterno.SetChannelAttribute ("Delay", StringValue ("2ms"));

	NetDeviceContainer R1Devices;

	for(uint16_t j=0;j<telef1;j++){
		NetDeviceContainer enlace1 = p2pInterno.Install (NodeContainer(terminales1.Get (j) ,R.Get(0))  );
		R1Devices.Add (enlace1.Get(0));
		R1Devices.Add (enlace1.Get(1));
	} //Estoy conectando cada telefono al R1

	std::cout << "En R1devices tengo: "<<R1Devices.GetN() / 2<< "enlaces" << std::endl;
	
	NetDeviceContainer R2Devices;
	
	for(uint16_t j=0;j<telef2;j++){
		NetDeviceContainer enlace1 = p2pInterno.Install (NodeContainer(terminales2.Get(j),R.Get(1))  );
		R2Devices.Add (enlace1.Get(0));
		R2Devices.Add (enlace1.Get(1));
	}

	std::cout << "En R2devices tengo: "<<R2Devices.GetN()/ 2 << "enlaces" << std::endl;

	//Añado la pila TCP/IP a los dispositivos
	InternetStackHelper stack;


	//no instalo pila de internet a routers, ya están incluidos en terminales1/2
	stack.Install(terminales1);
	stack.Install(terminales2);

	Ipv4AddressHelper ipv4;
	Ipv4InterfaceContainer telefonosSede1;
	ipv4.SetBase ("10.1.0.0","255.255.0.0");
	telefonosSede1 = ipv4.Assign (R1Devices);

	
	Ipv4InterfaceContainer telefonosSede2;
	ipv4.SetBase ("10.2.0.0","255.255.0.0");
	telefonosSede2 = ipv4.Assign (R2Devices);


	
	Ipv4InterfaceContainer routers;
	ipv4.SetBase ("10.3.0.0","255.255.255.0");
	routers = ipv4.Assign (Rdevices);


	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
	//Creo aplicacion UDP sumidero y tal.

	//Establezco un sumidero de paquetes en cada telefono
	//Utilizamos el puerto 5600 para el sumidero
	uint16_t port = 5600;
	PacketSinkHelper sink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));


	ApplicationContainer sumidero1 = sink.Install(terminales1);
	ApplicationContainer sumidero2 = sink.Install(terminales2);

	//Aplicacion que envia paquetes.

	



	//Cambio de cola de ROUTER 1 (por si hace falta)
	PointerValue tmp;
	Rdevices.Get(0)->GetObject<NetDevice>()->GetAttribute("TxQueue",tmp);
	Ptr<Object> txQueue = tmp.GetObject();
	Ptr<DropTailQueue> dtq = txQueue->GetObject <DropTailQueue>();
	NS_ASSERT (dtq!=0);
	UintegerValue limit;
	dtq->GetAttribute("MaxPackets",limit);
	NS_LOG_INFO ("Tamaño de la cola del Router 1: "<<limit.Get()<<" paquetes" );
	dtq->SetAttribute("MaxPackets",UintegerValue(5));
	dtq->GetAttribute("MaxPackets",limit);
	NS_LOG_INFO("Tamaño de la cola del Router 1 cambiado a: "<<limit.Get()<<" paquetes");

	Rdevices.Get(1)->GetObject<NetDevice>()->GetAttribute("TxQueue",tmp);
	txQueue = tmp.GetObject();
	dtq = txQueue->GetObject <DropTailQueue>();
	NS_ASSERT (dtq!=0);
	dtq->GetAttribute("MaxPackets",limit);
	NS_LOG_INFO ("Tamaño de la cola del Router 2: "<<limit.Get()<<" paquetes" );
	dtq->SetAttribute("MaxPackets",UintegerValue(5));
	dtq->GetAttribute("MaxPackets",limit);
	NS_LOG_INFO("Tamaño de la cola del Router 2 cambiado a: "<<limit.Get()<<" paquetes");


	//Configuracion del cambio de enlace
	Ptr <Node> n1 = R.Get(0);
		Ptr<Ipv4> punteroIp1 = n1->GetObject<Ipv4> ();
	Ptr <Node> n2 = R.Get(1);
		Ptr<Ipv4> punteroIp2 = n2->GetObject<Ipv4> ();

	 cambiaEnlace (Time("2s") ,  Time ("20s") , Time ("1s") , punteroIp1, punteroIp2, resultado->enlaces);


	//SUSCRIPCION DE TRAZAS



	tx1.Start (Seconds (2.0));
	tx2.Start (Seconds (2.0));
	tx1.Stop (Seconds (15.0));
	tx2.Stop (Seconds (15.0));

	Simulator::Run ();
	Simulator::Destroy ();

}

	


void cambiaEnlace(Time inicio, Time fin, Time salto, Ptr<Ipv4> R1, Ptr<Ipv4> R2, uint8_t interfaz){

	uint8_t metrica = interfaz;
	Time actual = inicio;

	while (actual.GetSeconds() < fin.GetSeconds()){ //Bucle donde programo el cambio de enlaces en la simulacion. Utilizare time inicio, fin..

		for (uint8_t j = 0; j < interfaz; j++){ //Bucle de los enlaces 
				//schedule (tiempoEnElQuePasara, Dir.Objeto, PtrIpv4, Ifaz, Metrica)

			//Para R1 configuro los schedules con los cambios de metrica de enlaces
			Simulator::Schedule (actual-Seconds(0.001),&Ipv4::SetMetric, R1, j, metrica); //enlaces de R1

			Simulator::Schedule (actual-Seconds(0.001),&Ipv4::SetMetric, R2, j, metrica); //enlaces de R2

			
			metrica++;
				if(metrica >= 8)
					metrica=0;

			
		}
			//Cuando cambio todas las metricas, recalculo las tablas de enrutamiento
			Simulator::Schedule (actual, &Ipv4GlobalRoutingHelper::RecomputeRoutingTables);

			//Meto el salto de tiempo
			actual = actual + salto;


	} 


}




