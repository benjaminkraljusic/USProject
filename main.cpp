 #include "mbed.h"
 #include "stm32f413h_discovery_ts.h"
 #include "stm32f413h_discovery_lcd.h"
 #include <vector>
 #include "easy-connect.h"
 #include "MQTTNetwork.h"
 #include "MQTTmbed.h"
 #include "MQTTClient.h"
 #include <string>
 #include <sstream>

 #define TEMAPUBBROJNIK "ugradbeni/brojnik"
 #define TEMAPUBNAZIVNIK "ugradbeni/nazivnik"
 #define TEMAPUBGRAF "ugradbeni/brojGrafa"


 #define MQTTCLIENT_QOS2 0
 
int arrivedcount = 0;

char* str;
 
 TS_StateTypeDef TS_State = { 0 };
 std::vector<double> nizNula, nizPolova;
 int StepenBrojnika, StepenNazivnika;
 
int dajCifru(int x1, int y1);
void crtajTastaturu();
void ispisNuleTekst();
void ispisPoloviTekst();


void IspisZaStepenBrojnikaINazivnika(bool T) { //Od T zavisi je li brojnik ili nazivnik.
     BSP_LCD_Clear(LCD_COLOR_WHITE);
     BSP_LCD_SetTextColor(6591981);
     BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 40);
     BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
     BSP_LCD_SetBackColor(6591981);
     BSP_LCD_SetFont(&Font12);
     if(!T) BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)"Stepen polinoma u brojniku ", LEFT_MODE);
     else BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)"Stepen polinoma u nazivniku ", LEFT_MODE);
     BSP_LCD_DisplayStringAt(0, 23, (uint8_t *)"(unos potvrdite sa ENTER): ", LEFT_MODE);
     BSP_LCD_DrawRect(BSP_LCD_GetXSize()/2 - 12, 80, 19, 29);
     crtajTastaturu();
}

void CrtajOkvirove(bool T) { //Crta okvir, tacnije mjesta predvidjena za cifre
    int a = 80;
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    if(!T) ispisNuleTekst();
    else ispisPoloviTekst();
    crtajTastaturu();
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(a + 7, 105, (uint8_t *)"-", LEFT_MODE);
    BSP_LCD_DisplayStringAt(a + 31, 105, (uint8_t *)"-", LEFT_MODE);
    BSP_LCD_DisplayStringAt(a + 60, 105, (uint8_t *)"-", LEFT_MODE);
    BSP_LCD_DrawRect(a, 95, 19, 29);
    BSP_LCD_DrawRect(a + 24, 95, 19, 29);
    BSP_LCD_FillCircle(a + 48, 121, 2);
    BSP_LCD_DrawRect(a + 53, 95, 19, 29);
    
}

int StepenBrojnikaIliNazivnika(bool T) {//Stepen polinoma u brojniku ili nazivniku.
    // Za T = 0 se unosi brojnik, a za T = 1 se unosi nazivnik.
     IspisZaStepenBrojnikaINazivnika(T);
     char cifra[2];
     int stepen = -1; //Magicna konstanta. Stepen brojnika/nazivnika svakako nece biti negativan.
     while(1) {
         BSP_TS_GetState(&TS_State);
         if(TS_State.touchDetected) {
            uint16_t x1 = TS_State.touchX[0];
            wait_ms(200);
            uint16_t y1 = TS_State.touchY[0];
            //wait_ms(200);
            if(dajCifru(x1, y1) >= 0 && dajCifru(x1, y1) <= 5) { //Stepen pripada [0, 4]
                stepen = dajCifru(x1, y1);
                cifra[0] = stepen + 48;
                BSP_LCD_SetFont(&Font24);
                BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
                BSP_LCD_DisplayStringAt(BSP_LCD_GetXSize()/2 - 10, 85, (uint8_t *) cifra,LEFT_MODE);
            }
            if(dajCifru(x1, y1) == 10 && stepen != -1) //Ne radi nista ako je enter pritisnut prije unosa cifre
                return stepen;
        }
         wait_ms(100);
     }
}

 void ispisNuleTekst()//Unesite koeficijente polinoma u brojniku
 {
     BSP_LCD_Clear(LCD_COLOR_WHITE);
     BSP_LCD_SetTextColor(6591981);
     BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 40);
     BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
     BSP_LCD_SetBackColor(6591981);
     BSP_LCD_SetFont(&Font16);
     BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)"Unesite koeficijente", LEFT_MODE);
     BSP_LCD_DisplayStringAt(0, 23, (uint8_t *)"za nule: ", LEFT_MODE);
     crtajTastaturu();
 }
 void ispisPoloviTekst()//Unesite koeficijente polinoma u nazivniku
 {
     BSP_LCD_SetTextColor(6591981);
     BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 40);
     BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
     BSP_LCD_SetBackColor(6591981);
     BSP_LCD_SetFont(&Font16);
     BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)"Unesite koeficijente", LEFT_MODE);
     BSP_LCD_DisplayStringAt(0, 23, (uint8_t *)"za polove: ", LEFT_MODE);
 }
 
 void crtajTastaturu()
 {
     BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
     BSP_LCD_FillRect(0,150,BSP_LCD_GetXSize(), 100);
     BSP_LCD_SetFont(&Font16);
     BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
     BSP_LCD_FillRect(5,155,25,25);
     BSP_LCD_DisplayStringAt(11, 158, (uint8_t *)"0", LEFT_MODE);
     BSP_LCD_FillRect(35,155,25,25);
     BSP_LCD_DisplayStringAt(41, 158, (uint8_t *)"1", LEFT_MODE);
     BSP_LCD_FillRect(65,155,25,25);
     BSP_LCD_DisplayStringAt(71, 158, (uint8_t *)"2", LEFT_MODE);
     BSP_LCD_FillRect(95,155,25,25);
     BSP_LCD_DisplayStringAt(101, 158, (uint8_t *)"3", LEFT_MODE);
     BSP_LCD_FillRect(125,155,25,25);
     BSP_LCD_DisplayStringAt(132, 158, (uint8_t *)"4", LEFT_MODE);
     BSP_LCD_FillRect(5,185,25,25);
     BSP_LCD_DisplayStringAt(11, 189, (uint8_t *)"5", LEFT_MODE);
     BSP_LCD_FillRect(35,185,25,25);
     BSP_LCD_DisplayStringAt(41, 189, (uint8_t *)"6", LEFT_MODE);
     BSP_LCD_FillRect(65,185,25,25);
     BSP_LCD_DisplayStringAt(71, 189, (uint8_t *)"7", LEFT_MODE);
     BSP_LCD_FillRect(95,185,25,25);
     BSP_LCD_DisplayStringAt(101, 189, (uint8_t *)"8", LEFT_MODE);
     BSP_LCD_FillRect(125,185,25,25);
     BSP_LCD_DisplayStringAt(131, 189, (uint8_t *)"9", LEFT_MODE);
     BSP_LCD_FillRect(5,215,25,25);
     BSP_LCD_DisplayStringAt(11,217, (uint8_t *)"-", LEFT_MODE);
     BSP_LCD_FillRect(35,215,200,20);
     BSP_LCD_DisplayStringAt(22,217, (uint8_t *)"NEXT", CENTER_MODE);
     BSP_LCD_FillRect(155,155,80,25);
     BSP_LCD_DisplayStringAt(167,160, (uint8_t *)"ENTER", LEFT_MODE);
     BSP_LCD_FillRect(155,185,80,25);
     BSP_LCD_DisplayStringAt(167,190, (uint8_t *)"BRISI", LEFT_MODE);
 }
 
 int dajCifru(int x1, int y1)
 {
     int broj;
     if(x1 >= 5 && x1 <= 30)
     {
         if(y1 >= 155 && y1 <= 180) broj = 0;
         if(y1 >= 185 && y1 <= 210) broj = 5;
     }
     if(x1 >= 35 && x1 <= 60)
     {
         if(y1 >= 155 && y1 <= 180) broj = 1;
         if(y1 >= 185 && y1 <= 210) broj = 6;
     }
     if(x1 >= 65 && x1 <= 90)
     {
         if(y1 >= 155 && y1 <= 180) broj = 2;
         if(y1 >= 185 && y1 <= 210) broj = 7;
     }
     if(x1 >= 95 && x1 <= 120)
     {
         if(y1 >= 155 && y1 <= 180) broj = 3;
         if(y1 >= 185 && y1 <= 210) broj = 8;
     }
     if(x1 >= 125 && x1 <= 150)
     {
         if(y1 >= 155 && y1 <= 180) broj = 4;
         if(y1 >= 185 && y1 <= 210) broj = 9;
     }
     if(x1 >= 155) {
         if(y1 >= 155 && y1 <= 180) return 10; //ENTER je 10
         if(y1 >= 185 && y1 <= 210) return 11; //BRISI je 11
     }
     if(x1 >= 35 && y1 >= 215) return 12; //NEXT je 12
     return broj;
 }
 
 double dajBroj(int digit[]) //unos max 3 cifre, u formatu _ _._
{
    double ret;
    ret = digit[0] * 10 + digit[1] + digit[2] * 0.1;
    return ret;
}

int vratiStepen(int i)
{
    switch(i)
    {
        case 0: return StepenBrojnika;
        case 1: return StepenNazivnika;
    }
    return 0;
}

void brisiCifru(int x, int y)
{
    int a = 78;
    if(x == 85 && y == 103)
        BSP_LCD_DisplayStringAt(a + 7, 104, (uint8_t *)"-", LEFT_MODE);
    else if(x == 109 && y == 103)
        BSP_LCD_DisplayStringAt(a + 31, 104, (uint8_t *)"-", LEFT_MODE);
    else 
        BSP_LCD_DisplayStringAt(a + 60, 104, (uint8_t *)"-", LEFT_MODE);
}

std::vector<double> koeficijenti(int T) //Funkcija za unos koeficijenata -> vraca niz sa koeficijentima
{
    std::vector<double> ret;
    int unoseniBroj[3];
    int brojac = 0, i = 0, predznak = 1;
    int stepen = vratiStepen(T);
    double coef;
    BSP_TS_GetState(&TS_State);
    while (brojac < stepen + 1) 
    {
        
        CrtajOkvirove(T);
        BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
        BSP_LCD_SetFont(&Font16);
        if(brojac == 0) BSP_LCD_DisplayStringAt(0, 55, (uint8_t *)"Slobodni koeficijent: ", LEFT_MODE);
        else {
            char potencija[2] = {brojac + 48};
            BSP_LCD_DisplayStringAt(0, 55, (uint8_t *)"Koeficijent uz s :       ", LEFT_MODE);
            BSP_LCD_SetFont(&Font12);
            BSP_LCD_DisplayStringAt(175, 52, (uint8_t *)potencija, LEFT_MODE);
        }

        BSP_TS_GetState(&TS_State);
        i = 0;
        while(i < 4)
        {
            
            BSP_TS_GetState(&TS_State);
            if(TS_State.touchDetected)
            {
                uint16_t x1 = TS_State.touchX[0];
                wait_ms(200);
                uint16_t y1 = TS_State.touchY[0];
                wait_ms(200);
                if(x1 >= 5 && x1 <= 30 && y1 >= 215 && y1 <= 240) //unos -
                {   
                   BSP_LCD_SetFont(&Font12);
                    if(predznak == 1)
                    {
                        predznak = -1;
                        BSP_LCD_DisplayStringAt(70, 105, (uint8_t *)"-", LEFT_MODE);
                    }
                    else
                    {
                        predznak = 1;
                        BSP_LCD_DisplayStringAt(70, 105, (uint8_t *)" ", LEFT_MODE);
                    }
                }
                    
                else
                {
                    int cifra = dajCifru(x1, y1);

                    if(cifra == 11 && i > 0)
                    {
                        i--;
                        switch(i)
                        {
                            case 0:  
                                brisiCifru(85, 103);
                                break;
                            case 1: 
                                brisiCifru(109, 103);
                                break;
                            case 2: 
                                brisiCifru(138, 103);
                                break;
                        }
                         
                    }
                    else if(i == 3)
                    {
                        while(1)//enter
                        {
                            if(dajCifru(x1, y1) == 10)
                            {
                                i++;
                                break;
                            }
                        }
                        break;
                    }
                    else if(cifra >= 0 && cifra <= 9)
                    {
                        unoseniBroj[i] = cifra;
                        char num[2] = {cifra + 48};
                        
                        BSP_LCD_SetFont(&Font16);
                        switch(i) { //Ispis cifre na predvidjeno mjesto
                            case 0: BSP_LCD_DisplayStringAt(85, 103, (uint8_t *)num, LEFT_MODE); 
                                break;
                            case 1: BSP_LCD_DisplayStringAt(109, 103, (uint8_t *)num, LEFT_MODE); 
                                break;
                            case 2: BSP_LCD_DisplayStringAt(138, 103, (uint8_t *)num, LEFT_MODE); 
                                break;
                        }
                        i++;
                    }
                }
            }
        }
        coef = dajBroj(unoseniBroj);
        ret.push_back(coef * predznak);

        predznak = 1;
        brojac++;
    }

    return ret;
}

void BrojUString(char* broj, double num) {
    if(num < 0) {
        broj[0] = '-';
        num *= -1;
    }
    else broj[0] = '+';
    int pomocnibroj = num*10;
    int decimala = pomocnibroj%10;
    
  
    if(num > 9.9) {
        broj[1] = int(num/10) + '0'; num -= int(num/10)*10;
        broj[2] = int(num) + '0'; broj[3] = '.'; broj[4] = decimala + '0';
        broj[5] = '\0'; 
    } else if(num < 1) {
        broj[1] = '0'; broj[2] = '.'; 
        broj[3] = decimala + '0';
        broj[4] = '0'; 
        broj[5] = '\0';
    } else {
        broj[1] = int(num) + '0'; broj[2] = '.'; 
        broj[3] = decimala + '0';
        broj[4] = '0'; broj[5] = '\0'; 
    }
}

void IspisiBrojnikINazivnik(int y, int x) {
    std::vector<double> niz;
    if(y == 82) niz = nizNula;//Na osnovu y-koordinate teksta se prepoznaje je li brojnik ili nazivnik
    else niz = nizPolova;
    int step = niz.size();
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    for(int i = 0; i < step; i++) {
        BSP_LCD_SetFont(&Font12);
        char broj[7];
        if(niz.at(step - 1 - i) == 0) {
            niz.erase(niz.end() - 1 - i);
            step--;
            i--;
            continue;
        }
        BrojUString(broj, niz.at(step - 1 - i));
        if(i == 0 && niz.at(step - 1 - i)>0) broj[0] = ' ';
        BSP_LCD_DisplayStringAt( x + i*45 , y, (uint8_t *) broj, LEFT_MODE);
        if(i < step- 1) {
            char pomocnistring[2];
            pomocnistring[0] = 's';
            pomocnistring[1] = '\0';
            
            BSP_LCD_DisplayStringAt( x + i*45 + 35, y, (uint8_t *) pomocnistring, LEFT_MODE);
            
            
            BSP_LCD_SetFont(&Font8);
            if(i < step - 2) {
                pomocnistring[0] = step - i - 1 + '0';
                pomocnistring[1] = '\0';
                BSP_LCD_DisplayStringAt( x + i*45 + 42, y-6, (uint8_t *) pomocnistring, LEFT_MODE);
            }
        } 
    }
}

int kolikonenultih(std::vector<double> vek) { //Broj nenultih koeficijenata
    //Nema potrebe da se ispisuju clanovi polinoma ciji su koeficijenti nula
    int brojac = 0;
    for(double x : vek) 
        if(x != 0) brojac++;
        
    return brojac;
}

void IspisiPrenosnuFunkciju() {

     int koefsirina;
     if(kolikonenultih(nizPolova) > kolikonenultih(nizNula)) koefsirina = kolikonenultih(nizPolova);
     else koefsirina = kolikonenultih(nizNula);
     BSP_LCD_Clear(LCD_COLOR_WHITE);
     BSP_LCD_SetTextColor(6591981);
     BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 40);
     BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
     BSP_LCD_SetBackColor(6591981);
     BSP_LCD_SetFont(&Font24);
     BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)"G(s)= ", LEFT_MODE);
     crtajTastaturu();
     BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
     BSP_LCD_SetFont(&Font12);
     BSP_LCD_DrawHLine((BSP_LCD_GetXSize() - koefsirina*42)/2, 96, koefsirina*42 );

    IspisiBrojnikINazivnik(82,  (BSP_LCD_GetXSize() -   kolikonenultih(nizNula)*42)/2 - 8);
    IspisiBrojnikINazivnik(105,  (BSP_LCD_GetXSize() -  kolikonenultih(nizPolova)*42)/2 - 8);
     
} 

int OdabirDijagrama() {
     BSP_LCD_Clear(LCD_COLOR_WHITE);
     BSP_LCD_SetTextColor(6591981);
     BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), 40);
     BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
     BSP_LCD_SetBackColor(6591981);
     BSP_LCD_SetFont(&Font16);
     BSP_LCD_DisplayStringAt(0, 8, (uint8_t *)"Odaberite dijagram", LEFT_MODE);
     BSP_LCD_DisplayStringAt(0, 23, (uint8_t *)"za prikaz: ", LEFT_MODE);
     crtajTastaturu();
     
     int odabir = 1; 
     
     
     BSP_LCD_DrawRect(80, 45, 80, 30);
     BSP_LCD_DrawRect(80, 80, 80, 30);
     BSP_LCD_DrawRect(80, 115, 80, 30);
     
     BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
     BSP_LCD_DisplayStringAt(83, 55, (uint8_t *)"Nyquist", LEFT_MODE);
     BSP_LCD_DisplayStringAt(98, 90, (uint8_t *)"Bode", LEFT_MODE);
     BSP_LCD_DisplayStringAt(95, 117, (uint8_t *)"step", LEFT_MODE);
     BSP_LCD_DisplayStringAt(91, 129, (uint8_t *)"odziv", LEFT_MODE);
     BSP_LCD_SetFont(&Font12);
    
     BSP_TS_GetState(&TS_State);
     
     while(1) {
         if(odabir == 1) {
             BSP_LCD_DisplayStringAt(40, 123, (uint8_t *) "    ", LEFT_MODE);
             BSP_LCD_DisplayStringAt(40, 55, (uint8_t *) "--->", LEFT_MODE);
         }
         if(odabir == 2) {
             BSP_LCD_DisplayStringAt(40, 55, (uint8_t *) "    ", LEFT_MODE);
             BSP_LCD_DisplayStringAt(40, 90, (uint8_t *) "--->", LEFT_MODE);
         }
         if(odabir == 3) {
             BSP_LCD_DisplayStringAt(40, 90, (uint8_t *) "    ", LEFT_MODE);
             BSP_LCD_DisplayStringAt(40, 123, (uint8_t *) "--->", LEFT_MODE);
         }
          BSP_TS_GetState(&TS_State);
          if(TS_State.touchDetected) {
            uint16_t x1 = TS_State.touchX[0];
            wait_ms(100);
            uint16_t y1 = TS_State.touchY[0];
            
            if(dajCifru(x1, y1) == 10) return odabir;
            

            if(dajCifru(x1, y1) == 12) {
                if(odabir == 3) odabir = 1;
                else odabir++;
             }
         }
        
         wait_ms(200);
     }
     BSP_LCD_DisplayStringAt(40, 55, (uint8_t *) "--->", LEFT_MODE);
     BSP_LCD_DisplayStringAt(170, 55, (uint8_t *) "<---", LEFT_MODE);
     return odabir;
}

int mqtt()
{

    NetworkInterface *network;
    network = NetworkInterface::get_default_instance();
    
    if (!network) {
        return -1;
    }

    MQTTNetwork mqttNetwork(network);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "broker.hivemq.com";
    int port = 1883;
    printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
        printf("rc from TCP connect is %d\r\n", rc);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "ugradbeni";
    data.username.cstring = "";
    data.password.cstring = "";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);

    MQTT::Message message;
            //odabir grafa
            char buf[100];
            
            //koeficijenti brojnika
            std::ostringstream vts;
            char char_arrayNula[60], char_arrayPolova[60];
  
            if (!nizNula.empty())
            {
                std::copy(nizNula.begin(), nizNula.end()-1,
                std::ostream_iterator<double>(vts, ","));
                vts << nizNula.back();
                std::string stringNula = vts.str();
                int n = stringNula.length();
                strcpy(char_arrayNula, stringNula.c_str());
                message.qos = MQTT::QOS0;
                message.retained = false;
                message.dup = false;
                message.payload = (void*)char_arrayNula;
                message.payloadlen = strlen(char_arrayNula);
                rc = client.publish(TEMAPUBBROJNIK, message);
            }
            
            wait_ms(100);
            //koeficijenti nazivnika
            std::ostringstream vts1;
            if (!nizPolova.empty())
            {
                std::copy(nizPolova.begin(), nizPolova.end()-1,
                std::ostream_iterator<double>(vts1, ","));
                vts1 << nizPolova.back();
                std::string stringPolova= vts1.str();
                int n1 = stringPolova.length();
                strcpy(char_arrayPolova, stringPolova.c_str());
                message.qos = MQTT::QOS0;
                message.retained = false;
                message.dup = false;
                message.payload = (void*)char_arrayPolova;
                message.payloadlen = strlen(char_arrayPolova);
                rc = client.publish(TEMAPUBNAZIVNIK, message);
            }
}

int mqttGraf(int opcijaGrafa) {
     NetworkInterface *network;
    network = NetworkInterface::get_default_instance();
    
    if (!network) {
        return -1;
    }

    MQTTNetwork mqttNetwork(network);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);
    
    const char* hostname = "broker.hivemq.com";
    int port = 1883;
    printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
        printf("rc from TCP connect is %d\r\n", rc);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "ugradbeni";
    data.username.cstring = "";
    data.password.cstring = "";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);
        
     MQTT::Message message;
     char buf[100];
     
    sprintf(buf, "%d", opcijaGrafa);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf);
    rc = client.publish(TEMAPUBGRAF, message);
}

 int main()
 {
     BSP_LCD_Init();
     //
     /* inicijalizacija ekrana */
     if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
         printf("BSP_TS_Init error\\n");}
     /* brisi ekran */
     BSP_LCD_Clear(LCD_COLOR_WHITE);
     StepenBrojnika = StepenBrojnikaIliNazivnika(0);
     StepenNazivnika = StepenBrojnikaIliNazivnika(1);
     ispisNuleTekst();
     crtajTastaturu();
     nizNula = koeficijenti(0);
     ispisPoloviTekst();
     nizPolova = koeficijenti(1);
     IspisiPrenosnuFunkciju();
     while(1) { //Cekaj dok korisnik ne pritisne ENTER. 
         
         BSP_TS_GetState(&TS_State);
         if(TS_State.touchDetected) {
            uint16_t x1 = TS_State.touchX[0];
            wait_ms(50);
            uint16_t y1 = TS_State.touchY[0];
            
            if(dajCifru(x1, y1) == 10) break;
            
         }
         wait_ms(25);
     }
    int a = OdabirDijagrama();
    
    int r = mqtt();
    r = mqttGraf(a);
    
    int provjera = a;
    while(1) {
        
        a = OdabirDijagrama();
        if(a != provjera) {
            r = mqttGraf(a);
            provjera = a;
        }
        
        wait(1);
    }
    
     wait(1);
 }
