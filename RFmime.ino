#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RCSwitch.h>

const byte ROWS = 4; //Numero de linhas
const byte COLS = 4; //Numero de colunas
//Mapeamento dos caracteres
char hexaKeys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
byte colPins[COLS] = {6,5,4,3}; //Pinos referentes as colunas
byte rowPins[ROWS] = {10,9,8,7}; //Pinos referentes as linhas
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //Inicializa a classe do teclado
char key;

LiquidCrystal_I2C lcd(0x27,16,2); //Inicializa a classe lcd
int Menu = 0;
byte lcdXCord;

RCSwitch transmitter = RCSwitch(); //Inicializa a classe Switch transmitter
RCSwitch receiver = RCSwitch(); //Inicializa a classe Switch receiver
byte tPin = 11; //Pino do transmissor
byte rPin = 0; //Pino do Receptor

class Signal {
    private:
    public:
    Signal();
    void Send();
    bool Receive();
    unsigned long _Code;
    unsigned int _Pl;
    unsigned int _P;
};

//Funções da classe Signal
Signal::Signal() {
}

void Signal::Send() {
    transmitter.enableTransmit(tPin);
    transmitter.setProtocol(_P);
    transmitter.setPulseLength(_Pl);
    transmitter.send(_Code, 28);
    transmitter.disableTransmit();
}

bool Signal::Receive() {
        receiver.enableReceive(0);
        while(!receiver.available()){
            key = keypad.getKey();
            if(key) {
                Menu = 0;
                receiver.disableReceive();
                return 0;
            }
        }
        _Code = receiver.getReceivedValue();
        _Pl = receiver.getReceivedDelay();
        _P = receiver.getReceivedProtocol();
        receiver.resetAvailable();
        receiver.disableReceive();
        return 1;
}

Signal *Sinal;
String code = "";
String pl = "";
String p = "";



void setup() {
    Sinal = new Signal();
    lcd.init();
    lcd.backlight();
    lcd.clear();
    delay(1000);
    lcd.print("---- RFmime ----");
    lcd.setCursor(0, 1);
    for(int i; i < 16; i++) {
        lcd.print(".");
        delay(80);
    };
    attMenu(Menu);
}

void loop() {
    delay(500);
    key = keypad.getKey();
    if(key) {
        if(key == 'A') {
            Menu = 1;
            sendMenu();
        }else if(key == 'B') {
            Menu = -1;
            receiveMenu();
        };
        attMenu(Menu);
    };
}

void sendMenu() {
    //Menu Codigo
    bool ready = 1;
    lcdXCord = 7;
    attMenu(Menu);
    while(ready == 1) {
        key = keypad.waitForKey();
        if((key == 'A') && (code.length() != 0)) {
            ready = 0;
            Sinal->_Code = code.toInt();
        }else if((key == 'B') && (code.length() != 0)) {
            lcdXCord--;
            code.remove(code.length()-1);
            lcd.setCursor(lcdXCord, 1);
            lcd.print(" ");
            lcd.setCursor(lcdXCord, 1);
        }else if(key != 'B') {
            lcdXCord++;
            code.concat(key);
            lcd.write(key);
        };
    };
    //Menu Largura de pulso
    Menu = 2;
    ready = 1;
    lcdXCord = 3;
    attMenu(Menu);
    while(ready == 1) {
        key = keypad.waitForKey();
        if((key == 'A') && (pl.length() != 0)) {
            ready = 0;
            Sinal->_Pl = pl.toInt();
        }else if((key == 'B') && (pl.length() != 0)) {
            lcdXCord--;
            pl.remove(pl.length()-1);
            lcd.setCursor(lcdXCord, 1);
            lcd.print(" ");
            lcd.setCursor(lcdXCord, 1);
        }else if(key != 'B') {
            lcdXCord++;
            pl.concat(key);
            lcd.write(key);
        };
    };
    //Menu protocolo
    Menu = 3;
    ready = 1;
    lcdXCord = 10;
    attMenu(Menu);
    while(ready == 1) {
        key = keypad.waitForKey();
        if((key == 'A') && (p.length() != 0)) {
            ready = 0;
            Sinal->_P = p.toInt();
        }else if((key == 'B') && (p.length() != 0)) {
            lcdXCord--;
            p.remove(p.length()-1);
            lcd.setCursor(lcdXCord, 1);
            lcd.print(" ");
            lcd.setCursor(lcdXCord, 1);
        }else if(key != 'B') {
            lcdXCord++;
            p.concat(key);
            lcd.write(key);
        };
    };
    enviar:
    Sinal->Send();

    Menu = 5;
    attMenu(Menu);
    delay(2000);

    Menu = 4;
    attMenu(Menu);
    key = keypad.waitForKey();
    if(key == 'A') {
        goto enviar;
    } else if(key == 'B') {
        Menu = 0;
        return;
    };
}

void receiveMenu() {
    bool ctrl;
    lcdXCord = 7;
    attMenu(Menu);
    if(Sinal->Receive()) {
        Menu = -2;
        attMenu(Menu);
        lcd.setCursor(7, 0);
        lcd.print(String(Sinal->_Code));
        lcd.setCursor(3, 1);
        lcd.print(String(Sinal->_Pl));
        lcd.setCursor(12, 1);
        lcd.print(String(Sinal->_P));
        keypad.waitForKey();

        Menu = -3;
        attMenu(Menu);
        key = keypad.waitForKey();
        if(key == 'A') {
            enviar2:
            Sinal->Send();
            Menu = 5;
            attMenu(Menu);
            delay(2000);
        } else if(key == 'B') {
            Menu = 0;
            return;
        };

        Menu = 4;
        attMenu(Menu);
        key = keypad.waitForKey();
        if(key == 'A') {
            goto enviar2;
        } else if(key == 'B') {
            Menu = 0;
            return;
        };
    }else return;
}

void attMenu(int menu) {
    switch(menu)
    {
        case -3:
        {
            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.print("Enviar Sinal?");
            lcd.setCursor(2, 1);
            lcd.print("A - S  B - N");
            break;
        }

        case -2:
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Codigo:");
            lcd.setCursor(0, 1);
            lcd.print("LP:");
            lcd.setCursor(10, 1);
            lcd.print("P:");
            break;
        }

        case -1:
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Receber Sinal");
            lcd.setCursor(0, 1);
            lcd.print("Escutando...");
            break;
        }

        case 0:
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("A - Enviar");
            lcd.setCursor(0, 1);
            lcd.print("B - Receber");
            break;
        }
        
        case 1:
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Enviar Sinal");
            lcd.setCursor(0, 1);
            lcd.print("Codigo:");
            break;
        }

        case 2:
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Enviar Sinal");
            lcd.setCursor(0, 1);
            lcd.print("LP:");
            break;
        }

        case 3:
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Enviar Sinal");
            lcd.setCursor(0, 1);
            lcd.print("Protocolo:");
            break;
        }

        case 4:
        {
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("Reenviar?");
            lcd.setCursor(2, 1);
            lcd.print("A - S  B - N");
            break;
        }

        case 5:
        {
            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.print("Sinal enviado");
            lcd.setCursor(1, 1);
            lcd.print("com sucesso!");
            break;
        }

        case 6:
        {
            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.print("Erro no envio!");
            break;
        }

        default:
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Erro!");
            lcd.setCursor(0, 1);
            lcd.print("Menu não existe");
            break;
        }
    }
}