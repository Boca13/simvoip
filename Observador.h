#pragma once

#include <ns3/packet.h>
#include <ns3/average.h>


using namespace ns3;


class Observador
{
public:
	Observador();
	virtual ~Observador();
	void PktEnviado(Ptr<Const Packet>paquete);
	void PktRecibido(Ptr<Const Packet>paquete);
	void PktEncolado(Ptr<Const Packet>paquete);
	float ActualizaJitter(); /* función que actualiza el jitter cada vez que se envia*/
	float ActualizaRetardo();/* función que actualiza el retardo */
	float QoSActual(); /*Función que devuelvo el QoS alcanzado*/
	void Reset(); /*bpor si tenemos que reiniciar los valores */
	std::map<uint64_t, Time> array;
private:
	Time t_enviado; /* time para introducir en la estructura   (se deja de utilizar)*/
	Time t_encolado /* tiempo con el que se encola un paquete en el buffer usado para introducir en la estructura*/
	float m_enviados;
	float m_recibidos;
	float m_perdidos;
	float m_porcentaje /* variable auxiliar para el porcentaje de paquetes perdidos */
	float m_QoS;   /* Qos que devolverá QoSActual*/
	uint64_t m_usuarios; /* (Duda) para calcular usuarios totales hablando al mismo tiempo*/
	Average<float>Jitter;  /* jitter del buffer */
	Average<float>Retardo /* retardo de propagación*/
};

