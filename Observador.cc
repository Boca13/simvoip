#include "Observador.h"

NS_LOG_COMPONENT_DEFINE("Observador");
Observador::Observador()
{
	NS_LOG_FUNCTION_NOARGS();
	m_enviados = 0;
	m_recibidos = 0;
	m_perdidos = 0;
	m_QoS = 0;
}


Observador::~Observador()
{
}



void
Observador::PktEnviado(Ptr<const Packet>paquete)
{
	/*suponemos que el paquete está en la estructura*/
	NS_LOG_FUNCTION(paquete);

	Ptr<Packet> copia = paquete->Copy();
	t_encolado = Simulator::Now();
	PppHeader header;
	copia->RemoveHeader(header);
	uint16_t tipo = header.GetProtocol();
	NS_LOG_DEBUG("[Observador] Enviado un paquete de tipo " << tipo);

	if (tipo == 0x21)
	{
		Ipv4Header cabecera;
		copia->RemoveHeader(cabecera);
		array[paquete->GetUid()] = t_encolado;

	}

	m_enviados++;

}



void
Observador::PktRecibido(Ptr<const Packet>paquete)
{
	NS_LOG_FUNCTION(paquete);
	

	Ptr<Packet> copia = paquete->Copy();
	PppHeader header;
	copia->RemoveHeader(header);
	uint16_t tipo = header.GetProtocol();
	NS_LOG_DEBUG("[Observador] Llega un paquete de tipo " << tipo);

	if (tipo == 0x21)
	{
		Ipv4Header cabecera;
		copia->RemoveHeader(cabecera);
		Ipv4Address direccion = cabecera.GetDestination();


		std::map<uint64_t, Time>::iterator aux = array.find(paquete->GetUid());
		if (array.end() == aux)
		{
			NS_LOG_DEBUG("[Observador] NO ESTA EN LA ESTRUCTURA");
		}
		else
		{
			NS_LOG_DEBUG("[Observador] SI ESTA EN LA ESTRUCTURA");



			Time tiempoEnvio = array[paquete->GetUid()];
			Time Ahora = Simulator::Now();
			std::map<Ipv4Address, Time>::iterator aux2 = arrayTiempoAnterior.find(direccion);
			Time tiempoAnterior = arrayTiempoAnterior[direccion];
			Retardo.Update((Ahora - tiempoEnvio).GetMilliSeconds());

			if (!tiempoAnterior.IsZero())
				Jitter.Update(((Ahora - tiempoEnvio) - tiempoAnterior).GetMilliSeconds());

			m_recibidos = m_recibidos + 1;
			arrayTiempoAnterior[direccion] = (Ahora - tiempoEnvio);
			array.erase(aux);
			NS_LOG_DEBUG("[Observador] Paquete recibido en el instante " << Ahora);
		}
	}
}

float
Observador::GetQoS()
{
	NS_LOG_FUNCTION_NOARGS();

	/*
	 modelo E
	 QoS= (Ro − Is) − Id − Ie-eff + A
	jitter >= 100ms
	 retardo >= 150ms
	 % >=1

	 */
	m_perdidos = array.size();
	/* debe ser menor a  un 0.01 o un 1%*/
	m_porcentaje = ((m_perdidos) / m_enviados) * 100; //referente a paquetes perdidos


	m_QoS = 1 - (m_porcentaje / PORCENTAJE + Jitter.Mean() / JITTER + Retardo.Mean() / RETARDO) / 3;
	return m_QoS;
}




float
Observador::GetJitter()
{
	NS_LOG_FUNCTION_NOARGS();

	/* el jitter debe se menor de 50 ms */
	return Jitter.Mean();
}

float
Observador::GetRetardo()
{
	NS_LOG_FUNCTION_NOARGS();
	/* el retardo debe ser menor de 100ms */
	return Retardo.Mean();
}

void
Observador::Reset()
{
	NS_LOG_FUNCTION_NOARGS();

	Jitter.Reset();
	Retardo.Reset();
}

void Observador::ActualizarNumLlamadas(uint32_t numero)
{
	NS_LOG_FUNCTION(numero);

	acNumLlamadas.Update(numero);
}

float Observador::GetMaxLlamadas() {
	NS_LOG_FUNCTION_NOARGS();

	return acNumLlamadas.Max();

}

double Observador::GetMediaLlamadas() {

	NS_LOG_FUNCTION_NOARGS();

	return acNumLlamadas.Mean();


}
void Observador::empiezaTransmitir(Ptr<const Packet>paquete) {
	NS_LOG_FUNCTION(paquete);

	tiempoInicial = Simulator::Now();


}




void Observador::terminaTransmitir(Ptr<const Packet>paquete) {

	NS_LOG_FUNCTION(paquete);

	//Almaceno aqui el tiempo total de ocupacion del enlace
	acumulaTiempo = acumulaTiempo + (Simulator::Now() - tiempoInicial);


}

//Recibe lo valores del start y del stop application.

float Observador::GetOcupacion(Time inicio, Time final) {




	return (final - inicio).GetSeconds() / acumulaTiempo.GetSeconds();


}








