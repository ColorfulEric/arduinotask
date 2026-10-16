// 串口通信代码：读 4 个按键，通过串口发送指令
// 可烧录到 Arduino 或 esp8266（改引脚编号即可）

const int button1 = 2;   // 按键1：循环夹取 A/B/C
const int button2 = 3;   // 按键2：录制开始/结束
const int button3 = 4;   // 按键3：播放
const int button4 = 5;   // 按键4：回中

int step = 0;

void setup() {
  Serial.begin(9600);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);

  Serial.println("Serial Controller Ready");
}

void loop() {
  // 按键1：循环发 A / B / C
  if (digitalRead(button1) == LOW) {
    if (step == 0)
      Serial.println("A");
    else if (step == 1) 
      Serial.println("B");
    else if (step == 2) 
      Serial.println("C");

    step = (step + 1) % 3;
    delay(300);
  }

  // 按键2：录制开始/结束
  if (digitalRead(button2) == LOW)
  {
    Serial.println("R");
    delay(300);
  }

  // 按键3：播放
  if (digitalRead(button3) == LOW)
  {
    Serial.println("P");
    delay(300);
  }

  // 按键4：回中
  if (digitalRead(button4) == LOW) 
  {
    Serial.println("I");
    delay(300);
  }
}