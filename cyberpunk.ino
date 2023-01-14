#define EXP 1.4             // степень усиления сигнала (для более "резкой" работы) (по умолчанию 1.4)
#define MAX_COEF 1.7        // коэффициент громкости (максимальное равно срднему * этот коэф) (по умолчанию 1.8)
#include "GyverButton.h"
GButton butt1(3);



#include "FastLED.h" // подключаем библиотеку фастлед
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        5 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 64 // Popular NeoPixel ring size
CRGB pixels[NUMPIXELS];







#include <OLED_I2C.h>

OLED  myOLED(SDA, SCL, 8);

extern uint8_t background_off[];
extern uint8_t smile[][512];
extern uint8_t fire[][512];
extern uint8_t birthday[][1024];




struct Animation { // создаём ярлык myStruct
  int len;
  uint8_t *cadrs;
  //uint8_t (*cadrs)[][512];
  int x;
  int y;
  int width;
  int height;
  int size_file;
};


Animation Animations[]={
  {1,&smile[0][0],32,0,64,64,512},  
  {1,&fire[0][0],32,0,64,64,512},
  {4,&birthday[0][0],0,0,128,64,1024}, 
  //{1,&mihael[0][0],0,0,128,64,1024},
 // {20,&mihael[0][0],0,0,128,64,1024},  
};
//Animations[0]=(Animation){}
int count_images = sizeof(Animations)  / sizeof(Animations[0]);
uint32_t time_for_rgb = millis();
uint32_t time_for_display = millis();
int regim=0;
int num_image=0;
int regim_line = 2;
int i_for_display = 0;
int r=255,g=127,b=0;//основной цвет
int r2=0,g2=127,b2=255;//второй цвет
float rk=0.2,gk=0.2,bk=0.2;//коэффициэнт основного цвета
float rk2=0.3,gk2=0.3,bk2=0.3;//коэффициэнт второго цвета






#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
void setup(){
//Animations[0].cadrs=loading;
  butt1.setTimeout(450);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  myOLED.begin();
  Serial.begin(9600);

  
  FastLED.addLeds <WS2812, PIN, GRB>(pixels, NUMPIXELS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);

  // жуткая магия, меняем частоту оцифровки до 18 кГц
  // команды на ебучем ассемблере, даже не спрашивайте, как это работает
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);  
}


// градиент-палитра от зелёного к красному

DEFINE_GRADIENT_PALETTE(soundlevel_gp) {
  0,    0,    255,  0,  // green
  100,  255,  255,  0,  // yellow
  150,  255,  100,  0,  // orange
  200,  255,  50,   0,  // red
  255,  255,  0,    0   // red
};
CRGBPalette32 myPal = soundlevel_gp;
int RsoundLevel, RcurrentLevel, RsoundLevel_f=200, maxLevel=0, LOW_PASS = 500, sred=500;
float SMOOTH = 0.5, averK = 0.006, averageLevel=0;          // коэффициент плавности анимации VU (по умолчанию 0.5)
float index = (float)255 / NUMPIXELS*2;   // коэффициент перевода для палитры
byte Rlenght;
void signal(){
  RsoundLevel = 0;
  LOW_PASS=0;
  for (byte i = 0; i < 100; i ++) {                                 // делаем 100 измерений
        RcurrentLevel = analogRead(A0);                            // с правого
        LOW_PASS += analogRead(A0);  
        if (RsoundLevel < RcurrentLevel) {RsoundLevel = RcurrentLevel;}   // ищем максимальное
      }
      LOW_PASS/=100;
      sred = (sred*49+LOW_PASS)/50;
      /*Serial.print(RsoundLevel);
      Serial.print(" ");
      Serial.print(sred);
      Serial.print(" ");*/
      Serial.print(RsoundLevel-sred);
      // фильтруем по нижнему порогу шумов
      RsoundLevel = map(RsoundLevel-sred, 0, 16, 0, 400);
      Serial.print(" ");
      //RsoundLevel = map(RsoundLevel, LOW_PASS, 1023, 0, 500);
      // ограничиваем диапазон
      RsoundLevel = constrain(RsoundLevel, 0, 500);
      // возводим в степень (для большей чёткости работы)
      RsoundLevel = pow(RsoundLevel, EXP);
      // фильтр
      Serial.print(RsoundLevel);
      Serial.print(" ");
      RsoundLevel_f = RsoundLevel * SMOOTH + RsoundLevel_f * (1 - SMOOTH);
      Serial.print(RsoundLevel_f);
      Serial.print(" ");
      Rlenght = map(RsoundLevel_f, 0, 2000, 0, NUMPIXELS/2);
      // если значение выше порога - начинаем самое интересное
      /*if (RsoundLevel_f > -1){//15) {
        // расчёт общей средней громкости с обоих каналов, фильтрация.
        // Фильтр очень медленный, сделано специально для автогромкости
        averageLevel = (float)(RsoundLevel_f) * averK + averageLevel * (1 - averK);
        // принимаем максимальную громкость шкалы как среднюю, умноженную на некоторый коэффициент MAX_COEF
        maxLevel = (float)averageLevel * MAX_COEF;
        // преобразуем сигнал в длину ленты (где NUMPIXELS/2 это половина количества светодиодов)
        Rlenght = map(RsoundLevel_f, 0, maxLevel, 0, NUMPIXELS/2);
        // ограничиваем до макс. числа светодиодов
        Rlenght = constrain(Rlenght, 0, NUMPIXELS/2);}*/
      Serial.print(Rlenght);
      Serial.print(" ");
}


float count=0;
float power_of_signal = 0;int k =0;
void rgb_line(int i=regim_line){
  regim_line = i%6;
  switch(regim_line){
    case 0:
      for(int i=0;i<NUMPIXELS;i++){pixels[i] = CRGB(r,g,b);}
      break;
    case 1:
      for(int i=0;i<NUMPIXELS;i++){pixels[i] = CRGB(r2,g2,b2);}
      break;
    case 2:
      if ((millis()-time_for_rgb)<50*k){return ;}
      k++;
      if ((millis()-time_for_rgb)>(random(1000)+400)){Rlenght = NUMPIXELS*random(100)/200;
      time_for_rgb=millis();
      k=0;}
      if (power_of_signal <= Rlenght){power_of_signal = Rlenght;}else{power_of_signal-=0.6;}
      Serial.println(power_of_signal);
      butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться
      FastLED.clear();
      for (int i = 0; i < power_of_signal/2; i++) {
        pixels[(NUMPIXELS+1)/2+i] = ColorFromPalette(RainbowColors_p, ((count+i) * index*0.75));   // заливка по палитре " от зелёного к красному"
        pixels[(NUMPIXELS+1)/2-i] = ColorFromPalette(RainbowColors_p, ((count+i) * index*0.75));   // заливка по палитре " от зелёного к красному"
      }
      count+=0.3;
      FastLED.show();
      return;
      break;
    case 3:    
      if ((millis()-time_for_rgb)<50){return ;}
      signal();
      if (power_of_signal <= Rlenght){power_of_signal = Rlenght;}else{power_of_signal-=0.6;}
      Serial.println(power_of_signal);
      butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться
      FastLED.clear();
      for (int i = 0; i < power_of_signal/2; i++) {
        pixels[(NUMPIXELS+1)/2+i] = ColorFromPalette(myPal, ((count+i) * index));   // заливка по палитре " от зелёного к красному"
        pixels[(NUMPIXELS+1)/2-i] = ColorFromPalette(myPal, ((count+i) * index));   // заливка по палитре " от зелёного к красному"
      }
      count+=0.3;
      time_for_rgb=millis();
      FastLED.show();
      return;
      break;
    case 4:
      if ((millis()-time_for_rgb)<50){return ;}
      signal();
      if (power_of_signal <= Rlenght){power_of_signal = Rlenght;}else{power_of_signal-=0.6;}
      Serial.println(power_of_signal);
      butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться
      FastLED.clear();
      for (int i = 0; i < power_of_signal; i++) {
        pixels[(NUMPIXELS+1)/2+i] = ColorFromPalette(RainbowColors_p, ((count+i) * index*0.75));   // заливка по палитре " от зелёного к красному"
        pixels[(NUMPIXELS+1)/2-i] = ColorFromPalette(RainbowColors_p, ((count+i) * index*0.75));   // заливка по палитре " от зелёного к красному"
      }      
      count+=0.3;
      time_for_rgb=millis();
      FastLED.show();
      return;
      break;
    case 5:
      if ((millis()-time_for_rgb)<50){return ;}
      signal();
      if (power_of_signal <= Rlenght){power_of_signal = Rlenght;}else{power_of_signal-=0.2;}
      Serial.println(power_of_signal);
      butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться
      FastLED.clear();
      for (int i = 0; i < power_of_signal*2; i++) {
        pixels[i] = ColorFromPalette(RainbowColors_p, ((count+i) * index*0.7));   // заливка по палитре " от зелёного к красному"
      }      
      count+=0.3;
      time_for_rgb=millis();
      FastLED.show();
      return;
      break;
  }
  FastLED.show();
  switch((millis()-time_for_rgb)/1000){
    case 0:
      r +=1*rk;
      if (r<0 ||r >255){rk=-rk;r+=2*rk;}
      g+=2*gk;
      if (g<0 || g >255){gk=-gk;g+=4*gk;}
      b+=3*bk;
      if (b<0 || b >255){bk=-bk;b+=6*bk;}

      r2 -=1*rk2;
      if (r2<0 ||r2 >255){rk2=-rk2;r2-=2*rk2;}
      g2-=2*gk2;
      if (g2<0 || g2 >255){gk2=-gk2;g2-=4*gk2;}
      b2-=3*bk2;
      if (b2<0 || b2 >255){bk2=-bk2;b2-=6*bk2;}
      
      break;
    case 1:
      r +=3*rk;
      if (r<0 ||r >255){rk=-rk;r+=6*rk;}
      g+=1*gk;
      if (g<0 || g >255){gk=-gk;g+=2*gk;}
      b+=2*bk;
      if (b<0 || b >255){bk=-bk;b+=4*bk;}

      r2 -=3*rk2;
      if (r2<0 ||r2 >255){rk2=-rk2;r2-=6*rk2;}
      g2-=1*gk2;
      if (g2<0 || g2 >255){gk2=-gk2;g2-=2*gk2;}
      b2-=2*bk2;
      if (b2<0 || b2 >255){bk2=-bk2;b2-=4*bk2;}

      break;
    case 2:
      r +=2*rk;
      if (r<0 ||r >255){rk=-rk;r+=4*rk;}
      g+=1*gk;
      if (g<0 || g >255){gk=-gk;g+=4*gk;}
      b+=3*bk;
      if (b<0 || b >255){bk=-bk;b+=6*bk;}

      r2 -=2*rk2;
      if (r2<0 ||r2 >255){rk2=-rk2;r2-=4*rk2;}
      g2-=1*gk2;
      if (g2<0 || g2 >255){gk2=-gk2;g2-=2*gk2;}
      b2-=3*bk2;
      if (b2<0 || b2 >255){bk2=-bk2;b2-=6*bk2;}

      break;
    case 3:
      r +=1*rk;
      if (r<0 ||r >255){rk=-rk;r+=2*rk;}
      g+=3*gk;
      if (g<0 || g >255){gk=-gk;g+=6*gk;}
      b+=2*bk;
      if (b<0 || b >255){bk=-bk;b+=4*bk;}

      r2 -=1*rk2;
      if (r2<0 ||r2 >255){rk2=-rk2;r2-=2*rk2;}
      g2-=3*gk2;
      if (g2<0 || g2 >255){gk2=-gk2;g2-=6*gk2;}
      b2-=2*bk2;
      if (b2<0 || b2 >255){bk2=-bk2;b2-=4*bk2;}

      break;
    case 4:
      r +=3*rk;
      if (r<0 ||r >255){rk=-rk;r+=6*rk;}
      g+=2*gk;
      if (g<0 || g >255){gk=-gk;g+=4*gk;}
      b+=1*bk;
      if (b<0 || b >255){bk=-bk;b+=2*bk;}

      r2 -=3*rk2;
      if (r2<0 ||r2 >255){rk2=-rk2;r2-=6*rk2;}
      g2-=2*gk2;
      if (g2<0 || g2 >255){gk2=-gk2;g2-=4*gk2;}
      b2-=1*bk2;
      if (b2<0 || b2 >255){bk2=-bk2;b2-=2*bk2;}

      break;
    /*case 5:
      r +=1*rk;
      if (r<0 ||r >255){rk=-rk;r+=2*rk;}
      g+=2*gk;
      if (g<0 || g >255){gk=-gk;g+=4*gk;}
      b+=3*bk;
      if (b<0 || b >255){bk=-bk;b+=6*bk;}
      break;*/
    default:
      time_for_rgb = millis();
      break;
  }
}




void display(int i=regim){
  if (i != regim){
    i = i%(count_images+1);
    i_for_display=0;
    regim=i;
  }
  if ((millis()-time_for_display)<600){return ;}
  if (regim<count_images){num_image=regim;}
  /*Serial.print(i);
  Serial.print(" ");
  Serial.print(num_image);
  Serial.print(" ");
  Serial.println(i_for_display);*/
  myOLED.clrScr();
  switch(num_image){
    case 0: 
      myOLED.invert(false);
      break;
    case 1:    
      myOLED.invert(true);
      myOLED.drawBitmap(0, 0, background_off, 128, 64);
      break;
    case 2: 
      myOLED.invert(false);
      break;
  };
  //Serial.print(sizeof(*Animations[num_image].cadrs));
  //Serial.print(" ");
  //Serial.println(i_for_display);
  myOLED.drawBitmap(Animations[num_image].x, Animations[num_image].y, (Animations[num_image].cadrs+(i_for_display*Animations[num_image].size_file)), Animations[num_image].width, Animations[num_image].height);
  //myOLED.drawBitmap(Animations[num_image].x, Animations[num_image].y, (loading[0]), Animations[num_image].width, Animations[num_image].height);
  myOLED.update();
  i_for_display = (i_for_display+1)%(Animations[num_image].len);
  time_for_display = millis();
  if (i_for_display==0 && regim==count_images){num_image=(num_image+1)%count_images;};
  return ;
}




void loop(){  
  butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться
  if (butt1.isHolded()) {     // кнопка удержана
        display(regim+1);
        Serial.println("press");
}else if (butt1.isClick())   {   
        Serial.println("click");   // если единичное нажатие
        rgb_line(1+regim_line);
  }
  rgb_line();
  display();
}
