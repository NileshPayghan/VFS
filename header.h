#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdio_ext.h>
//#include<sys/wait.h>
#include<stdlib.h>
#include<malloc.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define MAXINODE 10
#define READ 1
#define WRITE 2
#define REGULAR 1
#define SPECIAL 2
#define MAXFILESIZE 1024

typedef unsigned int UINT;

/////////////////////////////// Structure delcarations	/////////////////////

//Superblock intialisation
//Superblock contains the information about the totalinodes in the incore inode table
//free inodes in the list and used inodes in the list
typedef struct superblock
{
	UINT TotalInode;
	UINT FreeInode;
	UINT UsedInode;
}SUPERBLOCK;
//creating the object of superblock which global throughout the program
//Access superblock from anywhere in the program
SUPERBLOCK SUPERBLOCKobj;

typedef struct inode
{
	char FileName[50];
	char Owner[50];
	UINT InodeNumber;
	UINT permissions;
	int FileType;
	UINT FileSize;
	UINT FileActualSize;
	UINT ReferenceCount;
	UINT LinkCount;
	char *Buffer;
	struct inode* next;
	//Access time
	//modified time
	//last inode changing time
}INODE,*PINODE;

PINODE head=NULL;

typedef struct filetable
{
		int readoffset;
		int writeoffset;
		int count;
		int mode;
		PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[MAXINODE];

////////////////	Function declarations 	/////////////////////////////////
void printvfs();
BOOL CheckUserName(char*);
BOOL CheckPassword(char *);
void InitialiseSuperBlock();
void InitialiseUFDT();
void InitialiseInodes();
int CreateFile(char*,int);
PINODE Get_Inode(char*);
void ls_File();
int OpenFile(char*,int);




////////////////////// 	User name and password varaibles	//////////////
const char* username="Nilesh";
const char* gpassword="nilesh";
