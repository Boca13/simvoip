﻿#pragma once

#include <ns3/packet.h>
#include <ns3/average.h>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ethernet-header.h"

#define JITTER 100 
#define RETARDO 150
#define PORCENTAJE  1

using namespace ns3;


class Observador
{
public:
	Observador();
	virtual ~Observador();
	void PktEnviado(Ptr<const Packet>paquete);
	void PktRecibido(Ptr<const Packet>paquete);
	float GetJitter(); /* función que actualiza el jitter cada vez que se envia*/
	float GetRetardo();/* función que actualiza el retardo */
	float GetQoS(); /*Función que devuelvo el QoS alcanzado*/
	void Reset(); /*bpor si tenemos que reiniciar los valores */
	std::map<uint64_t, Time> array;
	std::map<Ipv4Address, Time> arrayTiempoAnterior;
	void ActualizarNumLlamadas(uint32_t numero);
	float GetMaxLlamadas();
	double GetMediaLlamadas();
	void empiezaTransmitir(Ptr<const Packet>paquete);
	void terminaTransmitir(Ptr<const Packet>paquete);
	float GetOcupacion(Time inicio, Time final);


private:
	Time t_enviado; /* time para introducir en la estructura   (se deja de utilizar)*/
	Time t_encolado; /* tiempo con el que se encola un paquete en el buffer usado para introducir en la estructura*/
	Time t_TiempoPaqueteAnterior; /*tiempo de la llegada del anterior paquete */
	uint64_t m_recibidos;
	uint64_t m_perdidos;
	uint64_t m_enviados;
	float m_porcentaje; /* variable auxiliar para el porcentaje de paquetes perdidos */
	float m_QoS;   /* Qos que devolverá QoSActual*/
	Average<float>Jitter;  /* jitter del buffer */
	Average<float>Retardo; /* retardo de propagación*/
	Average<float>acNumLlamadas;
	Time acumulaTiempo;
	Time tiempoInicial;
};

