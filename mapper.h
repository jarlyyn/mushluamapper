#ifndef luamapper_H
#define luamapper_H
#include <iostream>
#include <string>
using namespace std;
#define infile_buff 2048 		//地图文件每行最大长度
#define roomname_length 20	 //房间名的最大长度
#define pathtag_length 20	 //tag名的最大长度
#define pathcontent_length 40 	//每条路径的行走指令的最大长度
#define room_max 5000		 //默认房间数
#define tagsize sizeof(struct pathtag)
#define pathsize sizeof(struct path)
#define stepsize sizeof(struct step)
#define bindinfosize sizeof(struct bindinfo)
#if defined(BUILDDLL)
extern "C" _declspec(dllexport) int luaopen_mapper (lua_State *L);
#endif

 struct bindinfo		//把符合tag的path绑定到rooms的信息，用来撤销tags用的
{
	struct path *bindto;
	struct path *path;
	struct bindinfo* next;
};

 struct path
{
	char content[pathcontent_length];	//出口的指令
	int delay;				//延时
	int from;				//起点房间
	int to;					//终点房间
	char tag[pathtag_length];
	struct path *next;			//下一出口
	struct path *backnext;
};


 struct room
{
	char name[roomname_length];
	struct path *firstexit;
	struct path *lastexit;
};
 struct room_back
{
	struct path *firstexit;
	struct path *lastexit;
};

 struct pathtag
{
	char tag[pathtag_length];
	struct path *firstpath;
	struct path *lastpath;
	struct pathtag *next;
};

 struct roadmap
{
	struct path *path;

};
 struct step
{
	int id;
	int delay;
	struct path *path;
	struct step *next;
};

class tags //储存tag信息的类
{
	public:
		tags();
		~tags();
		void addpath(char sstag[pathtag_length],struct path *tmppath); /*为指定的tag添加路径*/
		struct path* getpath(char stag[pathtag_length]);
		struct pathtag *firsttag;
		struct pathtag *lasttag;
};

class mapper
{
	public:
	mapper();
	~mapper();
	int open(string filename);
	void save();
	string getpath(int fr,int to,int _fly);
	void newmapper();
	room rooms[room_max];
	roadmap roadmaps[room_max];
	roadmap roadmaps_back[room_max];
	void settags(string _tags);
	void setflylist(string flylist);
	struct path *firstfly;
	struct path *lastfly;
	void delpaths(struct path* paths);
	void _debug();
	class tags tags;
	int room_count;//房间数
	struct bindinfo *firstbind;
	struct bindinfo *lastbind;
	void debind();
	void bind(struct pathtag *tag);
	struct room_back rooms_back[room_max];
	char vchar[8];//文本处理中的控制字符
	void readdata(char data[infile_buff]);
	void readexits(string data);
	void exit_to_path(string data,int roomid);
	void delsteps(struct step* steps);
	struct path* makepath(string datatxt,int roomid);
};




#endif
