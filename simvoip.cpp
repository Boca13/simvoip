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

// Punto 2D
typedef std::pair<uint8_t, uint8_t> Punto;

/*
*	Recibe el número de enlaces y su velocidad y el objeto observador a utilizar
*	devuelve la QoS conseguida
*/
double simular(uint8_t enlaces, DataRate velocidad, Observador *observador);


int main()
{
	double qos_objetivo = 1.0;
	DataRate velocidades[10] = {DataRate(1), DataRate(2), DataRate(5), DataRate(10), DataRate(20), DataRate(30), DataRate(50), DataRate(100), DataRate(200), DataRate(300)};



	double qos = 0.0;
	uint8_t enlaces = 0;
	uint8_t velocidad = 0;
	Observador * observador = new Observador();
	std::map<Punto, double> historia;
	std::map<Punto, double>::iterator iterador;
	// Los resultados de las simulaciones se guardan en un map para el algoritmo de predicción
	while (qos < qos_objetivo)
	{
		qos = simular(enlaces, velocidades[velocidad], observador);
		historia[Punto(enlaces, velocidad)] = qos;

		delete observador;
	}

	return 0;
}

double simular(uint8_t enlaces, DataRate velocidad, Observador * observador)
{
	// Preparar escenario

	// Ejecutar simulación


	// Calcular calidad de servicio y devolverla
	return(1.0);
}