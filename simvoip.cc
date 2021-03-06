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
#include "Central.h"
#include "voip.h"
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
void simular(Punto * resultado, std::map<uint8_t, DataRate> velocidades, Observador *observador, uint16_t telef1, uint16_t telef2, DataRate tasas[2], uint32_t tamPkt);
void cambiaEnlace(Time salto, Ptr<Ipv4> R1, Ptr<Ipv4> R2, uint8_t interfaz);

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
	uint32_t tamPkt = 500;

	// Obtener parámetros de cmd
	CommandLine cmd;
	cmd.AddValue("qos", "Calidad de servicio objetivo", qos_objetivo);
	cmd.AddValue("tA", "Número de usuarios en la sede A", telef1);
	cmd.AddValue("tB", "Número de usuarios en la sede B", telef2);
	cmd.AddValue("velocidades", "velocidades de enlace que se pueden contratar con el operador separadas por ;", sVelocidades);
	cmd.AddValue("tasaLlamadas", "Tasa media de ocurrencia de llamadas de los usuarios", tasaLlamadas);
	cmd.AddValue("tasaCodec", "Intervalo de la tasa de bits del códec de audio", sTasaCodec);
	cmd.AddValue("tamPaquete", "Tamaño de los paquetes", tamPkt);
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
		Observador * observador = new Observador();



		// Cuenta del tiempo que tarda la iteración
		Time elapsed = Seconds(Time(0));
		NS_LOG_DEBUG("Iteración número " << ++iteraciones);
		NS_LOG_DEBUG("	Usando " << (uint32_t)resultado.enlaces << " enlaces a " << velocidades[resultado.velocidad]);

		simular(&resultado, velocidades, observador, telef1, telef2, tasaCodec, tamPkt);
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
		while ((x / velocidades[resultado.velocidad].GetBitRate() / 1e6) > resultado.enlaces)
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



		elapsed = Seconds(Time(0)) - elapsed;
		NS_LOG_DEBUG("La iteración " << iteraciones << " ha tardado " << elapsed.GetSeconds() << " segundos.");
	}
	// FASE II: Descenso fino
	// Ahora se bajan los parámetros de entrada poco a poco hasta que la QoS obtenida baja de la requerida.
	// Cuando esto ocurra el algoritmo termina, dando como solución los parámetros de la simulación anterior.
	NS_LOG_DEBUG("FASE II");
	while (resultado.qos > qos_objetivo)
	{
		Observador * observador = new Observador();


		// Cuenta del tiempo que tarda la iteración
		Time elapsed = Seconds(Time(0));
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
		NS_LOG_DEBUG("	Usando " << (uint32_t)resultado.enlaces << " enlaces a " << velocidades[resultado.velocidad]);
		simular(&resultado, velocidades, observador, telef1, telef2, tasaCodec, tamPkt);
		NS_LOG_DEBUG("	QoS obtenida: " << resultado.qos);

		// Pasar el resultado al anterior
		anterior = resultado;

		elapsed = Seconds(Time(0)) - elapsed;
		NS_LOG_DEBUG("La iteración " << iteraciones << " ha tardado " << elapsed.GetSeconds() << " segundos.");


		delete observador;

	}
	// El resultado final es el de la estructura anterior
	NS_LOG_INFO("¡Simulación finalizada!");
	NS_LOG_INFO("La solución óptima es utilizar " << (unsigned int)anterior.enlaces << " enlaces con una tasa de " << anterior.velocidad << ".");
	NS_LOG_INFO("Se obtiene una calidad de servicio de " << anterior.qos);
	return 0;
}


// TODO: Crear objetos voip y pasarles los parámetros adecuados
void simular(Punto * resultado, std::map<uint8_t, DataRate> velocidades, Observador *observador, uint16_t telef1, uint16_t telef2, DataRate tasas[2], uint32_t tamPkt)
{

	Ptr<UniformRandomVariable> varon;
	Ptr<UniformRandomVariable> varoff;


	//primero creamos los R de salida de cada central
	NodeContainer R;
	R.Create(2);
	//Estos nodos irán unidos por enlaces p2p


	//Configuro la topologia p2p
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", DataRateValue(velocidades[resultado->velocidad]));
	p2p.SetChannelAttribute("Delay", StringValue("2ms"));

	//Creo los enlaces p2p entre centrales

	NetDeviceContainer Rdevices;

	for (uint8_t j = 0; j < resultado->enlaces; j++) {

		NetDeviceContainer enlaceRouters = p2p.Install(NodeContainer(R.Get(0), R.Get(1)));

		Rdevices.Add(enlaceRouters.Get(0));
		Rdevices.Add(enlaceRouters.Get(1));
	}
	//En los netdevices 0-2-4...van los enlaces del Router 1
	//En los netdevices 1-3-5... van los enlaces del Router 2


	NS_LOG_DEBUG("En Rdevices tengo: " << Rdevices.GetN() / 2 << "enlaces");


	//----Esto seria para las centrales. Ahora los telefonos/bridges
	NodeContainer terminales1; //Sede1
	NodeContainer terminales2; //Sede 2
	terminales1.Create(telef1);
	terminales2.Create(telef2);

	//Añado el R al nodecontainer de los telefonos para realizar los enlaces despues
	terminales1.Add(R.Get(0));
	terminales2.Add(R.Get(1));


	PointToPointHelper p2pInterno;
	p2pInterno.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
	p2pInterno.SetChannelAttribute("Delay", StringValue("2ms"));

	NetDeviceContainer R1DevicesTelefonos;
	NetDeviceContainer R1DevicesRouter;

	for (uint16_t j = 0; j < telef1; j++) {
		NetDeviceContainer enlace1 = p2pInterno.Install(NodeContainer(terminales1.Get(j), R.Get(0)));
		R1DevicesTelefonos.Add(enlace1.Get(0));
		R1DevicesRouter.Add(enlace1.Get(1));
	} //Estoy conectando cada telefono al R1

	//NS_LOG_DEBUG("En R1devices tengo: " << R1Devices.GetN() / 2 << "enlaces");

	NetDeviceContainer R2DevicesTelefonos;
	NetDeviceContainer R2DevicesRouter;
	for (uint16_t j = 0; j < telef2; j++) {
		NetDeviceContainer enlace1 = p2pInterno.Install(NodeContainer(terminales2.Get(j), R.Get(1)));
		R2DevicesTelefonos.Add(enlace1.Get(0));
		R2DevicesRouter.Add(enlace1.Get(1));
	}

	//NS_LOG_DEBUG("En R2devices tengo: " << R2Devices.GetN() / 2 << "enlaces");

	//Añado la pila TCP/IP a los dispositivos
	InternetStackHelper stack;


	//no instalo pila de internet a routers, ya están incluidos en terminales1/2
	stack.Install(terminales1);
	stack.Install(terminales2);

	NetDeviceContainer R1Devices;
	R1Devices.Add(R1DevicesRouter);
	R1Devices.Add(R1DevicesTelefonos);

	NetDeviceContainer R2Devices;
	R2Devices.Add(R2DevicesRouter);
	R2Devices.Add(R2DevicesTelefonos);

	Ipv4AddressHelper ipv4;
	Ipv4InterfaceContainer telefonosSede1;
	ipv4.SetBase("10.1.0.0", "255.255.0.0");
	telefonosSede1 = ipv4.Assign(R1Devices);

	Ipv4InterfaceContainer telefonosSede2;
	ipv4.SetBase("10.2.0.0", "255.255.0.0");
	telefonosSede2 = ipv4.Assign(R2Devices);

	Ipv4InterfaceContainer routers;
	ipv4.SetBase("10.3.0.0", "255.255.255.0");
	routers = ipv4.Assign(Rdevices);

	//Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	// Tabla de encaminamiento de los teléfonos de la sede 1
	for (uint32_t c = 0; c < terminales1.GetN(); c++)
	{
		// Añadir rutas estáticas
		Ipv4StaticRoutingHelper ipv4RoutingHelper;
		Ptr<Ipv4StaticRouting> tablaT1 = ipv4RoutingHelper.GetStaticRouting(terminales1.Get(c)->GetObject<Ipv4>());
		Ipv4Address direccion("10.1.0.0");
		tablaT1->SetDefaultRoute(Ipv4Address(direccion.Get() + c + 1), 1); // Ruta hacia la otra sede pasando por el router
		NS_LOG_DEBUG("Se ha creado una entrada en la tabla de encaminamiento del teléfono con dirección " << terminales1.Get(c)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal() << " hacia la direación " << Ipv4Address(direccion.Get() + c + 1));
	}

	// Tabla de encaminamiento de los teléfonos de la sede 2
	for (uint32_t c = 0; c < terminales2.GetN(); c++)
	{
		// Añadir rutas estáticas
		Ipv4StaticRoutingHelper ipv4RoutingHelper;
		Ptr<Ipv4StaticRouting> tablaT2 = ipv4RoutingHelper.GetStaticRouting(terminales2.Get(c)->GetObject<Ipv4>());
		Ipv4Address direccion("10.2.0.0");
		tablaT2->SetDefaultRoute(Ipv4Address(direccion.Get() + c + 1), 1); // Ruta hacia la otra sede pasando por el router
		NS_LOG_DEBUG("Se ha creado una entrada en la tabla de encaminamiento del teléfono con dirección " << terminales2.Get(c)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal() << " hacia la dirección " << Ipv4Address(direccion.Get() + c + 1));
	}

	// Tabla de encaminamiento del router de la sede 1
	// Añadir rutas estáticas
	{
		Ipv4StaticRoutingHelper ipv4RoutingHelper;
		Ptr<Ipv4StaticRouting> tablaT1 = ipv4RoutingHelper.GetStaticRouting(R.Get(0)->GetObject<Ipv4>());
		for (uint8_t c = 0; c < resultado->enlaces; c++) // Iteramos en el número de enlaces
		{ // TODO
			Ipv4Address direccion("10.3.0.0");
			tablaT1->SetDefaultRoute(Ipv4Address(direccion.Get() + c + 1), c + 1); // Ruta hacia la otra sede pasando por el router
			NS_LOG_DEBUG("Se ha creado una entrada en la tabla de encaminamiento del router 1 con dirección " << R.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal() << " hacia la dirección " << Ipv4Address(direccion.Get() + c + 1) << " por la interfaz " << (uint32_t)(c + 1));
		}
	}
	// Tabla de encaminamiento del router de la sede 1
	// Añadir rutas estáticas
	{
		Ipv4StaticRoutingHelper ipv4RoutingHelper;
		Ptr<Ipv4StaticRouting> tablaT2 = ipv4RoutingHelper.GetStaticRouting(R.Get(0)->GetObject<Ipv4>());
		for (uint8_t c = 0; c < resultado->enlaces; c++) // Iteramos en el número de enlaces
		{ // TODO
			Ipv4Address direccion("10.3.0.0"); // HAY QUE CAMBIAR EL TIPO DE ENTRADA DE DEFAULT RUTE A HOST RUTE CON SU RUTA MADRE
			tablaT2->SetDefaultRoute(Ipv4Address(direccion.Get() + c + 1 + resultado->enlaces), c + 1); // Ruta hacia la otra sede pasando por el router
			NS_LOG_DEBUG("Se ha creado una entrada en la tabla de encaminamiento del router 2 con dirección " << R.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal() << " hacia la dirección " << Ipv4Address(direccion.Get() + c + 1 + resultado->enlaces) << " por la interfaz " << (uint32_t)(c + 1));
		}
	}

	// BASURA TABLAS
	// Trace routing tables 
	Ipv4GlobalRoutingHelper g;
	Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>("dynamic-global-routing.routes", std::ios::out);
	g.PrintRoutingTableAllAt(Seconds(1), routingStream);



	//Creo aplicacion UDP sumidero y tal.

	//Establezco un sumidero de paquetes en cada telefono
	//Utilizamos el puerto 5600 para el sumidero

	Central centralita(telef1, telef2, observador);
	voip ** aplicacionTelefonos1 = (voip **)calloc(telef1, sizeof(voip*));
	voip ** aplicacionTelefonos2 = (voip **)calloc(telef2, sizeof(voip*));

	for (uint32_t i = 0; i < telef1; i++)
	{
		NS_LOG_DEBUG("Direccion IPv4: " << telefonosSede1.GetAddress(i));
		//R1DevicesTelefonos.Get(i)->GetObject<NetDevice>()->GetAddress()
		aplicacionTelefonos1[i] = new voip(&centralita, tamPkt, Time("2s"), Time("1s"),
			tasas, telefonosSede1.GetAddress(i), terminales1.Get(i));
		terminales1.Get(i)->AddApplication(aplicacionTelefonos1[i]);
		aplicacionTelefonos1[i]->SetStartTime(Seconds(1.0));
		aplicacionTelefonos1[i]->SetStopTime(Seconds(10.0));
	}

	for (uint32_t i = 0; i < telef2; i++)
	{
		//R2DevicesTelefonos.Get(i)->GetObject<NetDevice>()->GetAddress()
		aplicacionTelefonos2[i] = new voip(&centralita, tamPkt, Time("2s"), Time("1s"),
			tasas, telefonosSede2.GetAddress(i), terminales2.Get(i));
		terminales2.Get(i)->AddApplication(aplicacionTelefonos2[i]);

		aplicacionTelefonos2[i]->SetStartTime(Seconds(1.0));
		aplicacionTelefonos2[i]->SetStopTime(Seconds(10.0));
	}





	//Cambio de cola de ROUTER 1 (por si hace falta)
	PointerValue tmp;
	Rdevices.Get(0)->GetObject<NetDevice>()->GetAttribute("TxQueue", tmp);
	Ptr<Object> txQueue = tmp.GetObject();
	Ptr<DropTailQueue> dtq = txQueue->GetObject <DropTailQueue>();
	NS_ASSERT(dtq != 0);
	UintegerValue limit;
	dtq->GetAttribute("MaxPackets", limit);
	NS_LOG_DEBUG("Tamaño de la cola del Router 1: " << limit.Get() << " paquetes");
	dtq->SetAttribute("MaxPackets", UintegerValue(5));
	dtq->GetAttribute("MaxPackets", limit);
	NS_LOG_DEBUG("Tamaño de la cola del Router 1 cambiado a: " << limit.Get() << " paquetes");

	Rdevices.Get(1)->GetObject<NetDevice>()->GetAttribute("TxQueue", tmp);
	txQueue = tmp.GetObject();
	dtq = txQueue->GetObject <DropTailQueue>();
	NS_ASSERT(dtq != 0);
	dtq->GetAttribute("MaxPackets", limit);
	NS_LOG_DEBUG("Tamaño de la cola del Router 2: " << limit.Get() << " paquetes");
	dtq->SetAttribute("MaxPackets", UintegerValue(5));
	dtq->GetAttribute("MaxPackets", limit);
	NS_LOG_DEBUG("Tamaño de la cola del Router 2 cambiado a: " << limit.Get() << " paquetes");


	//Configuracion del cambio de enlace
	Ptr <Node> n1 = R.Get(0);
	Ptr<Ipv4> punteroIp1 = n1->GetObject<Ipv4>();
	Ptr <Node> n2 = R.Get(1);
	Ptr<Ipv4> punteroIp2 = n2->GetObject<Ipv4>();

	cambiaEnlace(MilliSeconds(100), punteroIp1, punteroIp2, resultado->enlaces);


	//SUSCRIPCION DE TRAZAS

	p2p.EnablePcap("barranca-1", Rdevices, true);


	//Modelamos errores en el canal
	Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
	em->SetAttribute("ErrorUnit", EnumValue(RateErrorModel::ERROR_UNIT_PACKET));
	em->SetRate(0.00015);


	for (uint16_t i = 0; i < telef1; i++)
	{

		R1DevicesTelefonos.Get(i)->TraceConnectWithoutContext("MacRx", MakeCallback(&Observador::PktRecibido, observador));
		R1DevicesTelefonos.Get(i)->TraceConnectWithoutContext("MacTx", MakeCallback(&Observador::PktEnviado, observador));
		NS_LOG_DEBUG("El nodo de R1 al que le instalo el observador es: " << R1DevicesTelefonos.Get(i)->GetNode()->GetId());
		NS_LOG_DEBUG("El nodo de R1 tiene: " << R1DevicesTelefonos.Get(i)->GetNode()->GetNDevices() << " devices");
		NS_LOG_DEBUG("El nodo de R1 tiene: " << R1DevicesTelefonos.Get(i)->GetNode()->GetNApplications() << " aplicaciones");
		NS_LOG_DEBUG("La direccion del netdevice es: " << R1DevicesTelefonos.Get(i)->GetAddress());
		R1DevicesTelefonos.Get(i)->SetAttribute("ReceiveErrorModel", PointerValue(em));

	}

	for (uint16_t i = 0; i < telef2; i++)
	{
		R2DevicesTelefonos.Get(i)->TraceConnectWithoutContext("MacRx", MakeCallback(&Observador::PktRecibido, observador));
		R2DevicesTelefonos.Get(i)->TraceConnectWithoutContext("MacTx", MakeCallback(&Observador::PktEnviado, observador));
		NS_LOG_DEBUG("El nodo de R2 al que le instalo el observador es: " << R2DevicesTelefonos.Get(i)->GetNode()->GetId());
		NS_LOG_DEBUG("El nodo de R2 tiene: " << R2DevicesTelefonos.Get(i)->GetNode()->GetNDevices() << " devices");
		NS_LOG_DEBUG("El nodo de R2 tiene: " << R2DevicesTelefonos.Get(i)->GetNode()->GetNApplications() << " aplicaciones");
		NS_LOG_DEBUG("La direccion del netdevice es: " << R2DevicesTelefonos.Get(i)->GetAddress());
		R2DevicesTelefonos.Get(i)->SetAttribute("ReceiveErrorModel", PointerValue(em));

	}


	//Instalo una traza en un netdevice de los enlaces entre los Routers para sacar la ocupacion media del enlace

	Rdevices.Get(0)->TraceConnectWithoutContext("PhyTxBegin", MakeCallback(&Observador::empiezaTransmitir, observador));
	Rdevices.Get(0)->TraceConnectWithoutContext("PhyTxEnd", MakeCallback(&Observador::terminaTransmitir, observador));

	Simulator::Stop(Seconds(10));
	Simulator::Run();


	//Obtenemos los datos de la simulacion


	//QoS

	float QoS = observador->GetQoS();
	NS_LOG_INFO("Calidad de Servicio: " << QoS);

	//Jitter

	float Jitter = observador->GetJitter();

	NS_LOG_INFO("Jitter: " << Jitter);

	//Retardo

	float retardo = observador->GetRetardo();

	NS_LOG_INFO("Retardo medio de los pkt's: " << retardo);


	for (uint32_t c = 0; c < telef1; c++)
		delete(aplicacionTelefonos1[c]);

	for (uint32_t c = 0; c < telef2; c++)
		delete(aplicacionTelefonos2[c]);

	free(aplicacionTelefonos1);
	free(aplicacionTelefonos2);

	Simulator::Destroy();
}




void cambiaEnlace(Time salto, Ptr<Ipv4> R1, Ptr<Ipv4> R2, uint8_t interfaz)
{
	//NS_LOG_FUNCTION(salto, R1, R2, interfaz);
	uint8_t metrica = interfaz;

	for (uint8_t j = 1; j <= interfaz; j++) { //Bucle de los enlaces 
			//schedule (tiempoEnElQuePasara, Dir.Objeto, PtrIpv4, Ifaz, Metrica)

		//Para R1 configuro los schedules con los cambios de metrica de enlaces
		R1->SetMetric(j, metrica); //enlaces de R1

		R2->SetMetric(j, metrica); //enlaces de R2
		//NS_LOG_DEBUG("Cambiando a enlace " << (uint32_t)j);

		metrica++;
		if (metrica >= interfaz)
		{
			metrica = 0;
			//NS_LOG_DEBUG("Cambiando a enlace " << (uint32_t)j);
		}
	}

	//Cuando cambio todas las metricas, recalculo las tablas de enrutamiento
	Ipv4GlobalRoutingHelper::RecomputeRoutingTables();

	//Meto el salto de tiempo

	//Arreglar esta linea
	Simulator::Schedule(salto, cambiaEnlace, salto, R1, R2, interfaz);



}




