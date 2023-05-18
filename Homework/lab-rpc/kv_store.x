typedef char buf <>;
struct PutArgs {
    buf key;
    buf value;
};

struct GetArgs {
    buf key;
};
 program KVSTORE {
	version KVSTORE_V1 {
		int EXAMPLE(int) = 1;
		string ECHO(string) = 2;
		void PUT(PutArgs) = 3;
        buf GET(GetArgs) = 4;
	} = 1;
} = 0x20000001;
