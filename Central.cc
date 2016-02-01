#include "Central.h"


Central::Central(uint32_t tSede1, uint32_t tSede2)
{
	tel1 = tSede1;
	tel2 = tSede2;
}


Central::~Central()
{
}

Address Central::llamar(uint32_t llamante, Time duracion)
{
	ns3::UniformRandomVariable rnd;
	// Se escoge aleatoriamente un número al que llamar
	uint32_t llamado = rnd.GetValue(0, tel1+tel2);
	// Si el número no se encuentra en la lista, es que está libre
	if (llamadas.find(llamado)==llamadas.end())
	{
		llamadas.insert(std::pair<uint32_t, uint32_t>(llamante, llamado));
		Simulator::Schedule(duracion, this, colgar, llamante);
		return telefonos[llamado];
	}
	else // Si se encontró, está ocupado y no se produce la llamada
	{
		return Address(); // Devolver dirección inválida
	}
}

void Central::colgar(uint32_t llamante)
{
	llamadas.erase(llamante);
}

uint32_t Central::registrar(Address llamante)
{
	uint32_t indice = (uint32_t) telefonos.count;
	telefonos.insert(std::pair<uint32_t, Address>(telefonos.count, llamante));
	return indice;
}

