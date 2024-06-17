// #include <Adafruit_VEML6070.h>
/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        3                10
 * IRQ         ?            ?             ?         ?          2                10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/
/*
 + Động cơ Servo:
  -3: nối với dây cam của động cơ servo    
 + Còi buzzer:
  -4: nối với chân dương của còi buzzer
 + Led:
  -2: nối với chân dương của led red
 + Keypad 4x4:
  -5: nối với chân hàng 1 của keypad 4x4
  -6: nối với chân hàng 2 của keypad 4x4
  -7: nối với chân hàng 3 của keypad 4x4
  -8: nối với chân hàng 4 của keypad 4x4
  -A2: nối với chân cột 1 của keypad 4x4
  -A1: nối với chân cột 2 của keypad 4x4  
  -A0: nối với chân cột 3 của keypad 4x4    
 */

#include <Keypad.h>                                                                         // Sử dụng thư viện Keypad để đọc dữ liệu từ bàn phím số


#include <Arduino.h>
// #include <Eeprom24Cxx.h>
#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#include <Servo.h>                                                                          // Sử dụng thư viện Servo để điều khiển Servo motor

LiquidCrystal_I2C lcd(0x27,16,2);

#define RST_PIN         9
#define SS_PIN          10
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo sg90;  

constexpr uint8_t led = 2;                                                               // Chân kết nối LED màu đỏ
constexpr uint8_t servoPin = 3;                                                             // Chân kết nối Servo motor
constexpr uint8_t buzzerPin = 4;                                                            // Chân kết nối buzzer
constexpr uint8_t emergencyButtonPin ;   // chan D0
int sensorPin = A3; //define analog pin A3 quang tro
int value = 0; //value of light dependent resister

char key_pressed = 0;                                                                       // Biến lưu giữ phím được nhấn trên bàn phím

unsigned long UID[4];
unsigned long i;
int diachi = 1; int diachi2 = 0; int a;
int gtmas = 0; int tong = 0; int gttong = 0;
int bandau= true;

char initial_password[4] = {'1', '2', '3', '3'};                                            // Mật khẩu khởi tạo
char password[4];                                                                           // Mật khẩu nhập từ bàn phím


int dem_tong = 0; int dem_menu = 0; int dem = 0; 

const byte rows = 4;                                                                        // Số hàng trên bàn phím
const byte columns = 4;                                                                     // Số cột trên bàn phím

char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};    

byte row_pins[rows] = {5, 6, 7, 8};   
byte column_pins[columns] = {A2, A1, A0};                                                      // Chân kết nối cột trên bàn phím

Keypad keypad_key = Keypad(makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);     // Khởi tạo đối tượng Keypad


int id_moi[4]; int id_er[4];
    int diachi_zero; 
    int m = 5; int moi = 0;
    int gt_zero;
    int gt_er = 0; 
    int n = 4; int o=0;
  int demco = 0; int demchua = 0; int demmas = 0;

void setup() 
{
  Serial.begin(9600);  
  SPI.begin();    
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  pinMode(led, OUTPUT);

  pinMode(emergencyButtonPin, INPUT_PULLUP); 
  pinMode(buzzerPin, OUTPUT);                                                               // Đặt chân buzzer là chế độ OUTPUT
  sg90.attach(servoPin);                                                                    // Kết nối Servo motor với chân điều khiển
  sg90.write(0);                                                                            // Đưa Servo motor về góc quay 0 độ ở vị trí ban đầu
  lcd.clear();                                                                              // Xóa nội dung trên màn hình LCD

  //in();
  //****
// for(int i=0; i<EEPROM.length(); i++)
// {
//   EEPROM.write(i,0);
// }
}
void manhinh()
{
  // lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Moi nhap the....");
  lcd.setCursor(0,1);
  lcd.print(" MK: ");  
}
void nhap_mas()
{
  lcd.setCursor(2,0);
  lcd.print("SETUP MASTER");
  lcd.setCursor(0,1);
  lcd.print("MOI NHAP THE....");
}

void menu_tong()
{
  if (dem_tong == 0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(">Quay lai");
    lcd.setCursor(0,1);
    lcd.print(" Them The");  
  }
  else if (dem_tong == 1)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" Quay lai");
    lcd.setCursor(0,1);
    lcd.print(">Them The");  
  }
  else if (dem_tong == 2)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(">Xoa the bat ky");
    lcd.setCursor(0,1);
    lcd.print(" Xoa tat ca the");  
  }
  else if (dem_tong == 3)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" Xoa the bat ky");
    lcd.setCursor(0,1);
    lcd.print(">Xoa tat ca the");  
  }    
  else if (dem_tong == 4){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(">Mo Cua");
  }
}

void chon_menu()
{
  if (dem_tong == 1) //Nhập thẻ mới
  {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Them The Moi");
    lcd.setCursor(4,1);
    lcd.print("Nhap The");     
  }
  else if (dem_tong == 2) //Xóa thẻ bất kỳ
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" Xoa The Bat Ky ");
    lcd.setCursor(4,1);
    lcd.print("Nhap The");     
  } 
  else if (dem_tong == 3) //XÓA TẤT CẢ THẺ
  {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Xoa Tat Ca The");    
  }     
  else if (dem_tong == 4){
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Cua dang mo");
  }
}

void tim_zero()
{
  while( n < a) //Tìm ô nhớ có GT 0 đầu tiên
    {
      gt_zero = EEPROM.read(n);
      if (gt_zero == 0)
      {
        diachi_zero = n;
        //Serial.print("Zero: "); Serial.print(diachi_zero); 
        break;
      }      
      //Serial.print("   n: "); Serial.println(n);
      n++;
    }
  //Serial.println(".....................");
  if(diachi_zero==0)
  {
    diachi_zero = a; //Nếu trong đoạn từ 4 đến số ô đã sử dụng không có ô nào có GT 0
                    // thì diachi_zero = a là ô cuối cùng lưu thẻ
    n=0;
  }

}

void ss_epprom()
{
    while(m < a) //chạy từ ô 5 đến ô sử dụng cuối ... 5 < 20
    {
      moi = m + 4; //moi = 9
      for(m; m<moi; m++)
      {
        gt_er = EEPROM.read(m);
        // Serial.print("m: "); Serial.print(m);
        // Serial.print("   moi: "); Serial.print(moi);
        // Serial.print("   GT eep: "); Serial.print(gt_er);
        // o=0
        if(o<=3) //Lưu giá trị đọc từ eeprom vào id_er
        {
          id_er[o]=gt_er;
          // Serial.print("   id eeprom: "); Serial.print(id_er[o]);
          // Serial.print("   o: "); Serial.println(o);
        }
        o++;
      }   
      // Serial.print("$$$$$$$$$$ o: "); Serial.println(o);
      if(id_moi[0] == id_er[0] && id_moi[1] == id_er[1] && id_moi[2] == id_er[2] && id_moi[3] == id_er[3]) //Nếu thẻ có trong EEPR
      {
        demco = 1; demchua=0; demmas=0;
        //Serial.print("   demco: "); Serial.println(demco);
        break;
      }
      else if(id_moi[0] != id_er[0] or id_moi[1] != id_er[1] or id_moi[2] != id_er[2] or id_moi[3] != id_er[3]) //Nếu thẻ KO có trong EEPR
      { // Dùng or vì chỉ cần  1 byte khác với 1 byte lưu trong eeprom thì thẻ đó là thẻ khác
        // Nếu dùng && có thể có thẻ sẽ trùng 1 đến 2 byte
        demchua += 1; demco=0; demmas=0;
        //Serial.print("   demchua: "); Serial.println(demchua);
      } 
      if(id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2) && 
              id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //THẺ mới nhập là MASTER
      {
        demmas=1; demco=0; demchua=0;
      }           
      o = 0;
      m = moi;
    }  
}
{
  for(int g = 0; g < 30; g++)
  {
    Serial.print(g);Serial.print("= "); Serial.print(EEPROM.read(g)); Serial.print(" ");
  }
  Serial.println(" ");
}

void beep() {
  digitalWrite(buzzerPin, HIGH);                                                            // Kích hoạt buzzer
  delay(300);                                                                               // Delay 300ms
  digitalWrite(buzzerPin, LOW);                                                             // Tắt buzzer
}
void light_nigh(){
  if(value >900){
    digitalWrite(led, HIGH);
  }
  else{
    digitalWrite(led, LOW);
  }
}

void loop() 
{ 
  value = analogRead(sensorPin); 
  light_nigh();
  if(bandau == true) //Kiểm tra có thẻ MAS chưa
{light_nigh();
  for(int j = 1; j < 4; j++) // đọc từ 1 - 4 xem trong ô nhớ có gì ko, Nếu không là chưa có thẻ MAS
  {
      gtmas = EEPROM.read(j);
      gttong = gttong + gtmas;
      //Serial.print("Gtmas: "); Serial.print(gtmas); Serial.print("  ");
      //Serial.print("Tổng: "); Serial.print(tong); Serial.print("  ");
  } 
}
if (gttong == 0) //Chưa có thẻ MAS
{light_nigh();
  tong = 0;
  if (tong == 0 && dem == 0)//Nếu ô 1, 2, 3, 4 chưa có Mas thì nhập Mas
  { 
    nhap_mas();

    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++) 
    { 
          // Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " " :" "); 
          // Serial.print(mfrc522.uid.uidByte[i], HEX);         
      UID[i] = mfrc522.uid.uidByte[i];

      EEPROM.write(diachi, UID[i]); //bắt đầu lưu từ ô 1-4
      diachi = diachi + 1;
    }
    mfrc522.PICC_HaltA();  
    mfrc522.PCD_StopCrypto1();
    delay(50);
    lcd.clear();   
  }
} 
else // Đã có thẻ MAS
{light_nigh();
  tong = 1; bandau = false;
  if (tong == 1 && dem == 0) // Đã có MASTER
  { 
    manhinh();
    key_pressed = keypad_key.getKey();
    if (key_pressed) {
      password[i++] = key_pressed;
      lcd.setCursor(i+5, 1);
      lcd.print('*');
      delay(300);
                                                                          
    }
    if (i == 4) {
      delay(200);
      if (!(strncmp(password, initial_password, 4))) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Dung Mat Khau ");
        lcd.setCursor(4,1);
        lcd.print("Khan cap");
        sg90.write(180);                                                                      // Mở khóa bằng cách đưa Servo motor về góc quay 180 độ
        beep();
        delay(500);
        beep();
        delay(500);
        beep();
        delay(6000);
        sg90.write(0); 
        i = 0;
        lcd.clear();
       
      }
      else {
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("    Sai Mat Khau       "); 
          beep();
          delay(300);
          beep();
          delay(300);
          beep();
          delay(3000);
          i = 0; 
          lcd.clear(); 
      }
    }

    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++) 
      {        
        UID[i] = mfrc522.uid.uidByte[i];
        id_moi[i] = UID[i];
      }
    mfrc522.PICC_HaltA();  
    mfrc522.PCD_StopCrypto1();

    if (id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2)
        && id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //Check xem phải thẻ MAS ko
    {
      dem = 1;
      if (tong == 1 && dem == 1) //Để thẻ MASTER để vào vào menu
      {
        menu_tong();
      }   
    }
    else //Nếu không phải thẻ MASTER
    {light_nigh();
      ss_epprom(); //So sánh thẻ mới với EEPROM
      if(demco == 1 && demchua == 0 && demmas == 0) //Nếu thẻ có trong EEPROM
        {
          o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
          lcd.setCursor(0,1);
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("Cua dang mo");

        sg90.write(180);                                                                      // Mở khóa bằng cách đưa Servo motor về góc quay 180 độ
        beep();
        delay(6000);
        sg90.write(0);
          // digitalWrite(led, LOW); 
          lcd.clear();
        }  
      else if(demchua > 0 && demco == 0 && demmas == 0) //Nếu thẻ chưa có trong EEPROM
        {
          // digitalWrite(led, LOW);
          o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
          lcd.setCursor(0,1);
          lcd.print("    SAI THE!    "); 
          digitalWrite(buzzerPin, HIGH);                                                            // Kích hoạt buzzer
          delay(2000);                                                                               // Delay 300ms
          digitalWrite(buzzerPin, LOW);                                                             // Tắt buzzer 
          lcd.clear();        
        } 
    }
  }  
}
if (dem_menu == 1 && dem_tong == 1 && dem == 1) //Nếu chọn THÊM THẺ MỚI
{light_nigh();   
  a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  //Serial.print("a: "); Serial.println(a); 

  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  {
    int diachi_phu = 5; // 5,6,7,8
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {          
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1(); 
    if(id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2) && 
        id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //THẺ mới nhập là MASTER  
    {
      lcd.clear();
      menu_tong();
      dem_menu = 0;      
    }
    
    else //THẺ MỚI KHÔNG PHẢI LÀ THẺ MASTER thi LƯU vào ô 5,6,7,8
    { 
      for (byte i = 0; i < 4; i++)
      {
        EEPROM.write(diachi_phu, id_moi[i]);
        diachi_phu = diachi_phu + 1;
        a = diachi_phu;
      }
      EEPROM.write(0, a); //Sau khi lưu 1 thẻ mới vào thì cập nhật số ô nhớ đã sử dụng vào ô 0
      lcd.setCursor(0,1);
      lcd.print("   Dang luu...  "); 
      delay(1000); 
      lcd.clear();
      chon_menu();       
    }  
  }
  else if( a != 0) // Đã có 1 or nhiều thẻ phụ
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {          
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();  

    ss_epprom(); //So sánh thẻ mới đưa vào với Eeprom
    
    if(demco == 1 && demchua == 0 && demmas == 0) //Nếu thẻ đã có
    {
        // Serial.print("................THE DA CO TRONG EEPROM..................");
        // Serial.print(" Zero: "); Serial.print(diachi_zero);
        // Serial.print("   Demco: "); Serial.println(demco);           
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.setCursor(0,1);
      lcd.print("   The da co!   "); 
      delay(1000); 
      lcd.clear();
      chon_menu(); 
      //in();
    }
    else if(demchua > 0 && demco == 0 && demmas == 0) //Nếu thẻ chưa có...THÌ LƯU THẺ ĐÓ VÀO EEPROM
    {
      tim_zero(); //Tìm vị trí 0 đầu tiên
      //   Serial.print("................THE CHUA CO..................");
      //   Serial.print(" Zero: "); Serial.print(diachi_zero);
      //   Serial.print("   Demchua: "); Serial.print(demchua); 
      // Serial.print("  ID mới: ");Serial.print(id_moi[0]);Serial.print(":");Serial.print(id_moi[1]);Serial.print(":");
      // Serial.print(id_moi[2]);Serial.print(":");Serial.println(id_moi[3]);
      if(diachi_zero == a) //Nếu trong đoạn từ 4 đến số ô đã sử dụng không có ô nào có GT 0
                           // a là ô cuối cùng lưu thẻ       
      {
        for(int i=0; i<4; i++)
        {
          EEPROM.write(diachi_zero, id_moi[i]);
          diachi_zero = diachi_zero + 1;
          a = diachi_zero;
        }
        EEPROM.write(0, a); //Sau khi lưu 1 thẻ mới vào thì cập nhật số ô nhớ đã sử dụng vào ô 0
      }

      else if(diachi_zero == n) ////Nếu trong đoạn từ 4 đến số ô đã sử dụng CÓ ô = 0, thì gán ô đó vào n
      {
        for(int i=0; i<4; i++) //Lưu thẻ mới vào bắt đầu từ ô 0 đó
        {
          EEPROM.write(diachi_zero, id_moi[i]);
          diachi_zero = diachi_zero + 1;
          //a = diachi_zero;
        }        
        diachi_zero=a;
      }
      tim_zero();
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.setCursor(0,1);
      lcd.print("   Dang luu...  "); 
      delay(1000); 
      lcd.clear();
      chon_menu();     
      //in();    
    }
    if(demmas == 1 && demco == 0 && demchua == 0) //THẺ MASTER thì quay về menu
    {light_nigh();
      //   Serial.print("................THE NAY LA THE MASTER..................");
      //   Serial.print(" Zero: "); Serial.print(diachi_zero);
      //   Serial.print("   Demchua: "); Serial.print(demchua); 
      // Serial.print("  ID mới: ");Serial.print(id_moi[0]);Serial.print(":");Serial.print(id_moi[1]);Serial.print(":");
      // Serial.print(id_moi[2]);Serial.print(":");Serial.println(id_moi[3]);
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.clear();
      menu_tong();
      dem_menu = 0; 
      //in();            
    }
  }
}
else if (dem_menu == 1 && dem_tong == 2 && dem == 1) //Nếu chọn XÓA THẺ BẤT KỲ
{light_nigh();
  a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  //Serial.print("a: "); Serial.println(a); 

  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  {
    lcd.setCursor(0,1);
    lcd.print(" CHUA CO THE... ");
    delay(1000);
    lcd.clear();
    menu_tong();
    dem_menu = 0;     
  }
  else //Nếu đã có 1 hoặc nhiều thẻ PHỤ
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {          
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();  
    ss_epprom();
    if(demco == 1 && demchua == 0 && demmas == 0) //thẻ có trong EEPROM
    {
      tim_zero();
        //Serial.print("................THE DA CO TRONG EEPROM..................");
      m = m - 4;
        // Serial.print(" m: "); Serial.print(m);
        // Serial.print("   Zero: "); Serial.print(diachi_zero);
        // Serial.print("   Demco: "); Serial.println(demco); 
      for(int i=0; i<4; i++)
      {
        EEPROM.write(m, 0);
        //Serial.print(" m: "); Serial.print(m);
        m = m + 1; 
        //Serial.print(" m: "); Serial.print(m);
      }
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0; n=4;
      lcd.setCursor(0,1);
      lcd.print(" Da xoa the.... "); 
      delay(1000); 
      lcd.clear();
      chon_menu();
      tim_zero();
      //in();      
    }
    else if (demchua > 0 && demco == 0 && demmas == 0) //Thẻ chưa có trong EEPROM
    {      
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0; 
      lcd.setCursor(0,1);
      lcd.print(" The chua co... "); 
      delay(1000); 
      lcd.clear();
      chon_menu();  
      //in();      
    }
    if(demmas == 1 && demco == 0 && demchua == 0) //Là thẻ MASTER
    {
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.clear();
      menu_tong();
      dem_menu = 0;
      //in();
    }
  }
}
else if (dem_menu == 1 && dem_tong == 3 && dem == 1) //Chọn XÓA TẤT cả thẻ
{light_nigh();
  a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  {
    lcd.setCursor(0,1);
    lcd.print(" Chua co the... ");
    dem_menu = 0;
    delay(2000);
    lcd.clear();
    menu_tong();
  }
  else if (a > 0) //Nếu đã có 1 hoặc nhiều thẻ PHỤ
  {  
    lcd.setCursor(0,1);
    lcd.print("  Dang xoa....  "); 
    for (int diachi = 5; diachi<a; diachi++) //Bắt đầu từ ô 5 đến ô đang sử dụng
    {
      EEPROM.write(diachi, 0);
    }  
    EEPROM.write(0, 0); //Sau đó cập nhật vào ô 0 là 0 vì đã xóa hết thẻ   
    delay(3000);
    lcd.clear();
    menu_tong();
    dem_menu = 0;
  } 
  diachi_zero = 0; n=4; a=0;
  tim_zero();
  //in(); 
}
else if (dem_menu == 1 && dem_tong == 4 && dem == 1) //mo cua
{light_nigh();
    sg90.write(180);                                                                      // Mở khóa bằng cách đưa Servo motor về góc quay 180 độ
    beep();
    delay(6000);
    sg90.write(0);
    lcd.clear();
    menu_tong();
    dem_menu = 0;
}

  key_pressed = keypad_key.getKey();    

  if(key_pressed == '*')
  {
    if (dem == 1) //Lên ở menu
    {      
      if (dem_tong <= 0)
        dem_tong = 4;
      else 
        dem_tong --;
      menu_tong();
    }
  delay(100);
  }

  if(key_pressed == '0')
  {
    if (dem == 1) //Xuống ở menu
    {  
      if(dem_tong >= 4)
        dem_tong = 0;
      else 
        dem_tong ++;
      menu_tong();
    }
  delay(100);
  }


  if(key_pressed == '#')
  {
    if (dem == 1 && dem_tong != 0) //Chọn menu
    {
      chon_menu();
      dem_menu = 1;
    }
    else if (dem == 1 && dem_tong == 0) //Nhấn nút BACK để về màn hình
    {
      lcd.clear();
      manhinh();
      dem = 0;
    }    
  delay(100);
  } 
}
