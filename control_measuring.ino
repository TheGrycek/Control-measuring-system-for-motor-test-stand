
#include "HX711.h"
#include <LiquidCrystal.h>

#define DOUT  8
#define CLK  13
#define C1 12             //zdefiniowanie uzwojeń
#define C2 11
#define C3 10
#define C4 9
#define reczny 19         //zdefiniowanie przycisków
#define odjedz 18
#define zatwierdz 17
#define krancowka 15


LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

HX711 scale(DOUT, CLK);
 
float calibration_factor = 461000;
float sila;
float nastawa;
int t1=4;                               //czas dla posuwu przy z<<1 lub z>>1
int t2=500;                             //czas dla posuwu przy z>0.9 lub z<1.1
int p;                                //zmienna ilości wykonanych0 pętli po 4 kroki
int n;                                 //ilość pętli przy uzyskaniu zadanej siły
int polozenie_p=1;                     // początkowe położenie dla sterownia półkrokowego
int polozenie_l=1;
bool manual;
bool ok;
bool set0;
bool koniec;

void setup() {
  pinMode(C1, OUTPUT);                 
  pinMode(C2, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(C4, OUTPUT);
  pinMode(A0,INPUT); 
  pinMode(reczny, INPUT);         //A5
  pinMode(odjedz,INPUT);          //A4
  pinMode(zatwierdz,INPUT);       //A3
  pinMode(krancowka, INPUT);      //A2

  digitalWrite(reczny,HIGH);
  digitalWrite(odjedz,HIGH);
  digitalWrite(zatwierdz,HIGH);
  digitalWrite(krancowka,HIGH);

  manual=false;
  ok=false;
  set0=false;
  koniec=false;
  n=0;
  
  lcd.begin(16,2);
  scale.set_scale();
  scale.tare();
  }

void pomiar()
{
 scale.set_scale(calibration_factor);
 sila=scale.get_units()*9.81;
 
 if(sila<0)
 sila=0;
 
 lcd.setCursor(0, 0);
 lcd.print("Sila:");
 lcd.setCursor(5,0); 
 lcd.print(sila);
 lcd.setCursor(11,0);
 lcd.print("N");   
}

void nastawa_pot()
{
 nastawa =analogRead(A0)*32/1023.00; //odczyt z potencjometru- nastawa od 0 do 32, dokładność do 0.01
 lcd.setCursor(0, 1);
 lcd.print("Nastawa:");
 lcd.setCursor(8,1);
 lcd.print(nastawa);
 lcd.setCursor(13,1);
 lcd.print("N");
 delay(30);
  }
  
void loop() {
  
  lcd.clear();
  
  if(digitalRead(krancowka)==LOW)
  {
   koniec=true;
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW); 

   ok=false;
   set0=false;
  }
  else
  koniec=false;

 if(digitalRead(reczny)==LOW)
  {
    manual=!manual;
    delay(200);
    ok=false;
    set0=false;
    }
 
 if(manual==true)
  {
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW);
   
  pomiar();
  
  lcd.setCursor(0, 1);
  lcd.print("Posuw reczny");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
  delay(30);
  }  

 else if(manual==false)
  {

  pomiar();
  nastawa_pot(); 

  }

 if(digitalRead(zatwierdz)==LOW)
  {
    ok=!ok;
    delay(200);
    set0=false;
    }

 if(digitalRead(odjedz)==LOW)
  {
    set0=!set0;
    delay(200);
    ok=false;
    }
    
 if(sila<0.1)
  {
    set0=false; 
    }  

 if(set0==false & ok==false)
 {
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW);
  }
    
 if(set0==true & manual==false)
  {    
    int p=12;
    for(int i; i<=p; i++)
    {
   digitalWrite(C4, LOW);
   digitalWrite(C1, HIGH);
   digitalWrite(C2, HIGH);
   delay(t1);
   digitalWrite(C1, LOW);
   digitalWrite(C3, HIGH);
   delay(t1);
   digitalWrite(C2, LOW);
   digitalWrite(C4, HIGH);
   delay(t1);
   digitalWrite(C3, LOW);
   digitalWrite(C1, HIGH);
   delay(t1);
   }
  }

  float z= nastawa/sila;              //zmienna z do określenia posuwu w zależności od odległości od zadanej siły
  if(((z>2) | (z<0.05)) & sila<3 )
 {
    p=12;
 }
  
 if(((z>1.5 & z<=2) | (z<0.5 & z>=0.05)) & (sila>=3 & sila<8))
 { 
    p=6;
 }
 else if(((z<=1.3 & z>1.2) | (z>=0.7 & z<0.8)) | sila>=8)
 {   
    p=1;
 }
  
  float blad_p=nastawa+0.06;         // zadane granicy błędu
  float blad_l=nastawa-0.06;  

  if (sila>=blad_p & z>=0.8 & manual==false & ok==true)         //posuw dla siły bliskiej zadanej - pojedyncze kroki
  {
  if (polozenie_p>8)
  polozenie_p=1;

  switch (polozenie_p)
  {
   case 1:
   digitalWrite(C1, HIGH);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW);
   delay(t1);
   break;
   
   case 2:
   digitalWrite(C1, HIGH);
   digitalWrite(C2, HIGH);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW);
   delay(t1);
   break;
   
   case 3:
   digitalWrite(C1, LOW);
   digitalWrite(C2, HIGH);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW);
   delay(t1);
   break;

   case 4:
   digitalWrite(C1, LOW);
   digitalWrite(C2, HIGH);
   digitalWrite(C3, HIGH);
   digitalWrite(C4, LOW);
   delay(t1);
   break;

   case 5:
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, HIGH);
   digitalWrite(C4, LOW);
   delay(t1);
   break;

   case 6:
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, HIGH);
   digitalWrite(C4, HIGH);
   delay(t1);
   break;

   case 7:
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, HIGH);
   delay(t1);
   break;

   case 8:
   digitalWrite(C1, HIGH);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, HIGH);
   delay(t1);
   break;
  }
  polozenie_p++;  
  }

  if (sila<=blad_l & (z<=1.2 | sila>=12) & koniec==false & manual==false & ok==true & koniec==false)
  {
  if (polozenie_l>8)
  polozenie_l=1;

  switch (polozenie_l)
  {
   case 1:
   digitalWrite(C1, HIGH);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW);
   delay(t1);
   break;
   
   case 2:
   digitalWrite(C1, HIGH);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, HIGH);
   delay(t1);
   break;
   
   case 3:
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, HIGH);
   delay(t1);
   break;

   case 4:
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, HIGH);
   digitalWrite(C4, HIGH);
   delay(t1);
   break;

   case 5:
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, HIGH);
   digitalWrite(C4, LOW);
   delay(t1);
   break;

   case 6:
   digitalWrite(C1, LOW);
   digitalWrite(C2, HIGH);
   digitalWrite(C3, HIGH);
   digitalWrite(C4, LOW);
   delay(t1);
   break;

   case 7:
   digitalWrite(C1, LOW);
   digitalWrite(C2, HIGH);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW);
   delay(t1);
   break;

   case 8:
   digitalWrite(C1, HIGH);
   digitalWrite(C2, HIGH);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW);
   delay(t1);
   break;
  }
  polozenie_l++; 
  }

  if(sila>blad_p & z<0.8 & manual==false & ok==true)                 //posuw dla siły mocno różniącej się od zadanej 4kroki*p
  {
    for(int i; i<=p; i++)
    {
   digitalWrite(C4, LOW);
   digitalWrite(C1, HIGH);
   digitalWrite(C2, HIGH);
   delay(t1);
   digitalWrite(C1, LOW);
   digitalWrite(C3, HIGH);
   delay(t1);
   digitalWrite(C2, LOW);
   digitalWrite(C4, HIGH);
   delay(t1);
   digitalWrite(C3, LOW);
   digitalWrite(C1, HIGH);
   delay(t1);
   }
   }

   else if(sila<blad_l & z>1.2 & sila<12 & koniec==false & manual==false & ok==true & koniec==false)
  {
    for(int i; i<=p; i++)
    {
   digitalWrite(C1, LOW);
   digitalWrite(C3, HIGH);
   digitalWrite(C4, HIGH);
   delay(t1);
   digitalWrite(C4, LOW);
   digitalWrite(C2, HIGH);
   delay(t1);
   digitalWrite(C3, LOW);
   digitalWrite(C1, HIGH); 
   delay(t1);
   digitalWrite(C2, LOW);
   digitalWrite(C4, HIGH);
   delay(t1);

  }
  }
  
  else if (sila<=blad_p & sila>=blad_l & manual==false)  //zatrzymanie posuwu przy osiągnięciu wartości zadanej w granicach błędu
  {

   pomiar();
   nastawa_pot();
    
   digitalWrite(C1, LOW);
   digitalWrite(C2, LOW);
   digitalWrite(C3, LOW);
   digitalWrite(C4, LOW); 

  n++;

  }
  if(n>=60)
  {
  ok=false;  
  n=0;
  }
}
