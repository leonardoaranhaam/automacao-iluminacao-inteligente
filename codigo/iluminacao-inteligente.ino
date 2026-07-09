// PINOS
// Luz da casa
int led1 = 2;
// Indicador de presença
int led2 = 3;

// Sensor ultrassônico HC-SR04
// Pulso de saída
int trigPin = 6;
// Eco de retorno
int echoPin = 5;

// Sensor de luminosidade
int ldrPin = A0;
// Buzzer de alerta
int buzzerPin = 7;

// VARIÁVEIS DOS SENSORES
// Leitura do LDR filtrada
int ldrValor = 0;
// Abaixo disso, está escuro
int limiarLuz = 100;

// Distância filtrada (cm)
float distancia = 0;
// Distância que conta como "tem algo passando" (cm)
const int distanciaLimite = 30;

// Se já havia objeto no ciclo passado
bool objetoAnterior = false;
// true = tem alguém no ambiente
bool ambientePresente = false;

//FILTRO DE MÉDIA MÓVEL (LDR) 
// Guarda as últimas leituras e usa a média, pra evitar ruído

// Quantidade de leituras na média
const int NUM_AMOSTRAS_LDR = 10;
// Buffer com as últimas leituras
int amostrasLDR[NUM_AMOSTRAS_LDR];
// Posição atual no buffer
int indiceLDR = 0;
// Soma das leituras do buffer
long somaLDR = 0;

// FILTRO PASSA-BAIXA (HC-SR04)
// Suaviza leituras ruins do ultrassônico

// Peso da leitura nova (0 = ignora, 1 = sem filtro)
const float fatorFiltroDistancia = 0.5;
// Começa alto = nada detectado
float distanciaFiltrada = 999;

// Tempo máximo esperando o eco
const unsigned long timeoutEcho = 30000;

// CONTROLE DE TOGGLE (DEBOUNCE)
// Evita contar a mesma passagem várias vezes
unsigned long ultimoToggle = 0;
// Tempo mínimo entre uma alternância e outra
const unsigned long cooldownToggle = 3000;

// CONTROLE DO BUZZER 
unsigned long buzzerInicio = 0;
bool buzzerAtivo = false;
// Duração do bipe
const unsigned long duracaoBuzzer = 2000;

// PONTUAÇÃO 
// Soma pontos com o tempo quando a luz está apagada
long pontuacaoTotal = 0;
unsigned long tempoAnterior = 0;

// PARA TESTE: 5 segundos
// Depois altere para 60000 (1 minuto)
const unsigned long intervalo = 5000;

// PROTÓTIPOS 
int lerLDRFiltrado();
float lerDistanciaFiltrada();
void PontosPorHora();
String Nivel(long pontos);
void enviarDados();

// SETUP 
void setup() {

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Preenche o buffer do LDR com a primeira leitura
  int leituraInicial = analogRead(ldrPin);
  for (int i = 0; i < NUM_AMOSTRAS_LDR; i++) {
    amostrasLDR[i] = leituraInicial;
    somaLDR += leituraInicial;
  }

  Serial.begin(9600);
}

void loop() {

  // LDR
  ldrValor = lerLDRFiltrado();

  // Sensor Ultrassônico 
  distancia = lerDistanciaFiltrada();

  // Detectado quando está dentro do limite
  bool objetoDetectado = (distancia < distanciaLimite);

  // Só libera alternar se já passou o cooldown
  bool podeAlternar = (millis() - ultimoToggle) > cooldownToggle;

  // Alterna presença quando o objeto acabou de aparecer
  if (objetoDetectado && !objetoAnterior && podeAlternar) {

    ambientePresente = !ambientePresente;
    ultimoToggle = millis();

    buzzerAtivo = true;
    buzzerInicio = millis();
  }

  objetoAnterior = objetoDetectado;

  //  BUZZER 
  if (buzzerAtivo) {

    tone(buzzerPin, 1000);

    if (millis() - buzzerInicio >= duracaoBuzzer) {
      noTone(buzzerPin);
      buzzerAtivo = false;
    }
  }

  //  LEDs 
  digitalWrite(led2, ambientePresente);

  bool poucaLuz = (ldrValor < limiarLuz);
  // Só acende no escuro E com alguém
  bool luzLigada = (poucaLuz && ambientePresente);

  digitalWrite(led1, luzLigada);

  //  PONTUAÇÃO 
  PontosPorHora();

  //  SERIAL 
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.print(" cm");

  Serial.print(" | LDR: ");
  Serial.print(ldrValor);

  Serial.print(" | Luz: ");
  Serial.print(luzLigada ? "Ligada" : "Apagada");

  Serial.print(" | Presente: ");
  Serial.print(ambientePresente ? "Sim" : "Nao");

  Serial.print(" | Pontos: ");
  Serial.print(pontuacaoTotal);

  Serial.print(" | Nivel: ");
  Serial.println(Nivel(pontuacaoTotal));

  delay(50);
}

//  FILTRO DO LDR 
// Devolve a média das últimas leituras (buffer circular)
int lerLDRFiltrado() {

  // Remove a leitura mais antiga da soma
  somaLDR = somaLDR - amostrasLDR[indiceLDR];
  // Faz a nova leitura
  amostrasLDR[indiceLDR] = analogRead(ldrPin);
  // Adiciona na soma
  somaLDR = somaLDR + amostrasLDR[indiceLDR];

  // Avança pra próxima posição
  indiceLDR = (indiceLDR + 1) % NUM_AMOSTRAS_LDR;

  return somaLDR / NUM_AMOSTRAS_LDR;
}

//  FILTRO DO SENSOR ULTRASSÔNICO 
// Mede a distância e aplica o filtro passa-baixa
float lerDistanciaFiltrada() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH, timeoutEcho);

  float leituraAtual = duracao * 0.0343 / 2;

  // Sem eco = nada por perto, não zero
  if (leituraAtual == 0) {
    leituraAtual = distanciaLimite + 50;
  }

  // Mistura a leitura nova com o valor antigo
  distanciaFiltrada = (fatorFiltroDistancia * leituraAtual) + ((1 - fatorFiltroDistancia) * distanciaFiltrada);

  return distanciaFiltrada;
}

//  PONTUAÇÃO 
// A cada intervalo, soma pontos se a luz estiver apagada
void PontosPorHora() {

  if (millis() - tempoAnterior >= intervalo) {

    tempoAnterior = millis();

    bool luzOff = (digitalRead(led1) == LOW);

    if (luzOff && !ambientePresente) {
      pontuacaoTotal += 2;
    }
    else if (luzOff && ambientePresente) {
      pontuacaoTotal += 5;
    }

    enviarDados();
  }
}

//  NÍVEIS 
// Converte a pontuação em um "título"
String Nivel(long pontos) {

  if (pontos <= 150)
    return "Consumista";

  else if (pontos <= 300)
    return "Iniciante";

  else if (pontos <= 450)
    return "Ajudante do Meio Ambiente";

  else if (pontos <= 600)
    return "Economizador";

  else
    return "Amigo da Mae Natureza";
}

//  SERIAL 
// Imprime a pontuação no Monitor Serial
void enviarDados() {

  Serial.println("--------------------------------");

  Serial.print("PONTOS: ");
  Serial.println(pontuacaoTotal);

  Serial.print("NIVEL: ");
  Serial.println(Nivel(pontuacaoTotal));

  Serial.println("--------------------------------");
}
