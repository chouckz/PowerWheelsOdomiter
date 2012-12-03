#include <TinyWireM.h>  
#include <avr/eeprom.h>

#define capacity  10800 //seconds on full battary 
int voltage_factor  = 1325;// Actual voltage when Analog in == Vcc (about 5v)    
float m_per_sec = 2.0; //Meters per sec

int read_interval = 50; //refresh dispaly every X ms
int rep = 1000/read_interval;
#define  power_off_level   612
#define  button_off_level   150
#define buttonPin  3     
#define powerPin  2     
#define resetPin  1  
#define deltaV  30 //diff in voltage reading drop 0..1023 (1 equals 5mV) big enaugh to trigger counter     
#define eepromaddress 150
unsigned long ltime, last_read_time, start_time;
long  total_time = 0;
int saved_time = 0;
int  current_time  = 0;
int mode =0;
int minV =9999;
int  last_point = 0; //blinking dots

int  voltage, last_voltage, HundredsHours, TensMeters,TensHours,  Hours, TensMinutes, BaseMinutes, TensSeconds, HundredsKm, TensKm, BaseKm, HundredsMeters, addressLong, addressInt;
const int lookup[10] = {0x3F,0x06,0x5B,0x4F,0x66, 0x6D,0x7D,0x07,0x7F,0x6F}; 
bool power = false;
bool button = false;
bool pasasite = true;

//int min=9999;
//int max =0;

void setup() 
{ 
pinMode(resetPin, INPUT);
//	Serial.begin(9600);	 
	TinyWireM.begin();        // join i2c bus (address optional for master) 

  if ((digitalRead(resetPin) == HIGH) && ( analogRead(powerPin) > power_off_level) )
  {	
	total_time =0;
	current_time = 0;
	saved_time=0;
	save_total() ;
	save() ;
  }
 //delay(100); eeprom_write_dword((unsigned long *) eepromaddress, 21600);
//eeprom_write_word((unsigned int *) eepromaddress+10, 600);
//return;
/*total_time =21100;
	current_time = 600;
	save_total() ;
	save() ;*/
	
	total_time = eeprom_read_dword((unsigned long *) eepromaddress);//EEPROM.readLong(addressLong);

		//Serial.print("tatal: ");//Serial.println(total_time);
	saved_time = eeprom_read_word((unsigned int *) eepromaddress+10);//EEPROM.readInt(addressInt);
	current_time = saved_time;
	last_voltage = analogRead(powerPin);

	
	if ( analogRead(buttonPin) < button_off_level) 
		{
			pasasite = false;
			show_cur_time();delay(4000);
			show_tot_time();delay(4000);
			show_tot_dis();delay(4000);
		}
} 

void save() 
{ 
	//eeprom_write_dword((unsigned long *) 350, total_time + current_time);//EEPROM.writeLong(addressLong,total_time);
	eeprom_write_word((unsigned int *) eepromaddress+10, current_time);//EEPROM.writeInt(addressInt,current_time);
		//Serial.print("SSS tatal: ");//Serial.println(total_time + millis()/1000);
				//Serial.print("SSS cuttent: ");//Serial.println(current_time);
	} 
	
void save_total() 
{ 
	eeprom_write_dword((unsigned long *) eepromaddress, total_time + current_time);//EEPROM.writeLong(addressLong,total_time);
	}
	
void loop() 
{ 
  ltime = millis();   
  voltage = analogRead(powerPin);
   	//Serial.print("V: "); Serial.println(voltage);
  if ( voltage < power_off_level) 
	{
		 if (power) save(); 
		power = false;
	}
	else power = true;
	
 if ((ltime >= (last_read_time + read_interval)) or (ltime < last_read_time ))
  {	
  //Serial.print("tatal: ");Serial.println(total_time);
	if ( analogRead(buttonPin) > button_off_level) 
   { 
	if (!button)	
		{
			start_time = ltime;
		}
	button = true;
	}
else
	{
		if (button)	
		{
			saved_time += (ltime - start_time)/1000 ;
			current_time = saved_time;
		}
		button = false;
	}
	if (button)
		{	
			current_time = saved_time+ (ltime - start_time)/1000 ;
		}
	last_voltage= voltage;
	//	display(lookup[ (minV/1000) % 10],lookup[ minV/100 % 10],lookup[ (minV/10) % 10],lookup[ minV % 10]);


  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if ((digitalRead(resetPin) == HIGH) && (voltage > power_off_level)&&(current_time != 0)  )
  {	
		save_total() ;
		current_time = 0;
		saved_time= 0;
  }
	  //show_voltage();
    if (mode <4*rep) show_cur_dis();//show_cur_time
  //if (mode <6*rep) 
 // show_cur_time();
	//if (mode ==5*rep) show_dis_left();// display(lookup[ (minV/1000) % 10],lookup[ minV/100 % 10],lookup[ (minV/10) % 10],lookup[ minV % 10]);
//	//show_cur_dis
	
	if (mode ==4*rep) show_dis_left();
	//if (mode ==7*rep) show_tot_time();
	
	//if (mode ==6*rep)  show_tot_dis();//display(lookup[ (voltage/1000) % 10],lookup[ voltage/100 % 10],lookup[ (voltage/10) % 10],lookup[ voltage % 10]);
	//
	//if (mode >7*rep)  display(lookup[ (minV/1000) % 10],lookup[ minV/100 % 10],lookup[ (minV/10) % 10],lookup[ minV % 10]);
	//
	if (mode ==6*rep) show_voltage(); //show_int_voltage() ;
	if (mode ==8*rep) mode =0;
	mode++;
    last_read_time = ltime;
  }
    
} 
   
void fill_dist(long Count) 
{ 
 	TensMeters = abs((Count  * m_per_sec)/10) ;
 	//test//TensMeters = abs((Count  * m_per_sec)) ;
	//Serial.print("(");//Serial.print(Count);//Serial.print(") ");
//Serial.print(HundredsMeters);
	HundredsKm = (TensMeters/10000) % 10;
    TensKm = (TensMeters/1000) % 10;
    BaseKm = (TensMeters/100) % 10;
	  	HundredsMeters = (TensMeters/10) % 10;
	TensMeters = TensMeters % 10;
} 

void fill_time(long Count) 
{ 
	//Count += ltime/1000;
	HundredsHours = (Count/360000)%10;
	TensHours =(Count/36000)% 10;
    Hours = (Count/3600)% 10;
	TensSeconds = Count - Hours*3600 - TensHours *36000 - HundredsHours*360000; //Seconds within an hour
    BaseMinutes = TensSeconds/60;// Minutes within an hour
	TensSeconds = (TensSeconds - BaseMinutes*60)/10 % 10;
	TensMinutes = BaseMinutes/10 % 10; 
	//if (TensMinutes >5) TensMinutes =0;
	BaseMinutes = BaseMinutes % 10;
	//TensSeconds =TensSeconds/10 % 10; 
	//if (TensSeconds >5) 		TensSeconds =0;
} 

void display( int p1,int p2, int p3, int p4)
{
	TinyWireM.beginTransmission(0x38);
	TinyWireM.send(0);
	TinyWireM.send(B01000111);
	TinyWireM.endTransmission();
	TinyWireM.beginTransmission(0x38);
    TinyWireM.send(1);
	TinyWireM.send(p4); 
	TinyWireM.send(p3); 
	TinyWireM.send(p2); 
	TinyWireM.send(p1); 
	TinyWireM.endTransmission();
}

void show_dis_left() 	//show animated rotating star on display between hours and minutes
{
	//Serial.print("Left: ");
	int d2 =0; 
	int d1 = 0;
	/*	if (button) 
		{
			if (last_point == 0) last_point = 128; else  last_point = 0;
		}
		else last_point = 128;*/
	fill_dist(capacity - current_time);
	    	//Serial.println(" 00m");
			if (TensKm >0) d2 = lookup[TensKm] ; 
		else 
			{
				if (capacity < current_time)  d2 =64;
			}
			
		if ((TensKm >0) &&(capacity < current_time) ) d1 = 25; else d1 =24;
	display(d1,d2,lookup[BaseKm]+128,lookup[HundredsMeters]);

}


void show_voltage() 	//show animated rotating star on display between hours and minutes
{
 int d1 =0;

	/*	if (button) 
		{
			if (last_point == 0) last_point = 128; else  last_point = 0;
		}
		else last_point = 128;
*/
	//display(lookup[ (voltage/1000) % 10],lookup[ (voltage/100) % 10],lookup[ (voltage/10) % 10],lookup[ voltage % 10]);
 voltage = (long)voltage_factor*voltage/1023+65 ; //65 is 0.65v drop is for one diode on the way in
 if (pasasite) voltage +=65;//65 is 0.65v drop is for another one diode on the way in
	  if (voltage >999) d1 = lookup[(voltage/1000) % 10] ;
		display(d1,lookup[ (voltage/100) % 10]+128,lookup[ voltage/10 % 10],28);
}


void show_cur_dis() 	//show animated rotating star on display between hours and minutes
{
	int d2 =0;	  
	if (button) 
		{
			if (last_point == 0) last_point = 128; else  last_point = 0;
		}
		else last_point = 128;
	//Serial.print("Cur: ");
	fill_dist(current_time);
	if (TensKm >0) d2=lookup[TensKm] ;
		display(d2,lookup[BaseKm]+last_point,lookup[HundredsMeters],lookup[TensMeters]);
    	//Serial.println(" 00m");

}

void show_tot_dis() 	//show animated rotating star on display between hours and minutes
{
	fill_dist(total_time+current_time);		
	if (HundredsKm > 0) HundredsKm = lookup[HundredsKm]; else HundredsKm =0;
	display(99,HundredsKm,lookup[TensKm],lookup[BaseKm]);
}

void show_tot_time() 	//show animated rotating star on display between hours and minutes
{	
fill_time(total_time+current_time);
int d2 =lookup[HundredsHours];
int d3 =lookup[TensHours];
int d4 =lookup[Hours] ;

if (HundredsHours == 0) 
	{
	if (TensHours == 0)
		{
			d2=lookup[Hours]+128;
			d3= lookup[TensMinutes];
			d4 =lookup[BaseMinutes]; 
		}
		else
		{
			d2=lookup[TensHours];
			d3= lookup[Hours]+128 ;
			d4 =lookup[TensMinutes]; 
		}
	}
   	display(0x74,d2,d3,d4);
}


void show_cur_time() 	//show animated rotating star on display between hours and minutes
{
	if (button) 
		{
			if (last_point == 0) last_point = 128; else  last_point = 0;
		}
		else last_point = 128;
		
	
	fill_time(current_time);
		int d1 =lookup[Hours]+last_point;
	int d2 =lookup[TensMinutes];
int d3 =lookup[BaseMinutes]+last_point;
int d4 =lookup[TensSeconds]  ;


	if (TensHours  >0)
	{
		d1 =lookup[TensHours];
		d2 =lookup[Hours]+last_point ;
		d3 =lookup[TensMinutes];
		d4 =lookup[BaseMinutes];
		}
   	display(d1,d2,d3,d4);
}
/*

void show_int_voltage() 	//show animated rotating star on display between hours and minutes
{
 int d1 =0;
	 voltage = readVcc(); //60 is 0.6v drop is for one diode on the way in
	  if (voltage >999) d1 = lookup[(voltage/1000) % 10] ;
		display(16,d1+128,lookup[ (voltage/100) % 10],lookup[ voltage/10 % 10]);
}
long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
  // 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
    delay(2);

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000

  return result; // Vcc in millivolts
}
*/