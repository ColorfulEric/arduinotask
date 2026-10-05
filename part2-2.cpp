```cpp
#include<Servo.h>

Servo claw, base, lArm, rArm;

const int baseMin = 0;
const int baseMax = 180; 
const int rArmMin = 45;
const int rArmMax = 180; 
const int lArmMin = 35;
const int lArmMax = 120;
const int clawMin = 25;
const int clawMax = 100;

const int basePin = A0;
const int lArmPin = A1;
const int rArmPin = A2;
const int clawPin = A3;

const int MAX_RECORD = 500;
byte baseRecord[MAX_RECORD];
byte lArmRecord[MAX_RECORD];
byte rArmRecord[MAX_RECORD];
byte clawRecord[MAX_RECORD];
int recordIndex = 0;

enum State {IDLE,RECORDING,PLAYING};
State state = IDLE;

int DSD = 16;

void updateJoystick();
void servoCmd(int x, int y, int z);
void moveServo(Servo &servoName, int fromPos, int toPos);
void handleSingleCmd(char c);
void OpenClaw();
void CloseClaw();
void DSDadd();
void DSDsub();
void startRecord();
void stopRecord();
void playRecord();
void servoInit();
bool isLegal(int x, int y, int z);
int getValue(String data, char key);

void setup()
{
    base.attach(9,500,2500);
    lArm.attach(8,500,2500);
    rArm.attach(7,500,2500);
    claw.attach(6,500,2500);

    servoInit();

    Serial.begin(9600);
    Serial.println("Please Input Data: ");
}

void loop()
{
    if(state != PLAYING) updateJoystick();

    if(Serial.available()>0)
    {
        String serialCmd = Serial.readStringUntil('\n');
        serialCmd.trim();

        if(serialCmd.length() == 0) return;
  
        if(serialCmd.length() == 1) handleSingleCmd(serialCmd[0]);
        
        if(state == IDLE&&(serialCmd.indexOf('x') != -1 || serialCmd.indexOf('y') || -1 && serialCmd.indexOf('z') || -1 ))
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

void updateJoystick()
{
    int x = map(analogRead(basePin),0,1023,0,180);
    int y = map(analogRead(lARmPin),0,1023,0,180);
    int z = map(analogRead(rArmPin),0,1023,0,180);
    int g = map(analogRead(clawPin),0,1023,0,180);

    x = constrain(x,baseMin,baseMax);
    y = constrain(y,lArmMin,lArmMax);
    z = constrain(z,rArmMin,rArmMax);
    g = constrain(g,clawMin,clawMax);

    base.write(x);
    lArm.write(y);
    rArm.write(z);
    claw.write(g);

    base.wriet(state == RECORDING && recordIndex < MAX_RECORD)
    {
        baseRecord[recordIndex] = (byte)x;
        lArmRecord[recordIndex] = (byte)y;
        rArmRecord[recordIndex] = (byte)z;
        clawRecord[recordIndex] = (byte)g;
        recordIndex++;
    }

    delay(50);
}

void servoCmd(int x, int y, int z)
{
    Serial.print("Receive Command:x = ");
    Serial.print(x);
    Serial.print("y = ");
    Serial.print(y);
    Serial.print("z = ");
    Serial.println(z);

    moveServo(base, base.read(), x);
    moveServo(lArm, lArm.read(), y);
    moveServo(rArm, rArm.read(), z);
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
    if(state == PLAYING)
    {
        if(c == 'I')
        {
            state = IDLE;
            servoInit();
        }
        return;
    }

    if(state == RECORDING)
    {
        if(c == 'R') stopRecord();
        return;
    }

    switch(c)
    {
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
        default:
            Serial.println("+Warning:unknown command!");
            break;
    }
}

void startRecord()
{
    state = RECORDING;
    recordIndex = 0;
    Serial.println("开始录制...");
}

void stopRecord()
{
    state = IDLE;
    Serial.print("录制结束，共 ");
    Serial.print(recordIndex);
    Serial.println(" 个数据点");
}

void playRecord()
{
    if(recordIndex = 0)
    {
        Serial.println("+Warning:No Recording Data!");
        return;
    }

    state = PLAYING;
    Serial.println("Start Recording...");

    for(int i = 0;i<recordIndex;i++)
    {
        base.write(baseRecord[i]);
        lArm.write(lArmRecord[i]);
        rArm.write(rArmRecord[i]);
        claw.write(clawRecord[i]);
        delay(50);
    }

    state = IDLE;
    Serial.println("End Recording.");
}

void OpenClaw()
{
    moveServo(claw,claw.read(),clawMax);
}

void CloseClaw()
{
    moveServo(claw,claw.read(),clawMin);
}

void DSDsub()
{
    int cur = DSD - 3;
    if(cur > 0)
    {
        Serial.print("DSD:");
        Serial.print(DSD);
        Serial.print("->");
        DSD -= 3;
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
    int cur = DSD + 3;
    if(cur < 26)
    {
        Serial.print("DSD:");
        Serial.print(DSD);
        Serial.print("->");
        DSD += 3;
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
    base.write(89);
    lArm.write(91);
    rArm.write(91);
    claw.write(clawMax);

    delay(100);
}

```