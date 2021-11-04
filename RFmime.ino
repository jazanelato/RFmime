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
LiquidCrystal_I2C lcd(0x27,16,2); //Inicializa a classe lcd
RCSwitch transmitter = RCSwitch(); //Inicializa a classe Switch transmitter
RCSwitch receiver = RCSwitch(); //Inicializa a classe Switch receiver

byte tPin = 11; //Pino do transmissor
byte rPin = 0; //Pino do Receptor

int menu = 0; //Menu Principal

//Variaveis para o sinal recebido
unsigned long signalcode;
unsigned int signalpl, signalp;

void setup(){
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.print("---- RFmime ----");
    lcd.setCursor(0, 1);
    for(int i; i < 16; i++){
        lcd.print(".");
        delay(80);
    };
}
void loop(){
    int subMenu = 0;
    switch(menu){
        case 0:{ //Menu principal
        menu0(subMenu);
        while(true){
            char key = keypad.waitForKey();
            if(key == '*' && subMenu > 0){
                subMenu--;
                menu0(subMenu);
            } else if(key == '#'){
                subMenu++;
                menu0(subMenu);
            } else if(key == 'A'){
                menu = subMenu + 1;
                break;
            };
        };
        break;
        }

        case 1:{ //Menu enviar
        char key;
        String code, pl, prot;
        //Código
        menu1(subMenu);
        while(code.length() < 9){
            key = keypad.waitForKey();
            code.concat(key);
            lcd.write(key);
        };
        subMenu++;
        delay(1000);
        //Largura de pulso
        menu1(subMenu);
        while(pl.length() < 3){
            key = keypad.waitForKey();
            pl.concat(key);
            lcd.write(key);
        };
        subMenu++;
        delay(1000);
        //Protocolo
        menu1(subMenu);
        while(prot.length() < 1){
            key = keypad.waitForKey();
            prot.concat(key);
            lcd.write(key);
        };
        subMenu++;
        delay(1000);
        sendsignal(code.toInt(), pl.toInt(), prot.toInt());
        menu1(subMenu);
        menu = 0;
        keypad.waitForKey();
        break;
        }

        case 2:{ //Menu receber
        subMenu = 0;
        menu2(subMenu);
        receivesignal();
        subMenu++;
        menu2(subMenu);
        menu = 0;
        keypad.waitForKey();
        break;
        }
    }
}

void menu0(int subMenu){
    switch(subMenu){
        case 0:{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("----- Send -----");
        lcd.setCursor(0, 1);
        lcd.print("    A-enter  #->");
        break;
        }

        case 1:{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("--- Receive ----");
        lcd.setCursor(0, 1);
        lcd.print("<-* A-enter     ");
        break;
        }
    };
}
void menu1(int subMenu){
    switch(subMenu){
        case 0:{
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Send Signal");
        lcd.setCursor(0, 1);
        lcd.print("Code:");
        break;
        }

        case 1:{
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Send Signal");
        lcd.setCursor(0, 1);
        lcd.print("PL:");
        break;
        }

        case 2:{
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Send Signal");
        lcd.setCursor(0, 1);
        lcd.print("Protocol:");
        break;
        }

        case 3:{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("signal sent");
        lcd.setCursor(0, 1);
        lcd.print("successfully!");
        break;
        }
    }
}
void menu2(int subMenu){
    switch(subMenu){
        case 0:{
        lcd.clear();
        lcd.print("Receiving Signal");
        lcd.setCursor(0, 1);
        lcd.print("Listening...");
        break;
        }

        case 1:{
        lcd.clear();
        lcd.print("Code:");
        lcd.print(signalcode);
        lcd.setCursor(0, 1);
        lcd.print("PL:");
        lcd.print(signalpl);
        lcd.setCursor(10, 1);
        lcd.print("P:");
        lcd.print(signalp);
        break;
        }
    }
}

void sendsignal(unsigned long code, int pl, int p){
    transmitter.enableTransmit(tPin);
    transmitter.setProtocol(p);
    transmitter.setPulseLength(pl);
    transmitter.send(code, 28);
    transmitter.disableTransmit();
}
void receivesignal(){
    receiver.enableReceive(0);
    while(!receiver.available()){
        ;
    }
    signalcode = receiver.getReceivedValue();
    signalpl = receiver.getReceivedDelay();
    signalp = receiver.getReceivedProtocol();
    receiver.resetAvailable();
    receiver.disableReceive();
}