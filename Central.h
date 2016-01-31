#pragma once

#include "ns3/core-module.h"
#include "ns3/random-variable-stream.h"
#include "ns3/nstime.h"
#include "ns3/address.h"

using namespace ns3;

class Central
{
public:
	Central(uint32_t tSede1, uint32_t tSede2);
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
	Address llamar(uint32_t llamante, Time duracion);

	/*	Método que desahce una llamada entre dos teléfonos.
		Argumentos:
		- llamante: Número de teléfono que llamó.
		Debe llamarse este método tras comenzar una llamada con llamar();
	*/
	void colgar(uint32_t llamante);

	uint32_t registrar(Address llamante);


private:
	// Estructura que guarda las llamadas como asignaciones entre números de teléfono
	std::map<uint32_t, uint32_t> llamadas;
	std::map<uint32_t, Address> telefonos;
	// Número de teléfonos en la sede 1
	uint32_t tel1;
	// Número de teléfonos en la sede 2
	uint32_t tel2;
};