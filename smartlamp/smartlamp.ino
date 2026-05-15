// Defina os pinos de LED e LDR
const int ledPin = 18;  
const int ldrPin = 34;

// Variáveis de Controle e Protocolo
int ldrMax = 4000;     
int ledValue = 0;       // MODIFICADO: Intensidade do LED começa em 0 (Desligado)
int thresholdValue = 80; // Valor padrão para ativação automática

// Controle de Tempo (Envio Periódico)
unsigned long lastMillis = 0;
const long interval = 2000; // 2 segundos

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    pinMode(ldrPin, INPUT);
    
    ledUpdate(); // Inicia com o valor padrão (agora desligado)
    // Mensagem de boot não obrigatória pelo protocolo, mas útil
    // Serial.println("SmartLamp Initialized."); 
}

void loop() {
    // 1. Processamento de Comandos Seriais
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command.length() > 0) {
            processCommand(command);
        }
    }

    // 2. Envio Periódico do LDR (Tarefa 1.4 - Passo 6)
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis >= interval) {
        lastMillis = currentMillis;
        // O protocolo pede exatamente: RES GET_LDR Z
        Serial.printf("RES GET_LDR %d\n", ldrGetValue());
    }
}

void processCommand(String command) {
    // SET_LED X
    if (command.startsWith("SET_LED ")) {
        int val = command.substring(8).toInt();
        if (val >= 0 && val <= 100) {
            ledValue = val;
            ledUpdate();
            Serial.println("RES SET_LED 1"); // Sucesso conforme protocolo
        } else {
            Serial.println("RES SET_LED -1"); // Erro conforme protocolo
        }
    } 
    // GET_LED
    else if (command == "GET_LED") {
        Serial.printf("RES GET_LED %d\n", ledValue);
    }
    // GET_LDR
    else if (command == "GET_LDR") {
        Serial.printf("RES GET_LDR %d\n", ldrGetValue());
    }
    // SET_THRESHOLD X
    else if (command.startsWith("SET_THRESHOLD ")) {
        int val = command.substring(14).toInt();
        if (val >= 0 && val <= 100) {
            thresholdValue = val;
            Serial.println("RES SET_THRESHOLD 1");
        } else {
            // Valores fora do intervalo podem ser ignorados ou retornar erro
            Serial.println("ERR Invalid value"); 
        }
    }
    // GET_THRESHOLD
    else if (command == "GET_THRESHOLD") {
        Serial.printf("RES GET_THRESHOLD %d\n", thresholdValue);
    }
    // Comando Desconhecido
    else {
        Serial.println("ERR Unknown command.");
    }
}

void ledUpdate() {
    ledValue = constrain(ledValue, 0, 100);
    int pwmValue = map(ledValue, 0, 100, 0, 255);
    analogWrite(ledPin, pwmValue);
}

int ldrGetValue() {
    int rawValue = analogRead(ldrPin);
    // Normalização 0-4095 para 0-100
    int normalized = map(constrain(rawValue, 0, ldrMax), 0, ldrMax, 0, 100);
    return normalized;
}
