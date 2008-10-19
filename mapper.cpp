#pragma comment( lib, "lua.lib" )
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <malloc.h>
#include "lua.hpp"
#include "mapper.h"


mapper map;
/*******************************************
mapper对象的过程
*******************************************/

mapper::mapper()
{
strcpy(vchar,"|=,:><%;");//设置默认的控制字符
firstbind=NULL;
lastbind=NULL;
firstfly=NULL;
lastfly=NULL;
}
mapper::~mapper()
{
	int i;
	debind();
	delpaths(firstfly);
	firstfly=NULL;
	lastfly=NULL;
	for(i=0;i<room_max;i++)
	{
		delpaths(rooms[i].firstexit);
		rooms[i].firstexit=NULL;
		rooms[i].lastexit=NULL;
	}
	room_count=0;
}
void mapper::delsteps(struct step* steps)
{
	struct step* tmpstep;
	struct step* tmpstep2;
	tmpstep=steps;
	while(tmpstep){
		tmpstep2=tmpstep;
		tmpstep=tmpstep->next;
		delete tmpstep2;
	};
	steps=NULL;
}
void mapper::delpaths(struct path* paths)
{
	struct path* tmppath;
	while (paths)
	{
		tmppath=paths;
		paths=paths->next;
		delete tmppath;
	}
}

void mapper::setflylist(string flylist)
{
	struct path* tmppath;
	int i;
	delpaths(firstfly);
	firstfly=NULL;
	lastfly=NULL;
	i=flylist.find(vchar[2]);
	while (i!=string::npos)
	{
		tmppath=makepath(flylist.substr(0,i),0);
		if(tmppath)
		{
			if (firstfly)
			{
				lastfly->next=tmppath;
				lastfly=lastfly->next;
			}else{
				firstfly=lastfly=tmppath;
			}
		}
		flylist.assign(flylist,i+1,flylist.size());
		i=flylist.find(vchar[2]);
	}
}
string mapper::getpath(int fr,int to,int _fly)
{
	struct	roadmap roadmap_tofill ={NULL};
	struct step* steps=NULL;
	struct step* newsteps=NULL;
	struct step* walk;
	struct step* newwalk;
	struct step* steps_back=NULL;
	struct step* newsteps_back=NULL;
	struct step* walk_back;
	struct step* newwalk_back;
	struct path* exit;
	string tmpstring,tmpstring2;
	int steps_count=0;
	int steps_back_count=0;
	int i;
	int link=-1;
	if ((fr==to)||(fr<0)||(to<0)||(fr>room_count-1)||(to>room_count-1)) {return "";};
	exit=rooms[fr].firstexit;
	while (exit)
	{
		if (steps)
		{
			walk->next=(struct step*)malloc(stepsize);
			walk=walk->next;
		}else{
			steps=walk=(struct step*)malloc(stepsize);
		};
		walk->next=NULL;
		walk->delay=exit->delay;
		walk->path=exit;
		exit=exit->next;
	};
	if (_fly)
	{
		exit=firstfly;
			while (exit)
			{
				exit->from=fr;
				if (steps)
				{
					walk->next=(struct step*)malloc(stepsize);
					walk=walk->next;
				}else{
					steps=walk=(struct step*)malloc(stepsize);
				};
				walk->next=NULL;
				walk->delay=exit->delay;
				walk->path=exit;
				exit=exit->next;
			};
	}
	roadmaps[fr].path=NULL;
	exit=rooms_back[to].firstexit;
	while (exit)
	{
		if (steps_back)
		{
			walk_back->next=(struct step*)malloc(stepsize);
			walk_back=walk_back->next;
		}else{
			steps_back=walk_back=(struct step*)malloc(stepsize);
		};
		walk_back->next=NULL;
		walk_back->delay=exit->delay;
		walk_back->path=exit;
		exit=exit->backnext;	
	};
	roadmaps_back[to].path=NULL;
	for(i=0;i<room_count;i++)
	{
		roadmaps[i]=roadmap_tofill;
		roadmaps_back[i]=roadmap_tofill;

	};

	do
	{
		steps_count=0;
		newwalk=newsteps=NULL;
		walk = steps;
		while (walk)
		{
			if ((walk->delay)&&((walk->next)||(steps!=walk)))
			{
				walk->delay--;
				if (newsteps)
				{
					newwalk->next=(struct step*)malloc(stepsize);
					newwalk=newwalk->next;
				}else{
					newsteps=newwalk=(struct step*)malloc(stepsize);
				};
				newwalk->next=NULL;
				newwalk->delay=walk->delay;
				newwalk->path=walk->path;
				walk=walk->next;
				steps_count++;
				continue;						
			}
			if (roadmaps[walk->path->to].path==NULL)
			{
				roadmaps[walk->path->to].path=walk->path;
				if ((roadmaps_back[walk->path->to].path)||(walk->path->to==to))
				{
					delsteps(steps);
					delsteps(newsteps);
					link=walk->path->to;
					exit=roadmaps[link].path;
					while(exit){
						tmpstring2=exit->content;
						tmpstring2+=vchar[7];
						tmpstring=tmpstring2+tmpstring;
						if (exit->from==fr){
							exit=NULL;
						}else{
							exit=roadmaps[exit->from].path;
						}
					};
					exit=roadmaps_back[link].path;
					while(exit){
						tmpstring=tmpstring+exit->content;
						tmpstring=tmpstring+vchar[7];
						if (exit->to==to){
							exit=NULL;
						}else{
							exit=roadmaps_back[exit->to].path;
						};
					};
					return tmpstring;
				};
				steps_count++;
				exit=rooms[walk->path->to].firstexit;
				while (exit)
				{
					if (newsteps)
					{
						newwalk->next=(struct step*)malloc(stepsize);
						newwalk=newwalk->next;
					}else{
						newsteps=newwalk=(struct step*)malloc(stepsize);
					};
					newwalk->next=NULL;
					newwalk->delay=exit->delay;
					newwalk->path=exit;
					exit=exit->next;	
				};
			};
			walk=walk->next;
		};
		delsteps(steps);
		steps=newsteps;
		steps_back_count=0;
		newwalk_back=NULL;
		newsteps_back=NULL;
		walk_back = steps_back;

		while (walk_back)
		{
			if ((walk_back->delay)&&((walk_back->next)||(steps_back!=walk_back)))
			{
				walk_back->delay--;
				if (newsteps_back)
				{
					newwalk_back->next=(struct step*)malloc(stepsize);
					newwalk_back=newwalk_back->next;
				}else{
					newsteps_back=newwalk_back=(struct step*)malloc(stepsize);
				};
				newwalk_back->next=NULL;
				newwalk_back->delay=walk_back->delay;
				newwalk_back->path=walk_back->path;
				walk_back=walk_back->next;
				steps_back_count++;
				continue;						
			}
			if (roadmaps_back[walk_back->path->from].path==NULL)
			{
				roadmaps_back[walk_back->path->from].path=walk_back->path;
				if ((roadmaps[walk_back->path->from].path)||(walk_back->path->from==fr))
				{

					delsteps(steps_back);
					delsteps(newsteps_back);
					link=walk_back->path->from;
					exit=roadmaps[link].path;
					while(exit){
						tmpstring2=exit->content;
						tmpstring2+=vchar[7];
						tmpstring=tmpstring2+tmpstring;
						if (exit->from==fr){
							exit=NULL;
						}else{
							exit=roadmaps[exit->from].path;
						}
					};
					exit=roadmaps_back[link].path;
					while(exit){
						tmpstring=tmpstring+exit->content;
						tmpstring=tmpstring+vchar[7];
						if (exit->to==to){
							exit=NULL;
						}else{
							exit=roadmaps_back[exit->to].path;
						}
					};
					return tmpstring;
				};
				steps_back_count++;
				
				exit=rooms_back[walk_back->path->from].firstexit;
				while (exit)
				{
					if (newsteps_back)
					{
						newwalk_back->next=(struct step*)malloc(stepsize);
						newwalk_back=newwalk_back->next;
					}else{
						newsteps_back=newwalk_back=(struct step*)malloc(stepsize);
					};
					newwalk_back->next=NULL;
					newwalk_back->delay=exit->delay;
					newwalk_back->path=exit;
					exit=exit->backnext;	
				};
			};
			walk_back=walk_back->next;

		};
		delsteps(steps_back);
		steps_back=newsteps_back;
	}while(link<0&&((steps_count>0)||(steps_back_count>0)));
	delsteps(steps);
	delsteps(steps_back);
	return "";
};

//打开地图文件
//参数filename为打开的文件名
int mapper::open(string filename)
{
	string in_txt;
	char txttemp[infile_buff];
	ifstream in_file(filename.c_str());
	if (!in_file.is_open()) return false;
	for(int i=0;i<room_max;i++){rooms_back[i].firstexit=rooms_back[i].lastexit=NULL;};
	room_count=0;
	do  
  	{
		in_file.getline(txttemp,infile_buff);
        	readdata(txttemp);
  	}while   (!in_file.eof());
	in_file.close();
	return true;
};

//处理读入的数据
//参数data为读入的数据
void mapper::readdata(char data[infile_buff])
{
	room_count ++;
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
	strcpy(rooms[room_count-1].name,dataroomname.c_str());    
	rooms[room_count-1].firstexit=NULL;
	readexits(datatxt);
};

//取得默认由,分隔的出口信息
void mapper::readexits(string datatxt)
{
	string datatxt2;
	int i;
	i = datatxt.find(vchar[2]);
	if (i!=string::npos){
	exit_to_path(datatxt.substr(0,i),room_count-1);
	datatxt2.assign(datatxt,i+1,datatxt.size());
	readexits(datatxt2);
	};
	return;
};
void mapper::exit_to_path(string data,int roomid)
{
	int i;
	string tmpstring;
	char tmptxt[pathtag_length];
	struct path* tmppath;
	tmpstring=vchar[4];
	tmpstring+=vchar[5];
	i=data.find_last_of(tmpstring);
	if (i!=string::npos)
	{
		strcpy(tmptxt,data.substr(0,i+1).c_str());
		data.assign(data,i+1,data.size());
		tmppath=makepath(data,roomid);
		if (tmppath) {
			tags.addpath(tmptxt,tmppath);
		};		
		return;
	};
	tmppath=makepath(data,roomid);
	if (tmppath ==NULL) {return;};
	if (rooms[roomid].firstexit)
	{
		rooms[roomid].lastexit->next=tmppath;
		rooms[roomid].lastexit=tmppath;
	}else{
	rooms[roomid].lastexit=rooms[roomid].firstexit=tmppath;
	};
	if (rooms_back[tmppath->to].firstexit)
	{	rooms_back[tmppath->to].lastexit->backnext=tmppath;
		rooms_back[tmppath->to].lastexit=tmppath;
	}else{
	rooms_back[tmppath->to].lastexit=rooms_back[tmppath->to].firstexit=tmppath;
	};
	return;
};

//处理读取出的出口信息为程序数据
struct path* mapper::makepath(string datatxt, int roomid)
{
	int i;
	int delay=0;
	struct path* tmppath;
	i=datatxt.rfind(vchar[6]);
	if (i!=string::npos){
		delay=atoi(datatxt.substr(i+1,(datatxt.size()-i)-1).c_str());
		datatxt.assign(datatxt,0,i);
	}
	i=datatxt.find(vchar[3]);
	if (i==string::npos){
		return NULL;
	};
	tmppath=(struct path*)malloc(pathsize);
	strcpy(tmppath->content,datatxt.substr(0,i).c_str());
	tmppath->from=roomid;
	datatxt.assign(datatxt,i+1,datatxt.size());
	tmppath->to=atoi(datatxt.c_str());
	tmppath->delay=delay;
	tmppath->next=NULL;
	tmppath->backnext=NULL;
	if ((tmppath->to<0)||(tmppath->to>room_max)){
		delete tmppath;
		return NULL;
	};
	return tmppath;
};

void mapper::debind()
{
	struct bindinfo* tmpbind;
	struct bindinfo* tmpbind2;
	tmpbind=firstbind;
	while (tmpbind)
	{
		if(tmpbind->bindto){
			if(tmpbind->isfirstexit>-1){
				rooms[tmpbind->isfirstexit].firstexit=rooms[tmpbind->isfirstexit].lastexit=NULL;
			}else{
				rooms[tmpbind->path->from].lastexit=tmpbind->bindto;
				tmpbind->bindto->next=NULL;
			};
		};
		tmpbind=tmpbind->next;
	};
	tmpbind=firstbind;
	while (tmpbind)
	{
		tmpbind2=tmpbind;
		delete tmpbind->path;
		delete tmpbind;
		tmpbind=tmpbind2->next;
	}
	firstbind=lastbind=NULL;
};
struct path* fakepath(struct path* tmppath){
	struct path* newpath;
	newpath=(struct path*)malloc(pathsize);
	strcpy(newpath->content,tmppath->content);
	newpath->delay=tmppath->delay;
	newpath->from=tmppath->from;
	newpath->to=tmppath->to;
	strcpy(newpath->tag,tmppath->tag);
	newpath->next=NULL;
	newpath->backnext=NULL;
	return newpath;
};

void mapper::bind(struct pathtag *tag)
{
	struct path* tmppath;
	struct bindinfo* tmpbind;
	tmppath=tag->firstpath;
	while (tmppath)
	{
		tmpbind=(struct bindinfo*)malloc(bindinfosize);
		tmpbind->next=NULL;
		tmpbind->path=fakepath(tmppath);
		tmpbind->bindto=rooms[tmpbind->path->from].lastexit;
		tmpbind->isfirstexit=-1;
		if(firstbind)
		{
			lastbind->next=tmpbind;
			lastbind=tmpbind;
		}else{
			firstbind=lastbind=tmpbind;
		}
		if (tmpbind->bindto)
		{
			rooms[tmpbind->path->from].lastexit->next=tmpbind->path;
			rooms[tmpbind->path->from].lastexit=tmpbind->path;
		}else{
			tmpbind->isfirstexit=tmpbind->path->from;
			tmpbind->bindto=rooms[tmpbind->path->from].firstexit=rooms[tmpbind->path->from].lastexit=tmpbind->path;
		};
		tmppath=tmppath->next;			
	};
};

void mapper::settags(string _tags)
{
	struct pathtag *tmptag;
	string tmpstring,tmpstring2,tmpstring3;
	int found;
	int lastfound;
	debind();
	_tags.insert(0,1,vchar[0]);
	_tags+=vchar[0];
	tmptag=tags.firsttag;
	tmpstring2=vchar[4];
	tmpstring2+=vchar[5];
	while (tmptag)
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
		bind(tmptag);};
		tmptag=tmptag->next;
	};
};

void mapper::_debug()
{
	int i;
	string data;
	data="chi>345>123|456";
	i=data.find_last_of("><",0);
	cout << i << endl; 
};

/**************************************
tags对象的处理过程
***************************************/

tags::tags()
{
};

tags::~tags()
{	
	struct pathtag *tmptag;
	struct path *tmppath;
	while(firsttag){
		while (firsttag->firstpath){
			tmppath=firsttag->firstpath;
			firsttag->firstpath=firsttag->firstpath->next;
			delete tmppath;
		};
		tmptag=firsttag;
		firsttag=firsttag->next;
		delete tmptag;
	} ;
};

void tags::addpath(char sstag[pathtag_length],struct path *tmppath) //为指定的tag添加路径
{
	struct pathtag *tmptag,*itag;
	string tmpstring=sstag;
	itag=firsttag;
	while(itag)
	{
		if (tmpstring.compare(itag->tag)==0)
		{//是已有tag的话，把路径加在已有tag的最后
			if(itag->firstpath){
				itag->lastpath->next=tmppath;
				itag->lastpath=tmppath;
			}else{
				itag->firstpath=itag->lastpath=tmppath;
			}
		return;
		};
		itag=itag->next;
	};
	//判断为新tag,把这个tag加入tag表
	tmptag=(struct pathtag*)malloc(tagsize);
	strcpy(tmptag->tag,sstag);
	tmptag->next=NULL;
	tmptag->firstpath=tmptag->lastpath=tmppath;
	if (firsttag)
	{
		lasttag->next=tmptag;
		lasttag=tmptag;
	}else{
		firsttag=lasttag=tmptag;
	};
};


/**************************************
与lua文件的接口
**************************************/
#if defined(BUILDDLL)
static int l_openfile(lua_State *L)
{
	string _filename;
	_filename = lua_tostring(L,1);
	lua_pushnumber(L,map.open(_filename));
	return 1;
};
static int l_settags(lua_State *L)
{
	string l_tags;
	l_tags = lua_tostring(L,1);
	map.settags(l_tags);
	return 0;
};
static int l_setflylist(lua_State *L)
{
	string l_flylist;
	l_flylist = lua_tostring(L,1);
	map.setflylist(l_flylist);
	return 0;
}
static int l_getroomid(lua_State *L)
{
	int i;
	int l_count=0;
	string l_roomname;
	l_roomname = lua_tostring(L,1);
	if (l_roomname.size()>roomname_length)
	{
		lua_pushnumber(L,0);
		return 1;
	}
	lua_settop(L,0);
	lua_pushnumber(L,0);
	for(i=0;i<map.room_count;i++)
	{
		if (l_roomname.compare(map.rooms[i].name)==0)
		{
			l_count++;
			lua_pushnumber(L,i);
		}
	}
	lua_pushnumber(L,l_count);
	lua_replace(L,1);
	return l_count+1;
}
static int l_getexits(lua_State *L)
{
	int l_roomid=luaL_checknumber(L,1);
	int l_count=0;
	struct path *tmppath;
	if ((l_roomid<0)||(l_roomid>room_max))
	{
		lua_pushnumber(L,0);
		return 1;
	}
	lua_settop(L,0);
	lua_pushnumber(L,0);
	tmppath=map.rooms[l_roomid].firstexit;
	while (tmppath)
	{
		l_count++;
		lua_pushstring(L,tmppath->content);
		lua_pushnumber(L,tmppath->to);
		tmppath=tmppath->next;
	}
	tmppath=map.firstfly;
	while (tmppath)
	{
		l_count++;
		lua_pushstring(L,tmppath->content);
		lua_pushnumber(L,tmppath->to);
		tmppath=tmppath->next;
	}
	lua_pushnumber(L,l_count);
	lua_replace(L,1);
	return (l_count*2+1);
}

static int l_getroomname(lua_State *L)
{
	int l_roomid=luaL_checknumber(L,1);
	if ((l_roomid<0)||(l_roomid>room_max))
	{
		lua_pushstring(L,"");
		return 1;
	}
	lua_pushstring(L,map.rooms[l_roomid].name);
	return 1;
};
static int l_debug(lua_State *L)
{
	map._debug();
	return 0;
};

static int l_getpath(lua_State *L)
{
	int l_fr = (int) luaL_checknumber(L , 1);
	int l_to = (int) luaL_checknumber(L , 2);
	int l_fly=1;
	int i=lua_gettop(L);
	if ((i<2)||(i>3))
	{
		lua_pushstring(L,"");
		return 1;
	}
	if (i=3) {l_fly=(int) luaL_checknumber(L , 3);};
	string result;
	result=map.getpath(l_fr,l_to,l_fly);
	lua_pushstring(L,result.c_str());
	return 1;
};

static const luaL_reg l_mushmapper[] = 
{
  {"openmap", l_openfile},
  {"getroomname", l_getroomname},
  {"getroomid", l_getroomid},
  {"getpath", l_getpath},
  {"getexits", l_getexits},
  {"settags", l_settags},
  {"setflylist", l_setflylist},
  {"debug", l_debug},
  {NULL, NULL}
};

int luaopen_mapper (lua_State *L)
 {
  luaL_openlib(L, "mushmapper", l_mushmapper, 0);
  return 1;
 };
#endif


