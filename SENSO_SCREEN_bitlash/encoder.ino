// алгоритм с "таблицей", позволяющий увеличить точность энкодера
// в 4 раза, работает максимально чётко даже с плохими энкодерами.
// https://alexgyver.ru/encoder/

#define CLK 12
#define DT 11
//long pos = 0;
byte lastState = 0;
const int8_t increment[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

//void setup() {
//  Serial.begin(9600);
//}

void encoder_loop() {
  byte state = digitalRead(CLK) | (digitalRead(DT) << 1);
  if (state != lastState) {
    pos += increment[state | (lastState << 2)];
    //Нажата ли кнопка?
    if (digitalRead(13) == 0) {
      pos += 3 * increment[state | (lastState << 2)];
      }
    lastState = state;
    //Serial.print(pos);
    //Serial.print(' ');
    //Serial.println(digitalRead(13));
    assignVar('W'-'A', pos); 
//  } else {
//    //Нажата ли кнопка?
//    if (digitalRead(13) == 0 && pos==0) {
//          func_resenc();
//      }
  }
}

numvar func_resenc(){
  assignVar('W'-'A', 0); 
  assignVar('V'-'A', pos); 
  pos = 0;
}
