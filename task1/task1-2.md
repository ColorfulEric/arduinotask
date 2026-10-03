```cpp
#include<Servo.h>

Servo servoX,servoY,servoZ;

const int xMin = 0;//设置舵机旋转范围
const int xMax = 180;
const int yMin = 45;
const int yMax = 180;
const int zMin = 35;
const int zMax = 120;

const int DSD = 15;

void setup()
{
    Serial.begin(9600);
    servoX.attach(6);
    servoY.attach(7);
    ServoZ.attach(8);

    servoX.write(90);
    delay(100);
    servoY.write(90);
    delay(100);
    servoZ.write(90);
    delay(100);

    Serial.println("please input data:");
}

void loop()
{
    if(Serial.available())
    {
        int x = Serial.parseInt();
        int y = Serial.parseInt();
        int z = Serial.parseInt();

        if(isLegal(x,y,z))
        {
            servoCmd(x,y,z);
        }
        else
        {
            Serial.println("+Warning:your command is out of limits !");
        }

    }
}

bool isLegal(int x,int y,int z)
{
    if(x>=xMAx||x<=xMin||y>=yMax||y<=yMin||z>=zMax||z<=zMin)
        return false;
    return true;
}

void servoCmd(int x,int y,int z)
{
    int fromPosX = servoX.read();
    int formPosY = servoY.read();
    int fromPosZ = servoZ.read();

    int toPosX = x;
    int toPosY = y;
    int toPosZ = z;

    for(int i = fromPosX; i <= toPosX; i++)
    {
        servoX.write(i);
        delay(DSD);
    }

    for(int i = fromPosY; i <= toPosY; i++)
    {
        servoY.write(i);
        delay(DSD);
    }

    for(int i = fromPosZ; i <= toPosZ; i++)
    {
        servoZ.write(i);
        delay(DSD);
    }
}
```