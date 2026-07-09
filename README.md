# Sistema de Iluminação Automática com Arduino
## Descrição

Projeto desenvolvido utilizando Arduino para automatizar o acionamento da iluminação de um ambiente.

O sistema utiliza:

- Sensor ultrassônico HC-SR04
- Sensor LDR
- LED representando a iluminação
- LED indicador de presença
- Buzzer de notificação

Além disso, implementa filtros digitais para melhorar a qualidade das leituras dos sensores e um sistema de pontuação baseado na economia de energia.

### Funcionalidades
- Detecção de presença
- Acionamento automático da iluminação
- Detecção de luminosidade ambiente
- Alerta sonoro ao detectar entrada/saída
- Sistema de pontuação por economia de energia
- Monitor Serial com informações em tempo real
- Filtro de média móvel para o LDR
- Filtro passa-baixa para o sensor ultrassônico
### Componentes utilizados
- Arduino Uno
- HC-SR04
- LDR
- Buzzer
- LEDs
- Resistores
- Protoboard
###Algoritmos utilizados
#### Sensor LDR

Filtro de média móvel para reduzir ruídos.

#### Sensor Ultrassônico

Filtro passa-baixa exponencial para suavizar oscilações.

#### Controle de Presença

Sistema de debounce utilizando millis() para evitar múltiplas detecções.

### Tecnologias
- Arduino IDE
- Linguagem C++
- Tinkercad

### Autores

- Nathan Mello Botelho
- Leonardo Aranha de Almeida Moreira
- Anna Gabriela Amorim Santos
- Filipe Santana Linhares Frota da Mota
- Gustavo Buitrago de Figueiredo

Estudantes de Ciência da Computação – PUC Minas
