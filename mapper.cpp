#pragma comment( lib, "lua.lib" )
#include <stdio.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <malloc.h>
#include "lua.hpp"
#include "mapper.h"
#include  <vector>
#include  <list>
using namespace std;
vector <mapper*> maps;
/*******************************************
mapper对象的过程
*******************************************/
int getmapperid(string uid)
{int i;
	mapper* tmpmapper;
	for (i=0; i<maps.size(); i++){
		if (maps.at(i)->uid==uid){
			return i;
		};
	};
	tmpmapper=new mapper();
	tmpmapper->uid=uid;
	maps.push_back(tmpmapper);
	return i;
};

room::room()
{
	exits.clear();
	tagexits.clear();
	name="";
}
room::~room()
{
	exits.clear();
	tagexits.clear();
};

pathtag::pathtag()
{
	paths.clear();
	tag="";
}
pathtag::~pathtag()
{
	paths.clear();
}
mapper::mapper()
{
strcpy(vchar,"|=,:><%;");//设置默认的控制字符
	rooms.resize(room_def);
	rooms_back.resize(room_def);
	room_max=room_def;
	room_count=-1;
}
mapper::~mapper()
{
	uid="";
}
//打开地图文件
//参数filename为打开的文件名
int mapper::open(string filename)
{
	lasttag="";
	string in_txt;
	debind();
	string txttemp;
	ifstream in_file(filename.c_str());
	if (!in_file.is_open()) return false;
	rooms.clear();
	rooms_back.clear();
	rooms.resize(room_def);
	rooms_back.resize(room_def);
	room_max=room_def;
	room_count=-1;
	do
  	{
		getline(in_file,txttemp);
        	readdata(txttemp,newarea(1));
  	}while   (!in_file.eof());
	in_file.close();
	return true;
};
//处理读入的数据
//参数data为读入的数据
int mapper::newarea(int count)
{
	int area=room_count+1;
	room_count=room_count+count;
	if (room_count>room_max)
	{
		room_max=room_max+room_step;
		rooms.resize(room_max);
		rooms_back.resize(room_max);
	}
	return area;
}
void mapper::clearroom(int roomid)
{
	if (roomid<0||roomid>room_count) {return;}
	rooms[roomid].name="";
	list<path>::iterator ipath;
	list<path>::iterator ipath2;
	list<pathtag>::iterator tmptag;;
	for(ipath=rooms[roomid].exits.begin();ipath!=rooms[roomid].exits.end();++ipath)
	{
		for(ipath2=rooms_back[ipath->to].exits.begin();ipath2!=rooms_back[ipath->to].exits.end();)
		{
			if((ipath2->from)==roomid){ipath2 = rooms_back[ipath->to].exits.erase(ipath2);}
			else{++ipath2;};
		};
	}
	for(ipath=rooms[roomid].tagexits.begin();ipath!=rooms[roomid].tagexits.end();++ipath)
	{
		for(ipath2=rooms_back[ipath->to].tagexits.begin();ipath2!=rooms_back[ipath->to].tagexits.end();)
		{
			if((ipath2->from)==roomid){ipath2 = rooms_back[ipath->to].tagexits.erase(ipath2);}else{++ipath2;};
		};
	}
	rooms[roomid].exits.clear();
	rooms[roomid].tagexits.clear();
		for(tmptag=tags.tag.begin();tmptag!=tags.tag.end();++tmptag)
	{
		for(ipath=tmptag->paths.begin();ipath!=tmptag->paths.end();)
		{
			if((ipath->from)==roomid){ipath = tmptag->paths.erase(ipath);}else{++ipath;};
		};
	};
};
void mapper::readdata(string data,int roomid)
{
//	room_count ++;
	if (roomid<0||roomid>room_count) {return;}
	string datatxt=data;
	string dataroomname;
	int i;
	i = datatxt.find(vchar[0]);//判断是否有房间名，默认用|分割的
	if (i==string::npos){
		return;
	}
	dataroomname=datatxt.substr(0,i);
	datatxt.assign(datatxt,i+1,datatxt.size());
	i = dataroomname.find(vchar[1]);
	if (i==string::npos){
	return;
	};//判断房间名结束
	dataroomname.assign(dataroomname, i+1, dataroomname.size());
	rooms[roomid].name=dataroomname;
	rooms.at(roomid).exits.clear();
	readexits(datatxt,roomid);
};

//取得默认由,分隔的出口信息
void mapper::readexits(string datatxt,int roomid)
{
	string datatxt2;
	int i;
	i = datatxt.find(vchar[2]);
	if (i!=string::npos){
	exit_to_path(datatxt.substr(0,i),roomid);
	datatxt2.assign(datatxt,i+1,datatxt.size());
	readexits(datatxt2,roomid);
	};
	return;
};
void mapper::exit_to_path(string data,int roomid)
{
	if (roomid<0||roomid>room_count) {return;}
	int i;
	string tmpstring;
	string tmptxt;
	struct path tmppath;
	tmpstring=vchar[4];
	tmpstring+=vchar[5];
	i=data.find_last_of(tmpstring);
	if (i!=string::npos)
	{
		tmptxt=data.substr(0,i+1);
		data.assign(data,i+1,data.size());
		tmppath=makepath(data,roomid);
		if (tmppath.to>-1) {
			tags.addpath(tmptxt,tmppath);
		};
		return;
	};
	tmppath=makepath(data,roomid);
	if (tmppath.to<0) {return;};
	rooms.at(roomid).exits.push_back(tmppath);
	rooms_back.at(tmppath.to).exits.push_back(tmppath);
	return;
};
//处理读取出的出口信息为程序数据
struct path mapper::makepath(string datatxt, int roomid)
{
	int i;
	int delay=1;
	struct path tmppath;
	tmppath.to=-1;
	i=datatxt.rfind(vchar[6]);
	if (i!=string::npos){
		delay=atoi(datatxt.substr(i+1,(datatxt.size()-i)-1).c_str());
		datatxt.assign(datatxt,0,i);
	}
	i=datatxt.find(vchar[3]);
	if (i==string::npos){
		return tmppath;
	};
	tmppath.content=datatxt.substr(0,i);
	tmppath.from=roomid;
	datatxt.assign(datatxt,i+1,datatxt.size());
	tmppath.to=atoi(datatxt.c_str());
	if (delay<1) {delay=1;}
	tmppath.delay=delay;
// roommax考虑修改下
	if ((tmppath.to<0)||(tmppath.to>room_max)){
		tmppath.to=-1;
		return tmppath;
	};
	return tmppath;
};

struct pathresult mapper::getpath(int fr,int to,int fly)
{
	class walking walk;
	return walk.getpath(to,fr,fly,&rooms,&rooms_back,&flylist);};

void mapper::bind(struct pathtag tag)
{
	struct bindinfo tmpbindinfo;
	list<path>::iterator ipath;
	struct path tmppath;
	for(ipath=tag.paths.begin();ipath!=tag.paths.end();++ipath)
	{
	if (ipath->to<0||ipath->to>room_count||ipath->from<0||ipath->from>room_count) {continue;}
	tmppath=*ipath;
	rooms[ipath->from].tagexits.push_back(tmppath);
	tmpbindinfo.from=ipath->from;
	rooms_back[ipath->to].tagexits.push_back(tmppath);
	tmpbindinfo.to=ipath->to;
	bindinfos.push_back(tmpbindinfo);}
}
void mapper::debind()
{
	list<bindinfo>::iterator ibind;
	for(ibind=bindinfos.begin();ibind!=bindinfos.end();++ibind)
	{
	if (ibind->to<0||ibind->to>room_count||ibind->from<0||ibind->from>room_count) {continue;}
		rooms[ibind->from].tagexits.clear();
		rooms_back[ibind->to].tagexits.clear();
	};
	bindinfos.clear();
}

void mapper::settags(string _tags)
{
	if (lasttag==_tags){return;};
	list<pathtag>::iterator tmptag;
	string tmpstring,tmpstring2,tmpstring3;
	int found;
	int lastfound;
	lasttag=_tags;
	debind();
	_tags.insert(0,1,vchar[0]);
	_tags+=vchar[0];
	tmpstring2=vchar[4];
	tmpstring2+=vchar[5];
	for(tmptag=tags.tag.begin();tmptag!=tags.tag.end();++tmptag)
	{
		tmpstring=tmptag->tag;
		lastfound=0;
		found=tmpstring.find_first_of(tmpstring2,0);
		 while(found!=string::npos)
		{
			tmpstring3=vchar[0];
			tmpstring3+=tmpstring.substr(lastfound,found - lastfound);
			tmpstring3+=vchar[0];
			if (_tags.find(tmpstring3)==string::npos) {
				if (tmpstring.at(found)==vchar[4]){
					break;
				}else{
					lastfound=found+1;
					found=tmpstring.find_first_of(tmpstring2,lastfound);
					continue;
				}
			}
			if (tmpstring.at(found)==vchar[5]) {break;};
			lastfound=found+1;
			found=tmpstring.find_first_of(tmpstring2,lastfound);
		};
		if (found==string::npos) {
			bind(*tmptag);}
	};
};

void mapper::setflylist(string _flylist)
{
	struct path tmppath;
	int i;
	flylist.clear();
	i=_flylist.find(vchar[2]);
	while (i!=string::npos)
	{
		tmppath=makepath(_flylist.substr(0,i),0);
		if(tmppath.to>-1)
		{
			flylist.push_back(tmppath);
			}
		_flylist.assign(_flylist,i+1,_flylist.size());
		i=_flylist.find(vchar[2]);
	}
}
/**************************************
tags对象的处理过程
***************************************/

tags::tags()
{
};

tags::~tags()
{
	tag.clear();
};

void tags::addpath(string sstag,struct path tmppath) //为指定的tag添加路径
{
	list<pathtag>::iterator itag;;
	string tmpstring=sstag;
	for(itag=tag.begin();itag!=tag.end();++itag)
	{
		if (tmpstring.compare(itag->tag)==0)
		{
			itag->paths.push_back(tmppath);
			return;
		}
	};
	//判断为新tag,把这个tag加入tag表
	struct pathtag tmptag;
	tmptag.tag=sstag;
	tmptag.paths.push_back(tmppath);
	tag.push_back(tmptag);

};
/**************************************
行走模块
***************************************/
walking::walking()
{
};
walking::~walking()
{
	walksteps.clear();
	walksteps_back.clear();
	roadmaps.clear();
	roadmaps_back.clear();
};

struct pathresult walking::getpath(int to,int fr,int fly,vector <room> *rooms,vector <room> *rooms_back,list <path> *flylist)
{	struct pathresult result,result2;
	struct roadmap newroadmap;
	if (to<0||to>=rooms->size()||fr<0||fr>=rooms->size())
	{
		result.path="";
		result.delay=-1;
		}
	newroadmap.walked=0;
	roadmaps.clear();
	roadmaps.assign (rooms->size(),newroadmap);
	walksteps.clear();
	walksteps_back.clear();
	roadmaps[fr].walked=1;
	walkroom(rooms,fr,&walksteps);
	roadmaps_back.clear();
	roadmaps_back.assign (rooms->size(),newroadmap);
	walksteps_back.clear();
	roadmaps_back[to].walked=1;
	walkroom(rooms_back,to,&walksteps_back);
	struct walkstep flystep;
	if (fly==1)
	{
		list <path>::iterator ifly;
		for(ifly=flylist->begin();ifly!=flylist->end();++ifly)
		{
			if (ifly->to<0||ifly->to>rooms->size()) {continue;}
			flystep.delay=ifly->delay;
			flystep.path=*ifly;
			flystep.path.from=fr;
			walksteps.push_back(flystep);
		};
	};
	int keypoint=-1;
	do
	{keypoint=walk(rooms);
		if (keypoint >-1 || walksteps.size()==0){break;};
		keypoint=walk_back(rooms_back);
		}while(keypoint==-1 && walksteps_back.size()>0);
	if (keypoint ==-1)
	{
		result.path="";
		result.delay=-1;
		return result;
	}
	result=getresult(keypoint,to,fr);

	if (keypoint!=to)
	{result2=getresult_back(keypoint,to,fr);
	result.path=result.path+result2.path;
	result.delay=result.delay+result2.delay;}
	return result;
};

int walking::walk(vector <room> *rooms)
{
	list <struct walkstep>::iterator istep;
	list <struct walkstep> newwalksteps;
	for (istep=walksteps.begin();istep!=walksteps.end();++istep)
	{
		istep->delay--;
		if (istep->path.to>=roadmaps.size()||istep->path.to<0){continue;};
		if (roadmaps[istep->path.to].walked==1){continue;};
		if (istep->delay<1)
		{
			roadmaps[istep->path.to].walked=1;
			roadmaps[istep->path.to].path=istep->path;
			if (roadmaps_back[istep->path.to].walked==1) {return istep->path.to;};
				walkroom(rooms,istep->path.to,&newwalksteps);
		}else{
			newwalksteps.push_back(*istep);
		}
	}
	walksteps=newwalksteps;
	return -1;
}
void walking::walkroom(vector <room> *rooms,int roomid,list <struct walkstep> *walks)
	{
		if (roomid>=rooms->size()||roomid<0) {return;};
		struct walkstep newstep;
		list <path>::iterator iexit;
		for (iexit=rooms->at(roomid).exits.begin();iexit!=rooms->at(roomid).exits.end();++iexit)
		{
			newstep.delay=iexit->delay;
			newstep.path=*iexit;
			walks->push_back(newstep);
		}
		for (iexit=rooms->at(roomid).tagexits.begin();iexit!=rooms->at(roomid).tagexits.end();++iexit)
		{
			newstep.delay=iexit->delay;
			newstep.path=*iexit;
			walks->push_back(newstep);
		}
	}
int walking::walk_back(vector <room> *rooms_back)
{
	list <struct walkstep>::iterator istep;
	list <struct walkstep> newwalksteps;
	for (istep=walksteps_back.begin();istep!=walksteps_back.end();++istep)
	{
		if (istep->path.from<0||istep->path.from>=rooms_back->size()) {continue;}
		if (roadmaps_back[istep->path.from].walked==1){continue;};
		istep->delay--;
		if (istep->delay<1)
		{
			roadmaps_back[istep->path.from].walked=1;
			roadmaps_back[istep->path.from].path=istep->path;
			if (roadmaps[istep->path.from].walked==1) {return istep->path.from;};
				walkroom(rooms_back,istep->path.from,&newwalksteps);
		}else{
			newwalksteps.push_back(*istep);
		}
	}
	walksteps_back=newwalksteps;
	return -1;
}

struct pathresult walking::getresult(int keyroom,int to,int fr)
{
	struct pathresult result;
	do
	{
	result.path=";"+result.path;
	result.path=roadmaps[keyroom].path.content+result.path;
	result.delay=roadmaps[keyroom].path.delay+result.delay;
	keyroom=roadmaps[keyroom].path.from;
	}while(keyroom!=to && keyroom!=fr && keyroom>-1 && keyroom <roadmaps.size());
	return result;
}

struct pathresult walking::getresult_back(int keyroom,int to,int fr)
{
	struct pathresult result;
	do{
		result.path=result.path+roadmaps_back[keyroom].path.content;
		result.path=result.path+";";
		result.delay=roadmaps_back[keyroom].path.delay+result.delay;
		keyroom=roadmaps_back[keyroom].path.to;
	}while(keyroom!=to && keyroom!=fr && keyroom>-1 && keyroom <roadmaps.size());
	return result;
}


/**************************************
与lua文件的接口
**************************************/
#if defined(BUILDDLL)
static int l_openfile(lua_State *L)
{
	string _filename;
	int mapid=luaL_checknumber(L,1);
	if (mapid<0||mapid>=maps.size()){
		lua_pushnumber(L,-1);
		return 1;
		}
	_filename = lua_tostring(L,2);
	lua_settop(L,0);
	lua_pushnumber(L,maps.at(mapid)->open(_filename));
	return 1;
};
static int l_settags(lua_State *L)
{
	string l_tags;
	int mapid=luaL_checknumber(L,1);
	if (mapid<0||mapid>=maps.size()){
		return 0;
	}
	l_tags = lua_tostring(L,2);
	maps.at(mapid)->settags(l_tags);
	lua_settop(L,0);
	return 0;
};
static int l_setflylist(lua_State *L)
{
	string l_flylist;
	int mapid=luaL_checknumber(L,1);
	if (mapid<0||mapid>=maps.size()){
		return 0;
	}
	l_flylist = lua_tostring(L,2);
	maps.at(mapid)->setflylist(l_flylist);
	lua_settop(L,0);
	return 0;
}
static int l_getroomid(lua_State *L)
{
	int i;
	int l_count=0;
	string l_roomname;
	int mapid=luaL_checknumber(L,1);
	l_roomname = lua_tostring(L,2);
	lua_settop(L,0);
	lua_newtable(L);
	if (mapid<0||mapid>=maps.size())
	{
		return 1;
	}
	for(i=0;i<=maps.at(mapid)->room_count;i++)
	{
		if (l_roomname.compare(maps.at(mapid)->rooms[i].name)==0)
		{
			l_count++;
			lua_pushnumber(L,l_count);
			lua_pushnumber(L,i);
			lua_settable(L,1);
		}
	}
	return 1;
}
static int l_getid(lua_State *L)
{
	string l_uid;
	l_uid = lua_tostring(L,1);
	lua_settop(L,0);
	lua_pushnumber(L,getmapperid(l_uid));
	return 1;
}
static int l_newarea(lua_State *L)
{
	int mapid=luaL_checknumber(L,1);
	if (mapid<0||mapid>=maps.size()){
		lua_pushnumber(L,-1);
		return 1;
		}
	int l_count=luaL_checknumber(L,2);
	lua_settop(L,0);
	lua_pushnumber(L,maps.at(mapid)->newarea(l_count));
	return 1;
}
static int l_clearroom(lua_State *L)
{
	int mapid=luaL_checknumber(L,1);
	if (mapid<0||mapid>=maps.size()){
		lua_pushnumber(L,-1);
		return 0;
		}
	int l_roomid=luaL_checknumber(L,2);
	if ((l_roomid<0)||(l_roomid>maps.at(mapid)->room_count))
	{
		return 0;
	}
	lua_settop(L,0);
	maps.at(mapid)->clearroom(l_roomid);
	return 0;
}
static int l_readroom(lua_State *L)
{
	int mapid=luaL_checknumber(L,1);
	int l_roomid=luaL_checknumber(L,2);
	if (mapid<0||mapid>=maps.size()){
		return 0;
		}
	if (l_roomid<0||l_roomid>maps.at(mapid)->room_count) {return 0;}
	string l_data = lua_tostring(L,3);
	lua_settop(L,0);
	maps.at(mapid)->clearroom(l_roomid);
	maps.at(mapid)->readdata(l_data,l_roomid);
	return 0;
}
static int l_getexits(lua_State *L)
{
	int mapid=luaL_checknumber(L,1);
	int l_roomid=luaL_checknumber(L,2);
	int l_count=0;
	lua_settop(L,0);
	lua_newtable(L);
	list <struct path>::iterator tmppath;
	if (mapid<0||mapid>=maps.size()){
		return 1;
	}
	if ((l_roomid<0)||(l_roomid>maps.at(mapid)->room_count))
	{
		return 1;
	}
	for (tmppath=maps.at(mapid)->rooms[l_roomid].exits.begin();tmppath!=maps.at(mapid)->rooms[l_roomid].exits.end();++tmppath)
	{
		l_count++;
		lua_pushnumber(L,l_count);
		lua_newtable(L);
		lua_pushnumber(L,1);
		lua_pushstring(L,tmppath->content.c_str());
		lua_settable(L,3);
		lua_pushnumber(L,2);
		lua_pushnumber(L,tmppath->to);
		lua_settable(L,3);
		lua_settable(L,1);
	}
	for (tmppath=maps.at(mapid)->rooms[l_roomid].tagexits.begin();tmppath!=maps.at(mapid)->rooms[l_roomid].tagexits.end();++tmppath)
	{
		l_count++;
		lua_pushnumber(L,l_count);
		lua_newtable(L);
		lua_pushnumber(L,1);
		lua_pushstring(L,tmppath->content.c_str());
		lua_settable(L,3);
		lua_pushnumber(L,2);
		lua_pushnumber(L,tmppath->to);
		lua_settable(L,3);
		lua_settable(L,1);
	}
		for (tmppath=maps.at(mapid)->flylist.begin();tmppath!=maps.at(mapid)->flylist.end();++tmppath)
	{
		l_count++;
		lua_pushnumber(L,l_count);
		lua_newtable(L);
		lua_pushnumber(L,1);
		lua_pushstring(L,tmppath->content.c_str());
		lua_settable(L,3);
		lua_pushnumber(L,2);
		lua_pushnumber(L,tmppath->to);
		lua_settable(L,3);
		lua_settable(L,1);
	}
	return 1;
}

static int l_getroomname(lua_State *L)
{
	int mapid=luaL_checknumber(L,1);
	int l_roomid=luaL_checknumber(L,2);
	lua_settop(L,0);
	if (mapid<0||mapid>=maps.size()){
		lua_pushstring(L,"");
			return 1;
		}
	if ((l_roomid<0)||(l_roomid>maps.at(mapid)->room_count))
	{
		lua_pushstring(L,"");
		return 1;
	}
	lua_pushstring(L,maps.at(mapid)->rooms[l_roomid].name.c_str());
	return 1;
};

static int l_getpath(lua_State *L)
{
	int mapid=luaL_checknumber(L,1);
	int l_fr = luaL_checknumber(L , 2);
	int l_to = luaL_checknumber(L , 3);
	int l_fly=1;
	int i=lua_gettop(L);
	if (mapid<0||mapid>=maps.size()){
		lua_settop(L,0);
		lua_pushstring(L,"");
		lua_pushnumber(L,-1);
		return 2;}
	if ((i<3)||(i>4)||(l_to<0)||(l_to>maps.at(mapid)->room_count)||(l_fr<0)||(l_fr>maps.at(mapid)->room_count))
	{
		lua_settop(L,0);
		lua_pushstring(L,"");
		lua_pushnumber(L,-1);
		return 2;
	}
	if (i=4) {l_fly=luaL_checknumber(L , 4);};
	lua_settop(L,0);
	pathresult result;
	result=maps.at(mapid)->getpath(l_fr,l_to,l_fly);
	lua_pushstring(L,result.path.c_str());
	lua_pushnumber(L,result.delay);
	return 2;
};
static int l_addpath(lua_State *L)
{
	int mapid=luaL_checknumber(L,1);
	int roomid=luaL_checknumber(L,2);
	if (mapid<0||mapid>=maps.size()){return 0;}
	if ((roomid<0)||(roomid>maps.at(mapid)->room_count))
	{
		return 0;
	}
	string l_path = lua_tostring(L,3);
	maps.at(mapid)->exit_to_path(l_path,roomid);
	lua_settop(L,0);
	return 0;
}
static const luaL_reg l_mushmapper[] =
{
  {"openmap", l_openfile},
  {"getroomname", l_getroomname},
  {"getroomid", l_getroomid},
  {"getpath", l_getpath},
  {"getexits", l_getexits},
  {"settags", l_settags},
  {"setflylist", l_setflylist},
  {"getid", l_getid},
  {"addpath", l_addpath},
  {"newarea", l_newarea},
  {"readroom", l_readroom},
  {"clearroom", l_clearroom},
  {NULL, NULL}
};

int luaopen_mapper (lua_State *L)
 {
  luaL_openlib(L, "mushmapper", l_mushmapper, 0);
  return 1;
 };
#endif
