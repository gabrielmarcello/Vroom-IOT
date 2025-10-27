# 🚀 Challenge Vroom IoT

Solução desenvolvida como parte do desafio Vroom IoT, com foco na organização e segurança de pátios de motocicletas da empresa Mottu.  
A proposta envolve o uso de tags inteligentes simuladas no Wokwi, integração com Node-RED, um dashboard interativo e registro persistente de dados.

---

## Integrantes
- Guilherme Guimarães - RM 557074 / 2TDSA
- Gabriel Marcello Cândido Freire - RM 556783 / 2TDSPW
- Matheus Oliveira de Luna - RM 555547 / 2TDSA

---

## 📍 Visão Geral
O sistema simula o funcionamento de uma tag inteligente com:

- LED RGB que muda de cor a cada clique no botão.
- Buzzer que emite sons diferentes conforme a cor.
- Esses eventos são enviados para o **Node-RED**, onde são processados, exibidos em tempo real em um **dashboard interativo** e gravados em arquivo JSON para histórico.

---

## 🛠 Tecnologias Utilizadas
- **ESP32 (Wokwi Simulator)** – Simulação do hardware da tag inteligente.  
- **Node-RED** – Processamento e roteamento dos dados da tag.  
- **Node-RED Dashboard** – Visualização em tempo real dos dados da tag.  
- **MQTT (simulado no Wokwi)** – Comunicação entre o dispositivo e o servidor.  
- **Registro Persistente em Banco de dados** – Histórico salvo no banco de dados Oracle.

---

## 🔌 Projeto no Wokwi

Acesse a simulação diretamente pelo link:  
👉 [https://wokwi.com/projects/445923098695576577](https://wokwi.com/projects/445923098695576577)

---

## 📦 Node-RED

- ⚠️ **É necessário instalar os pacotes `node-red-dashboard` e `node-red-node-ui-table`  no Node-RED.**

- 🗂️ O fluxo está disponível na pasta `node-red` deste repositório.
- 📊 Acesse o dashboard em:  
  👉 [http://localhost:1880/ui](http://localhost:1880/ui)

---

## 🎥 Vídeos

- 📽️ **Pitch das Tecnologias Utilizadas:**  
  👉 [https://www.youtube.com/watch?v=uRUtF_pEzsw](https://www.youtube.com/watch?v=uRUtF_pEzsw)

- 🧠 **Apresentação da Solução (Pitch Final):**  
  👉 [https://youtu.be/UBFGZRqeJI0?si=7aTC5ctlODD1IMW3](https://youtu.be/UBFGZRqeJI0?si=7aTC5ctlODD1IMW3)

---

## Link para API

- https://github.com/gabrielmarcello/VroomAPI

---

## 📁 Estrutura do Projeto

- `codigo-fonte-wokwi/` – Código da simulação IoT no Wokwi  
- `node-red/` – Fluxo do Node-RED exportado  
- `dashboard/` – Link da interface do painel  
- `descritivo-solucao/` – PDF com a descrição da solução  
- `README.md` – Este arquivo  

---

## 🛠️ Como Executar

1. Instale o [Node-RED](https://nodered.org/)  
2. Instale os pacotes `node-red-dashboard` e `node-red-node-ui-table` 
3. Importe o fluxo disponível na pasta `node-red` no editor do Node-RED  
4. Inicie o Node-RED e acesse o painel em [http://localhost:1880/ui](http://localhost:1880/ui)  
5. Para a simulação, abra o projeto no Wokwi pelo link [https://wokwi.com/projects/445923098695576577](https://wokwi.com/projects/445923098695576577)  

---

> Projeto desenvolvido para fins acadêmicos e de demonstração tecnológica.
