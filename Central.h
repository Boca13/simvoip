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
	/*	M�todo al que llamar� un tel�fono cuando quiera hacer una llamada.
		Decide a qu� n�mero intenta llamar el tel�fono con una variable aleatoria
		uniforme y si est� libre el tel�fono llamante podr� proceder a llamar.
		Argumentos:
		- llamante: N�mero del tel�fono llamante.
		Devuelve:
		- N�mero del tel�fono al que llamar.
		- Valor -1 si el tel�fono estaba ocupado.
		Cuando un tel�fono llama a este m�todo, se almacena con qui�n est� hablando
		y se establecen los dos como ocupados. Al acabar la llamada se debe llamar a colgar().
	*/
	Address llamar(uint32_t llamante, Time duracion);

	/*	M�todo que desahce una llamada entre dos tel�fonos.
		Argumentos:
		- llamante: N�mero de tel�fono que llam�.
		Debe llamarse este m�todo tras comenzar una llamada con llamar();
	*/
	void colgar(uint32_t llamante);

	uint32_t registrar(Address llamante);


private:
	// Estructura que guarda las llamadas como asignaciones entre n�meros de tel�fono
	std::map<uint32_t, uint32_t> llamadas;
	std::map<uint32_t, Address> telefonos;
	// N�mero de tel�fonos en la sede 1
	uint32_t tel1;
	// N�mero de tel�fonos en la sede 2
	uint32_t tel2;
};