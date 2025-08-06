int buzzPin = 2;
int buzzFreq = 1000;
int ledPin = 8;
int btnPin = 4;

struct CharStringDict{
  char key;
  String value;
};

CharStringDict MorseCodeDictionnary[] = {
  {'A', "01"},
  {'B', "1000"},
  {'C', "1010"},
  {'D', "100"},
  {'E', "0"},
  {'F', "0010"},
  {'G', "110"},
  {'H', "0000"},
  {'I', "00"},
  {'J', "0111"},
  {'K', "101"},
  {'L', "0100"},
  {'M', "11"},
  {'N', "10"},
  {'O', "111"},
  {'P', "0110"},
  {'Q', "1101"},
  {'R', "010"},
  {'S', "000"},
  {'T', "1"},
  {'U', "001"},
  {'V', "0001"},
  {'W', "011"},
  {'X', "1001"},
  {'Y', "1011"},
  {'Z', "1100"},
  {'(', "10110"},
  {'-', "100001"},
  {'&', "01000"},
  {',', "110011"},
  {'?', "001100"},
  {'\'', "011110"},
  {')', "101101"},
  {':', "111000"},
  {'!', "101011"},
  {'$', "0001001"},
  {';', "101010"},
  {'/', "10010"},
  {'=', "10001"},
  {'@', "011010"},
  {'"', "010010"},
  {'.', "010101"},
  {'+', "01010"},
  {'_', "001101"},
  {'0', "11111"},
  {'1', "01111"},
  {'2', "00111"},
  {'3', "00011"},
  {'4', "00001"},
  {'5', "00000"},
  {'6', "10000"},
  {'7', "11000"},
  {'8', "11100"},
  {'9', "11110"}
};

int dictSize = sizeof(MorseCodeDictionnary) / sizeof(MorseCodeDictionnary[0]);

char toUpper(char c){
  return (c >= 'a' && c <= 'z') ? c - 32 : c;
}

char toLower(char c){
  return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

String getMorseCode(char key){
  key = toUpper(key);
  for (int i=0; i<dictSize; i++){
    if (key == MorseCodeDictionnary[i].key){
      return MorseCodeDictionnary[i].value;
    }
  }
  return (String)key;
}

char getKey(String value){
  const int bufferSize = 10;  // adjust size based on your max expected string length
  char buffer[bufferSize];
  value.toCharArray(buffer, bufferSize);
  for (int i=0; i<dictSize; i++){
    if (strcmp(MorseCodeDictionnary[i].value.c_str(), buffer) == 0){
      return MorseCodeDictionnary[i].key;
    }
  }
  return '\0';
}



String TransformToMorse(String input){
  String output = "";
  for (int i=0; i<input.length(); i++){
    output += getMorseCode(input[i]);
  }
  return output;
}

char TranslateToText(String input){
  char output = '=';
  output = getKey(input);
  return output;
}

void send_as_binary(String input);

void setup() {
  pinMode(buzzPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);
  tone(buzzPin, buzzFreq, 1000);
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
  while (!Serial);
}

int lastBtnState = HIGH;
String morse_input = "";
unsigned long pressStart = 0;
unsigned long releaseTime = 0;

int pressedTimes = 0;

void loop() {
  int buttonState = digitalRead(btnPin);

  if (buttonState == LOW && lastBtnState == HIGH) {
    pressedTimes += 1;
    if (pressedTimes == 3){
      pressStart = millis();
      digitalWrite(ledPin, HIGH);
      tone(buzzPin, buzzFreq);
      lastBtnState = buttonState;
      pressedTimes = 0;
    }
  }
  
  if (buttonState == HIGH && lastBtnState == LOW){
    unsigned long duration = millis() - pressStart;
    if (duration <= 200) {
      morse_input += "0";  // short press (dot)
    } else if (duration <= 500) {
      morse_input += "1";  // long press (dash)
    }
    
    pressStart = 0;
    releaseTime = millis();

    digitalWrite(ledPin, LOW);
    noTone(buzzPin);

    lastBtnState = buttonState;
  }

  // Check for timeout (5 seconds no press after release)
  if (releaseTime != 0 && (millis() - releaseTime) > 1500 && morse_input.length() > 0) {
    Serial.print(TranslateToText(morse_input));
    morse_input = "";   // clear input after translating
    releaseTime = 0;
  }
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    Serial.println(input);
    Serial.print("Translating to morse code...\n");
    String morse = TransformToMorse(input);
    Serial.print("Translated successfully!\n");
    Serial.println(morse);
    Serial.print("Transmitting...\n");
    unsigned long lap = millis();
    send_as_binary(morse);
    unsigned long transDuration = millis() - lap;
    Serial.print("Morse code transmitted!\nTook " + String(transDuration) + " ms to transmit.\n");
  }
  //lastBtnState = buttonState;
}

void send_as_binary(String input){
  delay(500); // intial delay
  const int unit = 100;
  for (int i=0; i<input.length(); i++){
    if (input.charAt(i) == '1'){
      tone(buzzPin, buzzFreq, 3*unit);
      digitalWrite(ledPin, HIGH);
      delay(3*unit);
      digitalWrite(ledPin, LOW);
    }else if (input.charAt(i) == '0'){
      tone(buzzPin, buzzFreq, unit);
      digitalWrite(ledPin, HIGH);
      delay(unit);
      digitalWrite(ledPin, LOW);
    }else{
      delay(5 * unit);
    }
    delay(3 * unit);
  }
}
