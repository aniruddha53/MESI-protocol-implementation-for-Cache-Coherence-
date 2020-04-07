//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																						Team 5																						//
//																					   1.7v																							//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																					Declarations																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include<stdio.h>
#include<stdbool.h>
#include<conio.h>
#include<ctype.h>
#include<string.h>

#define tag_bits		11
#define index_bits		15
#define byte_offset		6

#define GETLINE			1
#define SENDLINE		2
#define INVALIDATELINE	3
#define EVICTLINE		4

#define NOHIT			0
#define HIT				1
#define HITM			2

#define READ			1
#define WRITE			2
#define INVALIDATELINE	3
#define RWIM			4
	
#define M	3
#define E	2
#define S	1
#define I	0

#define set 32768
#define way 8


////////////////////////////////////////
//			Global Declaration		  //
////////////////////////////////////////
unsigned int reads = 0;
unsigned int writes = 0;
unsigned int hit = 0;
unsigned int i_read_hit = 0;
unsigned int d_read_hit = 0;
unsigned int d_write_hit = 0;
unsigned int miss = 0;
unsigned int i_read_miss = 0;
unsigned int d_read_miss = 0;
unsigned int d_write_miss = 0;
double cache_hit_ratio = 0;
int on = 0;
int debug = 0;
/*
unsigned int exclu = 0;
unsigned int shar = 0;
unsigned int modi = 0;
unsigned int evictions = 0;
*/
////////////////////////////////////////

////////////////////////////////////////
///		Conversion Functions	   	 ///
////////////////////////////////////////
int get_dec(char ch);
////////////////////////////////////////

////////////////////////////////////////
//			Cache					  //
////////////////////////////////////////
typedef struct
{
	char state;
	unsigned int tag;
}cache;
cache c1[set][way];
////////////////////////////////////////


////////////////////////////////////////
//			Simulation Functions	  //
////////////////////////////////////////
void MessageToCache(char Message, unsigned int Address);
void BusOperation(char BusOp, unsigned int Address, char *SnoopResult);
char GetSnoopResult(unsigned int Address);
void PutSnoopResult(unsigned int Address, char SnoopResult);
/////////////////////////////////////////

////////////////////////////////////////
//			Address Functions		  //
////////////////////////////////////////
unsigned int get_tag(unsigned int Address);
unsigned int get_index(unsigned int Address);
unsigned int get_byte_select(unsigned int Address);
////////////////////////////////////////

////////////////////////////////////////
//			n Operations 			  //
////////////////////////////////////////
void read_request_from_L1_data_cache(unsigned int Address);
void write_request_from_L1_data_cache(unsigned int Address);
void read_request_from_L1_instruction_cache(unsigned int Address);
void snooped_invalidate_command(unsigned int Address);
void snooped_read_request(unsigned int Address);
void snooped_write_request(unsigned int Address);
void snooped_read_with_intent_to_modify_request(unsigned int Address);
void clear_the_cache_and_reset_all_state();
void print_contents_and_state_of_each_valid_cache_line();
/////////////////////////////////////////


////////////////////////////////////////
//			Cache Operations 		  //
////////////////////////////////////////
int search_cache(unsigned int Address);
int Find_Invalid(unsigned int Address);
////////////////////////////////////////


////////////////////////////////////////
//			PLRU Operations 		  //
////////////////////////////////////////
bool PLRU[set][(way-1)];
int UpdatePLRU(unsigned int nset, int nway);
int Read_PLRU(unsigned int nset);
////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////-------DECLERATIONS-------////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																					MAIN																							//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	FILE *fp;
	char *filename = argv[2];
	char *mode = argv[1];
	char ch;
	int flag = 0;																																																																																																																																																																																																																																																																																																																																						//Abhiraj Eksambekar
	char command;
	unsigned int Address = 0, tAddress = 0;
	
	if (mode[0] == '2')
	{
		on = 1;
	}
	else if(mode[0] == '1')
	{
		on = 0;
	}
	else if(mode[0] == '3')
	{
		debug = 1;
	}
	
	fp = fopen(filename,"r");

while((ch = fgetc(fp)) != EOF)
{
	if(ch != '\n')
	{
		if(flag == 0)
		{
			command = ch;
			flag = 1;
		}
		else
		{
			if(ch != ' ')
			{
				tAddress = get_dec(ch);
				Address = Address*16 + tAddress;
			}
		}
		
	}
	else
	{

		flag = 0;
		switch (command)
		{
			case '0':	read_request_from_L1_data_cache(Address);
						break;
						
			case '1':	write_request_from_L1_data_cache(Address);
						break;

			case '2':	read_request_from_L1_instruction_cache(Address);
						break;
						
			case '3':	snooped_invalidate_command(Address);
						break;
						
			case '4':	snooped_read_request(Address);
						break;
						
			case '5':	snooped_write_request(Address);
						break;
						
			case '6':	snooped_read_with_intent_to_modify_request(Address);
						break;
						
			case '8':	clear_the_cache_and_reset_all_state();
						break;
						
			case '9':	print_contents_and_state_of_each_valid_cache_line();
						break;
		}
		tAddress = 0;
		Address = 0;
		command = '7';
	}
}


	cache_hit_ratio = ((double)hit*100/((double)hit+(double)miss));
//	printf("\nNumber of Reads: %u, Number of Writes: %u, Number of Hits: %u, Number of Misses: %u, : %u\nCache Hit Ratio : %f", reads, writes, hit, miss, cache_hit_ratio);
	printf("\nReads: %u,\tWrites: %u,\tHits: %u,\tMisses: %u,\tCache Hit Ratio: %lf", reads, writes, hit, miss, cache_hit_ratio);
	if(debug == 1)
	{
		printf("\nL1 Instruction Read Hits: %u, \tL1 Instruction Read Miss: %u,\tL1 Data Read Hits: %u, \tL1 Data Read Miss: %u, \tL1 Data Write Hits: %u, \tL1 Data Write Miss: %u,", i_read_hit, i_read_miss, d_read_hit, d_read_miss, d_write_hit, d_write_miss);
	}

}
//////////////////////////////////////////////////////////////////////////////////-------MAIN-------//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																					DEFINATIONS																						//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//			Number Conversions		  //
////////////////////////////////////////
int get_dec(char ch)
{
	int hex_digit;
	char b;
	if(isdigit(ch))
	{
		hex_digit = ch - '0';
		return hex_digit;
	}
	else
	{
	if(ch >= 'A' && ch <= 'Z')
	{
		b = tolower(ch);
		hex_digit = b - 'a' + 10;
	}
	else
	{
		hex_digit = ch - 'a' + 10;
	}
	return hex_digit;
	}
}
////////////////////////////////////////


////////////////////////////////////////
//			Address Functions		  //
////////////////////////////////////////
unsigned int get_tag(unsigned int Address)
{
	unsigned int i =  Address>>(byte_offset + index_bits);
	return i;
}

unsigned int get_index(unsigned int Address)
{
	unsigned int i = (Address>>byte_offset) & 0x7fff;
	return i;
}

unsigned int get_byte_select(unsigned int Address)
{
	unsigned int i = Address & 0x3f;
	return i;
}
////////////////////////////////////////


////////////////////////////////////////
//			Simulation Functions	  //
////////////////////////////////////////
void MessageToCache(char Message, unsigned int Address)
{
	char ch;
	if(Message == GETLINE)
	{
		ch = 'G';
	}
	else if (Message == SENDLINE)
	{
		ch = 'S';
	}
	else if (Message == INVALIDATELINE)
	{
		ch = 'I';
	}
	else if (Message == EVICTLINE)
	{
		ch = 'E';
	}
	
	if(on == 1)
	{
	//	#ifndef SILENT
		printf("L2: %c	%x\n", ch, Address);
	//	#endif
	}
}

void BusOperation(char BusOp, unsigned int Address, char *SnoopResult)
{
	char ch;
	*SnoopResult = GetSnoopResult(Address);
	
	if(BusOp == READ)
		ch = 'R';
	else if (BusOp == WRITE)
		ch = 'W';
	else if (BusOp == INVALIDATELINE)
		ch = 'I';
	else if (BusOp == RWIM)
		ch = 'M';
	
	if(on == 1)
	{
	//	#ifndef SILENT
		printf("BusOp: %c,	Address: %x,	Snoop Result: %d\n", ch, Address, *SnoopResult);
	//	#endif
	}
}

char GetSnoopResult(unsigned int Address)
{
	if((Address & 3) == 0)
	{
		return HIT;
	}
	else if((Address & 3) == 1)
	{
		return HITM;
	}
	else if(((Address & 2) == 2))
	{
		return NOHIT;
	}
	return NOHIT;
}

void PutSnoopResult(unsigned int Address, char SnoopResult)
{
	char ch;
	if(SnoopResult == NOHIT)
	{
		ch = 'N';
	}
	else if (SnoopResult == HIT)
	{
		ch = 'H';
	}
	else if (SnoopResult == HITM)
	{
		ch = 'M';
	}
	if(on == 1)
	{
	//	#ifndef SILENT
		printf("SnoopResult: Address %x, SnoopResult: %c\n", Address, ch);
	//	#endif
	}
}
/////////////////////////////////////////


////////////////////////////////////////
//			n Operations			  //
////////////////////////////////////////

////////*****read_request_from_L1_data_cache*****////////////////
void read_request_from_L1_data_cache(unsigned int Address)					
{
	int i;
	char SnoopResult, dc;
	unsigned int p_address;
	unsigned int cache_index = get_index(Address);
	unsigned int cache_tag = get_tag(Address);
	reads++;
	 i = search_cache(Address);
	 if(i != 8)
	 {
		 MessageToCache(SENDLINE, Address);
		 UpdatePLRU(cache_index, i);
		 hit++;
		 d_read_hit++;
	 }
	 else if(i == 8)
	 {
		 miss++;
		 d_read_miss++;
		i = Find_Invalid(Address);
		if(i != 8)
		{
			BusOperation(READ, Address, &SnoopResult);
			c1[cache_index][i].tag = cache_tag;
			MessageToCache(SENDLINE, Address);
			UpdatePLRU(cache_index, i);
		}
		else if(i == 8)
		{
			i = Read_PLRU(cache_index);
			p_address = ((c1[cache_index][i].tag<<(index_bits)) | cache_index) << byte_offset;
			if(c1[cache_index][i].state == M)
			{
				MessageToCache(GETLINE, p_address);
				BusOperation(WRITE, p_address, &dc);
				MessageToCache(EVICTLINE, p_address);
				BusOperation(READ, Address, &SnoopResult);
				c1[cache_index][i].tag = cache_tag;
				MessageToCache(SENDLINE, Address);
				UpdatePLRU(cache_index, i);
			}
			else if((c1[cache_index][i].state == S) || (c1[cache_index][i].state == E))
			{
				MessageToCache(EVICTLINE, p_address);
				BusOperation(READ, Address, &SnoopResult);
				c1[cache_index][i].tag = cache_tag;
				MessageToCache(SENDLINE, Address);
				UpdatePLRU(cache_index, i);			
			}
		}
		if(SnoopResult == HITM)
		{
			c1[cache_index][i].state = S;
		}
		else if(SnoopResult == HIT)
		{
			c1[cache_index][i].state = S;
		}
		else if(SnoopResult == NOHIT)
		{
			c1[cache_index][i].state = E;
		}
	}
}
////////-----read_request_from_L1_data_cache-----////////////////


////////*****write_request_from_L1_data_cache*****////////////////
void write_request_from_L1_data_cache(unsigned int Address)
{
	int i;
	char SnoopResult, dc;
	unsigned int p_address;
	unsigned int cache_index = get_index(Address);
	unsigned int cache_tag = get_tag(Address);
	writes++;
	 i = search_cache(Address);
	 if(i != 8)
	 {
		 MessageToCache(SENDLINE, Address);
//		 MessageToCache(GETLINE, Address);			
		 UpdatePLRU(cache_index, i);
		 hit++;
		 if((c1[cache_index][i].state == S) || (c1[cache_index][i].state == E))
		 {
			 BusOperation(INVALIDATELINE, Address, &dc);
			 c1[cache_index][i].state = M;
		 }			
	 }
	 else if(i == 8)
	 {
		 miss++;
		 d_write_miss++;
		i = Find_Invalid(Address);
		if(i != 8)
		{
			BusOperation(RWIM, Address, &SnoopResult);
			c1[cache_index][i].tag = cache_tag;
			MessageToCache(SENDLINE, Address);
//			MessageToCache(GETLINE, Address);
			UpdatePLRU(cache_index, i);
		}
		else if(i == 8)
		{
			i = Read_PLRU(cache_index);
			p_address = ((c1[cache_index][i].tag<<(index_bits)) | cache_index) << byte_offset;
			if(c1[cache_index][i].state == M)
			{
				MessageToCache(GETLINE, p_address);				
				BusOperation(WRITE, p_address, &dc);
				MessageToCache(EVICTLINE, p_address);
				BusOperation(RWIM, Address, &SnoopResult);
				c1[cache_index][i].tag = cache_tag;
				MessageToCache(SENDLINE, Address);
//				MessageToCache(GETLINE, Address);
				UpdatePLRU(cache_index, i);
			}
			else if((c1[cache_index][i].state == S) || (c1[cache_index][i].state == E))
			{
				MessageToCache(EVICTLINE, p_address);
				BusOperation(RWIM, Address, &SnoopResult);
				c1[cache_index][i].tag = cache_tag;
				MessageToCache(SENDLINE, Address);
//				MessageToCache(GETLINE, Address);
				UpdatePLRU(cache_index, i);
			}
		}
		c1[cache_index][i].state = M;
	 }
	 
}
////////-----write_request_from_L1_data_cache-----////////////////


////////*****read_request_from_L1_instruction_cache*****////////////////
void read_request_from_L1_instruction_cache(unsigned int Address)					
{
	int i;
	unsigned int  p_address;
	unsigned int cache_index = get_index(Address);
	unsigned int cache_tag = get_tag(Address);
	char SnoopResult, dc;
	reads++;
	 i = search_cache(Address);
	 if(i != 8)
	 {
		 MessageToCache(SENDLINE, Address);
		 UpdatePLRU(cache_index, i);
		 hit++;
		 i_read_hit++;
	 }
	 else if(i == 8)
	 {
		 miss++;
		 i_read_miss++;
		i = Find_Invalid(Address);
		if(i != 8)
		{
			BusOperation(READ, Address, &SnoopResult);
			c1[cache_index][i].tag = cache_tag;
			MessageToCache(SENDLINE, Address);
			UpdatePLRU(cache_index, i);
		}
		else if(i == 8)
		{
			i = Read_PLRU(cache_index);
			p_address = (((c1[cache_index][i].tag<<(index_bits)) | cache_index) << byte_offset);
			if(c1[cache_index][i].state == M)
			{
				MessageToCache(GETLINE, p_address);
				BusOperation(WRITE, p_address, &dc);
				MessageToCache(EVICTLINE, p_address);
				BusOperation(READ, Address, &SnoopResult);
				c1[cache_index][i].tag = cache_tag;
				MessageToCache(SENDLINE, Address);
				UpdatePLRU(cache_index, i);
			}
			else if((c1[cache_index][i].state == S) || (c1[cache_index][i].state == E))
			{
				MessageToCache(EVICTLINE, p_address);
				BusOperation(READ, Address, &SnoopResult);
				c1[cache_index][i].tag = cache_tag;
				MessageToCache(SENDLINE, Address);
				UpdatePLRU(cache_index, i);			
			}
		}
		if(SnoopResult == HITM)
		{
			c1[cache_index][i].state = S;
		}
		else if(SnoopResult == HIT)
		{
			c1[cache_index][i].state = S;
		}
		else if(SnoopResult == NOHIT)
		{
			c1[cache_index][i].state = E;
		}
	 }
}
////////-----read_request_from_L1_instruction_cache	-----////////////////


////////*****snooped_invalidate_command*****////////////////
void snooped_invalidate_command(unsigned int Address)
{
	int i;
	i = search_cache(Address);
	if((i != 8) && (c1[get_index(Address)][i].state != I))
	{
		c1[get_index(Address)][i].state = I;
		MessageToCache(INVALIDATELINE, Address);
	}
}
////////-----snooped_invalidate_command-----////////////////


////////*****snooped_read_request*****////////////////
void snooped_read_request(unsigned int Address)
{
	int i;
	char dc;
	unsigned int cache_index = get_index(Address);
	i = search_cache(Address);
	if (i != 8)
	{
		if(c1[cache_index][i].state == M)
		{
			c1[cache_index][i].state = S;
			PutSnoopResult(Address, HITM);
			MessageToCache(GETLINE, Address);
			BusOperation(WRITE, Address, &dc);
		}
		else if((c1[cache_index][i].state == E))
		{
			PutSnoopResult(Address, HIT);
			c1[cache_index][i].state = S;
		}
		else if(c1[cache_index][i].state == S)
		{
			PutSnoopResult(Address, HIT);
		}
	}
	else
	{
		PutSnoopResult(Address, NOHIT);
	}
}
////////-----snooped_read_request-----////////////////


////////*****snooped_write_request*****////////////////
void snooped_write_request(unsigned int Address)
{
	////VERY IMPORTANT/////// NOTHING TO BE DONE LEAVING BLANK IS THE ANSWER
}
////////-----snooped_write_request-----////////////////


////////*****snooped_read_with_intent_to_modify_request*****////////////////
void snooped_read_with_intent_to_modify_request(unsigned int Address)
{
	int i;
	char dc;
	unsigned int cache_index = get_index(Address);
	i = search_cache(Address);
	if (i != 8)
	{
		if(c1[cache_index][i].state == M)
		{
			PutSnoopResult(Address, HITM);
			MessageToCache(GETLINE, Address);
			BusOperation(WRITE, Address, &dc);
			c1[cache_index][i].state = I;
			MessageToCache(INVALIDATELINE, Address);
		}
		else if((c1[cache_index][i].state == E) || (c1[cache_index][i].state == S))
		{
			PutSnoopResult(Address, HIT);
			c1[cache_index][i].state = I;
			MessageToCache(INVALIDATELINE, Address);
		}
	}
	else
	{
		PutSnoopResult(Address, NOHIT);
	}
}
////////-----snooped_read_with_intent_to_modify_request-----////////////////


////////*****n_eignt*****////////////////
void clear_the_cache_and_reset_all_state()
{
	unsigned int i,j;
	for(i = 0; i < set; i++)
	{
		for(j = 0; j < way; j++)
		{
			c1[i][j].tag = 0;
			c1[i][j].state = I;
		}
	}
}
////////-----n_eignt-----////////////////


////////*****print_contents_and_state_of_each_valid_cache_line*****////////////////
void print_contents_and_state_of_each_valid_cache_line()
{
	unsigned int i;
	int j,l;
	int c= 0;
	int tab = 0;
	char line_state;
	printf("SET\t\tWAY0\t\tWAY1\t\tWAY2\t\tWAY3\t\tWAY4\t\tWAY5\t\tWAY6\t\tWAY7");
	for(i=0;i<set;i++)
	{
		c = 0;
		tab = 0;
		for(j=0;j<8;j++)
		{
//			printf("\t");
			tab++;
			if(c1[i][j].state != I)
			{
				if(c1[i][j].state == E)
				{
					line_state = 'E';
				}
				else if(c1[i][j].state == S)
				{
					line_state = 'S';
				}
				else if(c1[i][j].state == M)
				{
					line_state = 'M';
				}
//				printf("\nSet : %x, Way: %u, State: %d, Starting Address: %x", i, j, c1[i][j].state, (((c1[i][j].tag<<index_bits) | i)<<byte_offset));
				if(c == 0)
				{
					printf("\n%x", i);
					c = 1;
				}
				for(l=0;l<tab;l++)
				{
					printf("\t\t");
				}
				tab = 0;
				printf("%c ", line_state);
				printf(" %x", c1[i][j].tag);
			}
		}
	}
	printf("\n");
}
////////-----print_contents_and_state_of_each_valid_cache_line-----////////////////
/////////////////////////////////////////


////////////////////////////////////////
//			Cache Operations 		  //
////////////////////////////////////////
int search_cache(unsigned int Address)
{
	int i = 0;
	for(i=0;i<8;i++) 
	{
		if((c1[get_index(Address)][i].tag == get_tag(Address)) && (c1[get_index(Address)][i].state != I))
		{
			return i;
			break;
		}
	}
	return 8;
}

int Find_Invalid(unsigned int Address)
{
	int i;
	for(i = 0; i<8; i++)
	{
		if(c1[get_index(Address)][i].state == I)
		{
			return i;
			break;
		}
	}
	return 8;
}
////////////////////////////////////////


////////////////////////////////////////
//			PLRU Operations 		  //
////////////////////////////////////////

////////*****UpdatePLRU*****///////////
int UpdatePLRU(unsigned int nset, int nway)
{
	
	switch(nway)
	{
		case 0:	PLRU[nset][0] = 0;
				PLRU[nset][1] = 0;
				PLRU[nset][3] = 0;
				break;
		
		case 1:	PLRU[nset][0] = 0;
				PLRU[nset][1] = 0;
				PLRU[nset][3] = 1;
				break;
				
		case 2:	PLRU[nset][0] = 0;
				PLRU[nset][1] = 1;
				PLRU[nset][4] = 0;
				break;
				
		case 3:	PLRU[nset][0] = 0;
				PLRU[nset][1] = 1;
				PLRU[nset][4] = 1;
				break;

		case 4:	PLRU[nset][0] = 1;
				PLRU[nset][2] = 0;
				PLRU[nset][5] = 0;
				break;

		case 5:	PLRU[nset][0] = 1;
				PLRU[nset][2] = 0;
				PLRU[nset][5] = 1;
				break;

		case 6:	PLRU[nset][0] = 1;
				PLRU[nset][2] = 1;
				PLRU[nset][6] = 0;
				break;

		case 7:	PLRU[nset][0] = 1;
				PLRU[nset][2] = 1;
				PLRU[nset][6] = 1;
				break;

	}
	return 0;
}
////////-----UpdatePLRU-----//////////////

////////*****Read_PLRU*****//////////////
int Read_PLRU(unsigned int nset)
{
	int i;
	if(PLRU[nset][0] == 0)
	{
		if(PLRU[nset][2] == 0)
		{
			if(PLRU[nset][6] == 0)
				i = 7;
			else
				i = 6;
		}
		else 
		{
			if(PLRU[nset][5] == 0)
				i = 5;
			else
				i = 4;
		}
	}
	else
	{
		if(PLRU[nset][1] == 0)
		{
			if(PLRU[nset][4] == 0)
				i = 3;
			else
				i = 2;
		}
		else 
		{
			if(PLRU[nset][3] == 0)
				i = 1;
			else
				i = 0;
		}		
	}
	return i;
}
////////-----Read_PLRU-----//////////////
////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////-------DEFINATIONS-------////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////