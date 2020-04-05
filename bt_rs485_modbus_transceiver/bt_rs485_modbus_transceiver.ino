#include <SoftwareSerial.h>

SoftwareSerial btSerial(2, 3); // for bluetooth (hc-05)
//SoftwareSerial rs485(11, 12); //
//char data;
//byte buf[10];

void setup() {
  pinMode(2, INPUT); // rx
  pinMode(3, OUTPUT); // tx
//  pinMode(11, INPUT); // rx
//  pinMode(12, OUTPUT); // tx
  
  pinMode(13, OUTPUT); // переключатель приём\передача
  digitalWrite(13, LOW); // включаем приём
  
  // экспериментально понял, что скорости должны быть одинаковы
  // иначе начинаются искажения: послал одно - пришло другое
  Serial.begin(9600);
  btSerial.begin(9600);
  
  //rs485.begin(9600);
  //Serial.println("begin");
}

void loop() {
  // со смарта в комп:
  if (btSerial.available()) {
    
    byte buf[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int bufLength = sizeof(buf);
    btSerial.readBytes(buf, bufLength);
	//Serial.println(bufLength);
	
	int funcCode = buf[1]; // код функции
	int reqLength = 8; // длина запроса для большинства функций
	
	// команды с кодами 15(0x0F) и 16(0x10) могут быть длиннее 8 байт
	if ((funcCode == 15) || (funcCode == 16)) {
		//Serial.println("more bites");
		
		// Читаем 7й байт (buf[6]), в нём количество байт идущих далее
		// и добавляем ещё 2 байта CRC
		int numNextBytes = buf[6];
		reqLength = 7 + numNextBytes + 2;
		
		// если вдруг запрос получился длиннее буфера (на всякий случай)
		// то просто шлём весь буфер
		if (reqLength > bufLength) {
			reqLength = bufLength;
		}
		//Serial.println(reqLength);
	}

    //int reqLength = buf[0];
    //Serial.println(reqLength);
	
    //byte request[reqLength];
    //digitalWrite(13, HIGH); // включаем передачу
    //for (int i = 0; i < reqLength; i++) {
      //request[i] = buf[i + 1];

      //Serial.print(buf[i + 1], HEX);
      //Serial.print(" ");      
      //rs485.print(buf[i + 1], HEX);
      
      //Serial.write(buf[i + 1]);
    //}
    //Serial.println();
    //digitalWrite(13, LOW); // включаем приём

    // первый вариант:
    digitalWrite(13, HIGH); // включаем передачу
    Serial.write(buf, reqLength);
    delay(10); // может уменьшить?
    digitalWrite(13, LOW); // включаем приём
    //===========================
    
//    int i = 0;
//    while (btSerial.available()) {
//      buf[i] = btSerial.read();
//      i++;
//    }
    //Serial.write(buf, 2);
    //Serial.println("i: " + String(i));
    //Serial.println(buf[0], HEX);

//    for (int i = 0; i < reqLength; i++) {
//      //Serial.println(buf[j], HEX);
//      Serial.print(buf[j], HEX);
//      Serial.print(" ");
//    }
//    Serial.println();
  }

  // с компа в смарт:
  if (Serial.available()) {
//    data = Serial.read();
//    btSerial.print(data);
    // byte buf2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // Serial.readBytes(buf2, 10);
	
    byte buf[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int bufLength = sizeof(buf);
    Serial.readBytes(buf, bufLength);	

	// Читаем 3й байт (buf[2]), в нём количество байт идущих далее,
	// добавляем ещё 2 байта CRC и отправляем
	int numNextBytes = buf[2];
	int answerLength = 3 + numNextBytes + 2;
	
	// если вдруг ответ получился длиннее буфера (на всякий случай)
	// то просто шлём весь буфер
	if (answerLength > bufLength) {
		answerLength = bufLength;
	}
	
    btSerial.write(buf, answerLength);
  }
  
}
