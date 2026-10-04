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

int DSD = 16;

void servoCmd(int x, int y, int z);
void moveServo(Servo &servoName, int fromPos, int toPos);
int getValue(String data, char key);
bool isLegal(int x, int y, int z);
void handleSingleCmd(char c);
void OpenClaw();
void CloseClaw();
void DSDadd();
void DSDsub();

void setup()
{
    claw.attach(9);
    base.attach(3);
    lArm.attach(5);
    rArm.attach(6);

    claw.write(90);
    base.write(90);
    lArm.write(90);
    rArm.write(90);

    delay(100);

    Serial.begin(9600);
    Serial.println("please input data:");
}

void loop()
{
    if(Serial.available()>0)
    {
        String serialCmd = Serial.readStringUntil('\n');
        serialCmd.trim();

        if(serialCmd.length() == 0) return;
  
        if(serialCmd.length() == 1) handleSingleCmd(serialCmd[0]);
        else if(serialCmd.indexOf('x') != -1 && serialCmd.indexOf('y') != -1 && serialCmd.indexOf('z') != -1 )
        {
            //x,y,z分别对应base,lArm,rArm
            int x = getValue(serialCmd, 'x');
            int y = getValue(serialCmd, 'y');
            int z = getValue(serialCmd, 'z');

            if(isLegal(x, y, z)) servoCmd(x,y,z);
            else Serial.println("+Warning: Your Command is Out Of Limits!");
        }
        else Serial.println("+Warning: Unknown Command!");
    }
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
    switch(c)
    {
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
        default:
            Serial.println("+Warning:unknown command!");
    }
}

void OpenClaw()
{
    int fromPos = claw.read();
    int toPos = clawMax;

    for(int i = fromPos;i <= toPos;i++)
    {
        claw.write(i);
        delay(DSD);
    }
}

void CloseClaw()
{
    int fromPos = claw.read();
    int toPos = clawMin;

    for(int i = fromPos;i >= toPos;i--)
    {
        claw.write(i);
        delay(DSD);
    }
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
```