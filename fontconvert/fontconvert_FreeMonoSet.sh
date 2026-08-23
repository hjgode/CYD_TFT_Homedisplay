#!/bin/bash
for fSize in 8 10 12 14 16 18 20;
    do 
        echo "./fontconvert FreeMono.ttf ${fSize} 32 255 >FreeMono${fSize}pt8b.h"
        ./fontconvert FreeMono.ttf ${fSize} 32 255 >FreeMono${fSize}pt8b.h
    done
#./fontconvert FreeMono.ttf 10 32 255 >FreeMonoBold10pt8b.h
