class PlayerPointers;
class PlayerObject;

class PlayerPointers
{
public:
	PlayerObject* PlayerArray[128]; //0x0000 

};//Size=0x0200

class PlayerObject
{
public:
char _0x0000[143];
	unsigned char isDead; //0x008F
	__int32 health; //0x0090 
char _0x0094[4];
	__int32 team; //0x0098 
char _0x009C[448];
	float posX; //0x025C 
	float posY; //0x0260 
	float posZ; //0x0264 
	float mouseY; //0x0268 
	float mouseX; //0x026C 
char _0x0270[220];
	unsigned char bodyState; //0x034C 
char _0x034D[3];

};//Size=0x0350