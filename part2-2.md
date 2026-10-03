```cpp
#include<Servo.h>

Servo claw;

const int clawMin = 25;
const int clawMax = 100;

int DSD = 15;

void setup()
{
    claw.attach(9);
    claw.write(90);
    Serial.begin(9600);
    Serial.println("please input data:");
}

void loop()
{
    if(Serial.available()>0)
    {
        char serialCmd = Serial.read();
        switch(serialCmd)
        {
            case 'O':
            case 'o':
                OpenClaw();
                break;
            case 'S':
            case 's':
                CloseClaw();
                break;
            case 'H':
            case 'h':
                DSDsub();
                break;
            case 'L':
            case 'l':
                DSDadd();
                break;
            default:
                Serial.println("+Warning:unknown command!");
        }
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
    if(cur < 25)
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