// Defina os pinos de LED e LDR
const int ledPin = 2;   // LED onboard (ou 4 conforme seu hardware)
const int ldrPin = 34;  // Pino analógico 34 (ADC1)

// Valor máximo do LDR (ajustado via teste da lanterna)
int ldrMax = 4000; 

// Variável para guardar o valor atual do LED (0 a 100)
int ledValue = 10;

void setup() {
    Serial.begin(9600);
    
    pinMode(ledPin, OUTPUT);
    pinMode(ldrPin, INPUT);
    
    // Configura o valor inicial do LED
    ledUpdate();

    Serial.printf("SmartLamp Initialized.\n");
}

void loop() {
    // Verifica se há dados na serial
    if (Serial.available() > 0) {
        // Lê a string até o caractere de nova linha (\n)
        String command = Serial.readStringUntil('\n');
        command.trim(); // Remove espaços em branco extras
        
        if (command.length() > 0) {
            processCommand(command);
        }
    }
}

void processCommand(String command) {
    // Comando para definir brilho: SET_LED [valor]
    if (command.startsWith("SET_LED ")) {
        String valStr = command.substring(8); // Pega o que vem depois de "SET_LED "
        ledValue = valStr.toInt();
        ledUpdate();
        Serial.println("RES SET_LED OK");
    } 
    // Comando para ler o LED: GET_LED
    else if (command == "GET_LED") {
        Serial.printf("RES LED %d\n", ledValue);
    }
    // Comando para ler o LDR: GET_LDR
    else if (command == "GET_LDR") {
        int ldrVal = ldrGetValue();
        Serial.printf("RES LDR %d\n", ldrVal);
    }
}

void ledUpdate() {
    // Garante que o valor esteja entre 0 e 100%
    ledValue = constrain(ledValue, 0, 100);
    
    // Converte a escala de 0-100 para 0-255 (PWM)
    int pwmValue = map(ledValue, 0, 100, 0, 255);
    
    // No ESP32 moderno, usa-se dacWrite ou ledcWrite, 
    // mas para compatibilidade simples:
    analogWrite(ledPin, pwmValue);
}

int ldrGetValue() {
    // Lê o valor bruto (0 a 4095 no ESP32)
    int rawValue = analogRead(ldrPin);
    
    // Normaliza: rawValue / ldrMax * 100
    // Usamos constrain para não passar de 100 se o rawValue > ldrMax
    int normalized = map(constrain(rawValue, 0, ldrMax), 0, ldrMax, 0, 100);
    
    return normalized;
}
