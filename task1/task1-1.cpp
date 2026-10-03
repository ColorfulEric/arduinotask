#include <Servo.h>
Servo claw;

int DSD = 15;

const int clawMin = 25;
const int clawMax = 100;

void setup()
{
    Serial.begin(9600);
    claw.attach(9);
    delay(100);
    claw.write(25);
    delay(10);

    Serial.println("please input data:");
}

void loop()
{
    if(Serial.available())
    {
        char serialCmd = Serial.read();
        switch(serialCmd)
        {
            case 'O':
            case 'o':
                for(int i=25;i<=100;i++)
                {
                    claw.write(i);
                    delay(DSD);
                }
                break;
            case 'S':
            case 's':
                for(int i=100;i>=25;i--)
                {
                    claw.write(i);
                    delay(DSD);
                }
                break;
            case 'H':
            case 'h':
                int cur = DSD - 5;
                if(cur>=5)
                {
                    DSD -=5;
                    Serial.print("your current spped is:");
                    Serial.println(DSD);
                }
                else
                {
                    Serial.print("Warning:your current speed is too fast!");
                    Serial.print("DSD:");
                    Serial.println(DSD);
                }
                break;
            case 'L':
            case 'l':
                int cur = DSD + 5;
                if(cur<=50)
                {
                    DSD += 5;
                    Serial.print("your current speed is:");
                    Serial.println(DSD);
                }
                else
                {
                    Serial.print("Warning:your current speed is too slow!");
                    Serial.print("DSD:");
                    Serial.println(DSD);
                }
            default:
                Serial.println("Unknown Command!");
        }
    }
}