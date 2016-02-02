#include "Central.h"

NS_LOG_COMPONENT_DEFINE("Central");

Central::Central(uint32_t tSede1, uint32_t tSede2, Observador * observador)
{
	tel1 = tSede1;
	tel2 = tSede2;
	m_observador = observador;
}


Central::~Central()
{
}

Ipv4Address Central::llamar(uint32_t llamante, Time duracion, voip ** objeto_llamado)
{
	Ptr<ns3::UniformRandomVariable> rnd;
	rnd = CreateObject <UniformRandomVariable>();

	// Se escoge aleatoriamente un número al que llamar
	uint32_t llamado = rnd->GetValue(0, tel1+tel2);
	
	NS_LOG_DEBUG("[Central] El nodo " << llamante << " quiere llamar a alguien durante " << duracion);
	NS_LOG_DEBUG("[Central] Le digo que llame al nodo " << llamado);
	// Si el número no se encuentra en la lista, es que está libre
	if (llamadas.find(llamado)==llamadas.end())
	{
		llamadas.insert(std::pair<uint32_t, uint32_t>(llamante, llamado));
		m_observador->ActualizarNumLlamadas(GetNumLlamadas());
		Simulator::Schedule(duracion, &Central::colgar, this, llamante);
		*objeto_llamado = objetos_voip[llamado];
		NS_LOG_DEBUG("[Central] El nodo al que se quiere llamar esta LIBRE. El puntero es " << *objeto_llamado);
		return telefonos[llamado];
	}
	else // Si se encontró, está ocupado y no se produce la llamada
	{
		NS_LOG_DEBUG("[Central] El nodo al que se quiere llamar esta OCUPADO.");
		return Ipv4Address((uint32_t)0); // Devolver dirección inválida
	}
}

void Central::colgar(uint32_t llamante)
{
	llamadas.erase(llamante);
}

uint32_t Central::registrar(Ipv4Address llamante, voip * objeto)
{
	uint32_t indice = (uint32_t)telefonos.size();
	telefonos.insert(std::pair<uint32_t, Ipv4Address>(telefonos.size(), llamante));
	objetos_voip.insert(std::pair<uint32_t, voip*>(objetos_voip.size(), objeto));
	NS_LOG_DEBUG("[Central] Registrado telefono con IP " << llamante);
	return indice;
}

