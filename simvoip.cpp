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
double simular(Punto * resultado, Observador *observador,uint32_t telef1,uint32_t telef2);


using namespace ns3;

// Velocidades permitidas
std::map<uint8_t, DataRate> velocidades;

int main()
{
	double qos_objetivo = 1.0;
	uint32_t telef1 = 100;
	uint32_t telef2 = 100;
	DataRate v_predef[10] = { DataRate(1), DataRate(2), DataRate(5), DataRate(10), DataRate(20), DataRate(30), DataRate(50), DataRate(100), DataRate(200), DataRate(300) };

	// TODO cmd

	for (uint8_t n = 0; n < 10; n++) // Meter valores por defecto
		velocidades.insert(std::pair<uint8_t, DataRate>(n, v_predef[n]));
	
	Time::SetResolution(Time::US);


	Observador * observador = new Observador();
	Punto anterior = { 0,0,0.0 };
	Punto resultado;
	// Los resultados de las simulaciones se guardan en una estructura para el algoritmo de predicci�n
	while (resultado.qos < qos_objetivo)
	{
		simular(&resultado, observador, telef1, telef2);
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
//todo repasar tipo de telef1, demasiado grande?
//todo posible error: a los Routers les estoy instalando las apps que llevan los telefonos y les estoy dando direccionamiento
//solucionar eso

double simular(Punto * resultado, Observador *observador, uint32_t telef1, uint32_t telef2)
{
	// Preparar escenario

	NodeContainer Sedes;		//Creamos 2 sedes que vamos a interconectar
	Sedes.Create(2);			//Son nodos p2p

	//todo supongo que

	//Creo telefonos de sede 1
	NodeContainer telefonosSede1;
	telefonosSede1.Add( Sedes.Get(0) );
	telefonosSede1.Create(telef1);  //Creo el numero de telefonos que se le pase por parametros

	//Creo telefonos de sede 2
	NodeContainer telefonosSede2;
	telefonosSede2.Add( Sedes.Get(1) );
	telefonosSede2.Create(telef2);

	//Instalo dispositivo en Routers entre sedes
	PointToPointHelper pointToPoint;
	NetDeviceContainer p2pDevices;
	pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1Gbps")); //todo supongo datos, ya se cambiara
	pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.1ms"));
	p2pDevices = pointToPoint.Install (Sedes);

	//Instalo dispositivo en telefonos Sede 1

	PointToPointHelper pointToPointSede1;
	NetDeviceContainer p2pDevicesSede1;
	pointToPointSede1.SetDeviceAttribute ("DataRate", StringValue ("100Mbps")); //todo supongo datos, ya se cambiara
	pointToPointSede1.SetChannelAttribute ("Delay", StringValue ("2ms"));
	p2pDevicesSede1 = pointToPoint.Install (telefonosSede1);

	//Instalo dispositivo en telefonos Sede 2

	PointToPointHelper pointToPointSede2;
	NetDeviceContainer p2pDevicesSede2;
	pointToPointSede2.SetDeviceAttribute ("DataRate", StringValue ("100Mbps")); //todo supongo datos, ya se cambiara
	pointToPointSede2.SetChannelAttribute ("Delay", StringValue ("2ms"));
	p2pDevicesSede2 = pointToPoint.Install (telefonosSede2);

	//Instalo pila TCP/IP en todos los nodos
	 InternetStackHelper stack;
	 stack.Install(telefonosSede1);	//todo tengo que instalar pila tcp/ip en Routers?
	 stack.Install(telefonosSede2);

	//Direccionamiento IP
	 Ipv4AddressHelper address;
	 Ipv4InterfaceContainer p2pInterfacesSede1;
	 address.SetBase("10.1.1.0","255.255.0.0");
	 p2pInterfacesSede1 = address.Assign(p2pDevicesSede1);
	 Ipv4InterfaceContainer p2pInterfacesSede2;
	 address.SetBase("10.1.2.0","255.255.0.0");
	 p2pInterfacesSede2 = address.Assign(p2pDevicesSede2);

	 Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	 //Establezco un sumidero de paquetes en cada telefono
	 //Utilizamos el puerto 9 para el sumidero
	 uint16_t port = 9;
	 PacketSinkHelper sink ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
	 ApplicationContainer sumidero = sink.Install(telefonosSede1);
	 ApplicationContainer sumidero = sink.Install(telefonosSede2);

	 //todo Crear aplicacion OnOff (mirar voip.cc y añadirlo a cada telefono)




	// Ejecutar simulaci�n
	 // Utilizar valores de la estructura resultado
	 // Para obtener la velocidad: velocidades[resultado->velocidad]

	// Calcular calidad de servicio y devolverla
	return(1.0);
}
