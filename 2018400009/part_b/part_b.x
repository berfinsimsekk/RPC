
struct arguments{
	
	int a;
	int b;
	char path[256];

};


program PARTB_PROG{
	version PARTB_VERS{
		/* Takes a numbers structure and gives the integer result. */
		string partb(arguments)=1;
	}=1;
}=0x12345678;