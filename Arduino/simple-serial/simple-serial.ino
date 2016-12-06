/*
 *  Download Noodl at:
 *  http://www.getnoodl.com/
 *   
 *  Use together with Node websocket-serial-server example
 *  
 */


String command;


//
// SETUP
//

void setup() {

  pinMode(13, OUTPUT);
  
  Serial.begin(9600);
  while(!Serial){
    ;
  }
  Serial.write("Arduino running;");
}


//
// LOOP
//

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if(c == ';'){
      parseCommand(command);
      command = "";
    } else {
      command += c;
    }
  }
}

void parseCommand(String com){
  String cmd;
  String arg;

  cmd = com.substring(0, com.indexOf(":"));
  arg = com.substring(com.indexOf(":")+1);

  if(cmd == "led"){
    if(arg == "off"){
      digitalWrite(13, LOW);
    }
    if(arg == "on"){
      digitalWrite(13, HIGH);
    }
  }
  
}

