```cpp
#include<Servo.h>

Servo claw, base, lArm, rArm;//创建对象

//限制角度
const int baseMin = 0;
const int baseMax = 180; 
const int rArmMin = 90;
const int rArmMax = 160; 
const int lArmMin = 35;
const int lArmMax = 120;
const int clawMin = 0;
const int clawMax = 100;

//初始化引脚
const int basePin = A0;
const int lArmPin = A1;
const int rArmPin = A2;
const int clawPin = A3;

//限制最大录制时长
const int MAX_RECORD = 200;
byte baseRecord[MAX_RECORD];
byte lArmRecord[MAX_RECORD];
byte rArmRecord[MAX_RECORD];
byte clawRecord[MAX_RECORD];
int recordIndex = 0;

//限制系统状态：空闲、录制、播放
enum State {IDLE,RECORDING,PLAYING};
//初始化状态
State state = IDLE;

//摇杆中值
const int JOY_CENTER = 512;
//摇杆死区
const int JOY_DEADZONE = 50;//死区，防止抖动

//每个舵机的当前角度（增量控制的“状态”）
int currentX = 90;
int currentY = 90;
int currentZ = 90;
int currentG = 90;

//移动速度系数，越大越慢
const int SPEED_DIV = 140;

//延迟时长
int DSD = 26;

void updateJoystick();//摇杆控制
void servoCmd(int x, int y, int z);//处理多个串口指令
void moveServo(Servo &servoName, int fromPos, int toPos);//舵机运动
void handleSingleCmd(char c);//处理单个转口指令
void OpenClaw();//打开机械钳
void CloseClaw();//合闭机械钳
void DSDadd();//减慢运行速度
void DSDsub();//加快运行速度
void startRecord();//开始录制
void stopRecord();//停止录制
void playRecord();//播放
void servoInit();//回中
void printServoPos();//打印舵机状态
bool isLegal(int x, int y, int z);//判断是否合法
int getValue(String data, char key);//获取指令数据

void setup()
{
    base.attach(9,500,2500);
    delay(500);
    lArm.attach(8,500,2500);
    delay(500);
    rArm.attach(7,500,2500);
    delay(500);
    claw.attach(6,500,2500);
    delay(500);

    servoInit();//回中

    Serial.begin(9600);
    Serial.println("Please Input Data: ");
}

void loop()
{
    //摇杆只有在播放模式下不能使用
    if(state != PLAYING) updateJoystick();

    if(Serial.available()>0)
    {
        String serialCmd = Serial.readStringUntil('\n');//截取整行指令
        serialCmd.trim();//删除空格、换行符

        if(serialCmd.length() == 0) return;//防误触
  
        if(serialCmd.length() == 1)//处理单指令
        {
            handleSingleCmd(serialCmd[0]);
            return;//执行一次直接结束，避免后续二次执行。
        }
        
        if(state == IDLE&&(serialCmd.indexOf('x') != -1 || serialCmd.indexOf('y') != -1 || -1 && serialCmd.indexOf('z') || -1 ))
        {
            //x,y,z分别对应base,lArm,rArm
            int x = (serialCmd.indexOf('x') != -1) ? getValue(serialCmd, 'x') : base.read();
            int y = (serialCmd.indexOf('y') != -1) ? getValue(serialCmd, 'y') : lArm.read();
            int z = (serialCmd.indexOf('z') != -1) ? getValue(serialCmd, 'z') : rArm.read();

            if(isLegal(x, y, z)) servoCmd(x,y,z);
            else Serial.println("+Warning: Your Command is Out Of Limits!");
        }
        else Serial.println("+Warning: Unknown Command!");
    }
}

void updateJoystick()//摇杆控制核心
{
    //读取数据
    int rawX = analogRead(basePin);
    int rawY = analogRead(lArmPin);
    int rawZ = analogRead(rArmPin);
    int rawG = analogRead(clawPin);

    int offsetX = rawX - JOY_CENTER;
    int offsetY = rawY - JOY_CENTER;
    int offsetZ = rawZ - JOY_CENTER;
    int offsetG = rawG - JOY_CENTER;

    //死区过滤，防止摇杆微小位移而发生抖动，确定需要多大的幅度才能使机械臂移动
    if(abs(offsetX)<JOY_DEADZONE) offsetX = 0;
    if(abs(offsetY)<JOY_DEADZONE) offsetY = 0;
    if(abs(offsetZ)<JOY_DEADZONE) offsetZ = 0;
    if(abs(offsetG)<JOY_DEADZONE) offsetG = 0;

    //累计增量
    currentX += offsetX / SPEED_DIV;
    currentY += offsetY / SPEED_DIV;
    currentZ += offsetZ / SPEED_DIV;
    currentG += offsetG / SPEED_DIV;

    //限幅
    currentX = constrain(currentX, baseMin, baseMax);
    currentY = constrain(currentY, lArmMin, lArmMax);
    currentZ = constrain(currentZ, rArmMin, rArmMax);
    currentG = constrain(currentG, clawMin, clawMax);

    //写入
    base.write(currentX);
    lArm.write(currentY);
    rArm.write(currentZ);
    claw.write(currentG);

    //顺带录制
    if(state == RECORDING && recordIndex < MAX_RECORD)
    {
        baseRecord[recordIndex] = (byte)currentX;
        lArmRecord[recordIndex] = (byte)currentY;
        rArmRecord[recordIndex] = (byte)currentZ;
        clawRecord[recordIndex] = (byte)currentG;
        recordIndex++;
    }

    delay(50);
}

void servoCmd(int x, int y, int z)
{
    Serial.print("Receive Command:x = ");
    Serial.print(x);
    Serial.print(" y = ");
    Serial.print(y);
    Serial.print(" z = ");
    Serial.println(z);

    moveServo(base, base.read(), x);
    moveServo(lArm, lArm.read(), y);
    moveServo(rArm, rArm.read(), z);

    //只要涉及直接写入舵机的就要手动统一数据
    currentX = x;
    currentY = y;
    currentZ = z;
}

void moveServo(Servo &servoName, int fromPos, int toPos)
{
    if(fromPos>toPos)
    {
        for(int i = fromPos; i >= toPos; i--)
        {
            servoName.write(i);
            delay(DSD);
        }
    }
    else
    {
        for(int i = fromPos; i <= toPos; i++)
        {
            servoName.write(i);
            delay(DSD);
        }
    }
}

int getValue(String data, char key)
{
    int index = data.indexOf(key);
    if(index == -1) return -1;

    int end = data.indexOf(',',index);
    if(end == -1) end = data.length();

    return data.substring(index + 1, end).toInt();
}

bool isLegal(int x, int y, int z)
{
    if(x < baseMin || x > baseMax) return false;
    if(y < lArmMin || y > lArmMax) return false;
    if(z < rArmMin || z > rArmMax) return false;
    return true;
}

void handleSingleCmd(char c)
{
    if(state == RECORDING)
    {
        if(c == 'R') stopRecord();
        return;
    }

    switch(c)
    {
        case '?':
            Serial.println("----- Servo Position -----");
            printServoPos();
            Serial.println("--------------------------");
            break;
        case 'I':
            servoInit();
            break;
        case 'O':
            OpenClaw();
            break;
        case 'S':
            CloseClaw();
            break;
        case 'H':
            DSDsub();
            break;
        case 'L':
            DSDadd();
            break;
        case 'R':
            startRecord();
            break;
        case 'P':
            playRecord();
            break;
        case 'A':
            pickA();
            break;
        default:
            Serial.println("+Warning:unknown command!");
            break;
    }
}

void startRecord()
{
    state = RECORDING;
    recordIndex = 0;
    Serial.println("Start recording...");
}

void stopRecord()
{
    state = IDLE;
    Serial.print("End Recording, total ");
    Serial.print(recordIndex);
    Serial.println(" data points.");
}

void playRecord()
{
    if(recordIndex == 0)
    {
        Serial.println("+Warning:No Recording Data!");
        return;
    }

    state = PLAYING;
    Serial.println("Start Playing...");

    for(int i = 0;i<recordIndex;i++)
    {
        base.write(baseRecord[i]);
        lArm.write(lArmRecord[i]);
        rArm.write(rArmRecord[i]);
        claw.write(clawRecord[i]);
        delay(50);
    }

    state = IDLE;
    Serial.println("End Playing...");

    if (recordIndex > 0)
    {
        currentX = baseRecord[recordIndex - 1];
        currentY = lArmRecord[recordIndex - 1];
        currentZ = rArmRecord[recordIndex - 1];
        currentG = clawRecord[recordIndex - 1];
    }
}

void OpenClaw()
{
    moveServo(claw,claw.read(),clawMin);
    currentG = clawMin;
}

void CloseClaw()
{
    moveServo(claw,claw.read(),clawMax);
    currentG = clawMax;
}

void DSDsub()
{
    int cur = DSD - 5;
    if(cur > 0)
    {
        Serial.print("DSD:");
        Serial.print(DSD);
        Serial.print("->");
        DSD -= 5;
        Serial.println(DSD);
    } 
    else
    {
        Serial.print("+Warning:too fast! your current speed is:");
        Serial.println(DSD);
    }
}

void DSDadd()
{
    int cur = DSD + 5;
    if(cur < 51)
    {
        Serial.print("DSD:");
        Serial.print(DSD);
        Serial.print("->");
        DSD += 5;
        Serial.println(DSD);
    } 
    else
    {
        Serial.print("+Warning:too slow! your current speed is:");
        Serial.println(DSD);
    }
}

void servoInit()
{
    servoCmd(90,90,90);
    currentX = 90;
    currentY = 90;
    currentZ = 90;

    delay(300);

    moveServo(claw,claw.read(),90);
    currentG = 90;
}

void printServoPos()
{
    Serial.print("base: read=");
    Serial.print(base.read());
    Serial.print(" current=");
    Serial.print(currentX);

    Serial.print("  lArm: read=");
    Serial.print(lArm.read());
    Serial.print(" current=");
    Serial.print(currentY);

    Serial.print("  rArm: read=");
    Serial.print(rArm.read());
    Serial.print(" current=");
    Serial.print(currentZ);

    Serial.print("  claw: read=");
    Serial.print(claw.read());
    Serial.print(" current=");
    Serial.println(currentG);
}

void pickA()
{
    servoInit();
    int steps[2][3]={
        {0,180,0},
        {180,45,180}
    };
    for(int i = 0;i<=2;i++)
    {
        moveServo(base,base.read(),steps[0][i]);
        moveServo(lArm,lArm.read(),steps[1][i]);
        delay(50);
    }
}
```