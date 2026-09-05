//sigbee thermo class

#include "zigbee_thermo.h"

static zigbee_liste _liste;

zigbee_thermo::zigbee_thermo(){
    _txt="";
    _temp=0.0;
    _humi=0;
}

zigbee_thermo::zigbee_thermo(string name){
    _name=name;
    _txt="";
    _temp=0.0;
    _humi=0;
}

string zigbee_thermo::getName(){
    return _name;
}

string zigbee_thermo::getName(string topic){
    //find the device ID and return it
    //ie mqttGenericBridge/zigbee/zigbee_0xa4c1389ca963dfc0/humidity temperature, comment
    int pos = topic.find("zigbee_0x"); // "mqttGenericBridge/zigbee/", 0);
    
    string x="zigbee_0xa4c1389ca963dfc0";
    int leng = x.length();
    string n = topic.substr(pos, leng);
    Serial.printf("\ntopic: %s, pos=%i, name=%s\n", topic.c_str(), pos, n.c_str());
    return n;
}

/// @brief add new zigbee device to list
/// @param n name of the device
/// @param zb the zigbee element to add or change
/// @return 
zigbee_thermo zigbee_thermo::putItem(string n, zigbee_thermo zb){
    if(_liste.find(n) != _liste.end()){
        //if exists
        _liste[n]=zb;
        return _liste[n];
    }else{
        //add new
        _liste[n]=zb;
        return _liste[n];
    }
}

string zigbee_thermo::dumpItem(){
    string s;
    std::ostringstream ss;
    ss << _temp;
    string sTemp=ss.str();
    std::ostringstream ss2;
    ss2 << _humi;
    string sHumi=ss2.str();
    s = _name + ", " + _txt + ", "+ sTemp + ", " + sHumi;
    return s;
}
string zigbee_thermo::dumpItem(zigbee_thermo zb){
    string s;
    std::ostringstream ss;
    ss << zb.getTemp();
    string sTemp=ss.str();
    std::ostringstream ss2;
    ss2 << zb.getHumi();
    string sHumi=ss2.str();
    s = zb.getName() + ", " + zb.getText().c_str() + ", " + sTemp + ", " + sHumi;
    return s;
}

string zigbee_thermo::dumpList(){
    string buf;
    
    for (auto v :_liste){
        // map.first is key, map second is value
        buf = buf + v.first + ": " + dumpItem(v.second) + "\n";
    }
    return buf;
    
}

zigbee_liste zigbee_thermo::getListe(){
    return _liste;
}

/// @brief try to get a zibee item
/// @param n name of the zigbee device
/// @return the found zigbee device or a new one
zigbee_thermo zigbee_thermo::getItem(string n){
    for (auto v : _liste){
        ;
    }
    if(_liste.find(n) != _liste.end()){
        return _liste[n];
    }
    else{
        zigbee_thermo zb=zigbee_thermo(n);
        putItem(n, zb);
        return zb;
    }
}

zigbee_thermo::zigbee_thermo(string txt, float temp, int humi){
    _txt=txt;
    _temp=temp;
    _humi=humi;
}

zigbee_thermo::zigbee_thermo(string name, string txt, float temp, int humi){
    _name=name;
    _txt=txt;
    _temp=temp;
    _humi=humi;
}

zigbee_thermo zigbee_thermo::parseTempHumi(string topic, string payload){
    //parse topic and payload
    //topics = comment, temperature, humidity
    zigbee_thermo zb;
    if(topic.find("comment")){
        zb.setText(payload);
    }
    return zigbee_thermo();
}

zigbeeType zigbee_thermo::getType(string topic, string payload){
    zigbeeType myType=nothing;
    if(topic.find("comment")!=-1){
        myType=comment ;
    }
    else if(topic.find("temperature")!=-1){
        myType=temperature ;
    }
    else if(topic.find("humidity")!=-1){
        myType=humidity ;
    }
    return myType;
}

void zigbee_thermo::setText(string t){
    _txt=t;
}
void zigbee_thermo::setTemp(float t){
    _temp=t;
}
void zigbee_thermo::setHumi(int t){
    _humi=t;
}

int zigbee_thermo::getHumi(){
    return _humi;
}
float zigbee_thermo::getTemp(){
    return _temp;
}
string zigbee_thermo::getTempStr(){
    string s;
    std::ostringstream ss;
    ss << _temp;
    string sTemp=ss.str();
    return sTemp;
}
string zigbee_thermo::getHumiStr(){
    string s;
    std::ostringstream ss;
    ss << _humi;
    string sTemp=ss.str();
    return sTemp;
}
string zigbee_thermo::getText(){
    return _txt;
}