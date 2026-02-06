# GreenTech 328 - Automacao de Estufa com AVR

> Sistema de controle automatizado para estufas agricolas utilizando microcontrolador ATmega328P, focado em eficiencia e seguranca atraves de manipulacao direta de registradores.

![Tech](https://img.shields.io/badge/Language-C-blue)
![Hardware](https://img.shields.io/badge/Hardware-ATmega328P-orange)

## Sobre o Projeto

O **GreenTech 328** é um sistema embarcado desenvolvido para gerenciar o microclima de uma estufa de forma autônoma. Diferente de soluções baseadas em bibliotecas de alto nível (como Wiring/Arduino), este projeto foi escrito em **C puro (AVR-GCC)**, manipulando diretamente os registradores do hardware para garantir máxima performance e controle.

O sistema monitora temperatura e umidade, controla a irrigação e ventilação, simula um ciclo circadiano (Dia/Noite) e possui um sistema robusto de parada de emergência via interrupção de hardware.

## Funcionalidades

* **Controle de Temperatura:** Aciona ventilação (Fan) automaticamente quando a temperatura ultrapassa 30.0 graus Celsius.
* **Irrigação Inteligente:** Aciona a bomba de água quando a umidade do solo cai abaixo de 40%.
* **Trava de Segurança da Bomba:** Timer interno desliga a bomba após 5 segundos para evitar alagamentos em caso de falha do sensor.
* **Ciclo Circadiano:** Simulação automática de Dia/Noite controlando a iluminação artificial com temporização precisa.
* **Sistema de Emergência (INT0):** Botão físico de pânico que, via Interrupção de Hardware, corta instantaneamente a energia de todas as cargas e sinaliza erro via LED.

## Tecnologias e Conceitos Aplicados

Este projeto demonstra domínio sobre a arquitetura AVR:

* **Manipulação de Registradores:** Uso direto de DDR, PORT e PIN para controle de GPIOs (Bitwise Operations).
* **Interrupções Externas (INT0):** Resposta em tempo real para eventos críticos (Botão de Emergência).
* **Timers (Timer1 em modo CTC):** Contagem de tempo precisa para o relógio do sistema sem bloquear o processador (sem uso excessivo de delay).
* **Protocolo One-Wire:** Implementação manual ("bit-banging") da comunicação com o sensor DHT22.

## Pinout (Hardware)

| Componente | Pino AVR | Pino Arduino | Descrição |
| :--- | :--- | :--- | :--- |
| Sensor DHT22 | PB0 | D8 | Leitura de Temp/Umidade |
| LED Emergência | PB2 | D10 | Sinalização de Erro |
| Botão Pânico | PD2 | D2 | Interrupção INT0 (Pull-up) |
| Luz (Grow LED) | PD5 | D5 | Iluminação da Estufa |
| Ventilador (Fan) | PD6 | D6 | Resfriamento |
| Bomba d'água | PD7 | D7 | Irrigação |


### Pré-requisitos
* Compilador avr-gcc
* Simulador (Proteus/SimulIDE) ou Hardware físico (Arduino Uno + Componentes)


## Este projeto foi desenvolvido para fins educacionais, demonstrando programação bare-metal em microcontroladores.