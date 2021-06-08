#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 1024

#include <ESP8266WiFi.h>
#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = D3;     
constexpr uint8_t SS_PIN = D4;     

MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key;

WidgetLCD lcd(V2);

String tag;
const int priceOfItem1=20;              //Chips
const int priceOfItem2=30;              //Milk
int noOfItem1=0;
int noOfItem2=0;
int noOfTotalItems=0;
boolean currentItem1Flag=false;
boolean currentItem2Flag=false;
int totalBill=0;
String previousInput;
int isRemoving=0;

const char auth[] = "YourAuthToken";
const char ssid[]="Your wifi name";
const char pass[]="your password";

//for removal of item
BLYNK_WRITE(V5){                    
    int value=param.asInt();
    if(value){
      isRemoving=1;
      currentItem1Flag=false;
      currentItem2Flag=false;
      }
}

// exit(final billing)
BLYNK_WRITE(V6){
  int value = param.asInt();  
                    
   if(value && noOfTotalItems>0){
        String response=String("<pre>Thank you for shopping with us.This is System Generated Bill.</pre><hr/><table border=\"1\" width=\"80%\" align=\"center\" cellspacing=\"0\"><tbody><tr \"background:#04c0f8;\"><th>Product</th><th>Quantity</th><th>Price</th></tr>");
  if(noOfItem1>0){
    int item1Total=noOfItem1*priceOfItem1;
    response+="<tr style=\"text-align:center;\"><td>Chips</td><td>"+String(noOfItem1)+"</td><td>"+String(item1Total)+"</td></tr>";
    }
   if(noOfItem2>0){
    int item2Total=noOfItem2*priceOfItem2;
    response+="<tr style=\"text-align:center;\"><td>Milk</td><td>"+String(noOfItem2)+"</td><td>"+String(item2Total)+"</td></tr>";
    }
    response+="<tr><td colspan=\"3\" style=\"text-align:right;padding-right:2rem;\">Total Amount = "+String(totalBill)+"</td></tr></tbody></table>";  
  Blynk.email("Subject: Smart Trolley Bill",response);
  delay(1000);
  Blynk.notify("Email has been sent.");
  
  noOfItem1=0;
  noOfItem2=0;
  noOfTotalItems=0;
  currentItem1Flag=false;
  currentItem2Flag=false;
  totalBill=0;
  lcd.clear();
  lcd.print(2,0,"SMART TROLLEY");
  delay(1000);
  lcd.print(0,1,"Scan To Add Item");
   }
  if(value && noOfTotalItems==0){
      lcd.clear();
      lcd.print(0,0,"No Items In Cart");
      delay(2000);
    }
}

//increment current item
BLYNK_WRITE(V3){                      
  int value=param.asInt();
  if(value && isRemoving==0){
      if(currentItem1Flag){
        totalBill = totalBill + priceOfItem1;
        noOfItem1++;
        lcd.clear();
        lcd.print(0,0,"Chips Quantity.");
        lcd.print(0,1,"Increased to: "+String(noOfItem1));
        delay(2000);
  }
  else if(currentItem2Flag){
        totalBill = totalBill + priceOfItem2;
        noOfItem2++;
        lcd.clear();
        lcd.print(0,0,"Milk Quantity.");
        lcd.print(0,1,"Increased to: "+String(noOfItem2));
        delay(2000);
  }else{
      lcd.clear();
      lcd.print(0,0,"No Item present");
      lcd.print(0,1,"in the Cart");
      delay(2000);
    }
   defaultMessage();
    }
}

//decrement current
BLYNK_WRITE(V4){                    
  int value=param.asInt();
  if(value && isRemoving==0){
    if(currentItem1Flag){
      if(noOfItem1>0){
          totalBill = totalBill - priceOfItem1;
          noOfItem1--;
          lcd.clear();
          lcd.print(0,0,"Chips Quantity.");
          lcd.print(0,1,"Decreased to: "+String(noOfItem1));
          delay(2000);
        }else{
            lcd.clear();
            lcd.print(0,0,"Chips Quantity...");
            lcd.print(0,1,"Already 0");
            delay(2000);
          }
  }
  else if(currentItem2Flag){
      if(noOfItem2>0){
          totalBill = totalBill - priceOfItem2;
          noOfItem2--;
          lcd.clear();
          lcd.print(0,0,"Milk Quantity.");
          lcd.print(0,1,"Decreased to: "+String(noOfItem2));
          delay(2000);
        }else{
            lcd.clear();
            lcd.print(0,0,"Milk Quantity...");
            lcd.print(0,1,"Already 0");
            delay(2000);
          }
  }else{
      lcd.clear();
      lcd.print(0,0,"No Item present");
      lcd.print(0,1,"in the Cart");
      delay(2000);
    }
    defaultMessage();
    }
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth,ssid,pass);
  SPI.begin(); 
  rfid.PCD_Init();
  lcd.clear();
  lcd.print(2,0,"SMART TROLLEY");
  delay(2000);
  lcd.print(0,1,"Scan To Add Item");
  }

void loop() {
  Blynk.run();
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
  if(tag!=previousInput){
    currentItem1Flag=false;
    currentItem2Flag=false;
    }
    if ((tag=="1656920427") && (isRemoving==0))
      {
        if(noOfItem1==0){
          lcd.clear();
          lcd.print(0,0,"Chips Added.");
          lcd.print(0,1,"Price(Rs): 20");
          totalBill = totalBill + priceOfItem1;
          noOfItem1++;
          noOfTotalItems++;
          currentItem1Flag=true;
          previousInput=tag;
          delay(2000);
          }
      }
    else if ((tag=="611622082") && (isRemoving==0))
      {
        if(noOfItem2==0){
          lcd.clear();
          lcd.print(0,0,"Milk Added.");
          lcd.print(0,1,"Price(Rs): 30");
          totalBill = totalBill + priceOfItem2;
          noOfItem2++;
          noOfTotalItems++;
          currentItem2Flag=true;
          previousInput=tag;
          delay(2000);
          }
      }
     else if ((tag== "1656920427") && (isRemoving==1))
      {
        if(noOfItem1>0){
        lcd.clear();
        lcd.print(0,0,"Chips Removed...");
        totalBill = totalBill - (noOfItem1*priceOfItem1);
        noOfItem1=0;
        noOfTotalItems--;
        }else{
            lcd.clear();
            lcd.print(0,0,"No Chips In Cart");
          }
        isRemoving=0;
        delay(2000);
      }
    else if ((tag == "611622082") && (isRemoving==1))
      {
        if(noOfItem2>0){
           lcd.clear();
        lcd.print(0,0,"Milk Removed...");
        totalBill = totalBill - (noOfItem2*priceOfItem2);
        noOfItem2=0;
        noOfTotalItems--;
        }else{
            lcd.clear();
            lcd.print(0,0,"No Milk In Cart");
          }
        isRemoving=0;
        delay(2000);
      }
    defaultMessage(); 
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void defaultMessage(){
  lcd.clear();
lcd.print(0,0,"Add Items...");
lcd.print(0,1,"Items:"+String(noOfTotalItems)+" Total:"+String(totalBill));
}
