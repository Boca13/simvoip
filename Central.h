#ifndef CENTRAL_H

#define CENTRAL_H

#include "ns3/core-module.h"
#include "ns3/random-variable-stream.h"
#include "ns3/nstime.h"
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include "Observador.h"

class voip;

using namespace ns3;

class Central
{
public:
	Central(uint32_t tSede1, uint32_t tSede2, Observador * observador);
	virtual ~Central();
	/*	Método al que llamará un teléfono cuando quiera hacer una llamada.
		Decide a qué número intenta llamar el teléfono con una variable aleatoria
		uniforme y si está libre el teléfono llamante podrá proceder a llamar.
		Argumentos:
		- llamante: Número del teléfono llamante.
		Devuelve:
		- Número del teléfono al que llamar.
		- Valor -1 si el teléfono estaba ocupado.
		Cuando un teléfono llama a este método, se almacena con quién está hablando
		y se establecen los dos como ocupados. Al acabar la llamada se debe llamar a colgar().
	*/
	Ipv4Address llamar(uint32_t llamante, Time duracion, voip ** objeto_llamado);

	/*	Método que desahce una llamada entre dos teléfonos.
		Argumentos:
		- llamante: Número de teléfono que llamó.
		Debe llamarse este método tras comenzar una llamada con llamar();
	*/
	void colgar(uint32_t llamante);

	uint32_t registrar(Ipv4Address llamante, voip * objeto);

	uint32_t GetNumLlamadas()
	{
		return llamadas.size();
	}


private:
	// Estructura que guarda las llamadas como asignaciones entre números de teléfono
	std::map<uint32_t, uint32_t> llamadas;
	std::map<uint32_t, Ipv4Address> telefonos;
	// Map que guarda los punteros a los objetos voip de cada nodo
	std::map<uint32_t, voip*> objetos_voip;
	// Número de teléfonos en la sede 1
	uint32_t tel1;
	// Número de teléfonos en la sede 2
	uint32_t tel2;
	Observador * m_observador
};
#endif