//zigbee_thermo class

#include <string>
#include <sstream>
#include <map>
using std::string;
#include <Arduino.h>

enum zigbeeType{
    nothing=0,
    comment,
    temperature,
    humidity
};

class zigbee_thermo{
    public:
        zigbee_thermo();
        zigbee_thermo(string name);

        zigbee_thermo(string txt, float temp, int humi);
        zigbee_thermo(string name, string txt, float temp, int humi);
        void setTemp(float t);
        void setText(string t);
        void setHumi(int h);
        string getText();
        float getTemp();
        int getHumi();
        static zigbee_thermo parseTempHumi(string topic, string payload);
        static string getName(string topic);
        string getName();
        static zigbeeType getType(string topic, string payload);
        zigbee_thermo putItem(string name, zigbee_thermo zibee_item);
        zigbee_thermo getItem(string n);

        string dumpItem();
        string dumpItem(zigbee_thermo zb);
        string dumpList();

    private:
        string _name;
        string _txt;
        float _temp;
        int _humi;
};