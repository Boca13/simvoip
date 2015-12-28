#pragma once
class Observador
{
public:
	Observador();
	virtual ~Observador();
	void PktEnviado(Ptr<Const Packet>paquete);
	void PktRecibido(Ptr<Const Pakect>paquete);
	float ActualizaJitter(); /* función que actualiza el jitter cada vez que se envia*/
	float QoSActual(); /*Función que devuelvo el QoS alcanzado*/
	void Reset(); /*bpor si tenemos que reiniciar los valores */
	std::map<uint64_t, Time> array;
private:
	Time t_enviado; /* time para introducir en la estructura*/
	float m_enviados;
	float m_recibidos;
	float m_perdidos;
	float m_QoS;   /* Qos que devolverá QoSActual*/
	uint64_t m_usuarios; /* (Duda) para calcular usuarios totales hablando al mismo tiempo*/
	Average<float>Jitter;  /* Retardo de referencia */
};

