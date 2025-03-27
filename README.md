# 🐎 Hipismo Cronômetro - Projeto IPE

## 📌 Descrição do Projeto

Este projeto implementa um **cronômetro para competições equestres** utilizando **ESP32** com comunicação **LoRa**. O sistema permite:

* **Registro preciso de tempos** em eventos de hipismo
* **Transmissão dos dados via LoRa** para um servidor
* **Interface web responsiva** para monitoramento em tempo real

## 📂 Estrutura do Repositório

```
├── LoRa_Receiver_Web_Server/  # Código principal do receptor com servidor web
├── HTML/                      # Arquivos da interface web
├── Receptor com HTML/         # Versão do receptor com interface HTML
├── Receptor sem HTML/         # Versão do receptor sem interface HTML
├── Sender Certo/              # Código do transmissor (Sender)
└── README.md                  # Documentação do projeto
```

## 🎯 Funcionalidades

- ✅ Cronometragem precisa para eventos equestres 🏇
- ✅ Comunicação sem fio via **LoRa** 📡
- ✅ Servidor **web embutido** para exibição de dados 🌍
- ✅ Interface **HTML interativa** com cronômetro e imagens 🖥️

## 🔧 Requisitos de Hardware

* ESP32
* Módulo **LoRa** (SX1278 ou similar)
* Sensores de cronometragem (Ex: sensores infravermelhos)

## 🚀 Tecnologias Utilizadas

* **Linguagem**: C++
* **Comunicação**: LoRa, WiFi
* **Interface Web**: HTML, CSS, JavaScript

## 📌 Configuração do Projeto

1. Clone o repositório:

```bash
git clone https://github.com/raquelbelchior1/Hipismo-Cronometro.git
```

2. Configure o ambiente ESP32:
   * Instale o Arduino IDE ou PlatformIO
   * Adicione o **ESP32 Board Manager** e bibliotecas **LoRa**

3. Carregue o firmware no dispositivo:
   Abra o código do **Receptor** e **Transmissor** no Arduino IDE e faça upload para o ESP32.

4. Acesse a interface web:
   Conecte-se ao **IP do ESP32** no navegador para visualizar os dados.

## 👥 Contribuição

1. **Fork** o repositório
2. Crie uma **branch** para suas melhorias (`git checkout -b minha-feature`)
3. Faça commits (`git commit -m "Adicionando nova funcionalidade"`)
4. Envie um **Pull Request** 🚀

# :accessibility:: Conexões Sensor-Módulo

- `VIN`: 3,3V
- `GND`: GND 
- `SCL`: 22
- `SDA`: 21
