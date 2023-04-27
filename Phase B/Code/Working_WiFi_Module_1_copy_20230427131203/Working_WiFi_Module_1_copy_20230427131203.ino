#define LED_PIN1 9
#define BUFF_SIZE 20
char gIncomingChar;
char gLEDState=0;
char gCommsBuff[BUFF_SIZE];
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN1,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    gIncomingChar = Serial.read();
    if(gIncomingChar=='$'){
      gIncomingChar = Serial.read();
      int buffInd = 0;
    while(gIncomingChar!='\n'){
      if(gIncomingChar!=-1){
       gCommsBuff[buffInd] = gIncomingChar;
       buffInd++;
      }
      gIncomingChar = Serial.read();
    }
  }
  if(strcmp(gCommsBuff, "STR")==0){
    gLEDState=1;
    Serial.print("start\n");
  }else if(strcmp(gCommsBuff, "STP")==0){
    gLEDState=0;
    Serial.print("stop\n");
  }
  digitalWrite(LED_PIN1, gLEDState);
}
}

void printBuffer(char buffer[]){
  for(int i =0; i < sizeof(buffer)+1; i++){
    Serial.print(buffer[i]);
  }
}
