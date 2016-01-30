#include "Central.h"


Central::Central(uint32_t tSede1, uint32_t tSede2)
{
	tel1 = tSede1;
	tel2 = tSede2;
}


Central::~Central()
{
}

int32_t Central::llamar(uint32_t llamante)
{
	ns3::UniformRandomVariable rnd;
	// Se escoge aleatoriamente un número al que llamar
	int llamado = rnd.GetValue(0, tel1+tel2);
	// Si el número no se encuentra en la lista, es que está libre
	if (llamadas.find(llamado)==llamadas.end)
	{
		llamadas.insert(std::pair<uint32_t, uint32_t>(llamante, llamado));
		return llamado;
	}
	else // Si se encontró, está ocupado y no se produce la llamada
	{
		return -1;
	}
}

void Central::colgar(uint32_t llamante)
{
	llamadas.erase(llamante);
}

