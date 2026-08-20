//BenzinPreise.cpp
#include "BenzinPreise.h"

bool debugme=true;

BenzinPreis::BenzinPreis(){
    this->_brand="";
    this->_name="";
    this->_preis=0.0;
    this->_trend=false;
}

BenzinPreis::BenzinPreis(String brand, String name, float price, bool trend){
    this->_brand=brand;
    this->_name=name;
    this->_preis=price;
    this->_trend=trend;
}
String BenzinPreis::toString(){
    String s="";
    s+=this->_brand +", ";
    s+=this->_name + ", ";
    s+=String(this->_preis) + ", ";
    s+=(this->_trend)?"steigt":"fällt";
    return s; 
}

BenzinPreis BenzinPreise::parseAndUpdate(String topic, String data){
    BenzinPreis _benzinpreis;
    //get index fro 0_..., 1_...
    int idx=topic.substring(0,1).toInt();
    if (debugme) Serial.printf("\nparseAndUpdate idx=%i\n", idx);
    //x_brand, x_name, x_e10_price, x_e10_trend [steigt|fällt]
    if (topic.indexOf("brand")>0)
        _benzinpreis.setBrand(data);
    if (topic.indexOf("name")>0)
        _benzinpreis.setName(data);
    if (topic.indexOf("price")>0)
        _benzinpreis.setPreis(data.toFloat());
    if (topic.indexOf("trend")>0){
        if (data.endsWith("steigt"))
            _benzinpreis.setTrend(true);
        else
            _benzinpreis.setTrend(false);
    }
    _benzinpreise[idx]=_benzinpreis;
    return _benzinpreis;
}

BenzinPreise::BenzinPreise()
{
    for(int i=0; i<5; i++){
        _benzinpreise[i]=BenzinPreis();// BenzinPreis("", "", 0, false);
    }
}

