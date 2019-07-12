void byte2pinMap(){
   pinState = pinState % 128;
   for(int bitI =0; bitI <= 6; bitI++ ){
          pinMap[bitI] = bitRead(pinState,bitI);
      }
}
