////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2002 mamaich
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////
//
//  Module Name:
// 
//     world.cpp
// 
//  Abstract:
// 
//     World handling classes
//     
//  Author:
// 
//     mamaich
//
//  Revision history:
//
//  [2/24/2002] - created
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "afxmt.h"
#include "assert.h"

#include "api.h"
#include "world.h"
#include "defcmd.h"
#include "WndProcHook.h"
#include "DisplayTab.h"
#include "GuiThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The lock object
static CCriticalSection GameObjectLockVar;
class GameObjectLock
{
public:
	GameObjectLock() {GameObjectLockVar.Lock();}
	~GameObjectLock() {GameObjectLockVar.Unlock();}
};

/*
 *	Classes. They should be used locally in this file because all access is
 *	protected with GameObjectLock
 */

/*
	A generic game object. More specific objects would be
	descendants of this class.
	All access to the GameObject class should be protected 
	with GameObjectLock.

	If a game object is a container, its ContainedItem points
	to the first item contained in it. 
	Prev/Next pointers are used only for contained items to 
	find all of them knowing only the first one. For all other 
	items they are zero.
	Characters are considered to be containers. They contain
	all items worn on them, all items on different layers and
	a backpack.
 */
class GameObject
{
protected:
    GameObject * Prev, * Next;	// makes a list of items in the same container
	GameObject * ContainedItem;	// points to the head of that list, if this item is a container

    GameObject * Container;		// object containing this (or NULL)

    uint16 Color;
    uint16 X, Y;		// INVALID_XY if equipped
    int Z;				// (really signed 8 bit)
	int Layer;			// LAYER_NONE if not equipped (really uint8)

    uint16 Quantity;	// zero if unknown or not an item

    uint32 Serial;
    uint16 Graphic;

	GameObject() {}

	bool DecrementContainedCounters(GameObject * Cn);
	bool IncrementContainedCounters(GameObject * Cn);

public:
	GameObject(uint32 Serial);	
	virtual ~GameObject() {}

	bool IsInteresting();	// true - this item is located in backpack or on player (in hand, etc)
    uint16 GetGraphic() const { return Graphic; }
    uint16 GetColor() const { return Color; }
    uint32 GetSerial() const { return Serial; }
    GameObject* GetContainer() const { return Container; }
    void SetGraphic(const uint8 * buf) { SetGraphic(unpack_big_uint16(buf)); }
	void SetGraphic(uint16 graphic) { Graphic = graphic&0x7fff; }
    void SetColor(const uint8 * buf) { Color = unpack_big_uint16(buf); }
    void SetX(const uint8 * buf) { X = unpack_big_uint16(buf)&0x7fff; }
    void SetY(const uint8 * buf) { Y = unpack_big_uint16(buf)&0x3fff; }
    void SetZ(const uint8 * buf) { Z = static_cast<char>(*buf); }
    void SetX(uint16 x) { X = x&0x7fff; }
    void SetY(uint16 y) { Y = y&0x3fff; }
    void SetZ(uint8 z) { Z = static_cast<char>(z); }
    uint16 GetX() { return X; }
    uint16 GetY() { return Y; }
    int    GetZ() { return Z; }

    uint16 GetQuantity() { return Quantity; }
	void SetQuantity(const uint8 * buf) {Quantity = unpack_big_uint16(buf);}
    int  GetLayer() const { return Layer; }
	void SetLayer(int layer) {Layer = layer;}

    // Remove an object from this container.
    void Remove(GameObject * obj);

    // Empty this container.
    void Empty();

    // Add an object to this container. The object must not already be in
    // a container.
    void Add(GameObject * obj);

friend uint32 INJECTION_API FindObjectWithType(uint16 Type, uint16 Color, uint32 Container);
};

/*
 *	Characters inside game. Vendors, monsters, other players & so on.
 */
class Character : public GameObject
{
protected:
	int War;		// 0==peace, 1==war
	uint16 Hitpoints;
	uint16 MaxHitpoints;
	char Name[30];
public:
	Character(uint32 Serial);
	void SetHitpoints(uint16 hp) {Hitpoints = hp;}
	void SetMaxHitpoints(uint16 hp) {MaxHitpoints = hp;}
	uint16 GetHitpoints() {return Hitpoints;}
	uint16 GetMaxHitpoints() {return MaxHitpoints;}
	char *GetName() {return Name;}
	void SetName(const char *N) {strncpy(Name,N,29);}
	void SetName(const uint8 *N) {strncpy(Name,(const char *)N,29);}
	int GetWar() {return War;}
	void SetWar(int w) {War=(w!=0);}
};

class Player : public Character
{
	int Sex;		// 0==male, 1==female, currently unused
	uint16 STR;
	uint16 DEX;
	uint16 INT;
	uint16 Stamina;
	uint16 MaxStamina;	// for some shards it may be != DEX. The same for other MaxXXX
	uint16 Mana;
	uint16 MaxMana;		

	// Hitpoints are derived from Character class

	uint32 Gold;
	uint16 Armor;
	uint16 Weight;
public:
	Player(uint32 Serial);
	int GetSex() {return Sex;}
	void SetSex(int S) {Sex=(S!=0);}
	void SetSTR(uint16 S) {STR=S;}
	void SetDEX(uint16 S) {DEX=S;}
	void SetINT(uint16 S) {INT=S;}
	void SetStamina(uint16 S) {Stamina=S;}
	void SetMaxStamina(uint16 S) {MaxStamina=S;}
	void SetMana(uint16 S) {Mana=S;}
	void SetMaxMana(uint16 S) {MaxMana=S;}
	void SetGold(uint32 G) {Gold=G;}
	void SetArmor(uint16 A) {Armor=A;}
	void SetWeight(uint16 W) {Weight=W;}

	uint16 GetSTR() {return STR;}
	uint16 GetDEX() {return DEX;}
	uint16 GetINT() {return INT;}
	uint16 GetStamina() {return Stamina;}
	uint16 GetMaxStamina() {return MaxStamina;}
	uint16 GetMana() {return Mana;}
	uint16 GetMaxMana() {return MaxMana;}
	uint32 GetGold() {return Gold;}
	uint16 GetArmor() {return Armor;}
	uint16 GetWeight() {return Weight;}
};

/*
 *	The world. A collection of all objects in the world.
 */

static CMap<uint32,uint32,GameObject*,GameObject*> World;
static uint32 BackpackSerial=0;	// Player's backpack

static int Skill[50];
static int SkillBase[50];
static int SkillLock[50];

/*
 *	Item counters
 */

struct CounterObject
{
	uint16 Graphics;
	uint16 Color;
	int Quantity;
	CounterObject() {Graphics=0; Color=0x7fff; Quantity=0;}
	CounterObject(uint16 G, uint16 C=0x7fff) {Graphics=G&0x7fff; Color=C&0x7fff; Quantity=0;}
	bool operator==(const CounterObject &C) const
	{
		return Graphics==C.Graphics && Color==C.Color;
	}
	uint32 GetKey() {return (Graphics<<16) | (Color);}
};

CMap<uint32,uint32,CounterObject,CounterObject> Counters;

bool IncrementCounters(GameObject *obj);	// called on adding an item to container
bool DecrementCounters(GameObject *obj);	// called on removing item
int GetCounter(int Graph, int Color=0x7fff);// get quantity for the given item type and color

// returns the object for the given serial, or NULL if it is not found
GameObject *FindObject(uint32 Serial);

// looks for the object with the given serial in the world and if it
// is not found - creates the new GameObject and adds it to the list.
GameObject *AddObject(uint32 Serial);
// the same for the Character object
Character *AddCharacter(uint32 Serial);

// adds the specified obj to the list. obj may be any descendant of
// GameObject class. If the object with the given serial is already in the
// list - it is overwritten
void AddObject(uint32 Serial, GameObject *obj);

// removes the given object from the list
void RemoveObject(uint32 Serial);

Player * GetPlayer();	// will return NULL before logging in

/*
 *	Implementation
 */

GameObject::GameObject(uint32 Ser)
{
	Prev = Next = ContainedItem = Container = NULL;
	Color = 0;
	X = Y = INVALID_XY;
	Z = 0;
	Quantity = 0;
	Layer = LAYER_NONE;
	Graphic = 0;
	Serial = Ser&0x7fffffff;
}

void GameObject::Empty()
{
	GameObjectLock tmp;
	while(ContainedItem)
		RemoveObject(ContainedItem->GetSerial());
}

bool GameObject::IncrementContainedCounters(GameObject * Cn)
{
	if(Cn==0)
		return false;

	GameObjectLock tmp;
	bool ret=false;
	GameObject *obj=Cn;

	do{
		ret|=IncrementCounters(Cn);
		ret|=IncrementContainedCounters(Cn->ContainedItem);
		Cn=Cn->Next;
	} while(Cn);
	return ret;
}

bool GameObject::DecrementContainedCounters(GameObject * Cn)
{
	if(Cn==0)
		return false;

	GameObjectLock tmp;
	bool ret=false;
	GameObject *obj=Cn;

	do{
		ret|=DecrementCounters(Cn);
		ret|=DecrementContainedCounters(Cn->ContainedItem);
		Cn=Cn->Next;
	} while(Cn);

	return ret;
}

void GameObject::Add(GameObject * obj)
{
	if(obj->Container == this)
		return;

	GameObjectLock tmp;

	if(obj->Container)	// remove obj trom its prev. container
		obj->Container->Remove(obj);

	if(ContainedItem == NULL)	// if this container is empty...
	{
		ContainedItem = obj;
		obj->Prev = obj->Next = 0;
	} else
	{
		GameObject *tmp=ContainedItem;	// add to the end of a list
		
		while(tmp->Next)
			tmp=tmp->Next;

		tmp->Next=obj;
		obj->Prev=tmp;
		obj->Next=NULL;
	}
	obj->Container = this;
	if(obj->IsInteresting())
	{
		if(IncrementCounters(obj) || IncrementContainedCounters(obj->ContainedItem))
			UpdateClientCaption();

		// if this container is interesting - get its contents
		if(obj->ContainedItem==NULL && (obj->GetGraphic()==I_BACKPACK || obj->GetGraphic()==I_BAG))
			CountItemsInContainer(obj->GetSerial());
	}
}

void GameObject::Remove(GameObject * obj)
{
	GameObjectLock tmp;
	ASSERT(this == obj->Container);

	if(obj->IsInteresting())
		if(DecrementCounters(obj) || DecrementContainedCounters(obj->ContainedItem))
			UpdateClientCaption();

	if(obj->Prev)
		obj->Prev->Next=obj->Next;
	else
		ContainedItem = obj->Next;

	if(obj->Next)
		obj->Next->Prev=obj->Prev;

	obj->Prev = obj->Next = obj->Container = NULL;
}

bool GameObject::IsInteresting()
{
	GameObjectLock tmpL;
	GameObject *tmp=Container;
	while(tmp)
	{
		if(tmp->GetSerial()==GetPlayerSerial())
			return true;
		tmp=tmp->GetContainer();
	}
	return false;
}

Character::Character(uint32 Ser) : GameObject(Ser) 
{
	Hitpoints = MaxHitpoints = 0;
	Name[sizeof(Name)-1]=0;			// set tle last byte to zero, for strncpy to work
	sprintf(Name,"#%08x",Serial);	// let the default name == serial. Would be overwritten when server will send it
	War=0;
}

Player::Player(uint32 Serial) : Character(Serial) 
{
	STR=DEX=INT=0;
	Stamina=MaxStamina=0;
	Mana=MaxMana=0;
	Sex=0;
	Gold=0;
	Armor=0;
	Weight=0;
}

/*
 *	Supplementary functions
 */

void AddCounter(int Graph, int Color=0x7fff)	// Color==0x7fff -> any color
{
	CounterObject o(Graph,Color);
	CounterObject tmp;

	if(Counters.Lookup(o.GetKey(),tmp))	// don't add the same counter for the second time
		return;

	Counters.SetAt(o.GetKey(),o);
}

bool IncrementCounters(GameObject *obj)
{
	ASSERT(obj->IsInteresting());

	CounterObject o(obj->GetGraphic(),obj->GetColor());

	if(Counters.Lookup(o.GetKey(),o))
	{
		o.Quantity+=obj->GetQuantity();
		Counters.SetAt(o.GetKey(),o);
		return true;
	}
	else
	{
		o.Color=0x7fff;
		if(Counters.Lookup(o.GetKey(),o))
		{
			o.Quantity+=obj->GetQuantity();
			Counters.SetAt(o.GetKey(),o);
			return true;
		}
	}
	return false;
}

bool DecrementCounters(GameObject *obj)
{
	ASSERT(obj->IsInteresting());

	CounterObject o(obj->GetGraphic(),obj->GetColor());

	if(Counters.Lookup(o.GetKey(),o))
	{
		o.Quantity-=obj->GetQuantity();
		if(o.Quantity<0)
			o.Quantity=0;
		Counters.SetAt(o.GetKey(),o);
		return true;
	}
	else
	{
		o.Color=0x7fff;
		if(Counters.Lookup(o.GetKey(),o))
		{
			o.Quantity-=obj->GetQuantity();
			if(o.Quantity<0)
				o.Quantity=0;
			Counters.SetAt(o.GetKey(),o);
			return true;
		}
	}
	return false;
}

int GetCounter(int Graph, int Color)
{
	CounterObject o(Graph,Color);
	Counters.Lookup(o.GetKey(),o);
	return o.Quantity;
}

void DeleteWorld()
{
	GameObjectLock tmpL;
	GameObject *obj;

    POSITION pos = World.GetStartPosition();
	uint32 Key;
	while (pos != NULL)
	{
		World.GetNextAssoc( pos, Key, obj);
		World.RemoveKey(Key);
		delete obj;
	}	
	ASSERT(World.GetCount()==0);
	Counters.RemoveAll();
}

void InitWorld()
{
	GameObjectLock tmpL;
	static bool HashInited=false;
	if(!HashInited)
	{
		World.InitHashTable(67211);		//it think that slightly >64K is enough (or set 122867?)
		Counters.InitHashTable(257);
		HashInited=true;
	}

	DeleteWorld();

	// insert a player object to the list
	World.SetAt(GetPlayerSerial(),new Player(GetPlayerSerial()));
	
	// add default counters
	AddCounter(GRAPHIC_BANDAGES);	
	AddCounter(GRAPHIC_CURE_POTION);	
	AddCounter(GRAPHIC_MANA_POTION);	
	AddCounter(GRAPHIC_LAVA_POTION);	
	AddCounter(GRAPHIC_HEAL_POTION);	
	AddCounter(GRAPHIC_ARROWS);	
	AddCounter(GRAPHIC_BLACKMOOR);	
	AddCounter(GRAPHIC_BLACK_PEARL);	
	AddCounter(GRAPHIC_BLOOD_MOSS);	
	AddCounter(GRAPHIC_FERTILE_DIRT);	
	AddCounter(GRAPHIC_GARLIC);	
	AddCounter(GRAPHIC_GINSENG);	
	AddCounter(GRAPHIC_MANDRAKE_ROOT);	
	AddCounter(GRAPHIC_EYE_OF_NEWT);	
	AddCounter(GRAPHIC_NIGHTSHADE);	
	AddCounter(GRAPHIC_SULFUROUS_ASH);	
	AddCounter(GRAPHIC_SPIDERS_SILK);	
	AddCounter(GRAPHIC_VOLCANIC_ASH);	
	AddCounter(GRAPHIC_WYRMS_HEART);	
	AddCounter(GRAPHIC_BOLT);	
}

/*
 *	Functions used to hide "open container" message from a client
 *	(used mainly during item counting)
 */
static CList<uint32> HideList;

/*
 *	Add the given container to the hide list
 */
void HideOpenContainer(uint32 Serial)
{
	HideList.AddTail(Serial);
}

/*
 *	Remove the given container from a list and return TRUE if it
 *	was in the list.
 */
bool IsHiddenOpening(uint32 Serial)
{
	POSITION p=HideList.Find(Serial);
	if(p)
	{
		HideList.RemoveAt(p);
		return true;
	}
	return false;
}

void CountItemsInContainer(uint32 Serial)
{
	if(!GUIThread->MainTab->CbAutoCountItems)
		return;

	trace_printf("Counting items in container %08X\n",Serial);

	HideOpenContainer(Serial);

	uint8 Buff[5]="\x06";
	*((DWORD*)(Buff+1))=htonl(Serial);
	SendToServer(Buff,5);
}

GameObject *FindObject(uint32 Serial)
{
	Serial&=0x7fffffff;
	GameObject *obj=NULL;
	World.Lookup(Serial,obj);
	return obj;
}

// looks for the object with the given serial in the world and if it
// is not found - creates the new GameObject and adds it to the list.
GameObject *AddObject(uint32 Serial)
{
	Serial&=0x7fffffff;
	GameObjectLock tmpL;
	GameObject *obj=NULL;
	if(World.Lookup(Serial,obj))
		return obj;
	obj=new GameObject(Serial);
	World.SetAt(Serial,obj);
	return obj;
}

Character *AddCharacter(uint32 Serial)
{
	Serial&=0x7fffffff;
	GameObjectLock tmpL;
	GameObject *obj=NULL;
	if(World.Lookup(Serial,obj))
	{
		if(dynamic_cast<Character*>(obj)!=0)
			return (Character*)obj;
		else 
		{
			RemoveObject(obj->GetSerial());
		}
	}
	obj=new Character(Serial);
	World.SetAt(Serial,obj);
	{
		// try to request the name of this char
		uint8 Buff[]="\x34\xed\xed\xed\xed\x04\x00\x00\x8c\xd9";
		*((DWORD*)(Buff+6))=htonl(Serial);
		SendToServer(Buff,10);
	}
	return (Character*)obj;
}


// adds the specified obj to the list. obj may be any descendant of
// GameObject class
void AddObject(uint32 Serial, GameObject *obj)
{
	Serial&=0x7fffffff;
	GameObjectLock tmpL;
	GameObject *tmp=FindObject(Serial);
	if(tmp)
		RemoveObject(tmp->GetSerial());
	World.SetAt(Serial,obj);
}

void RemoveObject(uint32 Serial)
{
	Serial&=0x7fffffff;
	// When GM changes anyting in player state - sphere server
	// sends the delete message with player serial.
	// Do not allow to delete a player!
	if(Serial==GetPlayerSerial())
		return;

	GameObjectLock tmpL;
	GameObject *tmp=FindObject(Serial);

	if(tmp==0)
		return;

	tmp->Empty();	// empty this container
	if(tmp->GetContainer())	// remove it from other container
		tmp->GetContainer()->Remove(tmp);
	delete tmp;
	World.RemoveKey(Serial);
}

Player * GetPlayer()
{
	GameObjectLock tmpL;
	GameObject *tmp = FindObject(GetPlayerSerial());
	return dynamic_cast<Player*>(tmp);
}

/*
 *	Server message handlers
 */

bool INJECTION_CALLBACK OnCharStatus(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+3);
 
	Character *Ch=AddCharacter(Ser);

	Ch->SetHitpoints(unpack_big_uint16(Msg+37));
	Ch->SetMaxHitpoints(unpack_big_uint16(Msg+39)); 
	Ch->SetName(Msg+7);

	if(Ch->GetSerial() != GetPlayerSerial())
	{
		if(GUIThread->MainTab->DisplayHitpoints)
		{
			if(Ch->GetMaxHitpoints()!=100)
				ClientPrintColor(0x3b2,3,"%s has %d/%d hitpoints",
					Ch->GetName(),Ch->GetHitpoints(),Ch->GetMaxHitpoints());
			else	// TAR shard sends percentage of hitpoints. So if max HP is 100 - display it as %
				ClientPrintColor(0x3b2,3,"%s has %d%% hitpoints",
					Ch->GetName(),Ch->GetHitpoints()*100/Ch->GetMaxHitpoints());
		}
	}
	if(Len>0x2a && (Msg[0x2a]&1)!=0)
	{
		Player *Pl=dynamic_cast<Player*>(Ch);
		ASSERT(Pl!=0);
		Pl->SetSTR(unpack_big_uint16(Msg+0x2c));
		Pl->SetDEX(unpack_big_uint16(Msg+0x2c+2));
		Pl->SetINT(unpack_big_uint16(Msg+0x2c+4));
		Pl->SetStamina(unpack_big_uint16(Msg+0x2c+6));
		Pl->SetMaxStamina(unpack_big_uint16(Msg+0x2c+8));
		Pl->SetMana(unpack_big_uint16(Msg+0x2c+10));
		Pl->SetMaxMana(unpack_big_uint16(Msg+0x2c+12));
		Pl->SetGold(unpack_big_uint32(Msg+0x2c+14));
		Pl->SetArmor(unpack_big_uint16(Msg+0x2c+18));
		Pl->SetWeight(unpack_big_uint16(Msg+0x2c+20));
		UpdateClientCaption();
	}

	return false;
}

bool UpdateClientCaption()
{
	if(DisplayTab == 0)
		return false;

	GameObjectLock tmpL;
	Player *P=GetPlayer();
	if(P==0)
		return false;

	char Buff[256];
	GetWindowText(GetClientWindow(),Buff,256);
	char Buff1[256];

	wsprintf(Buff1,"UO -");
	if(DisplayTab->ShowHitpoints)
		wsprintf(Buff1+strlen(Buff1)," h:%d/%d",P->GetHitpoints(), P->GetMaxHitpoints());
	if(DisplayTab->ShowMana)
		wsprintf(Buff1+strlen(Buff1)," m:%d/%d",P->GetMana(), P->GetMaxMana());
	if(DisplayTab->ShowStamina)
		wsprintf(Buff1+strlen(Buff1)," s:%d/%d",P->GetStamina(), P->GetMaxStamina());
	if(DisplayTab->ShowAR)
		wsprintf(Buff1+strlen(Buff1)," ar:%d",P->GetArmor());
	if(DisplayTab->ShowWeight)
		wsprintf(Buff1+strlen(Buff1)," w:%d",P->GetWeight());
	if(DisplayTab->ShowGold)
		wsprintf(Buff1+strlen(Buff1)," g:%d",P->GetGold());

	if(DisplayTab->ShowArrows || DisplayTab->ShowBolts)
		strcat(Buff1," - ");
	if(DisplayTab->ShowArrows)
		wsprintf(Buff1+strlen(Buff1)," a:%d",GetCounter(GRAPHIC_ARROWS));
	if(DisplayTab->ShowBolts)
		wsprintf(Buff1+strlen(Buff1)," b:%d",GetCounter(GRAPHIC_BOLT));

	if(DisplayTab->ShowBM ||
		DisplayTab->ShowBP ||
		DisplayTab->ShowGL ||
		DisplayTab->ShowGS ||
		DisplayTab->ShowMR ||
		DisplayTab->ShowNS ||
		DisplayTab->ShowSA ||
		DisplayTab->ShowSS)
			strcat(Buff1," - ");
	if(DisplayTab->ShowBM)
		wsprintf(Buff1+strlen(Buff1)," bm:%d",GetCounter(GRAPHIC_BLOOD_MOSS));
	if(DisplayTab->ShowBP)
		wsprintf(Buff1+strlen(Buff1)," bp:%d",GetCounter(GRAPHIC_BLACK_PEARL));
	if(DisplayTab->ShowGL)
		wsprintf(Buff1+strlen(Buff1)," gl:%d",GetCounter(GRAPHIC_GARLIC));
	if(DisplayTab->ShowGS)
		wsprintf(Buff1+strlen(Buff1)," gs:%d",GetCounter(GRAPHIC_GINSENG));
	if(DisplayTab->ShowMR)
		wsprintf(Buff1+strlen(Buff1)," mr:%d",GetCounter(GRAPHIC_MANDRAKE_ROOT));
	if(DisplayTab->ShowNS)
		wsprintf(Buff1+strlen(Buff1)," ns:%d",GetCounter(GRAPHIC_NIGHTSHADE));
	if(DisplayTab->ShowSA)
		wsprintf(Buff1+strlen(Buff1)," sa:%d",GetCounter(GRAPHIC_SULFUROUS_ASH));
	if(DisplayTab->ShowSS)
		wsprintf(Buff1+strlen(Buff1)," ss:%d",GetCounter(GRAPHIC_SPIDERS_SILK));

	if(DisplayTab->ShowVAsh ||
		DisplayTab->ShowEon ||
		DisplayTab->ShowWHeart ||
		DisplayTab->ShowFDirt ||
		DisplayTab->ShowBMoor)
			strcat(Buff1," - ");
	if(DisplayTab->ShowVAsh)
		wsprintf(Buff1+strlen(Buff1)," va:%d",GetCounter(GRAPHIC_VOLCANIC_ASH));
	if(DisplayTab->ShowEon)
		wsprintf(Buff1+strlen(Buff1)," en:%d",GetCounter(GRAPHIC_EYE_OF_NEWT));
	if(DisplayTab->ShowWHeart)
		wsprintf(Buff1+strlen(Buff1)," wh:%d",GetCounter(GRAPHIC_WYRMS_HEART));
	if(DisplayTab->ShowFDirt)
		wsprintf(Buff1+strlen(Buff1)," fd:%d",GetCounter(GRAPHIC_FERTILE_DIRT));
	if(DisplayTab->ShowBMoor)
		wsprintf(Buff1+strlen(Buff1)," br:%d",GetCounter(GRAPHIC_BLACKMOOR));

	if(DisplayTab->ShowBandages || 
		DisplayTab->ShowHealPotions ||
		DisplayTab->ShowManaPotions ||
		DisplayTab->ShowCurePotions)
			strcat(Buff1," - ");
	if(DisplayTab->ShowBandages)
		wsprintf(Buff1+strlen(Buff1)," bg:%d",GetCounter(GRAPHIC_BANDAGES));
	if(DisplayTab->ShowHealPotions)
		wsprintf(Buff1+strlen(Buff1)," hp:%d",GetCounter(GRAPHIC_HEAL_POTION));
	if(DisplayTab->ShowManaPotions)
		wsprintf(Buff1+strlen(Buff1)," mp:%d",GetCounter(GRAPHIC_MANA_POTION));
	if(DisplayTab->ShowCurePotions)
		wsprintf(Buff1+strlen(Buff1)," cp:%d",GetCounter(GRAPHIC_CURE_POTION));

	if(strcmpi(Buff,Buff1)!=0)
		SetWindowText(GetClientWindow(),Buff1);
	return true;
}

bool INJECTION_CALLBACK OnUpdateItem(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+3);
	GameObject *obj=AddObject(Ser);
	obj->SetGraphic(unpack_big_uint16(Msg+7));
	if(Ser&0x80000000)
	{
		if(obj->GetGraphic()!=0x620)	// not corpse
			obj->SetQuantity(Msg+9);
		Msg+=2;	// skip
	}
	if(obj->GetGraphic()&0x8000)
		Msg++;

	obj->SetX(Msg+9);
	obj->SetY(Msg+11);
	if(unpack_big_uint16(Msg+9)&0x8000)
		Msg++;
	obj->SetZ(Msg[13]);
	if(unpack_big_uint16(Msg+11)&0x8000)
	{
		obj->SetColor(Msg+14);
	}

	return false;
}

bool INJECTION_CALLBACK OnDeleteItem(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+1);

	RemoveObject(Ser);
	return false;
}

bool INJECTION_CALLBACK OnUpdateCreature(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+1);

	Character *Ch=AddCharacter(Ser);

	Ch->SetGraphic(Msg+5);
	Ch->SetColor(Msg+8);
	Ch->SetWar(Msg[10]&0x40);

	Ch->SetX(Msg+11);
	Ch->SetY(Msg+13);
	Ch->SetZ(Msg+13+5);

	return false;
}

bool INJECTION_CALLBACK OnOpenContainer(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+1);
	GameObject *Container=AddObject(Ser);

	Container->Empty();	// next server message would fill this container with items

	return IsHiddenOpening(Ser);
}

bool INJECTION_CALLBACK OnAddToContainer(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+1);
	GameObject *obj=AddObject(Ser);

	obj->SetGraphic(Msg+5);
	obj->SetQuantity(Msg+8);
	obj->SetX(Msg+10);
	obj->SetY(Msg+12);
	obj->SetColor(Msg+14+4);

	GameObject *Container=AddObject(unpack_big_uint32(Msg+14));
	Container->Add(obj);

	return false;
}

bool INJECTION_CALLBACK OnClearSquare(const uint8 IN *Msg, int IN Len)
{
	return false;
}

bool INJECTION_CALLBACK OnEquipItem(const uint8 IN *Msg, int IN Len)
{
	// this message is received for items drawn on the chars
	// so place them to the player container
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+1);
	GameObject *tmp=AddObject(Ser);

	tmp->SetGraphic(Msg+5);
	tmp->SetLayer(Msg[8]);
	tmp->SetColor(Msg+9+4);

	uint32 OwnerS=unpack_big_uint32(Msg+9);
	GameObject *Owner=AddCharacter(OwnerS);

	if(tmp->GetContainer())	// if this object was in wrong container - remove it from there
	{
		if(tmp->GetContainer()!=Owner)
			tmp->GetContainer()->Remove(tmp);
	}

	if(tmp->GetContainer()==0)	// place to the player if it is not already there
		Owner->Add(tmp);

	return false;
}

bool INJECTION_CALLBACK OnUpdateSkills(const uint8 IN *Msg, int IN Len)
{
	if(Msg[3]!=0)
	{
		int SkillNo=(unpack_big_uint16(Msg+4))%50;
		Skill[SkillNo]=unpack_big_uint16(Msg+6);
		SkillBase[SkillNo]=unpack_big_uint16(Msg+8);
		SkillLock[SkillNo]=Msg[10];
		return false;
	} 

	for(int i=0; i<(Len-4)/7; i++)
	{
		int SkillNo=(unpack_big_uint16(Msg+4+i*7)-1)%50;
		Skill[SkillNo]=unpack_big_uint16(Msg+6+i*7);
		SkillBase[SkillNo]=unpack_big_uint16(Msg+8+i*7);
		SkillLock[SkillNo]=Msg[10+i*7];
	}

	return false;
}

bool INJECTION_CALLBACK OnUpdateContainer(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	int NItems=unpack_big_uint16(Msg+3);
	uint32 Ser=unpack_big_uint32(Msg+5+4+3+6);
	GameObject *Container=AddObject(Ser);

//	Container->Empty();	- it is emptied on OpenContainer

	for(int i=0; i<NItems; i++)
	{
		uint32 Ser1=unpack_big_uint32(Msg+5+4+3+6+(4+3+6+6)*i);
		ASSERT(Ser==Ser1);	// A packet should describe items in one container
		uint32 Item=unpack_big_uint32(Msg+5+(4+3+6+6)*i);
		GameObject *obj=AddObject(Item);
		obj->SetGraphic(Msg+5+4+(4+3+6+6)*i);
		obj->SetQuantity(Msg+5+4+3+(4+3+6+6)*i);
		obj->SetX(Msg+5+4+3+2+(4+3+6+6)*i);
		obj->SetY(Msg+5+4+3+2+2+(4+3+6+6)*i);
		obj->SetColor(Msg+5+4+3+2+6+2+(4+3+6+6)*i);
		Container->Add(obj);
	}

	return false;
}

bool INJECTION_CALLBACK OnUpdatePlayerPos(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+1);
	Character *Ch=AddCharacter(Ser);

	Ch->SetX(unpack_big_uint16(Msg+7));
	Ch->SetY(unpack_big_uint16(Msg+9));
	Ch->SetZ(Msg[11]);
	Ch->SetColor(Msg+13);

	return false;
}

bool INJECTION_CALLBACK OnUpdatePlayer(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+3);
	Character *Ch=AddCharacter(Ser);

	Msg+=7;
	Ch->SetGraphic(Msg); 
	Msg+=2;
	if(Ser&0x80000000)
	{
		Ch->SetColor(Msg);
		Msg+=2;
	}
	int X=unpack_big_uint16(Msg);
	Ch->SetX(Msg);
	Msg+=2;
	Ch->SetY(Msg);
	Msg+=2;
	if(X&0x8000)	// skip direction
		Msg++;
	Ch->SetZ(*Msg);
	Msg+=2;
	Ch->SetColor(Msg);
	Msg+=2;
	Ch->SetWar((*Msg)&0x40);
	Msg+=2;	// skip notoriety
	while(*((DWORD*)Msg))
	{
		uint32 Item=unpack_big_uint32(Msg);
		Msg+=4;
		uint16 Model=unpack_big_uint16(Msg);
		GameObject *obj=AddObject(Item);
		obj->SetGraphic(Msg);
		Msg+=2;
		obj->SetLayer(*Msg);
		Msg++;
		if(Model&0x8000)
		{
			obj->SetColor(Msg);
			Msg+=2;
		}
		if(Ser==GetPlayerSerial() && obj->GetLayer()==LAYER_BACKPACK)
			BackpackSerial = obj->GetSerial();
		Ch->Add(obj); 
	}

	return false;
}

bool INJECTION_CALLBACK OnUpdateHealth(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+1);
	Character *Ch=AddCharacter(Ser);

	Ch->SetMaxHitpoints(unpack_big_uint16(Msg+5));
	Ch->SetHitpoints(unpack_big_uint16(Msg+7));

	if(Ch->GetSerial() != GetPlayerSerial())
	{
		if(GUIThread->MainTab->DisplayHitpoints)
		{
			if(Ch->GetMaxHitpoints()!=100)
				ClientPrintColor(0x3b2,3,"%s has %d/%d hitpoints",
					Ch->GetName(),Ch->GetHitpoints(),Ch->GetMaxHitpoints());
			else	// TAR shard sends percentage of hitpoints. So if max HP is 100 - display it as %
				ClientPrintColor(0x3b2,3,"%s has %d%% hitpoints",
					Ch->GetName(),Ch->GetHitpoints()*100/Ch->GetMaxHitpoints());
		}
	} else
		UpdateClientCaption();

	return false;
}

bool INJECTION_CALLBACK OnUpdateMana(const uint8 IN *Msg, int IN Len)
{
	uint32 Ser=unpack_big_uint32(Msg+1);

	if(Ser != GetPlayerSerial())
	{
		trace_printf("SERVER BUG: Mana for NPC %08x %d/%d\n",
			Ser,unpack_big_uint16(Msg+5),unpack_big_uint16(Msg+7));
	} else
	{
		GameObjectLock tmpL;
		Player *Pl=GetPlayer();

		Pl->SetMaxMana(unpack_big_uint16(Msg+5));
		Pl->SetMana(unpack_big_uint16(Msg+7));
		UpdateClientCaption();
	}

	return false;
}
bool INJECTION_CALLBACK OnUpdateStamina(const uint8 IN *Msg, int IN Len)
{
	uint32 Ser=unpack_big_uint32(Msg+1);

	if(Ser != GetPlayerSerial())
	{
		trace_printf("SERVER BUG: Stamina for NPC %08x %d/%d\n",
			Ser,unpack_big_uint16(Msg+5),unpack_big_uint16(Msg+7));
	} else
	{
		GameObjectLock tmpL;
		Player *Pl=GetPlayer();

		Pl->SetMaxStamina(unpack_big_uint16(Msg+5));
		Pl->SetStamina(unpack_big_uint16(Msg+7));
		UpdateClientCaption();
	}

	return false;
}

bool INJECTION_CALLBACK OnWarPeace(const uint8 IN *Msg, int IN Len)
{
	GetPlayer()->SetWar(Msg[1]);
	return false;
}

bool INJECTION_CALLBACK OnPaperdoll(const uint8 IN *Msg, int IN Len)
{
	GameObjectLock tmpL;
	uint32 Ser=unpack_big_uint32(Msg+1);
	Character *Ch=AddCharacter(Ser);

	Ch->SetWar(Msg[65]);
	return false;
}


void DumpObject(GameObject *obj)
{
	GameObjectLock tmpL;
	trace_printf("Serial=%08X, Graphic=%04X, Color=%04X\n",
		obj->GetSerial(),obj->GetGraphic(),obj->GetColor());
	trace_printf("X=%d, Y=%d, Z=%d, Layer=%d\n",
		obj->GetX(),obj->GetY(),obj->GetZ(),obj->GetLayer());
	trace_printf("Quantity=%d\n",obj->GetQuantity());
	if(obj->GetContainer())
		trace_printf("Contained in: %08X\n",obj->GetContainer()->GetSerial());
}

void DumpCharacter(Character *obj)
{
	GameObjectLock tmpL;
	trace_printf("Hitpoints=%d/%d\n",obj->GetHitpoints(),obj->GetMaxHitpoints());
	trace_printf("In war=%d\n",obj->GetWar());
	trace_printf("Name=%s\n",obj->GetName());
	DumpObject(obj);
}

void DumpPlayer(Player *obj)
{
	GameObjectLock tmpL;

	trace_printf("Sex=%s\n",obj->GetSex()?"female":"male");
	trace_printf("Gold=%d, Armor=%d, Weight=%d\n",
		obj->GetGold(),obj->GetArmor(),obj->GetWeight());
	trace_printf("STR=%d, INT=%d, DEX=%d\n",obj->GetSTR(),
		obj->GetINT(), obj->GetDEX());
	trace_printf("Stamina=%d/%d, Mana=%d/%d\n",
		obj->GetStamina(),obj->GetMaxStamina(),obj->GetMana(),obj->GetMaxMana());
	DumpCharacter(obj);
}

void DumpWorldToLog()
{
	GameObjectLock tmpL;

	GameObject *obj;

	// first dump the player
	trace_printf("Starting world dump\n\n");
	DumpPlayer(GetPlayer());

	// then all chars
    POSITION pos = World.GetStartPosition();
	uint32 Key;
	while (pos != NULL)
	{
		World.GetNextAssoc( pos, Key, obj);
		if(dynamic_cast<Character*>(obj) && GetPlayer()!=obj)
		{
			trace_printf("-------\n");
			DumpCharacter((Character*)obj);
		}
	}
	// and then all items
    pos = World.GetStartPosition();
	while (pos != NULL)
	{
		World.GetNextAssoc( pos, Key, obj);
		if(dynamic_cast<Character*>(obj)==0)
		{
			trace_printf("-------\n");
			DumpObject(obj);
		}
	}
	trace_printf("\nWorld dump finished\n");
	ClientPrint("%d items in memory before cleaning up",World.GetCount()); 
	ForceCleanupWorld();
	ClientPrint("%d items in memory after cleaning up",World.GetCount()); 
}

void CleanupWorld()
{
	if(World.GetCount()>32767)
		ForceCleanupWorld();
}

/*
 *	remove all items from the world that are farther than 64 squares
 *	from player
 */
void ForceCleanupWorld()
{
	GameObjectLock tmpL;
	GameObject *obj;

    POSITION pos = World.GetStartPosition();
	uint32 Key;
	int X=GetPlayer()->GetX();
	int Y=GetPlayer()->GetY();
	RECT r={X-64,Y-64,X+64,Y+64};
	while (pos != NULL)
	{
		World.GetNextAssoc( pos, Key, obj);

		// do not delete objects that are in containers or 
		// special objects. 
		if(GetPlayer()==obj || obj->GetContainer()) /* || 
			obj->GetLayer() || (obj->GetX()<128 && obj->GetY()<128) ||
			(obj->GetX()==INVALID_XY && obj->GetY()==INVALID_XY))*/
			continue;
 
		POINT p={obj->GetX(),obj->GetY()};
		if(!PtInRect(&r,p))
		{
			RemoveObject(obj->GetSerial());
			// after removing an object we need to restart 
			// from the beginning, because RemoveObject could
			// delete items before the current one and 'pos'
			// could become invalid
			pos = World.GetStartPosition();
		}
	}
}

DWORD INJECTION_API GetBackpackSerial()
{
	return BackpackSerial;
}

int INJECTION_API GetSkill(int Num)
{
	return Skill[Num%50];
}

int INJECTION_API GetSkillBase(int Num)
{
	return SkillBase[Num%50];
}

int INJECTION_API GetSkillLock(int Num)
{
	return SkillLock[Num%50];
}

void INJECTION_API SetSkillLock(int Skill, int Lock)
{
	SkillLock[Skill%50]=Lock%3; 
	uint8 Buff[6]="\x3a\x00\x06\x00";
	Buff[4]=Skill%50;
	Buff[5]=Lock%3;
	SendToServer(Buff,6);
}


UINT CmdInfoThreadProc( LPVOID pParam )
{
	ClientPrint("Select Target...");
	uint32 Serial=SelectItem();
	if(Serial==INVALID_SERIAL)
		ClientPrintWarning("Targetting cancelled");
	else 
	{
		GameObjectLock tmpL;
		GameObject *obj=AddObject(Serial);
		uint8 Buff[1024];
		memset(Buff,0,1024);

		// BUG: UOTD does not show a scroll with info, so I
		// duplicate its output with ClientPrint
		ClientPrint("Serial=%08X, Graphic=%04X, Color=%04X",
			obj->GetSerial(),obj->GetGraphic(),obj->GetColor());
		ClientPrint("X=%d, Y=%d, Z=%d, Layer=%d",
			obj->GetX(),obj->GetY(),obj->GetZ(),obj->GetLayer());
		ClientPrint("Quantity=%d",obj->GetQuantity());
		if(obj->GetContainer())
			ClientPrint("Contained in: %08X",obj->GetContainer()->GetSerial());

		sprintf((char*)Buff+10,"Object information:\r\n\rSerial=%08X\n\rGraphic=%04X\n\rColor=%04X\n\r"
			"X=%d, Y=%d, Z=%d\n\rLayer=%d\n\r"
			"Quantity=%d\n\r",
			obj->GetSerial(),obj->GetGraphic(),obj->GetColor(),
			obj->GetX(),obj->GetY(),obj->GetZ(),obj->GetLayer(),
			obj->GetQuantity());
		if(obj->GetContainer())
			sprintf((char*)Buff+10+strlen((char*)Buff+10),"Container: %08X",obj->GetContainer()->GetSerial());
		sprintf((char*)Buff+10+strlen((char*)Buff+10),"\r\n\r\n\r\n\r\n\r");
		Buff[0]=0xa6;
		int Len=strlen((char*)Buff+10)+1;
		Buff[1]=Len/256;
		Buff[2]=Len%256;
		Buff[3]=2;
		Buff[8]=(Len-10)/256;
		Buff[9]=(Len-10)%256;
		SendToClient(Buff,Len);	// show the scroll
	}
	return 0;
}

void INJECTION_CALLBACK CmdInfo(int IN argc, char IN **argv)
{
	BeginHighLevelApiThread(CmdInfoThreadProc,0);
}

BOOL INJECTION_API GetObjectInformation(uint32 IN Serial, ObjectInformation * OUT Info)
{
	if(Info == 0)
		return FALSE;
	GameObjectLock tmpL;
	GameObject *obj;
	memset(Info,0,sizeof(ObjectInformation));
	Info->Serial=INVALID_SERIAL;
	obj=FindObject(Serial);
	if(obj==0)
		return FALSE;
	Info->Serial=obj->GetSerial();
	Info->Graphic=obj->GetGraphic();
	Info->Color=obj->GetColor();
	Info->Layer=obj->GetLayer();
	Info->Quantity=obj->GetQuantity();
	Info->X=obj->GetX();
	Info->Y=obj->GetY();
	Info->Z=obj->GetZ();
	if(obj->GetContainer())
		Info->Container=obj->GetContainer()->GetSerial();

	return TRUE;
}

uint32 INJECTION_API FindObjectWithType(uint16 Type, uint16 Color, uint32 Container)
{
	GameObjectLock tmpL;
	GameObject *obj=FindObject(Container);
	if(obj==0)
		return INVALID_SERIAL;
	GameObject *i=obj->ContainedItem;
	uint32 ret=INVALID_SERIAL;
	while (i && ret==INVALID_SERIAL)
	{
		// if this item is a container - process it also
		if(i->ContainedItem)
			ret=FindObjectWithType(Type, Color, i->Serial);

		if(i->GetGraphic()==Type && (Color==0 || i->Color==Color))
			ret=i->Serial;

		i=i->Next;
	}
	return ret;
}
