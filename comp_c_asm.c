#include <stdio.h>
#include <msx/gfx.h>
#include <math.h>
#include <malloc.h>

int i,j;

unsigned char map[768];

unsigned char pe_t[] = {151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151};
   
#asm   
._pe
defb 151,160,137,91,90,15
defb 131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23
defb 190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33
defb 88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166
defb 77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244
defb 102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196
defb 135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123
defb 5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42
defb 223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9
defb 129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228
defb 251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107
defb 49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254
defb 138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151
#endasm

unsigned char scale8_t(unsigned char i, unsigned char scale)
{
    return ((i)*(scale))>>8;
	}

unsigned char scale8(unsigned char i, unsigned char scale)
{
	//multiplica hl com e, resultado em hl
	#asm
		ld h,l    // i
		ld d,0
		ld l,d
		ld b,8 
	._Mult8_Loop  
		add hl,hl  
		jr nc,_Mult8_NoAdd  
		add hl,de 
	._Mult8_NoAdd  
		djnz _Mult8_Loop  
		ld l,h // resultado >> 8
	#endasm
}

unsigned char EASE8_t(unsigned char i)
{
    unsigned char j,jj,jj2;
    j=i;
    if(j&0x80) {j=255-j;}
    jj=scale8_t(j,j);
    jj2=jj<<1;
    if(i&0x80) {jj2=255-jj2;}
    return jj2;
}

unsigned char EASE8(unsigned char i)
{
	#asm
		ld a,l
		rlca // mais rapido que "and 0x80" 
		ld b,l
		jr nc,_ease8_1
		ld a,255
		sub a,b
		ld l,a
	._ease8_1	
		ld a,b
		ld e,l
		ld h,0
		call _scale8
		sla l
		rlca // mais rapido que "and 0x80"
		jr nc,_ease8_2
		ld a,255
		sub a,l
		ld l,a
	._ease8_2 
	#endasm
}

char grad8_t(unsigned char hash,char x,char y)
{
  char u,v;
  if(hash&4) {u=y;v=x;} else {u=x;v=y;}
  if(hash&1) {u=-u;}
  if(hash&2) {v=-v;}
  return ((u+v)>>1); // >>1
}

char grad8(unsigned char hash, char x, char y)
{
	#asm 
		ld a,c
		and 4
		jr z,_grad8_1
		ld h,e
		ld l,d
		jr _grad8_2
	._grad8_1
		ld h,d
		ld l,e
	._grad8_2
		ld a,c
		and 1
		jr z,_grad8_3
		xor a
		sub a,h
		ld h,a
	._grad8_3
		ld a,c
		and 2
		jr z,_grad8_4
		xor a
		sub a,l
		ld l,a
	._grad8_4
		ld c,h
		ld a,l
		rlca
        sbc a,a
        ld h,a
		push hl
		ld l,c
		ld a,l
		rlca
        sbc a,a
        ld h,a
		pop de
		add hl,de
		sra h
        rr l
	#endasm
}

char lerp_t(char a,char b,unsigned char frac)   // fact8 = unsigned char
{
    char result;
	unsigned char delta,scaled;
    if(b>a) {
        delta=b-a;
		scaled=scale8_t(delta,frac);
		result=a+scaled;
    } else {
        delta=a-b;
        scaled=scale8_t(delta,frac);
        result=a-scaled;
    }
}

char lerp(char a,char b,unsigned char frac)   // fact8 = unsigned char
{
 	#asm
		push de
		push bc
		ld a,d
		add a,0x80
		ld d,a
		ld a,c
		add a,0x80
		ld c,a
		cp d
		pop bc
		pop de
		jp nc,_lerp_1
		ld a,d
		sub a,c
		ld l,a
		call _scale8
		ld a,c
		add a,l
		ld l,a
		jr _lerp_2
	._lerp_1
		ld a,c
		sub a,d
		ld l,a
		call _scale8
		ld a,c
		sub a,l
		ld l,a
	._lerp_2
		ret
	#endasm
}

char inoise8_t(unsigned short x,unsigned short y)  
{
  char X1,X2,ans;	
  unsigned char X=x>>8;  
  unsigned char Y=y>>8;
  unsigned char A=pe_t[X]+Y;
  unsigned char AA=pe_t[A];
  unsigned char AB=pe_t[A+1];
  unsigned char B=pe_t[X+1]+Y;
  unsigned char BA=pe_t[B];
  unsigned char BB=pe_t[B+1];
  unsigned char u=x;
  unsigned char v=y;
  char xx=((unsigned char)(x)>>1) & 0x7F; 
  char yy=((unsigned char)(y)>>1) & 0x7F;
  unsigned char N=0x80;
  u=EASE8_t(u); 
  v=EASE8_t(v);
  X1=lerp_t(grad8_t(pe_t[AA],xx,yy),grad8_t(pe_t[BA],xx-N,yy),u);
  X2=lerp_t(grad8_t(pe_t[AB],xx,yy-N),grad8_t(pe_t[BB],xx-N,yy-N),u);
  ans=lerp_t(X1,X2,v);
  return ans+64;
}

unsigned char peAA,peAB,peBA,peBB,u,v;
char xx,yy,grad1,grad2,grad3,grad4,X1,X2;
unsigned short xc,yc,xc1,yc1;

char inoise8(unsigned short x,unsigned short y)  
{
	#asm
		pop	bc  // entradas e saidas do z88dk, talvez nao seja util 
		pop	de
		pop	hl
		push hl
		push de
		push bc
		ld b,0
		ld c,h
		push hl
		ld hl,_pe
		add hl,bc
		ld a,(hl)
		add a,d   // A
		ld hl,_pe
		ld c,a
		add hl,bc
		ld a,(hl)  //AA
		push bc
		ld hl,_pe
		ld c,a
		add hl,bc
		ld a,(hl)   
		ld (_peAA),a //peAA
		pop bc
		inc c
		ld hl,_pe
		add hl,bc
		ld a,(hl)  //AB
		ld hl,_pe
		ld c,a
		add hl,bc
		ld a,(hl)
		ld (_peAB),a //peAB
		pop hl
		ld c,h
		inc c
		push hl
		ld hl,_pe
		add hl,bc
		ld a,(hl)
		add a,d    //B
		ld hl,_pe
		ld c,a
		add hl,bc
		ld a,(hl)   //BA
		push bc
		ld hl,_pe
		ld c,a
		add hl,bc
		ld a,(hl)   
		ld (_peBA),a //peBA
		pop bc
		inc c
		ld hl,_pe
		add hl,bc
		ld a,(hl)   //BB
		ld hl,_pe
		ld c,a
		add hl,bc
		ld a,(hl)
		ld (_peBB),a //peBB
		pop hl
		push hl
		push de
		ld a,(_peAA)
		ld c,a
		ld d,l
		srl d
		srl e
		ld a,d
		and 0x7f
		ld (_xx),a
		ld a,e
		and 0x7f
		ld (_yy),a
		call _grad8  
		ld a,l
		ld (_grad1),a
		ld a,(_peBA)
		ld c,a
		ld a,(_xx)
		sub a,0x80
		ld d,a
		ld a,(_yy)
		ld e,a
		call _grad8 
		ld a,l
		ld (_grad2),a
		ld a,(_peAB)
		ld c,a
		ld a,(_xx)
		ld d,a
		ld a,(_yy)
		sub a,0x80
		ld e,a
		call _grad8 
		ld a,l
		ld (_grad3),a
		ld a,(_peBB)
		ld c,a
		ld a,(_xx)
		sub a,0x80
		ld d,a
		ld a,(_yy)
		sub a,0x80
		ld e,a
		call _grad8 
		ld a,l
		ld (_grad4),a
		pop de
		pop hl
		push de
		call _EASE8 
		ld a,l
		ld (_u),a
		pop de
		ld l,e
		call _EASE8
		ld a,l
		ld (_v),a
		ld a,(_grad1)
		ld c,a
		ld a,(_grad2)
		ld d,a
		ld a,(_u)
		ld e,a
		call _lerp  
		ld a,l
		ld (_X1),a
		ld a,(_grad3)
		ld c,a
		ld a,(_grad4)
		ld d,a
		ld a,(_u)
		ld e,a
		call _lerp   
		ld a,l
		ld (_X2),a
		ld a,(_X1)
		ld c,a
		ld a,(_X2)
		ld d,a
		ld a,(_v)
		ld e,a
		call _lerp   
		ld a,l
		add a,64
		ld l,a
		ld h,0
	#endasm
}

unsigned short a,b;
char c,d;

main() {

	set_color(15,1,1);
	
	b=0;c=0;
	
	while (1) {

	for(i=0;i<16;i++) {
		xc=(i)*32; yc=(j+a)*32;
		xc1=(i)*1; yc1=(j+a)*1;
		c=(inoise8(xc,yc)+inoise8(xc1,yc1))>>1;
		d=65;
		if(c>50) d=66;
		if(c>75) d=67;
		if(c>100) d=68;
		
		map[736+i*2]=d;map[736+i*2+1]=d;
		}
	
	#asm
	ld hl,_map
	ld de,0x1800 
	ld bc,0x300 
	call 0x5C //LDIRVM
	ld de,_map
	ld hl,_map+32
	ld bc,736
	ldir
	#endasm
	a=a+1;b=b+1;
	}
}

