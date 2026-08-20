//BenzinPreise.h
#include <Arduino.h>

//store benzinpreise in array

class BenzinPreis{
    private:
        String _brand;
        String _name;
        float _preis;
        bool _trend;
    public:
        BenzinPreis();
        // name, preis, trend (steigt=true, fällt=false)
        BenzinPreis(String brand, String name, float price, bool trend);
        //~BenzinPreis();
        String getBrand(){return _brand;}
        String getName(){return _name;}
        float getPreis(){return _preis;}
        bool getTrend(){return _trend;}
        void setBrand(String b){_brand=b;}
        void setName(String b){_name=b;}
        void setPreis(float b){_preis=b;}
        void setTrend(bool b){_trend=b;}
        String toString();
};

class BenzinPreise
{
    private:
        BenzinPreis _benzinpreise[5];
        /* data */
    public:
        BenzinPreise();
        //~BenzinPreise();
        BenzinPreis parseAndUpdate(String topic, String data);
};

