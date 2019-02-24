const String ver = "2.0m";

//HARDWARE PARAMS
const int analogInPin = A0; // Entrada do Analógico
const int analogOutPin = 13; // Saida D13 padrão
float maxOutV = 5.0; // A voltagem de saída PWM padrão do Arduino [V]
float maxRefInV = 1.1; // Referencia à voltagem analógica [V]
float R = 470.0; // Resistencia da corrente [Ohm]

//CONFIGURABLE PARAMS
bool plotter = false; // Defina: true, caso esteja usando o Serial plotter
bool putty = false; // Defina: true, caso esteja usando o PuTTT (pode ser alterado no CLI)
int maxmin = 30; // Tempo (em minutos) necessário para desligar [Min]
float target_mA = 2.73; // Essa é a corrente que passará pelo seu cérebro!!! [mA]
float epsilon_mA = 0.03; // Diferença máxima entre a corrente real e o target_mA (Não altere caso não saiba o que está fazendo!)

//INIT GLOBALS
int state = 1; /*     -1 - Cérebro não identificado
                       0 - Voltagem sendo alterada para definir a corrente padrão
                       1 - Tudo certo. Você esta na corrente definida
                     -10 - Voltagem desligada */
float outV = maxOutV; // Voltagem
int debounced_state = 0;
int zeros_len = 0;
float smoothed_mA=0;

String commandString = ""; //for CLI

//FEEDBACK HELPERS
float computeOutVoltage(float V, float new_mA){
  if(abs(new_mA-target_mA)<epsilon_mA){
    state = 1;
    return V;
    }
  if(new_mA<0.01){
     state = -1; 
     return maxOutV;
    }
  float new_V = (target_mA/new_mA)*V;
  if(new_V>maxOutV){
    state = -1; // resistência muito alta -> cérebro não encontrado?
    return maxOutV;
    //return maxOutV/5.0; // para segurança
    }
  state = 0;
  return 0.1*new_V+0.9*V;
  //return new_V;
  }

int convertVtoOutputValue(float V){
   return constrain(int(V/maxOutV*255), 0, 255);
  }

float sensorValue2mA(int sensorValue){
  float sensorVoltage = sensorValue/1023.0*maxRefInV;
  float sensor_mA = sensorVoltage/R*1000.0;
  return sensor_mA;
  }

int debounced_state_compute(int state){
  if(state < 0) {
    zeros_len = 0;
    return state;
  }
  if(state == 1){
    zeros_len = 0;
    return 1; 
  }
  if(debounced_state == 0){
    return 0;    
  }
  if(state == 0){
    zeros_len++;
    if(zeros_len>5) return 0;
  }
  return 1;
}

unsigned long start, endc;
void process_feedback(){
  int sensorValue = analogRead(analogInPin);
  float new_mA = sensorValue2mA(sensorValue);
  smoothed_mA = 0.2*new_mA+0.8*smoothed_mA;
  float V = outV;
  outV = computeOutVoltage(V, new_mA);
  analogWrite(analogOutPin, convertVtoOutputValue(outV));
  debounced_state = debounced_state_compute(state);
  
  // Exibir informações no CLI
  endc = (millis()-start)/1000;
  String tv = "[", ttm = "mA/", tsm = "V, ", ts = "mA] | Estado: ", h = " | Tempo: ", s = ":", leadM = "", leadS = "",
         plotT = "Target: ", plotmA = "\tSmoothed MA: ", plotMin = "\tMin: ", tempo;
  unsigned long tmin = endc/60-((endc/60)%1);
  // Formatação
  if (endc%60<10)
      leadS = "0";
  if (tmin<10)
      leadM = "0";
  if (endc%60==0)
      tempo = leadM + tmin + s + "00 ";
  else
      tempo = leadM + tmin + s + leadS + endc%60 + " ";
  if (debounced_state>=0)
      ts = ts + "+";
  // Parar automaticamente
  if (tmin>maxmin)
       stop_device();
  String txt;
  if (plotter) 
       txt = plotT + target_mA + plotMin + "0" + plotmA + smoothed_mA;
  else
       txt = tv + V + tsm + smoothed_mA + ttm + target_mA + ts + debounced_state + h + tempo;
  if (putty)
      Serial.print("\r\e[?25l" + txt);
  else
      Serial.println(txt);

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(5);  
  }

void stop_device(){
  state = -10;
  analogWrite(analogOutPin, 0);  
  clearAndHome();
  Serial.println("Sessão tDCS interrompida");
  Serial.println("------------------------");
  help();
}

//CLI HELPERS
void clearAndHome() 
{ 
  Serial.write(27); 
  Serial.print("[2J"); // limpa a tela
  Serial.write(27); // ESC 
  Serial.print("[H"); // \r
  if (!putty)
      for (int i = 0; i<=30; i++)
          Serial.println("");
}

void help(){
  Serial.println("tDSC arduino, ver "+ver);
  Serial.println("'?' - ajuda");
  Serial.println("'max_time <minutos>' - atualiza o tempo máximo (em minutos)");
  Serial.println("'target_mA <mA>' - atualiza o target (mA)");
  Serial.println("'epsilon_mA <mA>' - atualiza o epsilon_mA (mA)");
  Serial.println("'R <new Ohm>' - atualiza a resistência do hardware (Ohm)");
  Serial.println("'putty <true/false>' - muda a formatação de saída para o PuTTY");
  Serial.println("'stop' - para a estimulação");
  Serial.println("'restart' - inicia/reinicia a estimulação & o timer");
  Serial.println("'continue' - continua a estimulação");
  Serial.print("\n\rEstado:\n\r * max_time: ");
  Serial.print(maxmin);
  Serial.print(" minutos\n\r * target_mA: ");
  Serial.print(target_mA);
  Serial.print(" mA\n\r * epsilon_mA: ");
  Serial.print(epsilon_mA);
  Serial.print(" mA\n\r * R: ");
  Serial.print(R);
  Serial.println(" Ohms");
  }

bool parse_param(String &cmdString){
  int spacePos = cmdString.indexOf(' ');
  if(spacePos<=0) return false;  
  String command = cmdString.substring(0, spacePos);
  String fval = cmdString.substring(spacePos+1);
  if(command=="putty")
    if (fval=="true"){
        putty = true;
        return true;
    }else if (fval=="false"){
        putty = false;
        return true;
    }
  float val = fval.toFloat();
  if(command=="target_mA"){    
    if(val<=0.0 or val>100.0){
      return false;
      }
    target_mA = val;
    clearAndHome();
    help();
  }else if(command=="epsilon_mA"){
    if(val<=0.0 or val>0.3){
      return false;
    }
    epsilon_mA = val;
    clearAndHome();
    help();
  }else if(command=="R"){
    R = val;
    clearAndHome();
    help();
  }else if(command=="max_time"){
    maxmin = val;
    clearAndHome();
    help();
  }else{
    return false;  
  }
  return true;
}

//SETUP AND MAIN LOOP
void setup() {
  Serial.begin(115200);
  analogReference(INTERNAL); //1.1 V
  Serial.print("Sessão iniciada!");
  start = millis();
}
void loop(){
  if(state!=-10){
    process_feedback();
  }
  if (Serial.available() > 0){      
      char v = Serial.read();
      if (byte(v) == 13){  // Carriage return  
        bool accepted = true;
         if (commandString == "?" || commandString == "stop"){
                stop_device();
         }else if (commandString == "restart"){
                clearAndHome();
                state = -1;
                outV = maxOutV/5.0;
                start = millis();
                accepted = false;
         }else if (commandString == "continue"){
                clearAndHome();
                state = -1;
                outV = maxOutV/5.0;
                accepted = false;
         }else{         
                bool ok = parse_param(commandString);
                if(!ok){
                  clearAndHome();
                  help();
                  accepted = false;
                  Serial.println("Comando desconhecido: '" + commandString + "'");
                }
         }   
         commandString = "";
         if (accepted){
             clearAndHome();
             help();
             Serial.println("Ok!");
         }
      }else{
         commandString+=v;
         if(state==-10){
            Serial.print(v);
         }
      }      
   }
}
