/////////////////////////////////////////////////////////////////////////////////
//Project name:	Virtual file system (VFS)
//Author:Nilesh	(nileshpayghan7@gmail.com,nileshpayghan7@outlook.com)
//OS:Unix/Linux
//Platform:Linux
//Date:24-10-16
//Guided by:piyush sir
//Reference book:1) UNIX Advanced operating system by Maurice Bach
//				 2) Advanced programming in the UNIX Environment third edition by
//					W.Richard steven and Stephen A. Rago
/////////////////////////////////////////////////////////////////////////////////

//include header files

#include"header.h"
//create the structures


////////////////////////////////////////////////////////////////
//CheckUserName is function is used to check user name
//if user name is not correct then it return false
BOOL CheckUserName(char* UserName)
{
//input validation filter
if(UserName==NULL)
	return FALSE;
	
	const char* temp=username;	

	while(*UserName!='\0' && *temp!='\0')
	{		
		
		if(*temp!=*UserName)
		{
			printf("Enter correct UserName\n");
			break;
		}
				
	temp++;		
	UserName++;
	}
	if(*UserName=='\0' && *temp=='\0')
		return TRUE;
	else
		return FALSE;
}//CheckUserName() function close here

/////////////////////////////////////////////////////////////////////////////
//CheckPassword is function which is used to check password

BOOL CheckPassword(char *Password)
{
//input validation filter
if(Password==NULL)
	return FALSE;

	const char* temp=gpassword;
	while(*Password!='\0' && *temp!='\0')
	{			
		if(*temp!=*Password)
		{
			break;
		}				
	temp++;		
	Password++;
	}
	if(*Password=='\0' && *temp=='\0')
		return TRUE;
	else
		return FALSE;
}//CheckPasswordFunction Ends here

/////////////////////////	InitialiseSuperBlock////////////////////////
void InitialiseSuperBlock()
{
	printf("SUPERBLOCK INTIALISED\n");
	SUPERBLOCKobj.TotalInode=MAXINODE;
	SUPERBLOCKobj.FreeInode=MAXINODE;
	SUPERBLOCKobj.UsedInode=0;
	printf("totalindoe:%d\tfreeinode:%d\tusedinode:%d\n",SUPERBLOCKobj.TotalInode,SUPERBLOCKobj.FreeInode,SUPERBLOCKobj.UsedInode);
}

//////////////////////	Initialise UFDT array table//////////////////////
void InitialiseUFDT()
{
	int i=0;
	while(i<MAXINODE)
	{	
		UFDTArr[i].ptrfiletable=NULL;
		i++;
	}
	printf("UFDT table created\n");
}

//////////////////////////	Initialise Inode blocks	//////////////////////////
//	or create DILB(disk inode list block)	///////////////////////
void InitialiseInodes()
{
	int  i=0;
	PINODE newn=NULL;
	PINODE temp=head;
		
	while(i<MAXINODE)
	{
		//checked done
		printf("Inode Intialisation \n");
		newn=(PINODE)malloc(sizeof(INODE));
		newn->permissions=0;
		newn->FileType=0;
		newn->ReferenceCount=0;
		newn->LinkCount=0;
		newn->FileActualSize=0;
		newn->InodeNumber=i;
		newn->Buffer=NULL;
		newn->next=NULL;
		if(temp==NULL)
		{
			head=newn;
			temp=head;
		}
		else
		{	
			temp->next=newn;
			temp=temp->next;
		}
		i++;
	}
	printf("Inodes are initialised\n");
}

///////////////////////	Get_Inode function	/////////////////////////////
//	This function checks filename in the inode list and if Filetype is not zero 
//	then file is in use.other wise use that file
//	

PINODE Get_Inode(char* filename)
{
PINODE temp=head;
printf("Entered in the Get_Inode\n");
if(filename==NULL || SUPERBLOCKobj.FreeInode==0)
	{	
		printf("return from the first if statement\n");
		return NULL;
	}	
	while(temp!=NULL)
	{
		printf("Entered in while loop\n");
		//strcmp compare two strings and return zero if both are eqaul
		if((strcmp(filename,temp->FileName)==0) &&(temp->FileType!=0))
		{
			printf("filename:%s,temp->Filename:%s\n",filename,temp->FileName);
			break;
		}
		temp=temp->next;
		printf("loop executed\n");
	}
	return temp;
}



///////////////////////	Create File function	//////////////////////////
int CreateFile(char* filename,int permission)
{
//prameter checking
if(filename==NULL || permission<READ || permission>(READ+WRITE))
	return -1;
//Inode checking
if(SUPERBLOCKobj.FreeInode==0 || SUPERBLOCKobj.UsedInode==MAXINODE)
	return -2;
if(Get_Inode(filename)!=NULL)
	return -3;

	(SUPERBLOCKobj.FreeInode)--;
	(SUPERBLOCKobj.UsedInode)++;
	
	int i=0;
	PINODE temp=head;
	
	while(temp!=NULL)
	{
		printf("temproary pointer search the file type is eqaul to 0\n");
		if(temp->FileType==0)
		{
			printf("filetype is find in inode table\n");
			break;
		}
		temp=temp->next;
	}
	
	
	while(i<MAXINODE)
	{
		printf("UFDTarr[i].ptrfiletable checking\n");
		if(UFDTArr[i].ptrfiletable==NULL)
		{	
			printf("NULL is found in UFDT table at %d\n",i);
			break;
		}
		i++;
	}
	//if UFDT table is full then reuturn
	if(i>=MAXINODE)
	{
		return -4;
	}
	//check the inode filetype is 0
	//if FileType is zero then file not in use then use it
	//allocate the memory for filetable
	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable==NULL)
	{
		return -5;
	}
	//Initialise variables in file table
	UFDTArr[i].ptrfiletable->count=1;
	UFDTArr[i].ptrfiletable->mode=permission;
	UFDTArr[i].ptrfiletable->readoffset=0;
	UFDTArr[i].ptrfiletable->writeoffset=0;
	//assign the inode to filetable
	UFDTArr[i].ptrfiletable->ptrinode=temp;
	//Initialise variables in inodes table for creating new file
	strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,filename);
	strcpy(UFDTArr[i].ptrfiletable->ptrinode->Owner,username);
	UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;
	UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFILESIZE;
	UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=0;
	UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
	UFDTArr[i].ptrfiletable->ptrinode->permissions=permission;
	UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char*)malloc(sizeof(MAXFILESIZE));
	memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);
	
	return i;
}

//Display the files which created by the user
void ls_File()
{
PINODE temp=head;
printf("--------------------------------------------------------------------------------------\n");

	while(temp!=NULL)
	{
		if(temp->FileType!=0)
		{
			printf("FileName\tPermission\tOwner\tFile Size\tLinkCount\tFileType\n");
			printf("%s\t\t%d\t\t%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,
			temp->permissions,temp->Owner,temp->FileActualSize,temp->LinkCount,temp->FileType
			);
		}
	temp=temp->next;
	}


printf("--------------------------------------------------------------------------------------\n");

}


//////////////////////////////////  OpenFile function	//////////////////////
//	This function is used to open the file
int OpenFile(char* filename,int mode)
{
	
	PINODE temp=NULL;
	int i=0;
	
if(filename==NULL || mode<=0 || mode>3)	
	return -1;
	
	temp=Get_Inode(filename);
	if(temp==NULL)
		return -2;
		
	if(temp->permissions<mode)
		return -3;
	
	while(i<MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable==NULL)
			break;
			
		i++;
	}
	if(i>=MAXINODE)
		return -4;
	
	UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable==NULL)
		return -1;
	UFDTArr[i].ptrfiletable->count=1;
	UFDTArr[i].ptrfiletable->mode=mode;
	if(mode==READ+WRITE)
	{
		UFDTArr[i].ptrfiletable->readoffset=0;
		UFDTArr[i].ptrfiletable->writeoffset=0;
	}
	else if(mode==READ)
	{
		UFDTArr[i].ptrfiletable->readoffset=0;
	}
	else if(mode==WRITE)
	{
		UFDTArr[i].ptrfiletable->writeoffset=0;
	}
	
	UFDTArr[i].ptrfiletable->ptrinode=temp;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;
	
	return i;
}

////////////////////////////////////// CloseAllFile function	/////////////////////////\
//	
//	this functin is used to close all opened files in the file system
void CloseAllFile()
{
int i=0;
	printf("files are closing..\n");
	while(i<MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable!=NULL)
		{
			UFDTArr[i].ptrfiletable->readoffset=0;
			UFDTArr[i].ptrfiletable->writeoffset=0;
			(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
		}
		i++;
	}
}

/////////////////////////////////  stat_file function	////////////
// this function is used to display information about file using its name

int stat_file(char* filename)
{
if(filename==NULL)
	return -1;
	
	PINODE temp=head;
	while(temp!=NULL)
	{
		if(strcmp(filename,temp->FileName)==0)
		{
			break;
		}
		temp=temp->next;
	}
	if(temp==NULL)
		return -2;
		
	printf("-------Information about file------------\n");
	printf("File name %s:\n",temp->FileName);
	printf("Owner of file:%s\n",temp->Owner);
	printf("inode number is %u\n",temp->InodeNumber);
	printf("File Size:%d\n",temp->FileSize);
	printf("File Actual size:%d",temp->FileActualSize);
	printf("Reference count:%d\n",temp->ReferenceCount);
	printf("File Type:%d\n",temp->FileType);
	if(temp->permissions==1)
		printf("permission:Read only File\n");
	else if(temp->permissions==3)
		printf("permission:Read and Write only\n");
	else if(temp->permissions==2)
		printf("permission:Write only File\n");
return 0;
}	

//////////////////////////////////////////////  fstat_file()	////////////////////////////////
///// This function is used to display information about with their file inode number

int fstat_file(int fd)
{
if(fd<0)
	return -1;
	
	if(UFDTArr[fd].ptrfiletable==NULL)
		return -2;
	PINODE temp=UFDTArr[fd].ptrfiletable->ptrinode;
	
	printf("-------Information about file------------\n");
	printf("File name %s:\n",temp->FileName);
	printf("Owner of file:%s\n",temp->Owner);
	printf("inode number is %u\n",temp->InodeNumber);
	printf("File Size:%d\n",temp->FileSize);
	printf("File Actual size:%d",temp->FileActualSize);
	printf("Reference count:%d\n",temp->ReferenceCount);
	printf("File Type:%d\n",temp->FileType);
	if(temp->permissions==1)
		printf("permission:Read only File\n");
	else if(temp->permissions==3)
		printf("permission:Read and Write only\n");
	else if(temp->permissions==2)
		printf("permission:Write only File\n");
return 0;
}
//////////////////////////////////////////// GetFDFromName()	/////////////////////////////
// this function is used to get file descriptor from it s name 
//	file descriptor is the index of UFDT array table

int GetFDFromName(char* filename)
{
if(filename==NULL)
	return -1;
	
	int i=0;
	
	while(i<MAXINODE)
	{ 
	 if(UFDTArr[i].ptrfiletable!=NULL)
	 {
	 	if(strcmp(UFDTArr[i].ptrfiletable->ptrinode->FileName,filename)==0)
	 		break;
	 }
	 i++;
	}
	if(i==MAXINODE) return -1;
	else	return i;
}

////////////////////////////////////////  close()	//////////////////////////////////////////////
//this function is used to close opened file by its file descriptor

int closebyfd(int fd)
{
if(fd<0||fd>MAXINODE)
	return -1;

	UFDTArr[fd].ptrfiletable->readoffset=0;
	UFDTArr[fd].ptrfiletable->writeoffset=0;
	UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount--;
	
	return 0;
}






///////////////////////////////////////////////  close()	////////////////////////////////
//this function is used to close opened file by its filename

int closebyname(char* filename)
{
if(filename==NULL)
	return -1;

	int i=0;
	i=GetFDFromName(filename);
	if(i==-1)
		return -2;
		
	UFDTArr[i].ptrfiletable->readoffset=0;
	UFDTArr[i].ptrfiletable->writeoffset=0;
	UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount--;
	
	return 0;
}

//////////////////////// rm_file()	//////////////////////////
//this function removes file 
int rm_file(char* filename)
{
if(filename==NULL)
	return -1;

	int fd=GetFDFromName(filename);
	if(fd==-1)
		return-2;
	(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
	if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount==0)
	{
		UFDTArr[fd].ptrfiletable->ptrinode->FileType=0;
		free(UFDTArr[fd].ptrfiletable);
	}
	UFDTArr[fd].ptrfiletable=NULL;
	(SUPERBLOCKobj.FreeInode)++;
	(SUPERBLOCKobj.UsedInode)--;
}
////////////////////////////////////////// 	help function	/////////////////////////////////////////////////////////
//this function shows the all command information
void help(){
	printf("------------------------------- Info ----------------------------------------\n");
	printf("ls: list directory contents ,List  information  about  the FILEs \n");
	printf("exit: which used to exit the terminal\n");
	printf("logout: closes all files and which asks fog login and password\n");
	printf("shutdown: which terminates the process\n");
	printf("clear: to clear the screen\n");
	printf("show: show the graphics of VFS word\n");
	printf("closeall: which is used to close all the files which are opened\n");
	printf("stat: which shows the information of the file by its filename(syntax: stat filename)\n");
	printf("fstat: which shows the information of the file by fd (syntax: fstat fd)\n");
	printf("rm: remove the file \n");


}






////////////////////////////////////////////	Main function	///////////////////////////////////////////////

//Main function starts here
int main()
{
char UserName[50],Password[50];
char CommandString[80],CommandStringSplit[4][80];
int cmdCount=0,ret=0;


BOOL CheckP,CheckUN;
//Uncondtition while loop for the Taking user name and password
while(1)
{
	__fpurge(stdin);//to clear buffer
	printf("Enter UserName:");
	scanf("%s",UserName);
	CheckUN=CheckUserName(UserName);
	//if user name is correct then check for password
	if(CheckUN==TRUE)
	{
		__fpurge(stdin);//to clear buffer
		printf("Enter Password:");
		scanf("%s",Password);
		CheckP=CheckPassword(Password);
		if(CheckP==TRUE)
		{
			printf("\n\n");
			printvfs();
			printf("\n\n");
			printf("\t\t\t\t\tWelcome to Marvellous VFS:\n");
			break;
		}
		else
		{
			printf("Try Again!\n");
			continue;
		}
	}
	else
	{
		printf("Try Again!\n");
		continue;
	}

}//First uncondition while loop for user name and password

////////////////////////////////////////////////////////////
//intialisation of the structures
//Intialise UFDT table
//InitialiseSuperblock
//Initialise Inode list
//Intialise file table entries

	InitialiseSuperBlock();
	InitialiseUFDT();
	InitialiseInodes();

/////////////////////////////////////////////////////////////////////////
//	if User name and password is correct then starts the terminal here


while(1)
{
__fpurge(stdin);
printf("npsoft@VFS:");
scanf("%[^\n]s",CommandString);

//split the string into sperate command by space seperating
//copy the word of CommandString into the CommandStringSplit into four words either one two three or four words
cmdCount=sscanf(CommandString,"%s%s%s%s",CommandStringSplit[0],CommandStringSplit[1],CommandStringSplit[2],CommandStringSplit[3]);

//command exceeds the words in it then don't check command continue to terminal
if(cmdCount<=0 || cmdCount>4)
	{
		printf("bad command!\n");
		continue;
	}
//////////////////////////////////////////////////////////////
//
//		If the commands contains only single word in it then run these commands
	if(cmdCount==1)
	{
	
		if(strcmp(CommandStringSplit[0],"exit")==0)
			{
				printf("Thank You...!\n");
				break;
			}//exit command
		
		else if(strcmp(CommandStringSplit[0],"logout")==0)
		{
			CloseAllFile();
			main();
		}//logout command closed
	
		else if(strcmp(CommandStringSplit[0],"shutdown")==0)
		{
			CloseAllFile();
			exit(0);//stdlib.h
		}
		else if(strcmp(CommandStringSplit[0],"clear")==0)	//clear the screen of terminal
		{
			system("clear");
		}
		else if(strcmp(CommandStringSplit[0],"ls")==0)  		//this shows the currently all created files
		{
			ls_File();
		}
		else if(strcmp(CommandStringSplit[0],"show")==0)	//this command shows vfs graphics on screen
		{
			printvfs();
		}
		else if(strcmp(CommandStringSplit[0],"closeall")==0)	//close all files which are opened
		{
			CloseAllFile();
			printf("All file closed\n");
			continue;
		}
		else if(strcmp(CommandStringSplit[0],"help")==0)
		{
			help();
			continue;
		}

		else
		{
			printf("%s: command not found\n",CommandStringSplit[0]);
			continue;
		}
	}//count 1 if closed
	else if(cmdCount==2)
	{
	//used to display information about file by its name
		if(strcmp(CommandStringSplit[0],"stat")==0)
		{
			ret=stat_file(CommandStringSplit[1]);
			if(ret==-1)
				printf("ERROR:Incorrect parameter\n");
			else if(ret==-2)
				printf("ERROR:File is not found\n");
		}
		//used to display information about file by file descriptor
		else if(strcmp(CommandStringSplit[0],"fstat")==0)
		{
			ret=fstat_file(atoi(CommandStringSplit[1]));
			if(ret==-1)
				printf("ERROR:Incorrect parameter\n");
			else if(ret==-2)
				printf("ERROR:File is not found\n");
		}
	//used to close file
		else if(strcmp(CommandStringSplit[0],"close")==0)
		{
			ret=closebyname(CommandStringSplit[1]);
			if(ret==-1)
				printf("ERROR: To found file\n");
		}
	//use to close file by file descriptor
		else if(strcmp(CommandStringSplit[0],"close")==0)
		{
			ret=closebyfd(atoi(CommandStringSplit[1]));
			if(ret==-1)
				printf("ERROR: Incorrect parameter\n");
			else if(ret==-2)
				printf("ERROR: File not Found\n");
		}
		else if(strcmp(CommandStringSplit[0],"rm")==0)
		{
			ret=rm_file(CommandStringSplit[1]);
			if(ret==-1)
				printf("ERROR: Incorrect parameter\n");
			else if(ret==-2)
				printf("ERROR: File not Found\n");
		}
				

//Three words command		
	}
	else if(cmdCount==3)
	{
		if(strcmp(CommandStringSplit[0],"creat")==0)
		{
			ret=CreateFile(CommandStringSplit[1],atoi(CommandStringSplit[2]));
			if(ret>=0)
				printf("File is successfully create with file descriptor:%d\n",ret);
			if(ret==-1)//parameters checked
				printf("ERROR: parameter error\n");
			else if(ret==-2)//superblock free indoes are check if no free inode then return
				printf("ERROR: Inodes are full\n");
			else if(ret==-3)//Get_Inode function is called to check file is already exists or not
				printf("File Already exists\n");
			else if(ret==-4)
				printf("UFDT is not allocated\n");
			else if(ret==-5)
				printf("ERROR: Memory Error\n");
			//continue to while loop
			continue;
		}//creat command close
		else if(strcmp(CommandStringSplit[0],"open")==0)
		{
			ret=OpenFile(CommandStringSplit[1],atoi(CommandStringSplit[2]));
			if(ret>=0)
				printf("File is Successfully opened with file descriptor: %d\n",ret);
			else if(ret==-1)
				printf("ERROR:Parameter error\n");
			else if(ret==-2)
				printf("ERROR: File not found\n");
			else if(ret==-3)
				printf("ERROR: permission denied\n");
			else if(ret==-4)
				printf("ERROR: UFDT not found");
			//continue to while loop
			continue;
		}//open command close
	}
	else
	{
		printf("%s: command not found\n",CommandString);
		continue;
	}//cmdCount last else closed
	
}//second while loop closed for terminal




return 0;
}

