#include<p18F458.h>

void Delay(void);					/////////////////////////////////////////////
void LCD_ini(void);					/////////////////////////////////////////////
void LCD_CMD(unsigned char);		/////////////////////////////////////////////
void LCD_DATA(unsigned char);		/////////////////////////////////////////////
void Value(unsigned char);			////////	Decleration of Functions	/////
void chk_isr(void);					/////////////////////////////////////////////
void INT0_ISR(void);				/////////////////////////////////////////////
void calculate_ans(void);			/////////////////////////////////////////////
void Re_set(void);					/////////////////////////////////////////////
void Display(void);					/////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//	Start of Pragma code
#pragma interrupt chk_isr			//	Here "Interrupt" is a reserved keyword for interrupt
void chk_isr()						//	This is the Interrupt Service Routine
{
if(INTCONbits.INT0IF==1)			//	Check if the External Interrupt has occured or not?
{									//	If External interrupt occures than 
INT0_ISR();							//	execute the Interrupt 0 Interrupt Service Routine 	
INTCONbits.INT0IF=0;				//	Clear the Interrupt 0 flag to indicate that interrupt
}									//	has been served
}

#pragma code My_HiPrio_Int=0x08		//	High Priority Interrupt location 
void My_HiPrio_Int(void)
{
_asm
GOTO chk_isr						//	Go to interrupt vector table to transfer control  
_endasm								//	to desired Interrupt Service Routine ISR
}
#pragma code						//	End of Pragma code
//////////////////////////////////////////////////////////////////////////////////////////////////


#define RS PORTBbits.RB5			//	RS pin of LCD is connected to RB5 pin
#define RW PORTBbits.RB6			//	RW pin of LCD is connected to RB6 pin
#define EN PORTBbits.RB7			//	EN pin of LCD is connected to RB7 pin
#define LED PORTBbits.RB4			//	EN pin of LCD is connected to RB7 pin
//////////////////////////////////////////////////////////////////////////////////////////////////
//	Global Variables
unsigned char Data;					//	Variable to store data recieved at PORTC from the Keypad
unsigned char button=0;				//	Variable to check if the button on keypad is pressed or not?
unsigned char signA=1;				//	SignA represents the Sign assigned to variable "A"
unsigned char signB=1;				//	SignB represents the Sign assigned to variable "B"
									//  by default they are set to 1. "1" indicates +ive sign.	
unsigned int A;						//	It Represents the first number entered through keypad.
unsigned int B;						//	It Represents the Second number entered through keypad.
unsigned int Result;				//	It Represents the calculated result or answer.
unsigned int Re;					//	It Represents the Remainder in result of division
									//	used in the calculation of decimal part of Quatient.

unsigned char next=0;				//	To check whether the entered number belongs to A or B
unsigned char again=0;				//	To check whether the entered number is part of same variable
unsigned char op=0;					//	To Determine wheter to display result or reset the calculator?
unsigned char sig=0;				//	To Determine the operation to be performed on numbers
unsigned char  Decimal[8];			//	Array to store decimal part of result
unsigned char Res=0;				//	To check if the Result contains any decimal part or not?
unsigned char i=0;					//	To run for loops
unsigned char aa=0;					//	To check if the first pressed button is sign or a number
int q1,w1,e1,t1,q2,w2,e2,t2;		//	To convert data stored in variable "Result" to ascii format
unsigned char clc=0;				//	To check whether any result has been evaluated or not

/////////////////////////////////////////////////////////////////////////////////////////////////

void main()
{
TRISB=0x00;							//	Make PORTB an Output port
TRISD=0x00;							//	Make PORTD an Output port							
TRISC=0xFF;							//	Make PORTC an Input port
TRISBbits.TRISB0=1;					//	Make RB0 of PORTB an Input bit for the interrupt

PORTB=0x00;							//	Set initial value of PORTB
PORTD=0x00;							//	Set initial value of PORTD
PORTC=0xFF;							//	Set initial value of PORTC

//////////////////////////////////////////////////////////////////////////////////////////////////
									//	To enable External interrupt 0, Set concerned bits related  
INTCONbits.INT0IF=0;				//	Clear Interrupt 0 flag bit  
INTCONbits.INT0IE=1;				//	Enable Interrupt 0
INTCON2bits.INTEDG0=1;				//	Interrupt 0 will occure on rising edge 
INTCONbits.GIE=1;					//	Enable interrupts
//////////////////////////////////////////////////////////////////////////////////////////////////

LCD_ini();							//	Initialize LCD

A=0;								/////////////////////////////////////////////////////////////	
B=0;								/////////////////////////////////////////////////////////////
Result=0;							//	Set Default values in the variables
signA=1;							/////////////////////////////////////////////////////////////
signB=1;							/////////////////////////////////////////////////////////////
aa=0;								/////////////////////////////////////////////////////////////
clc=0;
while(1)
{

if(button==1)						//	If any Key is pressed than Enter otherwise ignore 
{

if(clc==1)							//	checks if any evaluation of result has been done or not
{									//	if yes then clear the LCD and set variables to default	
clc=0;
Re_set();							//	Call this function to reset the calculator
}

Value(Data);						//	Call function "Value" to check which key is pressed
									//	and store that value in variable "Data"

									//	If any of the keys "+,-,*,/,C,="is pressed then enter "|" represents OR operation

if(Data=='+' | Data=='/' | Data=='*' | Data=='-' | Data=='C' | Data=='=')	
{

if(aa==1)							//	Check if the first number has been entered or not? if yes than enter
{
next=1;								//	So that next pressed value gets stored in variable "B"
if(op=='=')							//	op=5 represents that '=' is pressed
{
calculate_ans();					//	Call this function to calculate the result  
Display();							//	Call this function to display result on LCD

///////////////////////////////////////////////////////////////////////////////////////////////////////
A=0;								//////////////////////////////////////////////////////////////////
B=0;								//////////////////////////////////////////////////////////////////
Result=0;							/////////	After calculating the result//////////////////////////
aa=0;								////////	Set all variables to defualt values	//////////////////			
signA=1;							//////////////////////////////////////////////////////////////////
signB=1;							//////////////////////////////////////////////////////////////////
}
}
if(op=='C')							//	If 'C' is pressed than Reset the calculator. op=6 represents 'C'
{
Re_set();							//	Call this function to reset the calculator
}
}
else								//	If any key other than "+,-,*,/,C" than store that in respective variable
{
if(next==0)							//	This means value is to be stored in variable 'A'
{
if(again==1)						//	If the any key other than "+,-,*,/,C" is pressed consectively than
{									//	continue to store value in variable 'A'
A=A*10;								//	Multiply to move A by 10th place
again=0;							//	Reset it
}
Data=Data & 0x0F;					//	Convert from Ascii to Decimal 
A=A+Data;							//	Store Decimal value to 'A'
aa=1;								//	Indicate that value has been enterd in 'A'
}

if(next==1)							//	After the selection of operation, "next" variable gets set
{									//	 to indicate that store value in variable 'B' 
if(again==1)						//	If the any key other than "+,-,*,/,C" is pressed consectively than
{									//	continue to store value in variable 'B'
B=B*10;								//	Multiply to move B by 10th place
again=0;							//	Reset it
}
Data=Data & 0x0F;					//	Convert from Ascii to Decimal 
B=B+Data;							//	Store Decimal value to 'B'
}
}
button=0;							//	Reset to check whether the button has been pressed again or not
again=1;							//	Set it indicate that pressed key belongs to either variable 'A' or 'B'
LCD_CMD(0x06);						//	Shift curser of LCD to right
} 
}
}


void INT0_ISR()						//	Interrupt 0 service routine
{
Data=PORTC & 0x0F;					//	Store the value at PORTC in variable "Data"
button=1;							//	Indicate that Key on keypad is pressed
}


void Delay()						// Function to generate an arbitrary delay
{
unsigned int i,j;
for(i=0;i<=50;i++)
{
for(j=0;j<=10;j++);
}
}

void LCD_ini()						//	Function to initialize LCD
{
LCD_CMD(0x38);						//	Command to initialize 2x16 LCD
Delay();
LCD_CMD(0x01);						//	Command to clear LCD
Delay();
LCD_CMD(0x0E);						//	Display on, cursor blinking
Delay();
LCD_CMD(0x80);						//	Command to move cursor to 1st line
Delay();
}

void LCD_CMD(unsigned char CMD)		//	Function to send commands to the LCD 
{
unsigned char z;
RS=0;								//	Clear RS to send command to LCD
Delay();
RW=0;
PORTD=CMD;							//	Send command to LCD
Delay();
EN=1;								//	Toggle EN to indicate LCD to Latch data	
z=0;								//	z variable has no purpose but to introduce a very small delay
EN=0;								//	so that data gets latched
}	

void LCD_DATA(unsigned char DATA) 	// Function to send data to LCD
{
unsigned char z;
RS=1;								//	Set RS to Send Data to LCD
RW=0;
PORTD=DATA;							//	Send Data to LCD
Delay();
EN=1;								//	Toggle EN to indicate LCD to Latch data
z=0;								//	z variable has no purpose but to introduce a very small delay
EN=0;								//	so that data gets latched
}

void Value(unsigned char v)		//	Function to identify that which button is pressed on keypad
{								//	Reason is, every key has its own identity which doesnot represent its actual identity
switch(v)
{
case 0:
Data=0x37;						//	Store Ascii value of 7 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 1:
Data=0x38;						//	Store Ascii value of 8 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 2:
Data=0x39;						//	Store Ascii value of 9 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 3:
Data='/';						//	Store Ascii value of '/' in variable "Data"
sig='/';						//	Store Ascii value of '/' in variable "sign"
LCD_DATA(sig);					//	Display the pressed key on LCD
break;

case 4:
Data=0x34;						//	Store Ascii value of 4 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 5:
Data=0x35;						//	Store Ascii value of 5 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 6:
Data=0x36;						//	Store Ascii value of 6 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 7:
Data='*';						//	Store Ascii value of '*' in variable "Data"
sig='*';						//	Store Ascii value of '*' in variable "sig"
LCD_DATA(sig);					//	Display the pressed key on LCD
break;

case 8:
Data=0x31;						//	Store Ascii value of 1 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 9:
Data=0x32;						//	Store Ascii value of 2 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 10:
Data=0x33;						//	Store Ascii value of 3 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 11:
if(aa==0)						//	If a=0, it indicates that very first pressed key is a sign of 'A' not a value
{
signA=2;						//	2 indicates '-' sign
}
sig='-';
LCD_DATA(sig);					//	Display the pressed key on LCD
Data='-';						//	Store Ascii value of '-' in variable "Data"
break;

case 12:
op='C';
LCD_DATA(op);					//	Display the pressed key on LCD
Data='C';						//	Store Ascii value of 'C' in variable "Data"
break;

case 13:
Data=0x30;						//	Store Ascii value of 0 in variable "Data"
LCD_DATA(Data);					//	Display the pressed key on LCD
break;

case 14:
op='=';
LCD_DATA(op);					//	Display the pressed key on LCD
Data='=';						//	Store Ascii value of '=' in variable "Data"
break;

case 15:
if(aa==0)						//	If a=0, it indicates that very first pressed key is a sign of 'A' not a value
{
signA=1;						//	1 indicates '+' sign.
}
sig='+';
LCD_DATA(sig);					//	Display the pressed key on LCD
Data='+';						//	Store Ascii value of '+' in variable "Data"
break;
}}


void calculate_ans()					//	Function to Calculate the Result
{
switch(sig)								//	Check, which operation is to be performed
{
case '+':
if(signA==1 & signB==1)					//	if both A and B are +ive values then enter
{
Result=A+B;
}

if(signA==2 & signB==1)					//	If A is -ive and B is +ive then enter
{
Result=B-A;
if(A>B)									//	If A>B then perform following operations
{
Result=~Result;							//	Take 2's complement of the Reseult
Result++;
LCD_CMD(0xC0);							
LCD_DATA('-');							//	Display '-' sign on LCD
}
}
if(signA==2 & signB==2)					//	If both A and B are -ive then perform following operation
{
Result=A+B;
LCD_CMD(0xC0);
LCD_DATA('-');							//	Display '-' sign on LCD
}
break;

case '-':
if(signA==1 & signB==1)					//	If both A and B are +ive values but the operation to be perfomed  
{										//	is negative then enter
Result=	A-B;
if(B>A)									//	If B>A then perform following operations
{
Result=~Result;							//	Take 2's complement of result
Result++;
LCD_CMD(0xC0);
LCD_DATA('-');							//	Display '-' sign on LCD
}
}

if(signA==2 & signB==1)					//	If A is negative and B is positive but the operation to be performed
{										//	 is '-' than enter
Result=A+B;
LCD_CMD(0xC0);
LCD_DATA('-');
}
break;

case '*':
Result=A*B;								//	Perform Multiplication
if(signA==2 & signB==1)					//	If A is -ive and B is +ive then Display '-' sign on LCD
{
LCD_CMD(0xC0);
LCD_DATA('-');
}
break;

case '/':								//	Perform Division
Result=A/B;								//	Calculate the result, before the decimal point
Re=A%B;
for(i=0;i<=4 & Re>0;i++)				//	Calculate the result after the decimal part
{
Re=Re*10;								//	Multiply the Remainder by 10 to make it divisible by B
Decimal[i]=Re/B;						//	Store result in an array
Re=Re%B;								//	Again calculate the remainder and repeat this process for 5 decimal places
Res=1;									//	Set to indicate that result contains remainder
}
if(signA==2 & signB==1)					//	If A is -ive and B is +ive then Display '-' sign on LCD
{
LCD_CMD(0xC0);
LCD_DATA('-');
}

break;
}
}

void Re_set()			// Function to Reset all the variables, if C on Keypad is pressed
{
LCD_CMD(0x01);			//	Clear LCD
Delay();
LCD_CMD(0x0E);
Delay();
LCD_CMD(0x80);			// Move the Curser to first row and first column of LCD

////////////////////////////////////////////////////////////////////////////////////////////////////////////

A=0;					//	Reset all variables to default values
B=0;
op=0;
sig=0;
next=0;
again=0;
signA=1;
signB=1;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


void Display()			// Function to Display Calculated Result on LCD
{

LCD_CMD(0xC1);			//	Move the curser to 2nd column of 2nd row

/////////////////////////////////////////////////////////////////////////////////////////////////////////

t1=Result/10000;		////////////////////////////////////////////////////////////////////////////////
q1=Result%10000;		////////////////////////////////////////////////////////////////////////////////
w1=q1/1000;				////////////////////////////////////////////////////////////////////////////////
e1=q1%1000;				///////////////////////////////////////////////////////////////////////////////
						///////////////	Conversion of Integer to Ascii ////////////////////////////////
t2=e1/100;				///////////////////////////////////////////////////////////////////////////////
q2=e1%100;				///////////////////////////////////////////////////////////////////////////////
w2=q2/10;				///////////////////////////////////////////////////////////////////////////////
e2=q2%10;				///////////////////////////////////////////////////////////////////////////////
t1=t1|0x30;				///////////////////////////////////////////////////////////////////////////////
w1=w1|0x30;				///////////////////////////////////////////////////////////////////////////////
t2=t2|0x30;				///////////////////////////////////////////////////////////////////////////////
w2=w2|0x30;				///////////////////////////////////////////////////////////////////////////////
e2=e2|0x30;				///////////////////////////////////////////////////////////////////////////////

clc=1;					//	Set to indicate that result is being evaluated

						//	Only display those characters which have any weight in the Result
if(t1>0x30)				//	Condition to display all characters
{
LCD_DATA(t1);
LCD_DATA(w1);
LCD_DATA(t2);
LCD_DATA(w2);
LCD_DATA(e2);
}

if(t1==0x30 & w1>0x30)	// Condition to display all characters except t1.
{
LCD_DATA(w1);
LCD_DATA(t2);
LCD_DATA(w2);
LCD_DATA(e2);
}


if(t1==0x30 & w1==0x30 & t2>0x30)					// Condition to display all characters except t1 and w1.
{
LCD_DATA(t2);
LCD_DATA(w2);
LCD_DATA(e2);
}

if(t1==0x30 & w1==0x30 & t2==0x30 & w2>0x30)		// Condition to display all characters except t1, w1,t2.
{
LCD_DATA(w2);
LCD_DATA(e2);
}

if(t1==0x30 & w1==0x30 & t2==0x30 & w2==0x30)		// Condition to display only e2.
{
LCD_DATA(e2);
}

if(Res==1)											// Condition to check if decimal part is present or not
{
Res=0;
LCD_DATA(0x2E);
for(i=0;i<=4;i++)
{
Decimal[i]=Decimal[i] | 0x30;						//	Display Decimal part
LCD_DATA(Decimal[i]);								
Decimal[i]=0;										//	Clear the array for future use
}
}
}