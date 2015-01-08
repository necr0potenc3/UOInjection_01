// thanks to RElf (spam4relf@chat.ru)

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include "getkeys.h"
#include "api.h"

// Load long from address C into LL with bytes swapped.
#define N2L(C, LL) \
    LL  = ((unsigned int)(*((C)++))) << 24, \
    LL |= ((unsigned int)(*((C)++))) << 16, \
    LL |= ((unsigned int)(*((C)++))) << 8, \
    LL |= ((unsigned int)(*((C)++)))

#define SIZE 62 	// we need less than 32 bytes to get k0, but we need at least 40 bytes to get k1...

static DWORD key0[62+1];
static DWORD key1[62+1];
static DWORD k0=0;
static DWORD k1=0;
static DWORD k30=0;	// wold be (L_{31}(k0) shr 1) ^ L_{30}(k0).

static void init(unsigned char * pseed)
{
    unsigned int seed;
    N2L(pseed, seed);

    key0[0] =
            (((~seed) ^ 0x00001357) << 16)
        |   ((seed ^ 0xffffaaaa) & 0x0000ffff);
    key1[0] =
            ((seed ^ 0x43210000) >> 16)
        |   (((~seed) ^ 0xabcdffff) & 0xffff0000);
}

static void crypt(BYTE *buff, int len)
{
	int m_key0=key0[0];
	int m_key1=key1[0];
    for(int i = 0; i < len; i++)
    {
        buff[i] = buff[i] ^ static_cast<unsigned char>(m_key0);

        unsigned int table0 = m_key0;
        unsigned int table1 = m_key1;

        m_key1 =
            (
                (
                    (
                        ((table1 >> 1) | (table0 << 31))
                    )
                    >> 1
                )
                | (table0 << 31)
            ) ^ k30;
        m_key0 = ((table0 >> 1) | (table1 << 31)) ^ k1;
    }
}

// low a bits of b
#define L(a,b) ((b) & (((__int64)1<<((a)+1))-1))
// a-st bit of b
#define B(a,b) (((b) >> (a)) & 1)

// everything goes from these formulas:
// key1[n+1] = ((( ((key1[n]>>1) ^ (key0[n]<<31)) ^ k0)>>1 )) ^ (key0[n]<<31) ) ^ k0;
// key0[n+1] = ((key0[n]>>1) ^ (key1[n]<<31)) ^ k1;
void CalculateKeys(BYTE Plaintext[62],BYTE Ciphertext[62])
{
	memset(key0,0,sizeof(key0));
	memset(key1,0,sizeof(key1));
	k0=0;
	k1=0;
	k30=0;	
	// fill key0[] array with known bits:
	for(int i=0; i<SIZE; i++)
		key0[i]=Plaintext[i]^Ciphertext[i];

	// calculate key0[0] & key1[0]
	init((BYTE*)&LoginSeed);

// Get first 8 bits of k1:
	k1 = B( 8, (key0[1] ^ ((key0[0]>>1) | (key1[0]<<31))) );

// use formula to calculate remaining bits of k1/key0[n]:
// L_{t+1}(key0[n]) = ((L_t(key0[n+1])^L_t(k1)) shl 1) | B_0(key0[n]);
	for(int t=7; t<32; t++)
	{
		for(int n=1; n<SIZE; n++)	
		{
			key0[n] = L(t+1, ( ((L(t,key0[n+1]) ^ L(t,k1))<<1) ) | (B(0,key0[n])) );
		}
		k1 = L(t+1, (key0[1] ^ ((key0[0]>>1) | (key1[0]<<31))) );
	}
	ClientLoginKey2 = k1;

// Calculate k30:
	k30=0;
// Calculate 1st bits of key1[n]:
// B_0(key1[n]) = (key0[n+1]^k1) shr 31;

	for(int n=1; n<SIZE; n++)
	{
		key1[n] = B(31, key0[n+1] ^ k1 );
	}

// calculate remaining bits of key1:
// B_t(key1[n+1]) = B_{t+2}(key1[n]) ^ B_t(k30)
// B_{t+2}(key1[n]) = B_t(key1[n+1]) ^ B_t(k30)
// B_t(k30) = B_{t+2}(key1[n]) ^ B_t(key1[n+1])
	k30=0;
	for(t=0; t<30; t+=2)	
	{
		k30 |= (B(t+2, key1[0]) ^ B(t, key1[1])) << t;

		for(int n=1; n<SIZE; n++)
		{
			// calculate t+2 bits
			key1[n] |= (B(t, key1[n+1]) ^ B(t, k30)) << (t+2);
		}
	}

// B_31(key1[n+1])=B_30(key1[n+1]) 
//	key1[1]|=B(30,key1[1])<<31;

// get 31st bit of k. By the way, it is == bit 30
	k30 |= (B(30, key1[1]) ^ L(1,key0[0])) << 31;

	crypt(Plaintext, SIZE);
	if(memcmp(Ciphertext,Plaintext,SIZE)!=0)
	{
		error_printf("Unable to calculate login keys!\n");
		MessageBox(0,"Unable to calculate login keys. Check your UserName and Password settings!",0,0);
		ExitProcess(1);
	}
	ClientLoginKey1=k30;
	trace_printf("Calculated ClientLoginKey1=%08X, ClientLoginKey2=%08X\n",ClientLoginKey1,ClientLoginKey2);
}
