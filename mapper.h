#ifndef luamapper_H
#define luamapper_H
#include <iostream>
#include <string>
#include  <vector>
#include  <list>
using namespace std;
#define infile_buff 2048 		//地图文件每行最大长度
#define roomname_length 20	 //房间名的最大长度
#define pathtag_length 20	 //tag名的最大长度
#define pathcontent_length 40 	//每条路径的行走指令的最大长度
#define room_def 5000		 //默认房间数
#define room_step 500		 //默认每次增加房间数
#define tagsize sizeof(struct pathtag)
#define pathsize sizeof(struct path)
#define stepsize sizeof(struct step)
#define bindinfosize sizeof(struct bindinfo)
#if defined(BUILDDLL)
extern "C" _declspec(dllexport) int luaopen_mapper (lua_State *L);
#endif
 struct path
{
	char content[pathcontent_length];	//出口的指令
	int delay;				//延时
	int from;				//起点房间
	int to;					//终点房间
	char tag[pathtag_length];
};
 class room
{	public:
	room();
	~room();
	char name[roomname_length];
	list <path> exits;
	list <path> tagexits;
};
struct pathresult
{
	string path;
	int delay;
};
struct bindinfo
{
	int to;
	int from;
};

 class pathtag
{
	public:
	pathtag();
	~pathtag();
	char tag[pathtag_length];
	list <path> paths;
};

struct roadmap
{	int walked;
	struct path path;
};
struct walkstep
{
struct path path;
int delay;
};
class walking
{public:
	walking();
	struct pathresult getpath(int to,int fr,int fly,vector <room> *rooms,vector <room> *rooms_back,list <path> *flylist);
	~walking();
	list <struct walkstep> walksteps;
	list <struct walkstep> walksteps_back;
	private:
	vector <roadmap> roadmaps;
	vector <roadmap> roadmaps_back;
	struct pathresult getresult(int keyroom,int ro,int fr);
	struct pathresult getresult_back(int keyroom,int to,int fr);
	int walk(vector <room> *rooms);
	int walk_back(vector <room> *rooms_back);
	void walkroom(vector <room> *rooms,int roomid,list <struct walkstep> *walks);
};


class tags //储存tag信息的类
{
	public:
		tags();
		~tags();
		void addpath(char sstag[pathtag_length],struct path tmppath); /*为指定的tag添加路径*/
		struct path getpath(char stag[pathtag_length]);
		list <pathtag> tag;
};

class mapper
{
	public:
	mapper();
	~mapper();
	int open(string filename);
//	void save();
//	void newmapper();
	struct pathresult getpath(int fr,int to,int fly);
	int room_max;
	vector <room> rooms;
	vector <room> rooms_back;
	void settags(string _tags);
	void setflylist(string flylist);
	list <path> flylist;
//	class walking walk;
	class tags tags;

	int room_count;//房间数
	list <path> binds;//已经榜定的tags,撤销用
	list <bindinfo> bindinfos;
	char vchar[8];//文本处理中的控制字符
	int newarea(int count);
	void readdata(char data[infile_buff],int roomid);
	void readexits(string data,int roomid);
	void exit_to_path(string data,int roomid);
	struct path makepath(string datatxt,int roomid);
	void bind(struct pathtag tag);
	void debind();
	string lasttag;
	string uid;
};

#endif
